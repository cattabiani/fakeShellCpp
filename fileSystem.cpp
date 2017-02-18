#include <iostream>

#include "fileSystem.h"
#include "parser.h"

// ---
// --- FileSystem
// ---
FileSystem::FileSystem()
{
    root_ = std::make_shared<Dir>();
    root_->itemList_["."] = root_;
    root_->itemList_[".."] = nullptr;
    
    currDir_ = root_;
}

// ---
// --- pwd
// ---
void FileSystem::pwd() const
{
    if (currDirPath_.size() == 0) {std::cout << '/'; }
    else { for (auto const &ii : currDirPath_) { std::cout << '/' << ii; } }
    
    std::cout << std::endl;
}

// ---
// --- ls
// ---
void FileSystem::ls() const
{
    for (auto const &ii : currDir_->itemList_)
    {
        std::cout << ii.first << ' ';
    }
    std::cout << std::endl;
}

// ---
// --- cd
// ---
bool FileSystem::cd(std::string name, std::shared_ptr<Dir> &localDir, std::list<std::string> &localDirPath) const
{
    std::unordered_map<std::string, std::shared_ptr<Item> >::iterator elem = localDir->itemList_.find(name);
    
    if (elem  == localDir->itemList_.end() ) 
    { 
        std::cout << "No such file or directory\n"; 
        return true; 
    } 
    
    if (elem->second == nullptr)
    {
        // I do not check to be in the root directory to increase performances
        return false;
    }
    
    if (elem->second->itTyp_ == ItemType::IT_DIR)
    {
        localDir = std::static_pointer_cast<Dir>(elem->second);
        
        if (name == "..")
        {
            localDirPath.pop_back();
        }
        else if (name != ".")
        {
            localDirPath.push_back(name);
        }
        return false;
    }
    else 
    { 
        std::cout << "cd: " << name << ": Not a directory\n"; 
        return true; 
    }
}

// ---
// --- mkItem
// ---
void FileSystem::mkItem(std::string name, const std::shared_ptr<Dir> localDir, const std::list<std::string> localDirPath, ItemType itemType)
{
    // pesky empty name
    if (name.empty() ) {std::cout << ((itemType == ItemType::IT_DIR) ? "mkdir" : "touch") << ": missing operand\n"; return; }
    
    std::unordered_map<std::string, std::shared_ptr<Item> >::iterator elem = localDir->itemList_.find(name);
    
    if (elem  == localDir->itemList_.end() )
    {
        switch (itemType)
        {
            case ItemType::IT_DIR : 
            { 
                auto newDir = std::make_shared<Dir>(); 
                localDir->itemList_[name] = newDir;
                newDir->itemList_["."] = newDir;
                newDir->itemList_[".."] = localDir;
                break; 
                
            }
            case ItemType::IT_FILE : { localDir->itemList_[name] = std::make_shared<File>(); break; }
        }
    }
    else
    {
        switch (itemType)
        {
            case ItemType::IT_DIR : { std::cout << "mkdir: cannot create directory '" << name << "': File exists"; break; }
            // the next feature is different from bash because touch changes also the access time
            case ItemType::IT_FILE : { std::cout << "touch: cannot create directory '" << name << "': File exists"; break; }
        }
    }
}


// ---
// --- cd - wrapper
// ---
void FileSystem::cd(std::string fullPath)
{
    std::list<std::string> pathList = Utils::Parser(fullPath, ' ', '\\', true);
    
    // pesky empty case
    if (pathList.size() == 0)
    {
        currDir_ = root_;
        currDirPath_.clear();
        return;
    }
    
    std::string path = pathList.front();
    std::shared_ptr<Dir> localDir;
    std::list<std::string> localDirPath;
    
    // decide the start
    if (path[0] == '/')
    {
        localDir = root_;
        localDirPath.clear();
    }
    else
    {
        localDir = currDir_;
        localDirPath = currDirPath_;
    }

    std::list<std::string> nameList = Utils::Parser(path, '/');

    // I do not initialize err because cd always overwrites (or breaks)
    bool err;
    for(auto name : nameList)
    {
        err = cd(name, localDir, localDirPath);
        if (err) {return; }
    }

    currDir_ = localDir;
    currDirPath_ = localDirPath;
}

// ---
// --- mkItem - wrapper
// ---
void FileSystem::mkItem(std::string fullPath, ItemType itemType)
{
    std::list<std::string> pathList = Utils::Parser(fullPath, ' ', '\\');
    
    // pesky empty case
    if (pathList.size() == 0) {return; }
    
    for (auto path : pathList)
    {
        std::shared_ptr<Dir> localDir;
        std::list<std::string> localDirPath;
        
        // decide the start
        if (path[0] == '/')
        {
            localDir = root_;
            localDirPath.clear();
        }
        else
        {
            localDir = currDir_;
            localDirPath = currDirPath_;
        }
        
        std::list<std::string> nameList = Utils::Parser(path, '/');
        
        // I do not initialize err because cd always overwrites (or breaks)
        // no auto because I need to skip the last element
        bool err;
        for(std::list<std::string>::const_iterator name = nameList.cbegin(); name != --nameList.cend(); ++name)
        {
            err = cd(*name, localDir, localDirPath);
            if (err) {break; }
        }
        
        if (!err) { mkItem(nameList.back(), localDir, localDirPath, itemType); }
    }
}