#if !defined(DIRECTORY_H)
#define DIRECTORY_H

#include "file.h"

struct directoryEntry
{
    bool isInUse = true;
    std::int8_t reversed = 0;
    char name[250] = {0};
    blockNum_t blockNum;

    static directoryEntry buildParentEntry(blockNum_t blockNum);
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
    blockNum_t findEntry(std::string name);
};

#endif // DIRECTORY_H
