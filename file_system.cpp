#include "file_system.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "user.h"
#include "directory.h"

using namespace std;
namespace fs = std::filesystem;

const blockNum_t rootDirBlockNum = 0;

int init(int argc, char const *argv[])
{
    initUser();
    auto dir = directory::create(rootDirBlockNum);
    dir.addEntry(directoryEntry::buildParentEntry(rootDirBlockNum));

    return 0;
}

const fs::path rootDirBlockFilePath = file::path(rootDirBlockNum);

bool hasInit()
{
    return hasUserInit() && fs::exists(rootDirBlockFilePath);
}
