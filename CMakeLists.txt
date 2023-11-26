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
        message(STATUS "Found CLANG, CLANG++ using for build")
        set(CMAKE_C_COMPILER ${CLANG_FOUND})
        set(CMAKE_CXX_COMPILER ${CLANGXX_FOUND})
    else ()
        message(STATUS "CLANG, CLANG++ not found, using default compiler")
    endif()
else ()
    message(STATUS "Using default compiler")
endif()
#================================================================================

#= TinyFileDialogs ==============================================================
set(TINYFILEDIALOGS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/tinyfiledialogs)
add_library(tinyfiledialogs STATIC ${TINYFILEDIALOGS_DIR}/tinyfiledialogs.cpp)
target_include_directories(tinyfiledialogs PRIVATE ${TINYFILEDIALOGS_DIR})
#================================================================================
#= SFML =========================================================================
message(STATUS "Fetching SFML...")
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.x)
FetchContent_MakeAvailable(SFML)
message(STATUS "SFML fetched")

if (USE_CLANG AND CLANG_FOUND AND CLANGXX_FOUND) 
    set(COMPILE_COMMANDS_SRC "${CMAKE_BINARY_DIR}/compile_commands.json")
    set(COMPILE_COMMANDS_DST "${CMAKE_SOURCE_DIR}/compile_commands.json")
    if (WIN32)
        message(STATUS "copying compile_commands.json to source dir")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy
            ${COMPILE_COMMANDS_SRC} ${COMPILE_COMMANDS_DST})
    else()
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink 
            ${COMPILE_COMMANDS_SRC} ${COMPILE_COMMANDS_DST})
    endif()
endif()
#================================================================================
#= My actual application ========================================================
set(EXECUTABLE_FLAGS "")
set(ASTAR_DEFINITIONS "")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building debug version")
    set(ASTAR_DEFINITIONS "ASTAR_DEBUG")
else()
    message(STATUS "Building release version")
endif()
set(SFML_LIBRARIES
    sfml-graphics
    sfml-window
    sfml-system)
if (WIN32) 
    set(EXECUTABLE_FLAGS WIN32)
    list(PREPEND SFML_LIBRARIES sfml-main)
endif()
set(SOURCES src/main.cc
            src/application.cc
            src/common/grid.cc)
add_executable(astar ${EXECUTABLE_FLAGS} ${SOURCES})
target_link_libraries(astar PRIVATE ${SFML_LIBRARIES} tinyfiledialogs)
target_compile_features(astar PRIVATE cxx_std_17)
target_compile_definitions(astar PRIVATE ${ASTAR_DEFINITIONS})
#================================================================================

if(WIN32)
    add_custom_command(
        TARGET astar 
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:astar>
        VERBATIM)
endif()

install(TARGETS astar)
