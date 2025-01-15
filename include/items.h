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

inline std::string categoryToString(Category category) {
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

struct ItemEntry {
    int32_t count{0};
    int32_t id{0};  // item.ID + item.Infusion + item.Upgrade + (int)item.Category)
    int32_t quantity{0};
    int32_t gem{0};

    Category category() const { return static_cast<Category>(id & 0xF0000000); }
    int32_t local_id() const { return id & 0x0FFFFFFF; }

    std::string id_string() const {
        return std::to_string(id) + "(" + categoryToString(category()) + ":" + std::to_string(local_id()) + ")";
    }

    static ItemEntry fromLocalIdCat(int32_t local_id, Category cat) { return {0, local_id + cat, 0, 0}; }
    static ItemEntry fromId(int32_t id) { return {0, id, 0, 0}; }
};
#endif