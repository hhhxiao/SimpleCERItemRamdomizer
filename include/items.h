#ifndef ITEM_H
#define ITEM_H

#include <cstdint>
#include <string>

enum Category {
    Weapons = 0x00000000,
    Protector = 0x10000000,
    Accessory = 0x20000000,
    Goods = 0x40000000,
    Gem = 0x80000000

};

[[nodiscard]] std::string categoryToString(Category category);
struct ItemEntry {
    int32_t count{0};
    int32_t id{0};  // item.ID + item.Infusion + item.Upgrade + (int)item.Category)
    int32_t quantity{0};
    int32_t gem{0};

    Category category() const { return static_cast<Category>(id & 0xF0000000); }
    int32_t real_id() const { return id & 0x0FFFFFFF; }
};

void init_item_set();

void begin_randomzie(int seed);

std::string get_name_by_real_id(long id, Category category);
std::string get_name_by_id(long id);

long random(long id);
#endif