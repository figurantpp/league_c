

#include <mysql/mysql.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "login.h"
#include "database/database.h"
#include "zalloc/zalloc.h"
#include "stream/stream.h"

#include "fullmacro/deconstruct.h"

#define HERO_IMAGE_FILE_PATH "/home/figurantpp/Desktop/programming/c/league/art"


SET_POINTER_DECONSTRUCTOR(StructHeroLogin, hero_login_delete)
SET_POINTER_DECONSTRUCTOR_STATIC(FILE, fclose)

static void display_hero_image(char *codename)
{
    // TODO: Move ascii part to another place

    // The path is defined by imagepath + '/' + name.lower().replace(' ', '_')

    /* Text Processing */

    char *position;

    AUTO_FREE char *buffer = NULL;

    AUTO_FREE char *file_path = NULL;

    FILE* file = NULL;

    AUTO_FREE char *message = NULL;

    // Replacing spaces with _
    while ((position = strchr(codename, ' ')))
    { *position = '_'; }

    // Lower casing
    for (position = codename; *position; position++)
    { *position = (char) tolower((unsigned char) *position); }

    //  + extra /
    //  + extra '\0'
    size_t image_path_size = strlen(HERO_IMAGE_FILE_PATH);
    size_t file_path_size = image_path_size + strlen(codename) + 2;

    file_path = zmalloc(file_path_size);

    snprintf(file_path, file_path_size, "%s/%s", HERO_IMAGE_FILE_PATH, codename);

    file = fopen(file_path, "r");

    if (file == NULL)
    {
        printw("Error while opening file: '%s'\n", strerror(errno));
        return;
    }

    message = figlet("Welcome");

    if (!message)
    {
        printw("Call to figlet failed");
        refresh();
        return;
    }

    size_t message_size = strlen(message);
    size_t ascii_art_size;

    AUTO_FREE char *ascii_art = read_whole_file(file, &ascii_art_size);

    if (!ascii_art)
    {
        goto error;
    }

    buffer = zmalloc(message_size + ascii_art_size + 1);
    snprintf(buffer, message_size + ascii_art_size, "%s%s", message, ascii_art);

    write_center(buffer);

    addch('\n');

    refresh();

    getch();

    clear();

    fclose(file);

    return;

    error:

    printw("Failed to print message.");
}

void *login_perform(char *username, char *password)
{
    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    char *format = "call LoginOf('%s', '%s');";

    // format length - two %s specifiers, + username + password + '\0'
    size_t size = strlen(format) - 4 + strlen(username) + strlen(password) + 1;

    AUTO_FREE char *buffer = zmalloc(size);

    snprintf(buffer, size, format, username, password);

    buffer[size - 1] = '\0';

    if (mysql_query(connection, buffer) != 0)
    {
        printw("Execution failed: %s\n", mysql_error(connection));
        refresh();
        return NULL;
    }


    MYSQL_RES *result = mysql_use_result(connection);

    MYSQL_ROW row;

    if ((row = mysql_fetch_row(result)))
    {
        struct HeroLogin *hero = zmalloc(sizeof(struct HeroLogin));

        AUTO_FREE char *codename = NULL;

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
        result = NULL;

        display_hero_image(codename);


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

        AUTO_FREE char *message = figlet("Error");
        write_center(message);

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

void hero_login_delete(struct HeroLogin *hero)
{
    if (hero)
    {
        free(hero->id);
        free(hero->username);
        free(hero);
    }
}
