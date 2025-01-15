#include "config.h"

#include <cstdint>
#include <cstdio>
#include <exception>
#include <fstream>
#include <string>

#include "json.hpp"
#include "logger.h"
#include "misc.h"

Option& Option::get() {
    static Option opt;
    return opt;
}

bool Option::load() {
    auto path = misc::thisDllDirectory() / "SCIR_config.json";
    LOG("Config path is %s", path.string().c_str());
    try {
        std::ifstream f(path);
        nlohmann::json data = nlohmann::json::parse(f);
        weapon_random = data["weapon"]["random"].get<bool>();
        weapon_indepent = data["weapon"]["independent_pool"].get<bool>();

        protector_random = data["protector"]["random"].get<bool>();
        protector_indepent = data["protector"]["independent_pool"].get<bool>();

        magic_random = data["magic"]["random"].get<bool>();
        magic_indenpent = data["magic"]["independent_pool"].get<bool>();

        ash_random = data["ash"]["random"].get<bool>();
        tear_random = data["tear"]["random"].get<bool>();
        accessory_random = data["accessory"]["random"].get<bool>();

        debug_mode = data["debug_mode"].get<bool>();
        seed = data["seed"].get<int32_t>();
        return true;
    } catch (std::exception& e) {
        ERR("Can not open config file:%s\n%s", path.string().c_str(), e.what());
        return false;
    }
}

void Option::print() {
    DBG("Weapon: %d %d", weapon_random, weapon_indepent);
    DBG("Protector: %d %d", protector_random, protector_indepent);
    DBG("Magic: %d %d", magic_random, magic_indenpent);
    DBG("Ash: %d", ash_random);
    DBG("Tear: %d", tear_random);
    DBG("Accessory: %d", accessory_random);
    DBG("Seed: %d", seed);
    DBG("Debug mode: %d", debug_mode);
}
