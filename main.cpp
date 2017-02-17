#include <iostream>
#include <unordered_map>
#include <list>
#include <memory>

enum ItemType
{
    IT_DIR = 0,
    IT_FILE = 1
};

enum Cd1stepReturns
{
    NoFileOrDirectory = 2,
    Acc2File = 3,
    EndPath = 4,
    Acc2Dir = 5
};


// --- Item --- //
class Item
{
public:
    Item(ItemType itTyp) : itTyp_(itTyp) {} 
    const ItemType itTyp_;
};

// --- File --- //
class File : public Item
{
public:
    //public methods
    File(unsigned int arraySize = 0) : Item(ItemType::IT_FILE), data_(new char[arraySize]) {}

    ~File() { delete [] data_; }
    //public variables
    char* data_;
};

// --- Dir --- //
class Dir : public Item
{
public:
    //public methods
    Dir() : Item(ItemType::IT_DIR) {}
    ~Dir() { itemList_.clear(); }
    //public variables
    std::unordered_map<std::string, std::shared_ptr<Item> > itemList_;
};

// --- FileSystem --- //
class FileSystem
{
public:
    FileSystem()
    {
        root_= std::make_shared<Dir>();
        root_->itemList_["."] = root_;
        root_->itemList_[".."] = nullptr;
        
        currDirPath_ = "/";
        currDir_ = root_;
    }
    
    inline void pwd() { std::cout << currDirPath_ << std::endl; }
    
    inline void ls() 
    {
        for (auto const &ii : currDir_->itemList_)
        {
            std::cout << ii.first << ' ';
        }
        std::cout << std::endl;
    }
    
    void mkdir(std::string path)
    {
        std::string localDirPath = currDirPath_;
        std::shared_ptr<Dir> localDir = currDir_;
        std::string name;
        Cd1stepReturns err;
        bool writable = cd(path, localDirPath, localDir, name, err);
        
        if (writable)
        {
            // pesky empty case
            if (name.empty()) {std::cout << "missing file operand!\n"; return; }
            
            if ( localDir->itemList_.find(name) == localDir->itemList_.end() ) 
            {
                std::shared_ptr<Dir> newDir = std::make_shared<Dir>();
                localDir->itemList_[name] = newDir;
                newDir->itemList_[".."] = localDir;
                newDir->itemList_["."] = localDir->itemList_[name];
                
            } 
            else 
            {
                std::cout << "mkdir: cannot create directory '" << name << "': File exists\n";
            }
        }
    }
    
    void cd(std::string path)
    {
        std::string localDirPath = currDirPath_;
        std::shared_ptr<Dir> localDir = currDir_;
        std::string name;
        Cd1stepReturns err;
        cd(path, localDirPath, localDir, name, err);
        
        if (name.empty() || err == Cd1stepReturns::EndPath || err == Cd1stepReturns::Acc2Dir)
        {
            currDirPath_ = localDirPath;
            currDir_ = localDir;
        }
        else
        {
            std::cout << "The remaining name: " << name << " error type " << err << std::endl;
        }
    }


    
    void touch(std::string path)
    {
        
        std::string localDirPath = currDirPath_;
        std::shared_ptr<Dir> localDir = currDir_;
        std::string name;
        Cd1stepReturns err;
        bool writable = cd(path, localDirPath, localDir, name, err);
        
        // pesky empty case
        if (name.empty()) {std::cout << "missing file operand!\n"; return; }
        if (writable)
        {
            if ( localDir->itemList_.find(name) == localDir->itemList_.end() ) 
            {
                std::shared_ptr<File> newFile = std::make_shared<File>();
                localDir->itemList_[name] = newFile;
            } 
            else 
            {
                std::cout << "mkdir: cannot create directory '" << name << "': File exists\n";
            }
        }
    }
    

        // no slashes at the beginning
    std::string ExtractWord(std::string path, size_t &ii)
    {
        ii = path.find_first_not_of("/", ii);
        
        if (ii>= path.size()) { return ""; }
        if (path[ii] == ' ') {ii = path.size(); return ""; }
        
        size_t ii_old = ii;
        ii = path.find_first_of("/", ii_old);
        return path.substr(ii_old, ii-ii_old);

    }
    
private:
    
    bool cd(std::string path, std::string &localDirPath, std::shared_ptr<Dir> &localDir, std::string &name, Cd1stepReturns &err)
    {
        if (path.empty()) 
        {
            name = "";
            err = Cd1stepReturns::EndPath;
            localDir = root_;
            localDirPath = '/';
            
            return false; 
        }
        
        // remove the frontal ///
        if (path[path.size()-1] == '/') 
        {
            size_t ii = path.find_last_not_of('/');
            path = path.substr(0,ii);
        }
        
        
        size_t ii = path.find_first_not_of(" ");
        if (ii == -1) {return false; }
        if (path[ii] == '/')
        {
            localDirPath = "/";
            localDir = root_;
        }
        
        ii = path.find_first_not_of("/",ii);
        if (ii == -1) {return false; }
        
        while (ii<path.size())
        {
            name = ExtractWord(path, ii);
            err = cd1step(name, localDirPath, localDir);
            if (err == Cd1stepReturns::NoFileOrDirectory) 
            {
                if (ii>=path.size()) { return true; }
                else { { std::cout << "No such file or directory\n"; } return false; }
            }
            if (err == Cd1stepReturns::Acc2File) {return false; }
        }       
        return false;
    }
    
    Cd1stepReturns cd1step(std::string name, std::string &path, std::shared_ptr<Dir> &localDir)
    {
        std::unordered_map<std::string, std::shared_ptr<Item> >::iterator it = localDir->itemList_.find(name);
        if (it  == localDir->itemList_.end() ) { return Cd1stepReturns::NoFileOrDirectory; } 
        else 
        {
            if (it->second != nullptr) 
            {
                if (it->second->itTyp_ == ItemType::IT_DIR)
                {
                    localDir = std::static_pointer_cast<Dir>(it->second);
                    
                    if (name == "..")
                    {
                        size_t currSize = path.size();
                        size_t lastNameSize = path.find_last_of("/", currSize);
                        path.resize(currSize - lastNameSize);
                    }
                    else if (name != ".")
                    {
                        path += name + "/";
                    }
                    return Cd1stepReturns::Acc2Dir;
                }
                else { std::cout << "cd: " << name << ": Not a directory\n"; return Cd1stepReturns::Acc2File; }
            }
        }
        return Cd1stepReturns::Acc2Dir;
    }
    
    std::shared_ptr<Dir> currDir_;
    std::string currDirPath_;
    std::shared_ptr<Dir> root_;
};


int main()
{
    
    auto disk0 = std::make_shared<FileSystem>();
    
    disk0->mkdir("base0");
    disk0->mkdir("base1");
    disk0->mkdir("base2base2");
    //disk0->ls();
    
    disk0->cd("base1");
    disk0->mkdir("derivata0");
    
    disk0->touch("derivata1File");
    disk0->cd("..");
    disk0->cd("..");
    disk0->cd("..");
    disk0->cd("..");
    disk0->touch("base1/derivata0/derivata1File");
    disk0->cd("/base1/derivata0");

    
    
    disk0->ls();
    disk0->pwd();
    
        disk0->cd("base0");
    disk0->cd("   /base1////derivata0");
    disk0->cd("");

    

    
    /*
    disk0->ls();
    disk0->pwd();
    
    disk0->cd(".");
    
    
    disk0->cd(".");
    disk0->cd(".");
    disk0->cd(".");
    disk0->cd("..");
    disk0->cd("..");
    
        disk0->cd("..");
    disk0->cd("..");
    
        disk0->ls();
    disk0->pwd();
    
    disk0->cd("base1");
    disk0->cd("derivata0");
    
    std::cin.get();
    
    disk0->cd("");
    
            disk0->ls();
    disk0->pwd();
*/
    
    
    
    return 0;
}



