cmake_minimum_required(VERSION 3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("firewall-ipset" LANGUAGES C)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(IPSET libipset REQUIRED IMPORTED_TARGET)
find_package(cJSON REQUIRED)

add_executable("firewall-ipset" src/main.c src/commands.c src/json.c src/ipset.c src/protocol.c src/utils.c src/wazuh.c)
target_link_libraries("firewall-ipset" PRIVATE PkgConfig::IPSET cjson)
