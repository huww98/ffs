#include "user.h"

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <stdlib.h>

using namespace std;
namespace fs = std::filesystem;

const fs::path userListFilePath("./users");
const fs::path currentUserFilePath("./current_user");

const string rootUserName("root");

namespace user
{
void doLogin(string userName)
{
    ofstream(currentUserFilePath) << userName;
}

int login(int argc, char const *argv[])
{
    if (argc == 0)
    {
        cerr << "ERROR 请指定用户名" << endl;
        return 1;
    }

    string userName(argv[0]);

    ifstream users(userListFilePath);
    string validUser;
    while (getline(users, validUser))
    {
        if (userName == validUser)
        {
            doLogin(userName);
            return 0;
        }
    }

    cerr << "ERROR 未知用户：" << userName << endl;
    return 1;
}

bool hasInit()
{
    return fs::exists(userListFilePath) && fs::exists(currentUserFilePath);
}

void init()
{
    if (hasInit())
        return;

    ofstream users(userListFilePath);
    users << rootUserName << endl;

    auto systemUser = getenv("LOGNAME");
    if (systemUser == nullptr)
        return;

    users << systemUser << endl;
    doLogin(systemUser);
}

std::string currentUser()
{
    ifstream userFile(currentUserFilePath);
    string user;
    getline(userFile, user);
    return user;
}

int whoami(int argc, char const *argv[])
{
    cout << currentUser() << endl;
    return 0;
}

} // namespace user
