#include "refs.h"

std::string html_middle(const std::vector<data> lines)
{
	std::string lstr;

	for (auto i = lines.rbegin(); i != lines.rend(); ++i)
	{
		lstr += "<tr style=\"background-color: #eee; color: black\"\><td>" + i->timestamp + "</td><td>" + std::to_string(i->id) + "</td><td>" + i->log + "</td></th>";
	}

	return lstr;
}


std::string Ltime()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;

	struct tm tmm;
	_localtime64_s(&tmm, &in_time_t);

	ss << std::put_time(&tmm, "%Y-%m-%d %X");
	return ss.str();
}