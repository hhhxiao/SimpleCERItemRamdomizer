#ifndef HOOKS_H
#define HOOKS_H
#include <stdint.h>

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "logger.h"
#include "mem.h"
#include "minhook/MinHook.h"

inline void CreateHookWrapper(const std::string &name, uintptr_t rva, LPVOID pDetour, LPVOID *ppOriginal) {
    auto pTarget = mem::rva2va(rva);
    auto status = MH_CreateHook(reinterpret_cast<void *>(pTarget), pDetour, ppOriginal);
    if (status != MH_OK) {
        ERR("Failed to  create function hook: %s 0x%lx", name.c_str(), rva);
    } else {
        LOG("Successful create function hook: %s 0x%lx", name.c_str(), rva);
    }
}

#define DeclareHook(name, ret, ...)                               \
    struct _Hook_##name {                                         \
        static ret (*_original)(__VA_ARGS__);                     \
        template <typename... Params>                             \
        static ret original(Params &&...params) {                 \
            return (*_original)(std::forward<Params>(params)...); \
        }                                                         \
        static ret _hook(__VA_ARGS__);                            \
    };                                                            \
    ret (*_Hook_##name::_original)(__VA_ARGS__) = nullptr;        \
    ret _Hook_##name::_hook(__VA_ARGS__)

//  Rva hooker
#define RVA_HOOK(name, rva)                                                       \
    CreateHookWrapper(#name, rva, reinterpret_cast<LPVOID>(&_Hook_##name::_hook), \
                      reinterpret_cast<LPVOID *>(&_Hook_##name::_original))
// Signature hooker
#define SIG_HOOK(name, signature)                                                                           \
    auto name##Sig = mem::FindSig(signature);                                                               \
    if (name##Sig != 0) {                                                                                   \
        TRACE("Found signature of " #name " va = 0x%llx, rva = 0x%llx", name##Sig, mem::va2rva(name##Sig)); \
        CreateHookWrapper(#name, mem::va2rva(name##Sig), reinterpret_cast<LPVOID>(&_Hook_##name::_hook),    \
                          reinterpret_cast<LPVOID *>(&_Hook_##name::_original));                            \
    } else {                                                                                                \
        ERR("Can not found signature of " #name ", this function won't be hooked");                         \
    }

namespace hooks {
    bool initilize();
}  // namespace hooks

#endif
