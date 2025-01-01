#include "items.h"

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "json.hpp"
#include "logger.h"
#include "misc.h"

namespace {

    std::unordered_map<long, std::string> good_set;
    std::unordered_map<long, std::string> weapon_set;
    std::unordered_map<long, std::string> protector_set;

    std::unordered_map<long, long> random_set;
}  // namespace
[[nodiscard]] std::string categoryToString(Category category) {
    switch (category) {
        case Weapons:
            return "Weapons";
        case Protector:
            return "Protector";
        case Accessory:
            return "Accessory";
        case Goods:
            return "Goods";
        case Gem:
            return "Gem";
        default:
            return "Unknown";
    }
    return "Unknown";
}

void init_item_set() {
    DBG("Begin to initialize item set");
    using namespace nlohmann;
    namespace fs = std::filesystem;
    auto path = fs::path(misc::thisDllDirectory()) / "SCIR_items.json";
    std::ifstream file(path);

    try {
        json data = json::parse(file);
        auto &goods = data["Good"];
        auto &weapons = data["Weapon"];
        auto &protectors = data["Protector"];

        for (auto &kv : goods.items()) {
            good_set[std::stol(kv.key())] = kv.value();
        }

        for (auto &kv : weapons.items()) {
            weapon_set[std::stol(kv.key())] = kv.value();
        }

        for (auto &kv : protectors.items()) {
            protector_set[std::stol(kv.key())] = kv.value();
        }

    } catch (std::exception &e) {
        ERR("Error while parsing items.json: %s", e.what());
    }
    DBG("Item set initialized, %zu goods, %zu weapons, %zu protectors", good_set.size(), weapon_set.size(),
        protector_set.size());
}

void begin_randomzie(int seed) {
    std::vector<long> ids;
    for (auto &good : good_set) {
        ids.push_back(good.first + Goods);
    }
    for (auto &weapon : weapon_set) {
        ids.push_back(weapon.first + Weapons);
    }

    for (auto &protector : protector_set) {
        ids.push_back(protector.first + Protector);
    }

    auto random = ids;
    std::shuffle(random.begin(), random.end(), std::default_random_engine(seed));

    std::ofstream file(fs::path(misc::thisDllDirectory()) / ("random_list_" + std::to_string(seed) + ".txt"));
    file << "Seed = " << seed << std::endl;
    for (int i = 0; i < ids.size(); i++) {
        ItemEntry e1{0, ids[i], 0, 0};
        ItemEntry e2{0, random[i], 0, 0};
        auto name1 = get_name_by_id(ids[i]);
        auto name2 = get_name_by_id(random[i]);
        file << name1 << "(" << e1.id << ") -> " << name2 << "(" << e2.id << ")" << std::endl;
        if (random_set.count(e1.id)) {
            ERR("Duplicate id %lld", e1.id);
        }
        random_set[e1.id] = e2.id;
    }
    file.close();
}

std::string get_name_by_real_id(long id, Category category) {
    switch (category) {
        case Goods:
            return good_set.count(id) ? good_set[id] : "Unknown";
        case Weapons:
            return weapon_set.count(id) ? weapon_set[id] : "Unknown";
        case Protector:
            return protector_set.count(id) ? protector_set[id] : "Unknown";
        case Accessory:
        case Gem:
        default:
            break;
    }
    return "Unknown(" + std::to_string(id) + +", " + categoryToString(category) + ")";
}

std::string get_name_by_id(long id) {
    ItemEntry e{0, id, 0, 0};
    return get_name_by_real_id(e.real_id(), e.category());
}

long random(long id) {
    auto it = random_set.find(id);
    return it == random_set.end() ? id : it->second;
}
