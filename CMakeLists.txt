cmake_minimum_required(VERSION 3.14)
project(PathFindPro CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Release by default, keep debug info ──────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -Wextra")

# ── Source files ─────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp

    # Data structures
    src/data_structures/graph.cpp
    src/data_structures/min_heap.cpp

    # Algorithms
    src/algorithms/dijkstra.cpp
    src/algorithms/astar.cpp
    src/algorithms/bfs.cpp
    src/algorithms/dfs.cpp

    # Benchmark
    src/benchmark/benchmarker.cpp

    # Utils
    src/utils/json_writer.cpp
    src/utils/graph_generator.cpp
)

add_executable(pathfind ${SOURCES})

# All includes relative to src/ so headers can use short paths
target_include_directories(pathfind PRIVATE src)
