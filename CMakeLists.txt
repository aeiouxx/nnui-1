cmake_minimum_required(VERSION 3.16)
project(astar LANGUAGES CXX)

option(USE_CLANG "Try to use clang compiler" OFF)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#= Select a compiler =============================================================
set(CMAKED_EXPORT_COMPILE_COMMANDS ON)
if (USE_CLANG)
    find_program(CLANG_FOUND "clang")
    find_program(CLANGXX_FOUND "clang++")
    if (AND CLANG_FOUND AND CLANGXX_FOUND)
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
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.x)
FetchContent_MakeAvailable(SFML)
#===============================================================================
# My actual application =========================================================
add_executable(astar src/main.cpp )
target_link_libraries(astar PRIVATE sfml-graphics sfml-window sfml-system tinyfiledialogs)
target_compile_features(astar PRIVATE cxx_std_17)
#================================================================================

if(WIN32)
    add_custom_command(
        TARGET astar 
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:astar>
        VERBATIM)
endif()

install(TARGETS astar)
