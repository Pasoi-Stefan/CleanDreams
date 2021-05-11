cmake_minimum_required(VERSION 3.16)
project(cleandreams_washing_machine)

set(CMAKE_CXX_STANDARD 17)

# https://github.com/pistacheio/pistache#cmake - CMake configuration for Pistache
find_package(PkgConfig)
pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)

# https://github.com/nlohmann/json#cmake - CMake configuration for nlohmann_json
find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(${PROJECT_NAME} cleandreams_washing_machine.cpp includes/WashingMachine.cpp includes/WashingMachine.h includes/ServerEndpoint.cpp includes/ServerEndpoint.h includes/WashingProgram.cpp includes/WashingProgram.h)
target_link_libraries(${PROJECT_NAME} PkgConfig::Pistache nlohmann_json::nlohmann_json)