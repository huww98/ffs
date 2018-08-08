#if !defined(USER_H)
#define USER_H

#include <string>

namespace user
{
int login(int argc, char const *argv[]);
int whoami(int argc, char const *argv[]);

bool hasInit();
void init();

std::string currentUser();
} // namespace user

#endif // USER_H
