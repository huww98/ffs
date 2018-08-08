#include "help.h"

#include <iostream>

using namespace std;

int help(int argc, char const *argv[])
{
    cout << R"---(基于文件的模拟文件系统

格式： ffs [命令] [命令参数]

命令：
help   显示本帮助信息
create 在当前目录创建一个文件系统
login  用户登录

有关各命令的详细使用方式请使用 ffs [命令] --help 查看)---"
         << endl;
    return 0;
}
