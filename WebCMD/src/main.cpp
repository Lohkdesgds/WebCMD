#include <history.h>
#include <page_sources.h>
#include <HTTPLIB/httplib.h>
#include <Process/process.h>

#include <iostream>

int main(int argc, char* argv[])
{
	httplib::Server svr;
	std::vector<std::unique_ptr<Lunaris::process_async>> procs;
	History log;

	log.append_log(TableLine{ 0, "This is the beginning of the application." });
	log.append_log(TableLine{ 0, "The number of arguments are: " + std::to_string(argc - 1)});
	log.append_log(TableLine{ 0, "Starting them soon."});
	
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

	for (int p = 1; p < argc; ++p) {
		procs.push_back(std::make_unique<Lunaris::process_async>(argv[p], [&, idcpy = p](auto&, const std::string& ou) {
			log.append_log(TableLine{ static_cast<uint64_t>(idcpy), ou });
		}));
	}

#ifdef _DEBUG
	svr.listen("0.0.0.0", 963);
#else
	svr.listen("0.0.0.0", 80);
#endif

	return 0;
}