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
help    显示本帮助信息
init    在当前目录创建一个文件系统
login   用户登录
whoami  查看当前登录用户
adduser 添加用户
cd      切换模拟文件系统的当前目录
pwd     查看模拟文件系统的当前目录
ls      列出模拟文件系统的目录内容
mkdir   创建目录
read    读取文件内容，写入标准输出流
write   将标准输入流写入文件
rm      删除文件
chmod   更改文件的权限

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

格式： ffs mkdir <路径>)---"},

    {"ls", R"---(列出模拟文件系统的目录内容

格式： ffs ls [<路径>] [<选项>]...

选项：
  -l  使用长列表格式
  -0  使用'\0'字符来分隔每项

若未指定路径，则列出当前目录的内容)---"},

    {"rm", R"---(删除文件

格式： ffs rm [<选项>]... <路径>...

选项：
  -r  递归地删除文件夹和里面的内容

根目录不可删除)---"},

    {"read", R"---(将指定文件内容输出到标准输出流

格式： ffs read <路径>)---"},

    {"write", R"---(将标准输入流的内容写入指定文件

格式： ffs write <路径>

若文件不存在则将自动创建)---"},

    {"chmod", R"---(更改文件的权限

格式： ffs chmod [<模式>][,<模式>]... [<路径>]...

模式： [uoa]*[+-=][rw]*)---"}};

int help(int argc, char *argv[])
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
