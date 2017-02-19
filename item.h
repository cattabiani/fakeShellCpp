#ifndef ITEM_H
#define ITEM_H

#include <unordered_map>
#include <memory>


enum ItemType
{
    IT_DIR = 0,
    IT_FILE = 1
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
    Dir(std::shared_ptr<Dir> parentDir) : Item(ItemType::IT_DIR) { itemList_[".."] = parentDir; }
    ~Dir() { itemList_.clear(); }
    //public variables
    std::unordered_map<std::string, std::shared_ptr<Item> > itemList_;
};
#endif 