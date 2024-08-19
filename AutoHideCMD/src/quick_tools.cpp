#include <fstream>
#include <string>

bool conv2file(const std::string& src_file, const std::string trg_file)
{
	std::fstream in(src_file, std::ios::in | std::ios::binary);
	std::fstream ou(trg_file, std::ios::out | std::ios::binary);

	if (!in || !ou || in.bad() || ou.bad()) return false;

	constexpr size_t buf_len = 64;
	unsigned char buf[buf_len]{};

	constexpr char beg_ref[] = "#pragma once\n\nconstexpr unsigned char __file[] = {\n";
	constexpr char end_ref[] = "\n}";
	constexpr char breakline[] = "\n";

	ou.write(beg_ref, sizeof(beg_ref) - 1);

	while (in && !in.eof() && in.good()) {
		in.read((char*)buf, buf_len);
		const auto rread = in.gcount();
		for (auto i = 0; i < rread; ++i) {
			char hex[16]{};
			const auto wr = snprintf(hex, 16, "0x%X,", (int)buf[i]);
			ou.write(hex, wr);
		}
		ou.write(breakline, 1);
	}

	ou.write(end_ref, sizeof(end_ref) - 1);
	return true;
}