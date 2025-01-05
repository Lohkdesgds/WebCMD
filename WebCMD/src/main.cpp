#include <history.h>
#include <page_sources.h>
#include <argument_handler.h>
#include <HTTPLIB/httplib.h>
#include <Process/process.h>

#include <iostream>

int main(int argc, char* argv[])
{
	if (argc == 1) {
		std::cout << 
			"Do the calls in this format:\n"
			"app.exe [--<property> <value> ...] [[$ <command> [<args> ...]] ...]\n\n"
			"Example:\n"
			"app.exe --port 8080 $ /bin/sh -c \"echo hi\" $ java --version $ echo Dollar sign \\\\\\$\\\\\\$\\!" << std::endl;

		return 0;
	}

	const auto args = parse_args(argc, argv);

#ifdef _DEBUG
	std::cout << "Got properties:" << std::endl;
	for(const auto& prop : args.props)
		std::cout << prop.first << " -> " << prop.second << std::endl;

	std::cout << "Got commands:" << std::endl;
	for(const auto& cmd : args.procs) {
		std::cout << "# " << cmd.beg << std::endl;
		for (const auto& arg : cmd.args)
			std::cout << "# -> " << arg << std::endl;
	}
#endif

	httplib::Server svr;
	std::vector<std::unique_ptr<Lunaris::process_async>> procs;
	std::thread procs_listener;
	std::mutex procs_shared_cout;
	bool procs_listener_keep_alive = false;
	History log;
	long port = 80;

	if (const auto val = args.prop("port"); val.size()) port = std::atoi(val.c_str());

	const auto kill_listener = [&] {
		if (!procs_listener_keep_alive) return;
		procs_listener_keep_alive = false;
		procs_listener.join();		
	};

	const auto kill_all_procs_safe = [&] {
		for(auto& proc : procs) {
			proc->stop();
			while(proc->is_running()) std::this_thread::sleep_for(std::chrono::milliseconds(20));
			proc.reset();
		}
	};

	const auto start_procs = [&]{
		kill_listener();

		log.append_log(TableLine{ 0, "This is the beginning of the application." });
		log.append_log(TableLine{ 0, "The number of processes are: " + std::to_string(args.procs.size())});
		log.append_log(TableLine{ 0, "Starting them soon."});
		
		uint64_t c = 1;
		for (const auto& proc : args.procs) {
			std::cout << "Initializing \"" << proc.beg << "\"." << std::endl;
			log.append_log(TableLine{ c, "[!] Process initializing."});
			procs.push_back(std::make_unique<Lunaris::process_async>(proc.beg, proc.args, 
				[&, id = c++](auto&, const std::string& ou) {
					log.append_log(TableLine{ static_cast<uint64_t>(id), ou });
					std::lock_guard<std::mutex> l(procs_shared_cout);
					std::cout << "$#" << id << ": " << ou << std::endl;
				}
			));
		}
		
		procs_listener_keep_alive = true;
		procs_listener = std::thread([&] {
			auto stat = std::unique_ptr<bool[]>(new bool[procs.size()]);
			for(size_t p = 0; p < procs.size(); ++p) stat[p] = false;

			while(procs_listener_keep_alive) {
				for(size_t p = 0; p < procs.size() && procs_listener_keep_alive; ++p) {
					const auto& proc = procs[p];
					if (!proc->is_running() && !stat[p]) {
						stat[p] = true;
						log.append_log(TableLine{ p + 1, "[!] Process terminated."});
					}
					
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}
		});

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

		kill_all_procs_safe();

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

	std::cout << "Starting endpoint..." << std::endl;
	start_procs();


	if (!svr.listen("0.0.0.0", port)) {
		std::cout << "Could not start endpoint at port " << port << "." << std::endl;
	}

	{
		std::lock_guard<std::mutex> l(procs_shared_cout);
		std::cout << "Ending app (forced after 10 sec)" << std::endl;
	}

	kill_listener();
	for(auto& proc : procs) proc->stop();

	for(const auto noww = std::chrono::system_clock::now(); std::chrono::system_clock::now() < noww + std::chrono::seconds(10);)
	{
		size_t stopped_count = 0;
		for(auto& proc : procs) {
			if (!proc->is_running()) ++stopped_count;
		}

		if (stopped_count == procs.size()) {
			std::lock_guard<std::mutex> l(procs_shared_cout);
			std::cout << "Ended gracefully." << std::endl;
			return 0;
		}
	}

	{
		std::lock_guard<std::mutex> l(procs_shared_cout);
		std::cout << "Ending FORCED." << std::endl;
	}

	return 1;
}