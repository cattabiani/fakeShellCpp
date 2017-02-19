#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <memory>
#include <list>

#include "item.h"
#include "errCodes.h"

class FileSystem
{
public:
    FileSystem();
    
    std::string pwd() const;
    
    std::string ls() const;
    
    // wrapper
    std::string cd(std::string fullPath);
    
    // wrapper, "name" is not empty only in case of errors
    std::list<std::string> mkdir(std::string fullPath) 
    { 
        std::list<std::string> errList = mkItem(fullPath, ItemType::IT_DIR); 
        for (auto ierr : errList) {ierr = "mkdir: " + ierr; }
        return errList;
    };
    
    // wrapper
    std::list<std::string> touch(std::string fullPath) 
    { 
        std::list<std::string> errList = mkItem(fullPath, ItemType::IT_FILE); 
        for (auto ierr : errList) {ierr = "touch: " + ierr; }
        return errList;
    };
    
private:
    
    ERRCODES cd(std::string name, std::shared_ptr<Dir> &localDir, std::list<std::string> &localDirPath_)  const;
    
    ERRCODES mkItem(std::string name, std::shared_ptr<Dir> localDir, std::list<std::string> localDirPath_, ItemType itemType);
    
    // wrapper
    std::list<std::string> mkItem(std::string fullPath, ItemType itemType);
    
    // --- variables --- //
    std::shared_ptr<Dir> currDir_;
    std::list<std::string> currDirPath_;
    std::shared_ptr<Dir> root_;
};
#endif 