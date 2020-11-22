
#include <ncurses.h>
#include <menu/menu.h>

#include "zalloc/zalloc.h"
#include "login/login.h"
#include "stream/stream.h"

struct HeroLogin *authenticate()
{
    printw("Authentication is required.\n");

    printw("Username: ");
    char *username = "batman01"; // read_single_line();

    noecho();

    printw("Password: ");
    char *password = "beepboop"; // read_single_line();

    echo();

    clear();

    struct HeroLogin *hero = login_perform(username, password);

    if (!hero)
    {
        exit(0);
    }


    return hero;
}

int main()
{
    UNIQUE_POINTER(StructHeroLogin) hero = authenticate();

    show_menu(hero);
}
