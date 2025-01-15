cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ERClient)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/third)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_compile_options(/wd4819)
# add_library(
#         ER_LIB
#         core/er.cpp
#         core/hooks.cpp
#         core/outputer.cpp
#         core/config.cpp
#         tools/logger.cpp
#         tools/misc.cpp
#         )
# include(FetchContent)
# FetchContent_Declare(
#   fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt
#   GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
# FetchContent_MakeAvailable(fmt)

# STATIC

add_library(SCIR_LIB  
        STATIC
        core/er.cpp
        core/hooks.cpp
        core/config.cpp
        tools/logger.cpp
        tools/misc.cpp
        core/randomizer.cpp
)
target_link_libraries(SCIR_LIB ${PROJECT_SOURCE_DIR}/third/minhook/MinHook.x64.lib)

# DLL
add_library(SCIR SHARED dllmain.cpp)
target_link_libraries(SCIR SCIR_LIB)

add_executable(SCIR_test core/test.cpp)
target_link_libraries(SCIR_test SCIR_LIB)


# target_link_libraries(ERLiveDisplay ER_LIB)