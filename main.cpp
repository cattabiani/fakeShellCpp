#include <iostream>
#include <memory>
#include <list>
#include <string>

#include "parser.h"
#include "fileSystem.h"

int main()
{
    
    auto disk0 = std::make_shared<FileSystem>();
    
    // this escape mechanism can change
    std::string command("keepGoing");
    while (command != "exit")
    {
        std::string fullCommand;
        std::getline (std::cin,fullCommand);
        
        std::list<std::string> commandList = Utils::Parser(fullCommand, ' ', true, true);
        if (commandList.size() == 0) { command.clear(); }
        else { command = commandList.front(); }
        
        if (command == "ls") { disk0->ls(); }
        else if (command == "pwd") { disk0->pwd(); }
        else if (command == "cd")     { disk0->cd(fullCommand); }
        else if (command == "mkdir") { disk0->mkdir(fullCommand); }
        else if (command == "touch") { disk0->touch(fullCommand); }
    }
    return 0;
}