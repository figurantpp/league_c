cmake_minimum_required(VERSION 3.17)
project(league C)

set(CMAKE_C_STANDARD 99)

# Preprocessor
# set(CMAKE_C_FLAGS "-E")

set(CMAKE_C_FLAGS "-Wall -Wno-unused-function")


include_directories(control)
include_directories(model)

add_executable(league main.c control/config/config.c)

add_library(
        zalloc
        STATIC
        control/zalloc/zalloc.h
        control/zalloc/zalloc.c
)

add_library(
        config
        STATIC
        control/config/config.h
        control/config/config.c
)

add_library(
        login
        STATIC
        control/login/login.h
        control/login/login.c
)

add_library(
        database
        STATIC
        control/database/operations/operations.c
        control/database/database.c
        control/database/database.h
        control/database/operations/operations.h
)

add_library(
        stream
        STATIC
        control/stream/stream.h
        control/stream/stream.c
)

add_library(
        model
        STATIC
        model/model_display/display.c
        model/model_display/display.h
        model/model_add/add.c model/model_add/add.h
        model/model_utils/util.c
        model/model_utils/util.h)

add_library(menu
        STATIC
        control/menu/menu.c
        control/menu/menu.h
        )

add_library(
        iterator
        STATIC
        control/c-iterator/iterator.h
        control/c-iterator/iterator.c
        control/c-iterator/implementations/track/track.c
        control/c-iterator/implementations/track/track.h
        control/c-iterator/implementations/variadic/variadic.c
        control/c-iterator/implementations/variadic/variadic.h
)

add_library(
        util
        control/util/util.c
        control/util/util.h
)

target_link_libraries(league -lncurses)
target_link_libraries(league -lmariadb)

target_link_libraries(league config)
target_link_libraries(league login)
target_link_libraries(league zalloc)
target_link_libraries(league database)
target_link_libraries(league stream)
target_link_libraries(league menu)
target_link_libraries(league model)
target_link_libraries(league util)
target_link_libraries(league iterator)
