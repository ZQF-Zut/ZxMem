# Main CMakeLists
cmake_minimum_required(VERSION 3.20)

# Project Name
project(ZxMem)

# On Test Preset
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(CMakeMycfg.cmake)
endif()

# Export Symbols
if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Set Source File Path
list(APPEND "${PROJECT_NAME}_SRC_FILE" 
    "src/ZxMem/ZxMem.cpp" 
    "src/ZxMem/Platform.cpp")

# Set Library
add_library("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC_FILE}")
target_include_directories("${PROJECT_NAME}" PUBLIC "src")
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_23)
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")



# On Test Config
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Test Project
    add_executable(ZxMemTest "test/main.cpp")
    target_compile_features(ZxMemTest PRIVATE cxx_std_23)
    target_compile_options(ZxMemTest PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

    # Add Library
    target_link_libraries(ZxMemTest PRIVATE ZxMem)
endif()
