#ifndef __FILESYSTEM_H_
#define __FILESYSTEM_H_

#include <fstream>
#include <stdio.h>
#include <string>
#ifdef _WIN32
    #include <Windows.h>
#else
    #include <sys/stat.h>
#endif

namespace filesystem
{
    bool exists(std::string path);
    void create_directory(std::string path);
    void copy(std::string src_path, std::string dst_path);
    void remove(std::string path);
    bool is_regular(std::string path);
}

#endif // __FILESYSTEM_H_
