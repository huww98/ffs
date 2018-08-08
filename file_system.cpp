#include "file_system.h"

#include <filesystem>
#include <fstream>

#include "user.h"

using namespace std;
namespace fs = std::filesystem;

const fs::path rootDirFilePath("./root_dir");
const fs::path pwdFilePath("./pwd");

int init(int argc, char const *argv[])
{
    user::init();
    ofstream(pwdFilePath) << "/";
    return 0;
}

bool hasInit()
{
    return user::hasInit() && fs::exists(rootDirFilePath) && fs::exists(pwdFilePath);
}
