cmake_minimum_required(VERSION 3.16)
project(astar LANGUAGES CXX)

# Copy all executable and dynamic libraries to bin folder. 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Generate exports for clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
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
