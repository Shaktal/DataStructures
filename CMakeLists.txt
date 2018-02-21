cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_CXX_FLAGS "-Wall -Wextra")
set (CMAKE_CXX_FLAGS_DEBUG "-Og -g")
set (CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")

project (DataStructureTests)

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB tests ${PROJECT_SOURCE_DIR}/tests/*.cpp)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(DataStructure_Tests ${tests})
target_link_libraries(DataStructure_Tests ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS DataStructure_Tests DESTINATION bin)
