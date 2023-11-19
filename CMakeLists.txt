cmake_minimum_required(VERSION 3.1)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

project("game_engine" VERSION 1.0.0)


file(GLOB_RECURSE source_list "src/*.cpp")
file(GLOB_RECURSE source_list_c "src/*.c")
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)
add_executable(out ${source_list} ${source_list_c})
target_link_libraries(out opengl32)