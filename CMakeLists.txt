cmake_minimum_required(VERSION 3.17)
project(league C)

set(CMAKE_C_STANDARD 11)

# Preprocessor
# set(CMAKE_C_FLAGS "-E")
set(CMAKE_C_FLAGS "-Wall")

include_directories(control)

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
        control/database/database.h
        control/database/database.c
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
        model/hero/Hero.c
        model/hero/Hero.h
        model/hero/Entity.c model/hero/Entity.h
)

add_library(menu
        STATIC
        control/menu/menu.c
        control/menu/menu.h
)

add_library(fullmacro
        STATIC
        control/fullmacro/fullmacro.c control/fullmacro/deconstruct.h)

target_link_libraries(league -lncurses)
target_link_libraries(league -lmariadb)

target_link_libraries(league config)
target_link_libraries(league login)
target_link_libraries(league zalloc)
target_link_libraries(league database)
target_link_libraries(league stream)
target_link_libraries(league menu)
target_link_libraries(league model)
