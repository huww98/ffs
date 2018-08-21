#include "file_system.h"

#include <filesystem>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <getopt.h>

#include "user.h"
#include "directory.h"
#include "mode_change.h"

using namespace std;
namespace fs = std::filesystem;

constexpr blockNum_t rootDirBlockNum = 0;
const fs::path pwdFilePath = "./pwd";

void initPwd()
{
    ofstream(pwdFilePath) << rootDirBlockNum;
}

int init(int argc, char *argv[])
{
    if (hasInit())
        throw runtime_error("无法初始化，该文件夹下已存在文件系统");

    initUser();

    fs::create_directory(blocksDirPath);

    auto dir = directory::create(rootDirBlockNum);
    dir.addEntry(directoryEntry(currentDirEntryName, rootDirBlockNum));
    dir.addEntry(directoryEntry(parentDirEntryName, rootDirBlockNum));

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

int cd(int argc, char *argv[])
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

class missing_argument : public runtime_error
{
  public:
    missing_argument(string msg) : runtime_error("缺少参数：" + msg) {}
};

int mkdir(int argc, char *argv[])
{
    if (argc == 0)
    {
        throw missing_argument("要创建的目录");
    }

    fs::path p = argv[0];
    blockNum_t parentBlockNum = getBlockNumberByPath(p.parent_path());

    auto parentDir = directory::open(parentBlockNum);
    auto u = currentUser();
    parentDir.dirFile().metadata().ensureWrite(u.uid);
    auto newDirBlockNum = findEmptyBlock();
    auto dir = directory::create(newDirBlockNum);
    dir.addEntry(directoryEntry(currentDirEntryName, newDirBlockNum));
    dir.addEntry(directoryEntry(parentDirEntryName, parentBlockNum));
    parentDir.addEntry(directoryEntry(p.filename(), newDirBlockNum));

    return 0;
}

int pwd(int argc, char *argv[])
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

int cmdoptind = 0;
int cmdgetopt(int argc, char *argv[], const char *shortopts)
{
    // getopt expect options start at argv index 1, not 0;
    auto result = getopt(argc + 1, argv - 1, shortopts);
    cmdoptind = optind - 1;
    return result;
}

int ls(int argc, char *argv[])
{
    int opt;
    bool l = false;
    bool nullTerminate = false;
    while ((opt = cmdgetopt(argc, argv, "l0")) != -1)
    {
        switch (opt)
        {
        case 'l':
            l = true;
            break;
        case '0':
            nullTerminate = true;
            break;
        default: /* '?' */
            throw runtime_error("invalid argument");
        }
    }

    auto dirBlockNum = getBlockNumberByPath(cmdoptind < argc ? argv[cmdoptind] : "");
    auto dir = directory::open(dirBlockNum);
    auto u = currentUser();
    dir.dirFile().metadata().ensureRead(u.uid);

    for (auto &e : dir.allEntries())
    {
        auto file = file::open(e.blockNum);
        auto &metadata = file.metadata();
        if (l)
        {
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
            cout << (nullTerminate ? '\0' : '\n');
        }
        else
        {
            cout << e.name;
            if (metadata.isDirectory())
                cout << "/";
            cout << (nullTerminate ? '\0' : '\t');
        }
    }

    if (!l)
        cout << endl;

    return 0;
}

void rmdir(file &dirFile, ffsuid_t currentUid)
{
    auto dir = directory(dirFile);
    for (auto &e : dir.allEntries())
    {
        dirFile.metadata().ensureWrite(currentUid);
        if (e.name == parentDirEntryName || e.name == currentDirEntryName)
            continue;

        auto file = file::open(e.blockNum);
        if (file.metadata().isDirectory())
        {
            rmdir(file, currentUid);
        }
        else
        {
            file.remove();
        }
    }
    dirFile.remove();
}

int rm(int argc, char *argv[])
{
    int opt;
    bool r = false;
    while ((opt = cmdgetopt(argc, argv, "r")) != -1)
    {
        switch (opt)
        {
        case 'r':
            r = true;
            break;
        default: /* '?' */
            throw runtime_error("invalid argument");
        }
    }
    if (cmdoptind >= argc)
    {
        throw missing_argument("要删除的目录");
    }

    auto u = currentUser();

    for (int i = cmdoptind; i < argc; i++)
    {
        fs::path p = argv[i];
        blockNum_t parentN = getBlockNumberByPath(p.parent_path());
        auto parentDir = directory::open(parentN);
        parentDir.dirFile().metadata().ensureWrite(u.uid);
        streampos pos;
        blockNum_t n = parentDir.findEntry(p.filename(), pos);
        if (n == rootDirBlockNum)
            throw runtime_error("根目录不能被删除");

        auto file = file::open(n);
        if (file.metadata().isDirectory())
        {
            if (!r)
            {
                stringstream errMsg;
                errMsg << p << " is a directory, but no -r is specified.";
                throw runtime_error(errMsg.str());
            }
            rmdir(file, u.uid);
        }
        else
        {
            file.remove();
        }
        parentDir.removeEntry(pos);
    }

    return 0;
}

int read(int argc, char *argv[])
{
    if (argc == 0)
    {
        throw missing_argument("要读取的文件");
    }
    blockNum_t n = getBlockNumberByPath(argv[0]);
    auto file = file::open(n);
    if (file.metadata().isDirectory())
        throw runtime_error("cannot read from a directory.");

    auto u = currentUser();
    file.metadata().ensureRead(u.uid);

    auto stream = file.openStream();
    cout << stream.rdbuf();
    return 0;
}

file openOrCreateForWrite(directory &dir, string filename)
{
    auto u = currentUser();
    try
    {
        blockNum_t n = dir.findEntry(filename);
        auto f = file::open(n);
        if (f.metadata().isDirectory())
            throw runtime_error("cannot write to a directory.");
        f.metadata().ensureWrite(u.uid);
        f.truncate();
        return f;
    }
    catch (out_of_range &)
    {
        dir.dirFile().metadata().ensureWrite(u.uid);
        blockNum_t n = findEmptyBlock();
        dir.addEntry(directoryEntry(filename, n));
        return file::create(fileMetadata(currentUser().uid), n);
    }
}

int write(int argc, char *argv[])
{
    if (argc == 0)
    {
        throw missing_argument("要写入的文件");
    }
    fs::path p(argv[0]);
    blockNum_t parentN = getBlockNumberByPath(p.parent_path());
    auto dir = directory::open(parentN);
    auto f = openOrCreateForWrite(dir, p.filename());

    auto stream = f.openStream();
    stream << cin.rdbuf();
    return 0;
}

int chmod(int argc, char *argv[])
{
    if (argc == 0)
    {
        throw missing_argument("要更改的权限");
    }
    modeChanges changes(argv[0]);

    if (argc <= 1)
    {
        throw missing_argument("要更改权限的文件");
    }

    auto u = currentUser();

    for (int i = 1; i < argc; i++)
    {
        fs::path p(argv[i]);
        auto f = file::open(getBlockNumberByPath(p));
        f.metadata().ensureOwnership(u.uid);
        auto permission = f.metadata().permission();
        changes.apply(permission);
        f.saveMetadata();
    }

    return 0;
}

int chown(int argc, char *argv[])
{
    if (argc == 0)
    {
        throw missing_argument("所有者");
    }
    auto newOwner = getUser(argv[0]);

    if (argc <= 1)
    {
        throw missing_argument("要更改所有者的文件");
    }

    auto u = currentUser();

    for (int i = 1; i < argc; i++)
    {
        fs::path p(argv[i]);
        auto f = file::open(getBlockNumberByPath(p));
        f.metadata().ensureOwnership(u.uid);
        f.metadata().ownerUID(newOwner.uid);
        f.saveMetadata();
    }

    return 0;
}
