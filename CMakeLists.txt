cmake_minimum_required(VERSION 3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_STATIC "Build static executable" OFF)

project("firewall-ipset" LANGUAGES C)

if(BUILD_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(SUFFIX _STATIC)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(IPSET libipset REQUIRED)
pkg_check_modules(CJSON libcjson REQUIRED)

function(create_imp_target pfx)
    if (NOT TARGET PkgCfg::${pfx})
        add_library(PkgCfg::${pfx} INTERFACE IMPORTED)

        if(${pfx}_INCLUDE_DIRS)
            set_property(TARGET PkgCfg::${pfx} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${${pfx}_INCLUDE_DIRS}")
        endif()
        if(${pfx}_LIBRARIES)
            set_property(TARGET PkgCfg::${pfx} PROPERTY INTERFACE_LINK_LIBRARIES "${${pfx}_LIBRARIES}")
        endif()
        if(${pfx}_LDFLAGS_OTHER)
            set_property(TARGET PkgCfg::${pfx} PROPERTY INTERFACE_LINK_OPTIONS "${${pfx}_LDFLAGS_OTHER}")
        endif()
        if(${pfx}_CFLAGS_OTHER)
            set_property(TARGET PkgCfg::${pfx} PROPERTY INTERFACE_COMPILE_OPTIONS "${${pfx}_CFLAGS_OTHER}")
        endif()
    endif()
endfunction()

create_imp_target("IPSET${SUFFIX}")
create_imp_target("CJSON${SUFFIX}")

add_executable("firewall-ipset" src/main.c src/commands.c src/json.c src/ipset.c src/protocol.c src/utils.c src/wazuh.c)
target_link_libraries("firewall-ipset" PRIVATE PkgCfg::IPSET${SUFFIX} PkgCfg::CJSON${SUFFIX})
