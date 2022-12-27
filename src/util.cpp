#include "constants.h"
#include "util.h"

std::vector<std::string> split(std::string target, std::string delimiter) {
    std::vector<std::string> v;

    if (!target.empty()) {
        std::string::size_type start = 0;
        while(true) {
            size_t x = target.find(delimiter, start);
            if (x == std::string::npos)
                break;

            v.push_back(target.substr(start, x-start));
            start = x + delimiter.size();
        }

        v.push_back(target.substr(start));
    }
    return v;
}
