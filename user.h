#if !defined(USER_H)
#define USER_H

#include <string>

namespace user
{

struct user
{
    int uid;
    std::string name;
};

int login(int argc, char const *argv[]);
int whoami(int argc, char const *argv[]);

bool hasInit();
void init();

user currentUser();
} // namespace user

#endif // USER_H
