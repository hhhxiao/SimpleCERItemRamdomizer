//clang-format on
#include <fcntl.h>
#include <io.h>
// clang-format of
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <thread>

#include "config.h"
#include "er.h"
#include "hooks.h"
#include "include/logger.h"
#include "include/misc.h"
#include "items.h"
#include "logger.h"

void openConsole() {
    if (AllocConsole()) {
        SetConsoleTitle("Debug");
        HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        INT hCrt = _open_osfhandle((intptr_t)hCon, _O_TEXT);
        FILE* hf = _fdopen(hCrt, "w");
        setvbuf(hf, NULL, _IONBF, 0);
        *stdout = *hf;
        freopen("CONOUT$", "w+t", stdout);  // NOLINT
        DWORD mode;
        // ANSI support
        GetConsoleMode(hCon, &mode);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hCon, mode);
    } else {
        throw std::exception("Can not open console");
    }
}

// 各种初始化
DWORD WINAPI start(LPVOID lpParam) {
    // openConsole();
    config::init();
    if (config::debug_mode()) {
        openConsole();
        system("chcp 65001");
    }
    DBG("Simple CER Item Randomier v0.0.1");
    DBG("By hhhxiao (https://space.bilibili.com/34227664)");
    config::print_all_info();
    DBG("Mod Injected!");
    init_item_set();
    begin_randomzie(config::random_seed());
    hooks::initilize();
    ExitThread(0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)start, hModule, NULL, NULL);
            DisableThreadLibraryCalls(hModule);
        case DLL_PROCESS_DETACH:
        default:
            break;
    }
    return TRUE;
}
