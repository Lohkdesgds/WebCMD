#pragma once

#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>

struct proc_desc{
    std::string beg;
    std::vector<std::string> args;
};

struct argument_parsed {
    std::vector<proc_desc> procs;
    std::unordered_map<std::string, std::string> props;

    std::string prop(const std::string&) const;
};

argument_parsed parse_args(int argc, char* argv[]);