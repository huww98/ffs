#include "file.h"

#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

const fs::path blocksDirPath = "blocks";

fs::path file::path(blockNum_t blockNum)
{
    stringstream name;
    name << hex << setw(8) << setfill('0') << blockNum;
    return blocksDirPath / name.str();
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

istream &operator>>(istream &input, fileMetadataPresistent &metadata)
{
    return input.read(reinterpret_cast<char *>(&metadata), fileMetadataSize);
}

file file::create(fileMetadata metadata, blockNum_t blockNum)
{
    ofstream newBlock(path(blockNum), ios::binary);
    newBlock << metadata;
    return file(metadata, blockNum);
}

file file::open(blockNum_t blockNum)
{
    fileMetadataPresistent metadata;
    ifstream(path(blockNum)) >> metadata;
    return file(metadata, blockNum);
}

fileMetadata::fileMetadata(ffsuid_t ownerUID, bool isDirectory)
    : _ownerUID(ownerUID)
{
    if(isDirectory)
    {
        _attributeData |= isDirectoryMask;
    }
    auto permission = this->permission();
    permission.user().read(true);
    permission.user().write(true);
    permission.other().read(true);
    permission.other().write(true);
}

fileMetadata::fileMetadata(fileMetadataPresistent p)
    : _attributeData(p.attributeData), _ownerUID(p.ownerUID)
{
}

fstream file::openStream()
{
    fstream s(path(), ios::binary | ios::in | ios::out);
    s.seekg(fileMetadataSize);
    s.seekp(fileMetadataSize);

    if(!s)
        throw runtime_error("something wrong when open stream");

    return s;
}

size_t file::size()
{
    return fs::file_size(this->path()) - fileMetadataSize;
}

void file::remove()
{
    fs::remove(this->path());
}
