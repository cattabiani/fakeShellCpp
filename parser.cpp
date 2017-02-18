#include <string>

#include "parser.h"

std::list<std::string> Utils::Parser(std::string &fullPath, char delimiter, char special, bool onlyTheFirst)
{
    size_t ii = 0;
    std::list<std::string> out;
    // pesky empty case
    size_t pathLength = fullPath.size();
    if (pathLength == 0) { return out; }

    size_t ii_old = 0;
    size_t ii_end = 0;
    while (ii != -1)
    {
        ii = fullPath.find_first_not_of(delimiter,ii);
        if (ii == -1) {break; }
        ii_old = ii;
        while (ii != -1)
        {
            ii = fullPath.find_first_of(delimiter,ii);
            if (fullPath[ii-1] != special) {break; }
            ++ii;
        }
        ii_end =  - ii_old + ((ii == -1) ? fullPath.size() : ii);
        out.push_back(fullPath.substr(ii_old,ii_end));

        if (onlyTheFirst) {fullPath = fullPath.substr(ii_end, fullPath.size()); break; }
    }
    
    return out;
}