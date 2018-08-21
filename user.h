#if !defined(USER_H)
#define USER_H

#include <string>

using ffsuid_t = std::uint16_t;

struct user
{
    ffsuid_t uid;
    std::string name;
};

int login(int argc, char *argv[]);
int whoami(int argc, char *argv[]);
int adduser(int argc, char *argv[]);

bool hasUserInit();
void initUser();

user currentUser();
user getUser(ffsuid_t);
user getUser(std::string name);
bool isRoot(ffsuid_t);

#endif // USER_H
