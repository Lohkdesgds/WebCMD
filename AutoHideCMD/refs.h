#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

struct data {
	std::string timestamp;
	uint64_t id;
	std::string log;
};

const std::string html_start = "<!DOCTYPE html>\
<html>\
    <head>\
		<meta charset=\"utf-8\">\
		<meta http-equiv=\"refresh\" content=\"1\" >\
		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
		<html lang=\"en\">\
		<title>Virtual Console</title>\
	</head>\
	<body>\
	<a href=\"/norefresh\" style=\"background-color: green; color: white; padding: 6px 20px; text-align: center; border-radius: 10px; margin: 10px 6px\">Auto refresh ON</a>\
	<a href=\"/stop\" style=\"background-color: red; color: white; padding: 6px 20px; text-align: center; border-radius: 10px; margin: 10px 6px\">Stop</a>\
	<table style=\"width: 100%; margin: 16px 0\"><tr style=\"background-color: black; color: white\"><th style=\"width: 140px\">Time</th><th style=\"width: 30px\">ID</th><th style=\"width: auto\">Content</th></tr>";

const std::string html_start_norefresh = "<!DOCTYPE html>\
<html>\
    <head>\
		<meta charset=\"utf-8\">\
		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
		<html lang=\"en\">\
		<title>Virtual Console</title>\
	</head>\
	<body>\
	<a href=\"/\" style=\"background-color: green; color: white; padding: 6px 20px; text-align: center; border-radius: 10px; margin: 10px 6px\">Auto refresh OFF</a>\
	<a href=\"/stop\" style=\"background-color: red; color: white; padding: 6px 20px; text-align: center; border-radius: 10px; margin: 10px 6px\">Stop</a>\
	<table style=\"width: 100%; margin: 16px 0\"><tr style=\"background-color: black; color: white\"><th style=\"width: 140px\">Time</th><th style=\"width: 30px\">ID</th><th style=\"width: auto\">Content</th></tr>";

std::string html_middle(const std::vector<data> lines);

const std::string html_end = "</table></body></html>";

std::string Ltime();