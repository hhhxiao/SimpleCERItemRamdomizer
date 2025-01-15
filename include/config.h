#ifndef CONFIG_H
#define CONFIG_H
#include <cstdint>
#include <string>

struct Option {
    // weapon
    bool weapon_random{true};
    bool weapon_indepent{false};
    // protector
    bool protector_random{false};
    bool protector_indepent{false};
    // magic
    bool magic_random{false};
    bool magic_indenpent{false};
    // others
    bool accessory_random{false};
    bool ash_random{false};
    bool tear_random{false};
    //
    uint32_t seed{0};
    bool debug_mode{false};
    //
    bool load();
    void print();

    static Option& get();
};

void print_all_info();

#endif
