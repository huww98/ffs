#if !defined(FILES_H)
#define FILES_H

int init(int argc, char *argv[]);
bool hasInit();

int cd(int argc, char *argv[]);
int mkdir(int argc, char *argv[]);
int pwd(int argc, char *argv[]);
int ls(int argc, char *argv[]);
int rm(int argc, char *argv[]);

#endif // FILES_H
