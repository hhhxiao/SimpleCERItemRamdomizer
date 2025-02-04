#include "randomizer.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iterator>
#include <random>
#include <ranges>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "config.h"
#include "items.h"
#include "json.hpp"
#include "logger.h"
#include "misc.h"

namespace {
    auto load_item(nlohmann::json &data, std::vector<ItemEntry> &itemList, const std::string &key,
                   std::unordered_map<uint32_t, std::string> &lang, Category cat) {
        try {
            auto &items = data[key];
            for (auto &kv : items.items()) {
                auto local_id = std::stol(kv.key());
                auto name = kv.value().get<std::string>();
                auto entry = ItemEntry::fromLocalIdCat(local_id, cat);

                auto it = lang.find(entry.id);
                if (it != lang.end()) {
                    ERR("Meet duplcaied key %s -> %s, it already has a name %s. this item will not be add to pool",
                        entry.id_string().c_str(), name.c_str(), it->second.c_str());
                } else {
                    lang.emplace(entry.id, kv.value());
                    itemList.emplace_back(entry);
                }
            }
            DBG("%zu %s loaded", itemList.size(), key.c_str());
        } catch (std::exception &e) {
            ERR("Error while read %s.\n%s", key.c_str(), e.what());
        }
    };

    void shuffle_pool(std::unordered_map<uint32_t, uint32_t> &pool, std::mt19937 &rd) {
        if (pool.empty()) return;
        auto ks = std::views::keys(pool);
        auto keys = std::vector<int>{ks.begin(), ks.end()};
        std::shuffle(keys.begin(), keys.end(), rd);
        auto idx{0};
        for (auto &kv : pool) kv.second = keys[idx++];
    }

    void write_pool(FILE *fp, const std::string &section, const std::unordered_map<uint32_t, uint32_t> &pool,
                    std::unordered_map<uint32_t, std::string> &langs) {
        fprintf(fp, "# %s\n", section.c_str());
        for (auto &kv : pool) {
            auto e1 = ItemEntry::fromId(kv.first);
            auto e2 = ItemEntry::fromId(kv.second);
            auto name1 = langs[e1.id];
            auto name2 = langs[e2.id];
            fprintf(fp, "%s:%s -> %s:%s\n", e1.id_string().c_str(), name1.c_str(), e2.id_string().c_str(),
                    name2.c_str());
        }
    }

}  // namespace

void Randomizer::try_load_cache() {
    auto &opt = Option::get();
    auto path = fs::path(misc::thisDllDirectory()) / ("SCIR_cache_" + std::to_string(opt.seed) + ".json");
    std::vector<uint32_t> acc_array, tear_array;
    std::ifstream file(path);
    if (!file.is_open()) {
        ERR("Cache file %s not found.", path.string().c_str());
    } else {
        try {
            auto data = nlohmann::json::parse(file);
            auto &acc_data = data["accessories"];
            auto &tear_data = data["tears"];
            for (auto &ac : acc_data.items()) {
                acc_array.push_back(std::stol(ac.key()));
            }
            for (auto &te : tear_data.items()) {
                tear_array.push_back(std::stol(te.key()));
            }
        } catch (std::exception &e) {
            ERR("Error while read %s.\n%s", path.string().c_str(), e.what());
        }
    }
    this->accessories_cache = std::unordered_set<uint32_t>(acc_array.begin(), acc_array.end());
    this->tears_cache = std::unordered_set<uint32_t>(tear_array.begin(), tear_array.end());
    DBG("Load %zu accessories and %zu tears from cache", accessories_cache.size(), tears_cache.size());
    for (auto &tear : tears_cache) {
        tears_pool.erase(tear);
    }
    for (auto &acc : accessories_cache) {
        accessories_pool.erase(acc);
    }
    DBG("Pool size of accessory %zu", accessories_pool.size());
    DBG("Pool size of tear %zu", tears_pool.size());
}

void Randomizer::save_cache() {
    auto &opt = Option::get();
    auto path = fs::path(misc::thisDllDirectory()) / ("SCIR_cache_" + std::to_string(opt.seed) + ".json");
    std::ofstream file(path);
    using namespace nlohmann;
    json j, acc_data, tear_data;
    for (auto &id : this->accessories_cache) {
        acc_data[std::to_string(id)] = langs[id];
    }
    for (auto &id : this->tears_cache) {
        tear_data[std::to_string(id)] = langs[id];
    }

    j["tears"] = tear_data;
    j["accessories"] = acc_data;
    file << j.dump(4);
    file.close();
}

bool Randomizer::init() {
    DBG("initializing items..");
    using namespace nlohmann;
    namespace fs = std::filesystem;
    auto path = fs::path(misc::thisDllDirectory()) / "SCIR_items.json";
    std::ifstream file(path);
    json data;
    try {
        data = json::parse(file);
    } catch (std::exception &e) {
        ERR("Can not load %s", path.c_str());
    }
    load_item(data, this->weapons, "weapon", langs, Category::Weapons);
    load_item(data, this->protectors, "protector", langs, Category::Protector);
    load_item(data, this->magics, "magic", langs, Category::Goods);
    load_item(data, this->ashes, "ash", langs, Category::Goods);

    //
    std::vector<ItemEntry> ts, as, trs, ars;
    load_item(data, ts, "tear", langs, Category::Goods);
    load_item(data, as, "accessory", langs, Category::Accessory);

    load_item(data, trs, "tear_remnant", langs, Category::Goods);
    load_item(data, ars, "accessory_remnant", langs, Category::Goods);

    // the full pool
    for (auto &e : ts) this->tears_pool.insert(e.id);
    for (auto &e : as) this->accessories_pool.insert(e.id);

    // the remeants pool
    for (auto &e : trs) this->tear_remnants.insert(e.id);
    for (auto &e : ars) this->accessory_remnants.insert(e.id);

    try_load_cache();
    prepare();
    shuffle();
    save_result();
    return true;
}

void Randomizer::prepare() {
    DBG("Preparing randomize pool...");
    auto &opt = Option::get();
    if (opt.weapon_random) {
        auto &pool = opt.weapon_indepent ? weapon_pool : mix_pool;
        for (auto &item : weapons) pool.emplace(item.id, -1);
    }

    if (opt.protector_random) {
        auto &pool = opt.protector_indepent ? protector_pool : mix_pool;
        for (auto &item : protectors) pool.emplace(item.id, -1);
    }

    if (opt.magic_random) {
        auto &pool = opt.magic_indenpent ? magic_pool : mix_pool;
        for (auto &item : magics) pool.emplace(item.id, -1);
    }
    if (opt.ash_random)
        for (auto &item : ashes) ash_pool.emplace(item.id, -1);
    DBG("Mixed pool size: %zu", mix_pool.size());
    DBG("Weapon pool size: %zu", weapon_pool.size());
    DBG("Protector pool size: %zu", protector_pool.size());
    DBG("Magic pool size: %zu", magic_pool.size());
    DBG("Ash pool size: %zu", ash_pool.size());
}
void Randomizer::shuffle() {
    DBG("Shuffle items...");
    auto &opt = Option::get();
    DBG("The seed is %zu", opt.seed);
    re_.seed(opt.seed);
    shuffle_pool(mix_pool, re_);
    shuffle_pool(weapon_pool, re_);
    shuffle_pool(protector_pool, re_);
    shuffle_pool(magic_pool, re_);
    shuffle_pool(ash_pool, re_);
}
void Randomizer::save_result() {
    auto &opt = Option::get();
    auto name = misc::thisDllDirectory() / ("items_" + std::to_string(opt.seed) + ".txt");
    FILE *fp = fopen(name.string().c_str(), "w");
    write_pool(fp, "Mixed", mix_pool, langs);
    write_pool(fp, "Weapons", weapon_pool, langs);
    write_pool(fp, "Protectors", protector_pool, langs);
    write_pool(fp, "Magics", magic_pool, langs);
    write_pool(fp, "Ashes", ash_pool, langs);
    fclose(fp);
}

uint32_t Randomizer::random(uint32_t id) {
    auto &opt = Option::get();
    auto entry = ItemEntry::fromId(id);
    auto cat = entry.category();
    std::vector<std::unordered_map<uint32_t, uint32_t> *> ps{&mix_pool, &magic_pool, &protector_pool, &magic_pool,
                                                             &ash_pool};
    for (auto pool : ps) {
        auto it = pool->find(id);
        if (it != pool->end()) return it->second;
    }

    if (tear_remnants.find(id) != tear_remnants.end() && opt.tear_random) {
        auto &pool = tears_pool;
        if (pool.empty()) pool = tears_cache;
        auto idx = re_() % pool.size();
        auto it = std::next(pool.begin(), idx);
        id = *it;
        pool.erase(it);
        tears_cache.insert(id);
        save_cache();
    } else if (accessory_remnants.find(id) != accessory_remnants.end() && opt.accessory_random) {
        auto &pool = accessories_pool;
        if (pool.empty()) pool = accessories_cache;
        auto idx = re_() % pool.size();
        auto it = std::next(pool.begin(), idx);
        id = *it;
        pool.erase(it);
        accessories_cache.insert(id);
        save_cache();
    }
    return id;
}
Randomizer &Randomizer::get() {
    static Randomizer randomizer;
    return randomizer;
}