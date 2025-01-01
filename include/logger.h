#ifndef LOGGER_H
#define LOGGER_H
#define LOG(...) log(__FUNCTION__, __VA_ARGS__)
#define ERR(...) error(__FUNCTION__, __VA_ARGS__)
#define DBG(...) debug(__FUNCTION__, __VA_ARGS__)
#define TRACE(...)
void log(const char *function_name, const char *fmt, ...);
void error(const char *function_name, const char *fmt, ...);
void debug(const char *function_name, const char *fmt, ...);
void init_logger();
#endif
