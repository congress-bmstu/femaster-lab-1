cmake_minimum_required(VERSION 3.22)

##############
### GLOBAL ###
##############

project(cpp_project)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -g")

find_package(Eigen3 REQUIRED)
find_package(yaml-cpp REQUIRED)

file(GLOB APP_HEADER_FILES "src/*.h")
file(GLOB APP_SOURCE_FILES "src/*.cpp")

add_executable(cpp_project ${APP_HEADER_FILES} ${APP_SOURCE_FILES})
target_include_directories(cpp_project PUBLIC src/ ${EIGEN3_INCLUDE_DIR})
target_link_libraries(cpp_project PRIVATE Eigen3::Eigen yaml-cpp)
