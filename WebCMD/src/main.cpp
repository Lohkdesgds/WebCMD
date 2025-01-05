#include <history.h>
#include <page_sources.h>
#include <HTTPLIB/httplib.h>
#include <Process/process.h>

#include <iostream>

int main(int argc, char* argv[])
{
	std::cout << "Starting..." << std::endl;

	httplib::Server svr;
	std::vector<std::unique_ptr<Lunaris::process_async>> procs;
	History log;
	int startpoint = 1;
	long port = 80;

	if (argc > 1 && (strncmp("port:", argv[1], 5) == 0))
	{
		startpoint = 2;
		port = std::atoi(argv[1] + 5);
		std::cout << "Port set to " << port << std::endl;
	}

	const auto start_procs = [&]{
		log.append_log(TableLine{ 0, "This is the beginning of the application." });
		log.append_log(TableLine{ 0, "The number of arguments are: " + std::to_string(argc - startpoint)});
		log.append_log(TableLine{ 0, "Starting them soon."});

		for (int p = startpoint; p < argc; ++p) {
			std::cout << "Initializing \"" << argv[p] << "\"." << std::endl;
			procs.push_back(std::make_unique<Lunaris::process_async>(argv[p], [&, idcpy = p](auto&, const std::string& ou) {
				log.append_log(TableLine{ static_cast<uint64_t>(idcpy), ou });
			}));
		}
	};

	std::cout << "Initialized logs." << std::endl;	
	std::cout << "Setting up endpoints..." << std::endl;

	svr.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
		res.set_content(_page_header, _page_header_len, "text/html");
	});
	svr.Get("/favicon", [&](const httplib::Request& req, httplib::Response& res) {
		res.set_content((const char*)_page_favicon, _page_favicon_len, "image/png");
	});
	svr.Get("/stop", [&](const httplib::Request& req, httplib::Response& res) {
		res.status = httplib::StatusCode::Accepted_202;
		svr.stop();
	});
	svr.Get("/restart", [&](const httplib::Request& req, httplib::Response& res) {
		res.status = httplib::StatusCode::OK_200;

		for(auto& proc : procs) {
			proc->stop();
			while(proc->is_running()) std::this_thread::sleep_for(std::chrono::milliseconds(20));
			proc.reset();
		}

		procs.clear();
		log.clear();
		start_procs();
	});
	svr.Get("/read", [&](const httplib::Request& req, httplib::Response& res) {
		if (!req.has_param("index")) {
			res.status = httplib::StatusCode::BadRequest_400;
			return;
		}
		const auto val = req.get_param_value("index");
		const uint64_t off = std::strtoull(val.c_str(), nullptr, 10);

		const std::string it_json = log.get_index_autofix(off);

		res.set_content(it_json, "application/json");
	});
	svr.set_exception_handler([](const httplib::Request& req, httplib::Response& res, std::exception_ptr ep) {
		auto fmt = "<h1>Error 500</h1><p>%s</p>";
		char buf[BUFSIZ];
		
		try { std::rethrow_exception(ep); }
		catch (const std::exception& e) { snprintf(buf, sizeof(buf), fmt, e.what()); }
		catch (...) { snprintf(buf, sizeof(buf), fmt, "Unknown Exception"); }

		res.set_content(buf, "text/html");
		res.status = httplib::StatusCode::InternalServerError_500;
	});

	start_procs();

	std::cout << "Starting endpoint..." << std::endl;

	if (!svr.listen("0.0.0.0", port)) {
		std::cout << "Could not start endpoint at port " << port << "." << std::endl;
	}

	return 0;
}