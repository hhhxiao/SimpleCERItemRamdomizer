#include "misc.h"
namespace misc {

    fs::path thisDllDirectory() {
        char path[MAX_PATH];
        HMODULE hm = NULL;

        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCSTR)&thisDllDirectory, &hm) == 0) {
            int ret = GetLastError();
            ERR("GetModuleHandle failed, error = %d\n", ret);
        }
        if (GetModuleFileName(hm, path, sizeof(path)) == 0) {
            int ret = GetLastError();
            ERR("GetModuleFileName failed, error = %d\n", ret);
        }
        return fs::path(path).parent_path();
    }

}  // namespace misc