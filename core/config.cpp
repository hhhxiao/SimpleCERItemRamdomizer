#include "config.h"

#include <exception>
#include <fstream>
#include <string>

#include "json.hpp"
#include "logger.h"
#include "misc.h"

namespace config {

    namespace {
        int seed{0};
        bool debugMode{false};

    }  // namespace
    void init() {
        auto path = misc::thisDllDirectory() / "SCIR_config.json";
        LOG("Config path is %s", path.string().c_str());
        try {
            std::ifstream f(path);
            nlohmann::json data = nlohmann::json::parse(f);
            debugMode = data.value("debug_mode", false);
            seed = data.value("seed", 0);
        } catch (std::exception &e) {
            ERR("Can not open config file:%s", path.string().c_str());
        }
    }
    void print_all_info() {
        LOG("Seed = %d", seed);
        LOG("Debug mode = %s", debugMode ? "true" : "false");
    }
    bool debug_mode() { return debugMode; }
    int random_seed() { return seed; }
}  // namespace config
