cmake_minimum_required(VERSION 3.16)
project(LiquidityAlgorithms)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, defaulting to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif()

add_executable(LiquidityAlgorithms
        main.cpp
        src/core/DataLoader.cpp
        src/core/FeatureEngine.cpp
        src/models/LinearModel.cpp
        src/models/OnlineBoost.cpp
        src/models/RegimeSwitch.cpp
)
target_include_directories(LiquidityAlgorithms PRIVATE include)

include(FetchContent)
FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https:
        GIT_TAG v2.13.6
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(cppmodel
        src/bindings.cpp
        src/core/DataLoader.cpp
        src/core/FeatureEngine.cpp
        src/models/LinearModel.cpp
)
target_include_directories(cppmodel PRIVATE include)

set_target_properties(cppmodel PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
