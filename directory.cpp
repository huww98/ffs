#include "directory.h"

#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "user.h"

using namespace std;

directoryEntry directoryEntry::buildParentEntry(blockNum_t blockNum)
{
    directoryEntry e;
    strcpy(e.name, "..");
    e.blockNum = blockNum;
    return e;
}

directory directory::create(blockNum_t blockNum)
{
    fileMetadata metadata(currentUser().uid, true);
    auto file = file::create(metadata, blockNum);
    return directory(file);
}

directory directory::open(blockNum_t blockNum)
{
    auto file = file::open(blockNum);
    if(!file.metadata().isDirectory())
    {
        throw not_a_directory();
    }
    return directory(file);
}

ostream &operator<<(ostream &output, const directoryEntry &entry)
{
    return output.write(reinterpret_cast<const char *>(&entry), directoryEntrySize);
}

istream &operator>>(istream &input, directoryEntry &entry)
{
    return input.read(reinterpret_cast<char *>(&entry), directoryEntrySize);
}

void directory::addEntry(directoryEntry entry)
{
    auto stream = _file.openStream();

    directoryEntry readEntry;
    while(stream >> readEntry)
    {
        if(!readEntry.isInUse)
        {
            stream.seekp(stream.tellg() - streamoff(directoryEntrySize));
            stream << entry;
            return;
        }
    }

    stream.clear();
    stream.seekp(0, ios::end);
    stream << entry;
}

blockNum_t directory::findEntry(string name)
{
    auto stream = _file.openStream();

    directoryEntry readEntry;
    while (stream >> readEntry)
    {
        if (!readEntry.isInUse)
        {
            continue;
        }
        if(readEntry.name == name)
        {
            return readEntry.blockNum;
        }
    }

    stringstream errMsg;
    errMsg << "Directory entry " << quoted(name) << " not found.";
    throw out_of_range(errMsg.str());
}
