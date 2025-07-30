#pragma once
// rcore: Configuration values
//------------------------------------------------------------------------------------
#define MAX_FILEPATH_CAPACITY        8192       // Maximum file paths capacity
#define MAX_FILEPATH_LENGTH          4096       // Maximum length for filepaths (Linux PATH_MAX default value)

#if defined(_WIN32)
#include <io.h>                 // Required for: _access() [Used in FileExists()]
#include <direct.h>             // Required for: _getch(), _chdir(), _mkdir()
#define GETCWD _getcwd          // NOTE: MSDN recommends not to use getcwd(), chdir()
#define CHDIR _chdir
#define MKDIR(dir) _mkdir(dir)
#else
#include <unistd.h>             // Required for: getch(), chdir(), mkdir(), access()
#define GETCWD getcwd
#define CHDIR chdir
#define MKDIR(dir) mkdir(dir, 0777)
#endif
