#include <iostream>
#include <string>
#include <functional>

#include "user.h"
#include "help.h"
#include "file_system.h"

using namespace std;

struct commandEntry
{
    string command;
    function<int(int, char **)> entry;
};

commandEntry initCommand{"init", init};

commandEntry validCommands[]{
    {"help", help},
    {"login", login},
    {"whoami", whoami},
    {"mkdir", mkdir},
    {"cd", cd},
    {"pwd", pwd},
    {"ls", ls},
    {"rm", rm},
    {"read", read},
    {"write", write},
    initCommand};

int main(int argc, char *argv[])
{
    string command("help");
    int cmdArgc = 0;
    char **cmdArgv = nullptr;

    if (argc > 1)
    {
        command = string(argv[1]);
        cmdArgc = argc - 2;
        cmdArgv = argv + 2;
    }

    if (command != initCommand.command && !hasInit())
    {
        cerr << "ERROR: 未找到文件系统" << endl;
        return 2;
    }
    else if (command == initCommand.command && hasInit())
    {
        cerr << "ERROR: 无法初始化，该文件夹下已存在文件系统" << endl;
        return 2;
    }

    for (auto &c : validCommands)
    {
        if (c.command == command)
        {
            try
            {
                return c.entry(cmdArgc, cmdArgv);
            }
            catch (const exception &e)
            {
                std::cerr << "ERROR: " << e.what() << endl;
                return 3;
            }
        }
    }

    cerr << "Invalid command " << command << endl;
    return 1;
}
