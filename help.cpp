#include "help.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>

using namespace std;

map<string, string> helpText{
    {"help", R"---(基于文件的模拟文件系统

格式： ffs <命令> [<命令参数>]

命令：
help   显示本帮助信息
init   在当前目录创建一个文件系统
login  用户登录
whoami 查看当前登录用户
cd     切换模拟文件系统的当前目录
pwd    查看模拟文件系统的当前目录
ls     列出模拟文件系统的当前目录内容
mkdir  创建目录
read   读取文件内容，写入标准输出流
write  将标准输入流写入文件
rm     删除文件

有关各命令的详细使用方式请使用 'ffs help <命令>' 查看)---"},

    {"login", R"---(登录模拟文件系统

格式： ffs login <用户名>

初始化文件系统时会自动创建root用户。也会自动创建一个与当前
登录的系统用户同名的用户，并自动登录该用户。

如需新增其他用户，请直接修改users文件。)---"},

    {"cd", R"---(切换模拟文件系统的当前目录

格式： ffs cd [<路径>]

若未指定路径，则切换到根目录。)---"},

    {"mkdir", R"---(在模拟文件系统中创建一个目录

格式： ffs mkdir <路径>)---"}};

int help(int argc, char const *argv[])
{
    string cmd = "help";
    if (argc > 0)
    {
        cmd = argv[0];
    }

    try
    {
        cout << helpText.at(cmd) << endl;
    }
    catch (const out_of_range &e)
    {
        stringstream errMsg;
        errMsg << "未找到" << cmd << "的帮助。";
        throw out_of_range(errMsg.str());
    }

    return 0;
}
