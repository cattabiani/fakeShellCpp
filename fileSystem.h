#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <memory>
#include <list>
#include "item.h"

class FileSystem
{
public:
    FileSystem();
    
    void pwd() const;
    
    void ls() const;
    
    // wrapper
    void cd(std::string fullPath);
    
    // wrapper
    void mkdir(std::string fullPath) { mkItem(fullPath, ItemType::IT_DIR); };
    
    // wrapper
    void touch(std::string fullPath) { mkItem(fullPath, ItemType::IT_FILE); };
    
private:
    
    bool cd(std::string name, std::shared_ptr<Dir> &localDir, std::list<std::string> &localDirPath_)  const;
    
    void mkItem(std::string name, std::shared_ptr<Dir> localDir, std::list<std::string> localDirPath_, ItemType itemType);
    
    // wrapper
    void mkItem(std::string fullPath, ItemType itemType);
    
    // --- variables --- //
    std::shared_ptr<Dir> currDir_;
    std::list<std::string> currDirPath_;
    std::shared_ptr<Dir> root_;
};
#endif 