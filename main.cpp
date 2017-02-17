#include <iostream>
#include <unordered_map>
#include <list>
#include <memory>

enum ItemType
{
    IT_DIR,
    IT_FILE
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
        
        std::shared_ptr<Dir> localDir = nullptr;
        std::string name = ResolvePath(path, localDir);
        
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
    
    void cd(std::string path)
    {
        
        std::shared_ptr<Dir> localDir = nullptr;
        std::string name = ResolvePath(path, localDir);
        
        std::unordered_map<std::string, std::shared_ptr<Item> >::iterator it = localDir->itemList_.find(name);
        if (it  == localDir->itemList_.end() ) 
        {
            std::cout << "cd: " << name << ". No such file or directory\n";
        } 
        else 
        {
            if (it->second != nullptr) 
            {
                if (it->second->itTyp_ == ItemType::IT_DIR)
                {
                    currDir_ = std::static_pointer_cast<Dir>(it->second);
                    currDirPath_ = path;
                    
                    if (name == "..")
                    {
                        size_t currSize = currDirPath_.size();
                        size_t lastNameSize = currDirPath_.find_last_of("/", currSize);
                        currDirPath_.resize(currSize - lastNameSize);
                    }
                    else if (name != ".")
                    {
                        currDirPath_ += name + "/";
                    }
                }
                else
                {
                    std::cout << "cd: " << name << ". Not a directory\n";
                }
            }
        }
    }
    
    void touch(std::string path)
    {
        
        std::shared_ptr<Dir> localDir = nullptr;
        std::string name = ResolvePath(path, localDir);
        
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
    
private:
    
    std::string ResolvePath(std::string &path, std::shared_ptr<Dir> &localDir)
    {
        // pesky no-path case
        if (path.size() == 0)
        {
            
        }
        
        
        std::string name(path);
        localDir = currDir_;
        path = currDirPath_;
        return name;
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
    disk0->ls();
    
    disk0->cd("base1");
    disk0->mkdir("derivata0");
    
    disk0->touch("derivata1File");
    disk0->touch("derivata1File");
    
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

    
    return 0;
}



/*

//////////////////////////////////////////
class Item
{
public:
    
    Item() {}
    Item(std::string name, Item* parentDir) : name_(name), parentDir_(parentDir) {}
    
    inline std::string Name() {return name_; }
    
    virtual inline std::shared_ptr<std::list<std::string> > GetFileContent() {return NULL; }
    
    virtual inline std::shared_ptr<std::unordered_map<std::string, Item* > > GetDirContent() {return NULL; }
    
    inline Item* ParentDir() {return parentDir_; }
    
private:
    
    std::string name_;
    Item* parentDir_;
};
//////////////////////////////////////////
class Dir : public Item
{
public:
    
    Dir(std::string name, Item* parentDir = NULL) : Item(name, parentDir)
    {
        // I do not know why I need 2 lines for this
        std::shared_ptr<std::unordered_map<std::string, Item* > > content(new std::unordered_map<std::string, Item* >() );
        content_ = content;
    }
    
    virtual inline std::shared_ptr<std::unordered_map<std::string, Item* > > GetDirContent() override {return content_; }
    
private:
    
    std::shared_ptr<std::unordered_map<std::string, Item* > > content_;   
};
//////////////////////////////////////////
class File : public Item
{
    public:
        
    File(std::string name, Item* parentDir) : Item(name, parentDir)
    {
        // I do not know why I need 2 lines for this
        std::shared_ptr<std::list<std::string> > content(new std::list<std::string>() );
        content_ = content;
    }
    
    virtual inline std::shared_ptr<std::list<std::string> > GetFileContent() override {return content_; }
    
private:
    
    std::shared_ptr<std::list<std::string> > content_;
};
//////////////////////////////////////////
class Shell
{
public:
    Shell()
    {
        fileSystem_["/"] = Dir("/");
        currentDirPtr_ = &fileSystem_["/"];
        currentDirName_ = "/";
    }
    
    inline void pwd() { std::cout << currentDirName_ << std::endl; }
    
    void mkdir(std::string path) 
    {
        
        
        std::string name = ExtractName(path);
        std::string root = ExtractRoot(path);
        std::string fullPath = root + name + "/";
        
        std::cout << root;
        
        
        
        fileSystem_[fullPath] = Dir(name, currentDirPtr_);
        
        Item* puzzolo = &fileSystem_[fullPath];
        
        (*fileSystem_[root].GetDirContent())["name"] = puzzolo;
    }
    
private:
    
    // handle all the exceptions and provide the next name
    std::string ExtractName(std::string path)
    {
        // TODO handle exceptions
        // TODO handle multiple names
        return path;
    }
    // handle all the exceptions and provide the root
    std::string ExtractRoot(std::string path)
    {
        // TODO handle exceptions
        // TODO handle multiple names
        return currentDirName_;
    }
    
    // variables
    std::unordered_map<std::string, Item > fileSystem_;
    Item* currentDirPtr_;
    std::string currentDirName_;
};



int main()
{
    std::unique_ptr<Shell> pc(new Shell);
    pc->mkdir("puppa");

    
    
    return 0;
}






class Item
{
    
public:
    
    Item() {}
    Item(std::string name) : name_(name) {}
    
    inline std::string Name() {return name_; }
    
    virtual inline std::shared_ptr<std::list<std::string> > GetFileContent() {return NULL; }
    
    virtual inline std::shared_ptr<std::unordered_map<std::string, Item> > GetDirContent() {return NULL; }
    
private:
    
    std::string name_;
};

class Dir : public Item
{
    
public:
    
    Dir(std::string name) : Item(name)
    {
        // I do not know why I need to lines for this
        std::shared_ptr<std::unordered_map<std::string, Item> > content(new std::unordered_map<std::string, Item>() );
        content_ = content;
    }
    
    virtual inline std::shared_ptr<std::unordered_map<std::string, Item> > GetDirContent() override {return content_; }
    
private:
    
    std::shared_ptr<std::unordered_map<std::string, Item> > content_;
};

class File : public Item
{
    
    public:
        
    File(std::string name) : Item(name)
    {
        // I do not know why I need to lines for this
        std::shared_ptr<std::list<std::string> > content(new std::list<std::string>() );
        content_ = content;
    }
    
    virtual inline std::shared_ptr<std::list<std::string> > GetFileContent() override {return content_; }
    
private:
    
    std::shared_ptr<std::list<std::string> > content_;
};


class Shell
{
public:
    Shell() : currentPath_("/") { fileSystem_["/"] = Dir("/"); }
    
    inline void pwd() { std::cout << currentPath_ << std::endl; }

    void mkdir(std::string localPath)
    {
        // resolve the empty case
        if (path.empty() ) {std::cout << "mkdir: missing operand\n"; return; }
        
        std::string fullName = currentDir_ + newDir + "/";
        
        // No "/" or existing folders please
        if (!newDir.find_first_of("/") || fileSystem_.find(fullName) ) { std::cout << "mkdir: cannot create directory '" << newDir << "': File exists\n"; return; }
        
        // push the new entry
        fileSystem_[fullName] = Dir(newDir);
    }


    // limited to 1 word, no spaces
    void cd(std::string path)
    {
        // solve the empty case
        if (path.empty() ) {currentPath_ = "/"; return; }
        
        // skip the initial empty spaces
        size_t ii = path.find_first_not_of(' ');
        
        // nothing in the path
        if (ii == -1) {currentPath_ = "/"; return; }
        
        if (path[ii] == '/' ) {currentPath_ = "/"; }
        
        // skip the ////
        size_t ii = path.find_first_not_of('/');
        
        size_t jj = ii;
        for (; jj < path.size(); ++jj)
        {
            if (path[jj] == ' ' || path[jj] == '/') { break; }
        }
    
        str::string newKey = path.substr(ii, jj - ii);
        
        if (newKey.empty() ) {return; }
        
        switch (newKey)
            case "." :
            {
                return;
            }
            case ".." :
            {
                
                
                break;
            }
        }
        
        
    }

private:


    
    // variables
    std::unordered_map<std::string, Item> fileSystem_;
    // this ends always with a "/"
    std::string currentPath_;
};


int main(int argc, char **argv) {
    
   std::unique_ptr<Shell> disk0(new Shell());

    //disk0->pwd();
    //disk0->mkdir("pio");
    

    

    
    return 0;
}




 * 
 *     // Ok, I have to consider more ill-formed paths... but it is boring
    std::string globalPath(std::string path)
    {
        // solve the empty case
        if (path.empty()) {currentPath_ = "/"; return; }
        
        // skip the initial empty spaces
        size_t ii = path.find_first_not_of(' ');
        
        // nothing in the path
        if (ii == -1) {currentPath_ = "/"; return; }
        
        std::string newPath;
        
        if (path[ii] == '/')
        {
            newPath = "/";
            ii = path.find_first_not_of('/');
            // nothing in the path
            if (ii == -1) {currentPath_ = "/"; return; }
        }
        else
        {
            newPath = currentPath_;
        }
        
        std::string newItem;
        for (size_t jj = ii; jj < path.size(); ++jj)
        {
            if (path[jj] == ' ') { break; }
            if (path[jj] == '/')
            {
                if (path[ii] != '.')
                {
                    newItem = path.substr(ii-1, jj-ii+1);
                }
                else
                {
                    if (jj != ii+1)
                    {
                        
                    }
                }
                
                jj = path.find_first_not_of('/');
                ii = jj;
            }
            
            
        }
    }
    */
