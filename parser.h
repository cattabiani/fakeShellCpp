#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <string>

class Utils
{
public:
    static std::list<std::string> Parser(std::string &fullPath, char delimiter, char special = true, bool onlyTheFirst = false);
};
#endif 