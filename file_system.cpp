#include "file_system.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

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
    auto dir = directory::create(rootDirBlockNum);
    dir.addEntry(directoryEntry::buildParentEntry(rootDirBlockNum));

    initPwd();

    return 0;
}

const fs::path rootDirBlockFilePath = file::path(rootDirBlockNum);

bool hasInit()
{
    return hasUserInit() && fs::exists(rootDirBlockFilePath) && fs::exists(pwdFilePath);
}

int cd(int argc, char const *argv[])
{
    if(argc == 0)
    {
        initPwd();
        return 0;
    }

    fs::path p(argv[0]);
    p = p.lexically_normal();

    if (p == ".")
        return 0;

    blockNum_t pwd;
    ifstream(pwdFilePath) >> pwd;

    for(auto& e : p)
    {
        if(e == "/")
        {
            pwd = rootDirBlockNum;
            continue;
        }
        auto dir = directory::open(pwd);
        pwd = dir.findEntry(e.string());
    }

    directory::open(pwd); // check pwd indeed is a directory
    ofstream(pwdFilePath) << pwd;

    return 0;
}
