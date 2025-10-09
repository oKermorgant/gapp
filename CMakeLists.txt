cmake_minimum_required(VERSION 3.5)
project(gapp)


add_compile_options(-pthread)
find_package(yaml-cpp REQUIRED)
find_package(TBB COMPONENTS tbb REQUIRED)
set(CMAKE_CXX_STANDARD 20)

add_definitions(-DTSP_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
add_executable(tsp main/tsp.cpp include/travel.h src/travel.cpp include/gapp/galg.h)
target_include_directories(tsp PRIVATE include ${YamlCpp_INCLUDE_DIRS})
target_link_libraries(tsp yaml-cpp tbb)

install (FILES include/gapp/galg.h DESTINATION include/${PROJECT_NAME})
