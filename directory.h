#if !defined(DIRECTORY_H)
#define DIRECTORY_H

#include "file.h"

#include <string>

const std::string parentDirEntryName = "..";
const std::string currentDirEntryName = ".";

struct directoryEntry
{
    bool isInUse = true;
    std::int8_t reversed = 0;
    char name[250] = {0};
    blockNum_t blockNum;

    directoryEntry(std::string name, blockNum_t blockNum);
    directoryEntry() {}
};

constexpr size_t directoryEntrySize = 256;
static_assert(sizeof(directoryEntry) == directoryEntrySize);

class directory
{
  private:
    file _file;

  public:
    directory(file f) : _file(f) {}
    static directory create(blockNum_t blockNum);
    static directory open(blockNum_t blockNum);
    void addEntry(directoryEntry entry);

    blockNum_t findEntry(std::string name, std::streampos &foundPosition);
    blockNum_t findEntry(std::string name);
    std::string findEntryName(blockNum_t);
    std::vector<directoryEntry> allEntries();
    void removeEntry(std::streampos foundPosition);
};

class not_a_directory : public std::runtime_error
{
    using base = std::runtime_error;

  public:
    not_a_directory() : base("Trying to open a file that is not directory.") {}
};

#endif // DIRECTORY_H
