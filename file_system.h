#if !defined(FILES_H)
#define FILES_H

int init(int argc, char const *argv[]);
bool hasInit();

int cd(int argc, char const *argv[]);
int mkdir(int argc, char const *argv[]);
int pwd(int argc, char const *argv[]);

#endif // FILES_H
