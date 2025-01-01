#ifndef MEM_H
#define MEM_H
// clang-format off
#include <Windows.h>
#include <cstdint>
#include <ios>
#include <sstream>
#include <Psapi.h>
#include <basetsd.h>
#include <stdint.h>
#include <vadefs.h>
#include <iomanip>
#include <wingdi.h>
#include <winnt.h>
#include <winscard.h>
#include "logger.h"
// clang-format on
#include <sstream>
#include <string>
#include <vector>

namespace mem {

    inline uintptr_t FindSig(const char *moduleName, const char *signature) {
        HMODULE moduleHandle = GetModuleHandleA(moduleName);
        MODULEINFO moduleInfo;
        GetModuleInformation(GetCurrentProcess(), moduleHandle, &moduleInfo, sizeof(MODULEINFO));
        std::vector<uint16_t> pattern;
        for (int i = 0; i < strlen(signature); i++) {
            if (signature[i] == ' ') continue;
            if (signature[i] == '?') {
                pattern.push_back(0xFF00);
                i++;
            } else {
                char buf[3]{signature[i], signature[++i], 0};
                pattern.push_back((uint16_t)strtoul(buf, nullptr, 16));
            }
        }

        if (pattern.size() == 0) return (uintptr_t)moduleHandle;

        int patternIdx = 0;
        uintptr_t match = 0;
        for (uintptr_t i = (uintptr_t)moduleHandle; i < (uintptr_t)moduleHandle + moduleInfo.SizeOfImage; i++) {
            uint8_t current = *(uint8_t *)i;
            if (current == pattern[patternIdx] || pattern[patternIdx] & 0xFF00) {
                if (!match) match = i;
                patternIdx++;
                if (patternIdx == pattern.size()) {
                    return match;
                }
            } else {
                if (match) {
                    i--;
                }
                match = 0;
                patternIdx = 0;
            }
        }

        return 0;
    }

    inline uintptr_t FindSig(const char *signature) { return FindSig("eldenring.exe", signature); }

    inline uintptr_t FingSigWithOffset(const char *signature, uint64_t offset, uint64_t additional) {
        auto addr = FindSig(signature);
        if (addr == 0) {
            ERR("Can't found Signature: [%s]", signature);
            return 0;
        }
        return addr + *reinterpret_cast<int32_t *>(addr + offset) + additional;
    }

    inline uintptr_t va2rva(uintptr_t va) { return va - reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)); }
    inline uintptr_t rva2va(uintptr_t rva) { return rva + reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)); }

    static void print_bytes(uintptr_t base, size_t bytes) {
        std::stringstream ss;
        ss << "\n";
        for (int i = 0; i < bytes; i++) {
            if (i % 16 == 0) {
                ss << "0x" << std::setfill('0') << std::setw(sizeof(uintptr_t) * 2) << std::hex << (base + i) << " ";
            }
            ss << std::setfill('0') << std::setw(2) << (uint16_t) * reinterpret_cast<uint8_t *>(base + i) << " ";
            if (i % 16 == 15) {
                ss << "\n";
            }
        }
        DBG("%s", ss.str().c_str());
    }

    static void print_num(uintptr_t base, size_t bytes) {
        std::stringstream ss;
        ss << "\n";
        for (int i = 0; i < bytes; i += 4) {
            ss << "0x" << std::setfill('0') << std::setw(sizeof(uintptr_t) * 2) << std::hex << (base + i) << " "
               << std::setw(4) << std::hex << (i);
            ss << std::setfill(' ') << std::setw(20) << std::to_string(*reinterpret_cast<int32_t *>(base + i)) << "   ";
            ss << std::to_string(*reinterpret_cast<float *>(base + i)) << "\n";
        }
        DBG("%s", ss.str().c_str());
    }

}  // namespace mem
#endif