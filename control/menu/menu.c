//
// Created by figurantpp on 10/11/2020.
//

#include "menu.h"

#include <stdlib.h>
#include <endian.h>
#include <ncurses.h>
#include <stdint.h>
#include <string.h>

#include "stream/stream.h"

static void add_hero()
{}

static void add_villain()
{}

static void add_attack()
{}

static void add_trait()
{}

static void add_equipment()
{}

static void add_species()
{}

static void add_hiding()
{}

static void finalize()
{
    exit(0);
}

//

void show_heroes();

static void show_villains()
{}

static void show_attacks()
{}

static void show_traits()
{}

static void show_equipment()
{}

static void show_species()
{}

static void show_hiding()
{}



static struct MenuOption MENU_OPTIONS[] = {
        { "1a", "Register a new Hero", add_hero, 10},
        { "1b", "Show Heroes", show_heroes, 3},

        { "2a", "Register a new Villain", add_villain, 9},
        { "2b", "Show Villains", show_villains, 2},

        { "3a", "Register a new Trait", add_trait, 2},
        { "3b", "Show Traits", show_traits, 1},

        { "4a", "Register a new Attack", add_attack, 4},
        { "4b", "Show Attacks", show_attacks, 3},

        { "5a", "Register a new Equipment", add_equipment, 6},
        { "5b", "Show Equipments", show_equipment, 5},

        { "6a", "Register a new Species", add_species, 8},
        { "6b", "Show Species", show_species, 7},

        { "7a", "Register a Hiding", add_hiding, 10},
        { "7b", "Show Hidings", show_hiding, 9},

        { "00", "Exit", finalize, 0},
};

#define MENU_OPTIONS_COUNT (sizeof(MENU_OPTIONS) / sizeof(MENU_OPTIONS[0]))

static struct MenuIndex
{
    struct MenuOption *option;
    struct MenuOption *next;
} INDEXES[MENU_OPTIONS_COUNT];

void setup_menu()
{
    size_t index, i;

    for (i = 0; i < MENU_OPTIONS_COUNT; i++)
    {
        index = (* (uint16_t *) MENU_OPTIONS[i].key) % MENU_OPTIONS_COUNT;

        if (INDEXES[index].option)
        {
            INDEXES[index].next = MENU_OPTIONS + i;
        }
        else
        {
            INDEXES[index].option = MENU_OPTIONS + i;
        }
    }
}

struct MenuOption *get_menu_option(const char input[2])
{
    size_t index = (*(uint16_t*) input) % MENU_OPTIONS_COUNT;

    if ((*(uint16_t*)INDEXES[index].option->key) == *(uint16_t*) input)
    {
        return INDEXES[index].option;
    }
    else
    {
        return INDEXES[index].next;
    }
}


void show_menu(struct HeroLogin *hero)
{
    for (;;)
    {
        clear();

        printw("Current Logged as: %s\n", hero->username);

        for (size_t i = 0; i < MENU_OPTIONS_COUNT; i++)
        {
            struct MenuOption *option = MENU_OPTIONS + i;

            if (option->min_access_level <= hero->accessLevel)
            {
                printw("%s - %s\n", option->key, option->name);
            }

            if ((i - 1) % 2 == 0)
            {
                addch('\n');
            }
        }

        printw("Select Your Option: ");

        char *line = read_single_line();

        if (strlen(line) != 2)
        {
            printw("Invalid Option\n");
            getch();

            goto end;
        }

        struct MenuOption *option = get_menu_option(line);

        if (!option)
        {
            printw("Option was not found\n");
            getch();
            goto end;
        }

        clear();
        option->function();

        end:
        free(line);
    }

}

