//
// Created by figurantpp on 10/11/2020.
//

#ifndef LEAGUE_MENU_H
#define LEAGUE_MENU_H

#include "login/login.h"

struct MenuOption
{
    char *key;
    char *name;
    void (*function)();
    int min_access_level;
};

void setup_menu();

_Noreturn void show_menu(struct HeroLogin *hero);

#endif //LEAGUE_MENU_H
