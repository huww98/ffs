#include "directory.h"

#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "user.h"

using namespace std;

directoryEntry::directoryEntry(string name, blockNum_t blockNum)
    : blockNum(blockNum)
{
    auto maxSize = sizeof(this->name) - 1;
    if (name.size() > maxSize)
        throw out_of_range("name too long.");

    name.copy(this->name, maxSize);
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
    if (!file.metadata().isDirectory())
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
    streampos writePos;
    bool foundPos = false;
    while (stream >> readEntry)
    {
        if (readEntry.isInUse)
        {
            if (strcmp(readEntry.name, entry.name) == 0)
                throw runtime_error("directory entry with the same name has exist.");
        }
        else
        {
            writePos = stream.tellg() - streamoff(directoryEntrySize);
            foundPos = true;
        }
    }

    stream.clear();
    if (foundPos)
        stream.seekp(writePos);
    else
        stream.seekp(0, ios::end);
    stream << entry;
}

blockNum_t directory::findEntry(string name)
{
    streampos pos;
    return this->findEntry(name, pos);
}

blockNum_t directory::findEntry(string name, streampos &foundPosition)
{
    auto stream = _file.openStream();

    directoryEntry readEntry;
    while (stream >> readEntry)
    {
        if (!readEntry.isInUse)
        {
            continue;
        }
        if (readEntry.name == name)
        {
            foundPosition = stream.tellg() - streamoff(directoryEntrySize);
            return readEntry.blockNum;
        }
    }

    stringstream errMsg;
    errMsg << "Directory entry " << quoted(name) << " not found.";
    throw out_of_range(errMsg.str());
}

string directory::findEntryName(blockNum_t n)
{
    auto stream = _file.openStream();

    directoryEntry readEntry;
    while (stream >> readEntry)
    {
        if (!readEntry.isInUse)
        {
            continue;
        }
        if (readEntry.blockNum == n)
        {
            return readEntry.name;
        }
    }

    stringstream errMsg;
    errMsg << "Directory entry matching the block number not found.";
    throw out_of_range(errMsg.str());
}

vector<directoryEntry> directory::allEntries()
{
    auto stream = _file.openStream();

    directoryEntry readEntry;
    vector<directoryEntry> entries;
    while (stream >> readEntry)
    {
        if (!readEntry.isInUse)
        {
            continue;
        }
        entries.push_back(readEntry);
    }

    return entries;
}

void directory::removeEntry(streampos pos)
{
    auto stream = _file.openStream();

    directoryEntry removedEntry;
    removedEntry.isInUse = false;
    stream.seekp(pos);
    stream << removedEntry;
}
