#include <iostream>
#include <string>
#include <functional>
#include <getopt.h>

#include "user.h"
#include "help.h"

using namespace std;

struct commandEntry
{
    string command;
    function<int(int, char const **)> entry;
};

commandEntry validCommands[]{
    {"help", help},
    {"login", login}};

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

    for (auto &c : validCommands)
    {
        if (c.command == command)
        {
            return c.entry(cmdArgc, cmdArgv);
        }
    }

    cerr << "Invalid command " << command << endl;

    return 0;
}
