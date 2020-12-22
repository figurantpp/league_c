

#include <mysql/mysql.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "login.h"
#include "database/database.h"
#include "zalloc/zalloc.h"
#include "stream/stream.h"

#include "model_utils/util.h"



// DONE
static void display_hero_image(char *codename)
{
    char *message = figlet("Welcome");

    if (!message)
    {
        printw("Call to figlet failed");
        refresh();
        return;
    }

    size_t message_size = strlen(message);
    size_t ascii_art_size;

    char *ascii_art = hero_image_from_codename(codename, &ascii_art_size);

    if (!ascii_art)
    {
        free(message);

        printw("Failed to print message.");
        refresh();
        return;
    }

    char *buffer = zmalloc(message_size + ascii_art_size + 1);
    snprintf(buffer, message_size + ascii_art_size, "%s%s", message, ascii_art);

    free(message);
    free(ascii_art);

    write_center(buffer);

    free(buffer);

    addch('\n');

    refresh();

    getch();

    clear();

}

// WORKING ON
void *login_perform(char *username, char *password)
{
    MYSQL *connection = database_connect();

    const char *format = "call LoginOf('%s', '%s');";

    // format length - two %s specifiers, + username + password + '\0'
    size_t size = strlen(format) - 4 + strlen(username) + strlen(password) + 1;

    // AUTO: Fix
    char *buffer = zmalloc(size);

    snprintf(buffer, size, format, username, password);

    buffer[size - 1] = '\0';

    if (mysql_query(connection, buffer) != 0)
    {
        printw("Execution failed: %s\n", mysql_error(connection));
        refresh();
        return NULL;
    }

    free(buffer);


    MYSQL_RES *result = mysql_use_result(connection);

    MYSQL_ROW row;

    if ((row = mysql_fetch_row(result)))
    {
        struct HeroLogin *hero = zmalloc(sizeof(struct HeroLogin));

        // AUTO: Fix
        char *codename = NULL;

        hero->id = strdup(row[0]);
        if (!hero->id)
        { goto invalid; }

        char *end;

        hero->accessLevel = strtol(row[1], &end, 10);
        if (*end != '\0')
        { goto invalid; }

        hero->username = strdup(row[2]);
        if (!hero->username)
        { goto invalid; }

        // The caller doesn't need the user's codename to work, but we need it to display the user's image

        codename = strdup(row[3]);
        if (!codename)
        { goto invalid; }

        mysql_free_result(result);

        mysql_close(connection);

        result = NULL;

        display_hero_image(codename);

        free(codename);


        return hero;

        invalid:

        printw("Invalid State");

        hero_login_delete(hero);

        mysql_free_result(result);

        mysql_close(connection);

        free(codename);

        return NULL;
    }
    else
    {
        mysql_free_result(result);

        mysql_close(connection);

        clear();

        if (has_colors())
        {
            init_pair(1, COLOR_RED, COLOR_BLACK);
            attron(COLOR_PAIR(1));
        }

        char *message = figlet("Error");
        write_center(message);
        free(message);

        printw("Unauthorized Access; Aborting.\n");

        refresh();
        getch();

        if (has_colors())
        {
            attroff(COLOR_PAIR(1));
        }


        return NULL;
    }
}

void hero_login_delete(struct HeroLogin *hero)
{
    if (hero)
    {
        free(hero->id);
        free(hero->username);
        free(hero);
    }
}
