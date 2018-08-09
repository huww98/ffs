#include "file.h"

#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

std::filesystem::path file::path(blockNum_t blockNum)
{
    stringstream name;
    name << hex << setw(8) << setfill('0') << blockNum;
    return name.str();
}

struct fileMetadataPresistent
{
    uint8_t attributeData;
    int8_t reversed = 0;
    ffsuid_t ownerUID;

    static fileMetadataPresistent build(const fileMetadata &metadata)
    {
        fileMetadataPresistent p;
        p.attributeData = metadata.attributeData();
        p.ownerUID = metadata.ownerUID();
        return p;
    }
};

constexpr size_t fileMetadataSize = 4;
static_assert(sizeof(fileMetadataPresistent) == fileMetadataSize);

ostream &operator<<(ostream &output, const fileMetadata &metadata)
{
    auto p = fileMetadataPresistent::build(metadata);
    return output.write(reinterpret_cast<char *>(&p), fileMetadataSize);
}

file file::create(fileMetadata metadata, blockNum_t blockNum)
{
    ofstream newBlock(path(blockNum), ios::binary);
    newBlock << metadata;
    return file(metadata, blockNum);
}

fileMetadata::fileMetadata(ffsuid_t ownerUID, bool isDirectory)
    : _permission(_attributeData), _ownerUID(ownerUID)
{
    if(isDirectory)
    {
        _attributeData |= isDirectoryMask;
    }
    _permission.owner().read(true);
    _permission.owner().write(true);
    _permission.all().read(true);
    _permission.all().write(true);
}

fstream file::openStream()
{
    fstream s(path(), ios::binary | ios::in | ios::out);
    s.seekg(fileMetadataSize);
    s.seekp(fileMetadataSize);
    return s;
}
