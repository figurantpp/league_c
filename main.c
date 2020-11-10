
#include <ncurses.h>

#include "zalloc/zalloc.h"
#include "config/config.h"
#include "login/login.h"
#include "stream/stream.h"


struct MenuOption
{
    char *key;
    char *name;
    void (*function)();
};


int main()
{
    config_setup();

    printw("Authentication is required.\n");

    printw("Username: ");
    char *username = read_single_line();

    noecho();

    printw("Password: ");
    char *password = read_single_line();

    echo();

    struct HeroLogin *hero = login_perform(username, password);

    if (hero)
    {
        printw("Username: '%s'\nAccessCode: '%s'\nID:%s", hero->username, hero->accessCode, hero->id);

        refresh();
        getch();
    }

    hero_login_delete(hero);
    free(username);
    free(password);
}
