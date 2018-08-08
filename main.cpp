#include <iostream>
#include <string>
#include <functional>
#include <getopt.h>

#include "user.h"
#include "help.h"
#include "file_system.h"

using namespace std;

struct commandEntry
{
    string command;
    function<int(int, char const **)> entry;
};

commandEntry initCommand{"init", init};

commandEntry validCommands[]{
    {"help", help},
    {"login", user::login},
    {"whoami", user::whoami},
    initCommand};

int main(int argc, char const *argv[])
{
    string command("help");
    int cmdArgc = 0;
    char const **cmdArgv = nullptr;

    if (argc > 1)
    {
        command = string(argv[1]);
        cmdArgc = argc - 2;
        cmdArgv = argv + 2;
    }

    // if(command != initCommand.command && !hasInit()) {
    //     cerr << "ERROR: 未找到文件系统" << endl;
    //     return 2;
    // }

    for (auto &c : validCommands)
    {
        if (c.command == command)
        {
            return c.entry(cmdArgc, cmdArgv);
        }
    }

    cerr << "Invalid command " << command << endl;
    return 1;
}
