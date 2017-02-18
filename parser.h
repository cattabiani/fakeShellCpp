#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <string>

class Utils
{
public:
    /* Parser parses fullPath considering special cases and the need of only the first element 
     * delimeter: describe where parsing. Mutiple consecutive delimeters are skipped
     * specal: is the special character before the delimeter that invalidate that specific delimeter
     * onlyTheFirst: if true it makes the function return when it finds the first parsed string. 
     *  Thus the return is a list with only one element. Another feature is that in this specific case
     *  the returned string is erased from fullPath (that is the reason for the & )
     */
    static std::list<std::string> Parser(std::string &fullPath, char delimiter, char special = true, bool onlyTheFirst = false);
};
#endif 