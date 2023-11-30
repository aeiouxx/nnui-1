cmake_minimum_required(VERSION 3.16)
project(astar LANGUAGES CXX)

option(USE_CLANG "Try to use clang compiler" OFF)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#= Select a compiler ============================================================
set(CMAKED_EXPORT_COMPILE_COMMANDS ON)
if (USE_CLANG)
    find_program(CLANG_FOUND "clang")
    find_program(CLANGXX_FOUND "clang++")
    if (CLANG_FOUND AND CLANGXX_FOUND)
        message(STATUS "Found Clang, Clang++ using for build")
        set(CMAKE_C_COMPILER ${CLANG_FOUND})
        set(CMAKE_CXX_COMPILER ${CLANGXX_FOUND})
    else ()
        message(STATUS "Clang, Clang++ not found, using default compiler")
    endif()
else ()
    message(STATUS "Using default compiler")
endif()
#================================================================================
#= wxWidgets ====================================================================
message(STATUS "wxWidgets... (will take a while to download and build if not populated)")
set(wxBUILD_SHARED OFF)
set(wxBUILD_PRECOMP OFF)
set(wxBUILD_MONOLITHIC OFF)
include(FetchContent)
FetchContent_Declare(
    wxWidgets
    GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
    GIT_TAG        v3.2.4)
FetchContent_MakeAvailable(wxWidgets)
#================================================================================
if (USE_CLANG AND CLANG_FOUND AND CLANGXX_FOUND) 
    set(COMPILE_COMMANDS_SRC "${CMAKE_BINARY_DIR}/compile_commands.json")
    set(COMPILE_COMMANDS_DST "${CMAKE_SOURCE_DIR}/compile_commands.json")
    if (WIN32)
        message(STATUS "Copying compile_commands.json to source dir")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy
            ${COMPILE_COMMANDS_SRC} ${COMPILE_COMMANDS_DST})
    else()
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink 
            ${COMPILE_COMMANDS_SRC} ${COMPILE_COMMANDS_DST})
    endif()
endif()
#= My actual application ========================================================
set(EXECUTABLE_FLAGS "")
set(ASTAR_DEFINITIONS "")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building debug version")
    set(ASTAR_DEFINITIONS "ASTAR_DEBUG")
else()
    message(STATUS "Building release version")
endif()
if (WIN32)
  set(EXECUTABLE_FLAGS WIN32)
endif()
set(SOURCES src/main.cc
            src/astar.cc
            src/common/grid.cc
            src/ui/my-frame.cc
            src/ui/maze-panel.cc)
add_executable(astar ${EXECUTABLE_FLAGS} ${SOURCES})
target_include_directories(astar PRIVATE ${wxWidgets_SOURCE_DIR}/include)
target_link_libraries(astar PRIVATE wx::base wx::core)
target_compile_features(astar PRIVATE cxx_std_20)
target_compile_definitions(astar PRIVATE ${ASTAR_DEFINITIONS})

install(TARGETS astar)
