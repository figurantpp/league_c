

#include <mysql/mysql.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "login.h"
#include "database/database.h"
#include "zalloc/zalloc.h"
#include "stream/stream.h"


void *login_perform(char *username, char *password)
{
    MYSQL* connection = database_get_connection();

    char *format = "call LoginOf('%s', '%s');";

    // format_len - two %s specifiers, + username + password
    size_t size = strlen(format) - 4 + strlen(username) + strlen(password) + 1;

    char *buffer = zmalloc(size);

    snprintf(buffer, size, format, username, password);

    buffer[size - 1] = '\0';

    if (mysql_query(connection, buffer) != 0)
    {
        printw("Execution failed: %s\n", mysql_error(connection));
        refresh();
        free(buffer);
        return NULL;
    }

    free(buffer);

    MYSQL_RES *result = mysql_store_result(connection);

    MYSQL_ROW row;

    if ((row = mysql_fetch_row(result)))
    {
        struct HeroLogin* hero = zmalloc(sizeof(struct HeroLogin));

        hero->id = strdup(row[0]);
        if (!hero->id) { goto invalid; }

        hero->accessCode = strdup(row[1]);
        if (!hero->accessCode) { goto invalid; }

        hero->username = strdup(row[2]);
        if (!hero->username) { goto invalid; }


        mysql_free_result(result);
        return hero;

        invalid:

        printw("Invalid State");

        hero_login_delete(hero);

        mysql_free_result(result);

        return NULL;
    }
    else
    {
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


        mysql_free_result(result);

        return NULL;

    }
    
}


void hero_login_delete(struct HeroLogin* hero)
{
    if (hero)
    {
        free(hero->username);
        free(hero);
    }
}
