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
        
        if (command == "ls") { std::cout << disk0->ls() << std::endl; }
        else if (command == "pwd") { std::cout << disk0->pwd() << std::endl; }
        else if (command == "cd") { std::cout << disk0->cd(fullCommand); }
        else if (command == "mkdir") 
        { 
            std::list<std::string> errList = disk0->mkdir(fullCommand);
            for (auto err : errList) { std::cout << err << std::endl; } 
        }
        else if (command == "touch")
        { 
            std::list<std::string> errList = disk0->touch(fullCommand);
            for (auto err : errList) { std::cout << err << std::endl; } 
        }
    }
    return 0;
}