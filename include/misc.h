#ifndef MISC_H
#define MISC_H
//clang-format on
#include <fcntl.h>
#include <io.h>
// clang-format of
#include <stdint.h>
#include <stdio.h>

#include <cstdio>
#include <filesystem>

#include "include/logger.h"
#include "logger.h"
#include "mem.h"
namespace fs = std::filesystem;
namespace misc {
    fs::path thisDllDirectory();

}  // namespace misc

#endif
