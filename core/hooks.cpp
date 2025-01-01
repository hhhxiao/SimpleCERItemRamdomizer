#include "hooks.h"

#include "er.h"
#include "logger.h"
#include "minhook/MinHook.h"

namespace hooks {
    bool initilize() {
        auto status = MH_Initialize();
        if (status != MH_OK) {
            ERR("Can not initialize Minhooks");
            return false;
        }
        er::init_hooks();
        status = MH_EnableHook(MH_ALL_HOOKS);
        if (status != MH_OK) {
            ERR("Can not enable hooks");
            return false;
        }
        return true;
    }
}  // namespace hooks