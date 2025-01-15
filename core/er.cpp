#include "er.h"

#include <winscard.h>

#include <cstdint>

#include "hooks.h"
#include "items.h"
#include "randomizer.h"

DeclareHook(ItemPickUpHook, void, int64_t a1, ItemEntry *itemEntry, int64_t a3, int a4) {
    int index = 0;
    auto &rand = Randomizer::get();
    while (index < 10) {
        auto *item = itemEntry + index;
        if (item->id == 0xfffffff) break;
        item->id = rand.random(item->id);
        ++index;
    }
    original(a1, itemEntry, a3, a4);
}

namespace er {

    void init_hooks() { RVA_HOOK(ItemPickUpHook, 0x0560670); }
}  // namespace er