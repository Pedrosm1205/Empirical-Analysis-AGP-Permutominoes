cmake_minimum_required(VERSION 3.18)
project(Empirical-Analysis-Iterative-Construction-Permutominoes)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0144 NEW)

set(ortools_DIR "~/or-tools/lib/cmake/ortools")
find_package(ortools CONFIG REQUIRED)


add_executable(swerc_inflate_cut swerc_inflate_cut.c)

add_executable(grid_new gridnew.c)

add_executable(draw_orthopol draw_orthopol.c)

add_executable(EXE main.cpp DCEL.cpp solver.cpp)
target_link_libraries(EXE PRIVATE ortools::ortools)
target_link_directories(EXE BEFORE PRIVATE
  ~/or-tools/lib
)
set_target_properties(EXE PROPERTIES
    BUILD_RPATH "~/or-tools/lib"
    INSTALL_RPATH "~/or-tools/lib"
)
