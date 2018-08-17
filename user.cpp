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

istream &operator>>(istream &input, user &u)
{
    input >> u.uid >> ws;
    return getline(input, u.name);
}

ostream &operator<<(ostream &output, const user &u)
{
    return output << u.uid << " " << u.name;
}

void doLogin(ffsuid_t uid)
{
    ofstream(currentUserFilePath) << uid;
}

int login(int argc, char *argv[])
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

bool hasUserInit()
{
    return fs::exists(userListFilePath) && fs::exists(currentUserFilePath);
}

void initUser()
{
    if (hasUserInit())
        return;

    ofstream users(userListFilePath);
    users << user{rootUID, rootUserName} << endl;
    int loginUID = rootUID;

    char *systemUser = getenv("LOGNAME");
    if (systemUser != nullptr && systemUser != rootUserName)
    {
        users << user{defaultUserUID, systemUser} << endl;
        loginUID = defaultUserUID;
    }

    doLogin(loginUID);
}

user currentUser()
{
    ffsuid_t uid;
    ifstream(currentUserFilePath) >> uid;
    return getUser(uid);
}

user getUser(ffsuid_t uid)
{
    ifstream users(userListFilePath);
    user u;
    while (users >> u)
    {
        if (u.uid == uid)
            return u;
    }

    stringstream errMsg;
    errMsg << "uid " << uid << " not exists";
    throw out_of_range(errMsg.str());
}

int whoami(int argc, char *argv[])
{
    cout << currentUser().name << endl;
    return 0;
}

int adduser(int argc, char *argv[])
{
    if(argc <= 0)
        throw runtime_error("缺少参数：要添加的用户");

    if(currentUser().uid != rootUID)
        throw runtime_error("only root can add user.");

    fstream users(userListFilePath);
    user u;
    ffsuid_t lastUid = 0;
    while (users >> u)
    {
        if(u.name == argv[0])
            throw runtime_error("user already exists.");
        lastUid = u.uid;
    }

    user newUser;
    newUser.uid = lastUid + 1;
    newUser.name = argv[0];
    users.clear();
    users.seekp(0, ios::end);
    users << newUser << endl;

    return 0;
}
