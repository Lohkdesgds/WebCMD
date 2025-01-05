#include <argument_handler.h>
#include <stdexcept>

#ifdef _DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

std::string argument_parsed::prop(const std::string& p) const
{
    if (const auto it = props.find(p); it != props.end()) return it->second;
    return {};
}

argument_parsed parse_args(int argc, char* argv[])
{
    if (argc <= 1) return {};

    argument_parsed res;
    bool only_cmd_from_now_on = false; 


    for(int p = 1; p < argc; ++p) {
        const char* curr = argv[p];

        DEBUG("\n_ arg: %s", curr);

        if (!only_cmd_from_now_on && strncmp("--", curr, 2) == 0) // property
        {
            DEBUG(" -> got --");

            const char* prop = curr + 2;
            const char* val = (p + 1 >= argc ? "" : argv[++p]);

            DEBUG(" [%s: %s]", prop, val);

            res.props[prop] = val;
        }
        else if (strncmp("$", curr, 1) == 0 && strlen(curr) == 1) {
            only_cmd_from_now_on = true;
            DEBUG(" -> got $ new cmd");
            res.procs.push_back({});
        }
        else if (res.procs.size() > 0) {
            DEBUG(" -> got $ args");
            proc_desc& cmd = res.procs.back();
            std::string filter(curr);
            
            for(size_t p = filter.find("\\$"); p != std::string::npos; p = filter.find("\\$")) {
                DEBUG(" [\\$ @ %zu ret]", p);
                filter.erase(filter.begin() + p);
            }

            if (cmd.beg.empty()) {
                DEBUG(" -- beg: %s", filter.c_str());
                cmd.beg = std::move(filter);
            }
            else {
                DEBUG(" -- plus: %s", filter.c_str());
                cmd.args.push_back(std::move(filter));
            }
        }
        else {
            DEBUG(" ! bad: '%s' has len %zu. exception!\n", curr, strlen(curr));
            throw std::invalid_argument("Invalid arguments, no $ before commands!");
        }
    }
    DEBUG("\n");
    return res;
}