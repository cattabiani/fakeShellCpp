#include <iostream>

#include "fileSystem.h"
#include "parser.h"

// ---
// --- FileSystem
// ---
FileSystem::FileSystem()
{
    root_ = std::make_shared<Dir>(nullptr);
    root_->itemList_["."] = root_;
    root_->itemList_[".."] = nullptr;
    
    currDir_ = root_;
}

// ---
// --- pwd
// ---
std::string FileSystem::pwd() const
{
    std::string out;
    if (currDirPath_.size() == 0) {out += '/'; }
    else { for (auto const &ii : currDirPath_) { out += '/' + ii; } }

    return out;
}

// ---
// --- ls
// ---
std::string FileSystem::ls() const
{
    std::string out;
    for (auto const &ii : currDir_->itemList_)
    {
        out += ii.first + ' ';
    }
    
    return out;
}

// ---
// --- cd
// ---
ERRCODES FileSystem::cd(std::string name, std::shared_ptr<Dir> &localDir, std::list<std::string> &localDirPath) const
{
    std::unordered_map<std::string, std::shared_ptr<Item> >::iterator elem = localDir->itemList_.find(name);
    
    if (elem  == localDir->itemList_.end() ) { return ERRCODES::NOITEM; } 
    // We are in the root directory, I do not check to be in the root directory to increase performances
    if (elem->second == nullptr) { return ERRCODES::ALLOK; }
    
    if (elem->second->itTyp_ == ItemType::IT_DIR)
    {
        localDir = std::static_pointer_cast<Dir>(elem->second);
        
        if (name == "..") { localDirPath.pop_back(); }
        else if (name != ".") { localDirPath.push_back(name); }
        return ERRCODES::ALLOK;
    }
    else { return ERRCODES::NOTDIR; }
}

// ---
// --- cd - wrapper
// ---
std::string FileSystem::cd(std::string fullPath)
{
    std::list<std::string> pathList = Utils::Parser(fullPath, ' ', '\\', true);
    
    // pesky empty case
    if (pathList.size() == 0)
    {
        currDir_ = root_;
        currDirPath_.clear();
        return "";
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
    ERRCODES err;
    for(auto name : nameList)
    {
        err = cd(name, localDir, localDirPath);
        if (err != ERRCODES::ALLOK) { return "cd: '" + path  + "': " + ErrCodes::Msg(err) + "\n"; }
    }

    currDir_ = localDir;
    currDirPath_ = localDirPath;
    
    return "";
}

// ---
// --- mkItem
// ---
ERRCODES FileSystem::mkItem(std::string name, const std::shared_ptr<Dir> localDir, const std::list<std::string> localDirPath, ItemType itemType)
{
    // pesky empty name
    if (name.empty() ) {return ERRCODES::NOOPERAND; }
    
    std::unordered_map<std::string, std::shared_ptr<Item> >::iterator elem = localDir->itemList_.find(name);
    
    if (elem  == localDir->itemList_.end() )
    {
        switch (itemType)
        {
            case ItemType::IT_DIR : 
            { 
                auto newDir = std::make_shared<Dir>(localDir); 
                localDir->itemList_[name] = newDir;
                newDir->itemList_["."] = newDir;
                break; 
            }
            case ItemType::IT_FILE : { localDir->itemList_[name] = std::make_shared<File>(); break; }
        }
    }
    else { return ERRCODES::ITEXISTS; }
    return ERRCODES::ALLOK;
}

// ---
// --- mkItem - wrapper
// ---
std::list<std::string> FileSystem::mkItem(std::string fullPath, ItemType itemType)
{
    std::string itemName;
    if (itemType == ItemType::IT_DIR) itemName = "directory";
    else itemName = "directory";
    
    std::list<std::string> errList;
    std::list<std::string> pathList = Utils::Parser(fullPath, ' ', '\\');
    
    // pesky empty case
    if (pathList.size() == 0) 
    {
        errList.push_back(ErrCodes::Msg(ERRCODES::NOOPERAND)); 
        return errList; 
    }
    
    for (auto path : pathList)
    {
        
        // check that I am not creating a file with a dir address
        if (path.back() == '/' && itemType == ItemType::IT_FILE) { errList.push_back("cannot create file ‘" + path  + "': " + ErrCodes::Msg(ERRCODES::NOOPERAND)); continue; }
        
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
        ERRCODES err = ERRCODES::ALLOK;
        for(std::list<std::string>::const_iterator name = nameList.cbegin(); name != --nameList.cend(); ++name)
        {
            err = cd(*name, localDir, localDirPath);
            if (err != ERRCODES::ALLOK) { errList.push_back("cannot create " + itemName + " ‘" + path  + "': " + ErrCodes::Msg(err)); break; }
        }
        
        if (err == ERRCODES::ALLOK) 
        {
            err = mkItem(nameList.back(), localDir, localDirPath, itemType); 
            if (err != ERRCODES::ALLOK) {  errList.push_back("cannot create " + itemName + " ‘" + path  + "': " + ErrCodes::Msg(err)); }
        }
    }

    return errList;
}