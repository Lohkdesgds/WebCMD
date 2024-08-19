#ifdef _WIN32

#include <Windows.h>
#include <vector>

extern int main(int, char**);

//#include <string>
//extern bool conv2file(const std::string& src_file, const std::string trg_file);

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR pCmdLine, _In_ int nCmdShow)
{
	//conv2file("src.png", "src.h");

	LPWSTR* szArglist = nullptr;
	int nArgs;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

	std::vector<char*> Arglist;

	for (int a = 0; a < nArgs; ++a) {
		//const auto len = wcslen(szArglist[a]);
		//char* cpy = new char[len];
		size_t required_size{};
		wcstombs_s(&required_size, (char*)nullptr, 0, szArglist[a], 1ULL << 12);

		if (required_size == static_cast<size_t>(-1)) {
			printf("Failed to convert wchar_t argument string to char string.\n");
			return 1;
		}

		char* conv = new char[required_size + 1];
		memset(conv, 0, required_size + 1);

		wcstombs_s(&required_size, conv, required_size + 1, szArglist[a], required_size);

		Arglist.push_back(conv);
	}

	Arglist.push_back(nullptr);

	return main(nArgs, Arglist.data());
}
#endif