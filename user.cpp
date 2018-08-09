#include "user.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <stdlib.h>

using namespace std;
namespace fs = std::filesystem;

const fs::path userListFilePath("./users");
const fs::path currentUserFilePath("./current_user");

const string rootUserName("root");
constexpr int rootUID = 0;
constexpr int defaultUserUID = 1;

namespace user
{

istream &operator>>(istream &input, user& u)
{
    input >> u.uid >> ws;
    return getline(input, u.name);
}

ostream &operator<<(ostream &output, const user& u)
{
    return output << u.uid << " " << u.name;
}

void doLogin(int uid)
{
    ofstream(currentUserFilePath) << uid;
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
    user u;
    while (users >> u)
    {
        if (userName == u.name)
        {
            doLogin(u.uid);
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
    users << user{rootUID, rootUserName} << endl;
    int loginUID = rootUID;

    char *systemUser = getenv("LOGNAME");
    if (systemUser != nullptr && systemUser != rootUserName) {
        users << user{defaultUserUID, systemUser} << endl;
        loginUID = defaultUserUID;
    }

    doLogin(loginUID);
}

user currentUser()
{
    int uid;
    ifstream(currentUserFilePath) >> uid;

    ifstream users(userListFilePath);
    user u;
    while(users >> u)
    {
        if(u.uid == uid)
            return u;
    }

    stringstream errMsg;
    errMsg << "invalid login state, uid " << uid << " not exists";
    throw runtime_error(errMsg.str());
}

int whoami(int argc, char const *argv[])
{
    cout << currentUser().name << endl;
    return 0;
}

} // namespace user
