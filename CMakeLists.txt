project(gapp)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-pthread)
set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_check_modules(YamlCpp yaml-cpp)
find_package (Threads)

add_definitions(-DTSP_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
add_executable(tsp main/tsp.cpp include/travel.h src/travel.cpp include/gapp/galg.h)
target_include_directories(tsp PRIVATE include ${YamlCpp_INCLUDE_DIRS})
target_link_libraries(tsp yaml-cpp ${CMAKE_THREAD_LIBS_INIT})

install (FILES include/gapp/galg.h DESTINATION include/${PROJECT_NAME})
