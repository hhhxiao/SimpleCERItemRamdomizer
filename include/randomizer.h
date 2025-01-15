#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <cstdint>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "items.h"

struct Randomizer {
   public:
    bool init();
    static Randomizer& get();

    uint32_t random(uint32_t id);

   private:
    void prepare();
    void shuffle();

    void save_result();

    void try_load_cache();
    void save_cache();

   private:
    // lang
    std::unordered_map<uint32_t, std::string> langs;
    // shuffle-based item set
    std::vector<ItemEntry> weapons, protectors, magics, ashes;
    std::unordered_map<uint32_t, uint32_t> mix_pool, weapon_pool, protector_pool, magic_pool, ash_pool;

    // cache-based random item set
    std::unordered_set<uint32_t> tear_remnants, accessory_remnants;
    std::unordered_set<uint32_t> tears_pool, accessories_pool;
    std::unordered_set<uint32_t> tears_cache, accessories_cache;

    // random
    std::mt19937 re_;
};

#endif
