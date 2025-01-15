#include <cassert>
#include <cstdint>
#include <cstdio>
#include <unordered_map>
#include <unordered_set>

#include "config.h"
#include "items.h"
#include "logger.h"
#include "randomizer.h"

bool test_normal() {
    if (!Option::get().load()) {
        return false;
    }
    Option::get().seed = 2025;
    Option::get().print();
    auto &ran = Randomizer::get();
    ran.init();
    for (int i = 0; i < 10; i++) {
        if (ran.random(271445456) != 1073746124) return false;
        if (ran.random(1075748034) != 269195756) return false;
    }
    return true;
}

bool test_cahce_random() {
    if (!Option::get().load()) {
        return false;
    }
    Option::get().seed = 2025;
    Option::get().accessory_random = true;
    Option::get().tear_random = true;
    Option::get().print();
    auto &ran = Randomizer::get();
    ran.init();
    std::unordered_set<uint32_t> as, ts;
    for (int i = 0; i < 148; i++) {
        auto id = ran.random(ItemEntry::fromLocalIdCat(20950, Category::Goods).id);
        as.insert(id);
    }
    for (int i = 0; i < 100; i++) {
        auto id = ran.random(ItemEntry::fromLocalIdCat(20950, Category::Goods).id);
        assert(as.count(id));
    }

    for (int i = 0; i < 60; i++) {
        auto id = ran.random(ItemEntry::fromLocalIdCat(20900, Category::Goods).id);
        as.insert(id);
    }
    for (int i = 0; i < 100; i++) {
        auto id = ran.random(ItemEntry::fromLocalIdCat(20900, Category::Goods).id);
        assert(as.count(id));
    }

    return true;
}

int main() {
    //    assert(test_cahce_random());
    if (!Option::get().load()) {
        return -1;
    }
    Option::get().seed = 2125;
    Option::get().accessory_random = true;
    Option::get().tear_random = true;
    Option::get().print();
    auto &ran = Randomizer::get();
    ran.init();
    for (int i = 0; i < 10; i++) {
        ran.random(ItemEntry::fromLocalIdCat(20950, Category::Goods).id);
        ran.random(ItemEntry::fromLocalIdCat(20900, Category::Goods).id);
    }

    return 0;
}