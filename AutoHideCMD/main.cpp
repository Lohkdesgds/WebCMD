#define WIN32_LEAN_AND_MEAN
#include <Process/process.h>
#include <HTTPLIB/httplib.h>

#include "refs.h"
#include <mutex>
#include <memory>

class AutoLog {
	std::vector<data> m_tst;
	mutable std::mutex m_mu;
public:
	auto lock_read() { return std::unique_lock<std::mutex>(m_mu); }
	const std::vector<data>& get() const { return m_tst; }
	void push(const std::string& str, uint64_t id = 0) { auto _l = lock_read(); m_tst.push_back({ Ltime(), id, str }); if (m_tst.size() > 200) m_tst.erase(m_tst.begin()); }
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow);

int main(int argc, char* argv[])
{
	httplib::Server svr;
	std::vector<std::unique_ptr<Lunaris::process_async>> procs;
	AutoLog lug;

	lug.push("Loading stuff...");

	lug.push("Processes to run: " + std::to_string(argc - 1));
	if (argc - 1 == 0) {
		lug.push("Please call the app with executables, like 'app.exe tocall.exe tocall2.exe tocall3.exe' so this terminal shows something.");
		lug.push("To stop all processes, click STOP on top. The service will STOP! You'll have to restart manually.");
	}

	svr.Get("/", [&lug](const httplib::Request&, httplib::Response& res) {
		const auto _l = lug.lock_read();
		const std::string buf = html_start + html_middle(lug.get()) + html_end;

		res.set_content(buf, "text/html");
	});

	svr.Get("/norefresh", [&lug](const httplib::Request&, httplib::Response& res) {
		const auto _l = lug.lock_read();
		const std::string buf = html_start_norefresh + html_middle(lug.get()) + html_end;

		res.set_content(buf, "text/html");
	});

	svr.Get("/stop", [&lug, &svr](const httplib::Request&, httplib::Response& res) {
		res.set_content("Server will end soon.", "text/plain");
		svr.stop();
	});


	lug.push("Ready.");

	for (int p = 1; p < argc; ++p) {
		procs.push_back(std::make_unique<Lunaris::process_async>(argv[p], [&, idcpy = p](auto&, const std::string& ou) { lug.push(ou, idcpy); }));
	}

	svr.listen("0.0.0.0", 80);

	return 0;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	const std::string str(pCmdLine);
	
	std::vector<char*> opts;
	std::string buf;
	bool in_dquote = false;

	{
		char* idc = new char[1] {};
		idc[0] = '\0';
		opts.push_back(idc);
	}

	for (auto ch = str.begin(); ch != str.end(); ++ch) {
		const char& cc = *ch;

		if (cc == '\"') {
			in_dquote = !in_dquote;
			continue;
		}

		if (!in_dquote && cc == ' ') {
			//opts.push_back(buf);
			char* cpy = new char[buf.size() + 1]{};
			memcpy_s(cpy, buf.size() + 1, buf.data(), buf.size());
			cpy[buf.size()] = '\0';
			opts.push_back(cpy);
			buf.clear();
		}
		else {
			buf += cc;
		}
	}

	if (buf.size()) {
		char* cpy = new char[buf.size() + 1] {};
		memcpy_s(cpy, buf.size() + 1, buf.data(), buf.size());
		cpy[buf.size()] = '\0';
		opts.push_back(cpy);
	}

	return main(opts.size(), opts.data());
}