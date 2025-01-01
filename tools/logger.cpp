#include "logger.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// void log(const char *file_name, const char *function_name, size_t line, const char *fmt, ...) {}

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

namespace {

    std::string now() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

}  // namespace

void init_logger() {
    // todo: write log to file
}

void log(const char *function_name, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, KGRN "%s" KCYN " [%s] " KWHT, now().c_str(), function_name);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    fflush(stdout);
}

void error(const char *function_name, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, KGRN "%s" KCYN " [%s] " KRED, now().c_str(), function_name);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    fflush(stdout);
}

void debug(const char *function_name, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, KGRN "%s" KCYN " [%s] " KGRN, now().c_str(), function_name);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    fflush(stdout);
}