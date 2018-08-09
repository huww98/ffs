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
    decltype(filePermission().data()) permissionData;
    int8_t reversed = 0;
    ffsuid_t ownerUID;

    static fileMetadataPresistent build(const fileMetadata &metadata)
    {
        fileMetadataPresistent p;
        p.permissionData = metadata.permission().data();
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
