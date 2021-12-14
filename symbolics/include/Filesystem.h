/*
This file is part of PyMbs.

PyMbs is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

PyMbs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with PyMbs.
If not, see <http://www.gnu.org/licenses/>.

Copyright 2011, 2012 Carsten Knoll, Christian Schubert, 
                     Jens Frenkel, Sebastian Voigt
*/

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
