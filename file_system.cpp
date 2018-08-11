#include "file_system.h"

#include <filesystem>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>

#include "user.h"
#include "directory.h"

using namespace std;
namespace fs = std::filesystem;

constexpr blockNum_t rootDirBlockNum = 0;
const fs::path pwdFilePath = "./pwd";

void initPwd()
{
    ofstream(pwdFilePath) << rootDirBlockNum;
}

int init(int argc, char const *argv[])
{
    initUser();

    fs::create_directory(blocksDirPath);

    auto dir = directory::create(rootDirBlockNum);
    dir.addEntry(directoryEntry::buildParentEntry(rootDirBlockNum));

    initPwd();

    return 0;
}

const fs::path rootDirBlockFilePath = file::path(rootDirBlockNum);

bool hasInit()
{
    return hasUserInit() &&
           fs::exists(blocksDirPath) &&
           fs::exists(rootDirBlockFilePath) &&
           fs::exists(pwdFilePath);
}

blockNum_t getPwd()
{
    blockNum_t n;
    ifstream(pwdFilePath) >> n;
    return n;
}

blockNum_t getBlockNumberByPath(fs::path p)
{
    p = p.lexically_normal();
    if (p == "." || p.empty())
        return getPwd();

    blockNum_t n = p.has_root_directory() ? rootDirBlockNum : getPwd();

    for (auto &e : p.relative_path())
    {
        auto dir = directory::open(n);
        n = dir.findEntry(e.string());
    }
    return n;
}

blockNum_t findEmptyBlock()
{
    vector<bool> bitmap;
    for (auto &p : fs::directory_iterator(blocksDirPath))
    {
        blockNum_t blockNum;
        stringstream filename(p.path().filename());
        filename >> hex >> blockNum;

        if (blockNum + 1 > bitmap.size())
            bitmap.resize(blockNum + 1, false);

        bitmap[blockNum] = true;
    }

    for (size_t i = 0; i < bitmap.size(); i++)
    {
        if (bitmap[i] == false)
            return i;
    }
    return bitmap.size();
}

int cd(int argc, char const *argv[])
{
    if (argc == 0)
    {
        initPwd();
        return 0;
    }

    blockNum_t pwd = getBlockNumberByPath(argv[0]);

    directory::open(pwd); // check pwd indeed is a directory
    ofstream(pwdFilePath) << pwd;

    return 0;
}

int mkdir(int argc, char const *argv[])
{
    if (argc == 0)
    {
        throw runtime_error("缺少参数：要创建的目录。");
    }

    fs::path p = argv[0];
    blockNum_t parentBlockNum = getBlockNumberByPath(p.parent_path());

    auto parentDir = directory::open(parentBlockNum);
    auto newDirBlockNum = findEmptyBlock();
    auto dir = directory::create(newDirBlockNum);
    dir.addEntry(directoryEntry::buildParentEntry(parentBlockNum));
    parentDir.addEntry(directoryEntry(p.filename(), newDirBlockNum));

    return 0;
}

int pwd(int argc, char const *argv[])
{
    vector<fs::path> pathSegments;
    blockNum_t n = getPwd();
    while (n != rootDirBlockNum)
    {
        auto dir = directory::open(n);
        auto parentDirBlockNum = dir.findEntry(parentDirEntryName);
        auto parentDir = directory::open(parentDirBlockNum);
        pathSegments.push_back(parentDir.findEntryName(n));
        n = parentDirBlockNum;
    }

    fs::path pwd = "/";
    for (auto it = pathSegments.rbegin(); it != pathSegments.rend(); ++it)
    {
        pwd /= *it;
    }
    cout << pwd.string() << endl;
    return 0;
}

int ls(int argc, char const *argv[])
{
    auto dirBlockNum = getBlockNumberByPath(argc > 0 ? argv[0] : "");
    auto dir = directory::open(dirBlockNum);

    for (auto &e : dir.allEntries())
    {
        auto file = file::open(e.blockNum);
        auto &metadata = file.metadata();
        auto permission = metadata.permission();

        cout << (metadata.isDirectory() ? "d" : "-")
             << (permission.user().read() ? "r" : "-")
             << (permission.user().write() ? "w" : "-")
             << (permission.other().read() ? "r" : "-")
             << (permission.other().write() ? "w" : "-")
             << " " << hex << setw(8) << setfill('0') << e.blockNum
             << " " << dec << setw(10) << setfill(' ') << file.size()
             << " " << setw(8) << getUser(file.metadata().ownerUID()).name
             << " " << e.name;
        if (metadata.isDirectory())
            cout << "/";
        cout << endl;
    }

    return 0;
}
