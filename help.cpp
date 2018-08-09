#include "help.h"

#include <iostream>

using namespace std;

int help(int argc, char const *argv[])
{
    cout << R"---(基于文件的模拟文件系统

格式： ffs <命令> [<命令参数>]

命令：
help   显示本帮助信息
init   在当前目录创建一个文件系统
login  用户登录
whoami 查看当前登录用户
ls     列出模拟文件系统的当前目录内容
mkdir  创建目录
read   读取文件内容，写入标准输出流
write  将标准输入流写入文件
rm     删除文件

有关各命令的详细使用方式请使用 'ffs help <命令>' 查看)---"
         << endl;
    return 0;
}
