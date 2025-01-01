#ifndef CONFIG_H
#define CONFIG_H
#include <string>

namespace config {

    void init();

    void print_all_info();
    bool debug_mode();
    int random_seed();
}  // namespace config

#endif
