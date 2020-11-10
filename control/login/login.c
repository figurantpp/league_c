

#include <mysql/mysql.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "login.h"
#include "database/database.h"
#include "zalloc/zalloc.h"
#include "stream/stream.h"

#define HERO_IMAGE_FILE_PATH "/home/figurantpp/Desktop/programming/c/league/art"


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

        char *codename;

        hero->id = strdup(row[0]);
        if (!hero->id) { goto invalid; }

        hero->accessCode = strdup(row[1]);
        if (!hero->accessCode) { goto invalid; }

        hero->username = strdup(row[2]);
        if (!hero->username) { goto invalid; }

        // The caller application doesn't need the user's codename to work, but we need it to display the user's
        // image

        codename = strdup(row[3]);
        if (!codename) { goto invalid; }

        mysql_free_result(result);
        result = NULL;

        ////////////////////////
        // display hero image //
        ////////////////////////

        // TODO: Move to another place

        /* Text Processing */

        char *position;

        // Replacing spaces with _
        while ((position = strchr(codename, ' ')))
        { *position = '_'; }


        // Lower casing

        // narrowing is ok here because `man tolower` says:
        // > If c is neither an unsigned char value nor EOF, the  behavior  of these functions is undefined.
        for (position = codename; *position; position++)
        { *position = tolower((unsigned char) *position); }// NOLINT(cppcoreguidelines-narrowing-conversions)

        //  + extra /
        //  + extra '\0'
        size_t image_path_size = strlen(HERO_IMAGE_FILE_PATH);
        size_t file_path_size = image_path_size + strlen(hero->username) + 2;

        char *file_path = zmalloc(file_path_size);

        snprintf(file_path, file_path_size, "%s/%s", HERO_IMAGE_FILE_PATH, codename);

        FILE *file = fopen(file_path, "r");

        if (file == NULL)
        {
            printw("Error while opening file: '%s'\n", strerror(errno));

            goto invalid;
        }

        char *welcome = figlet("Welcome");

        if (welcome)
        {
            size_t welcome_size = strlen(welcome);
            size_t ascii_art_size;

            char *ascii_art = read_whole_file(file, &ascii_art_size);

            buffer = zmalloc(welcome_size + ascii_art_size + 1);
            snprintf(buffer, welcome_size + ascii_art_size, "%s%s", welcome, ascii_art);

            write_center(buffer);

            addch('\n');

            free(welcome);
            free(ascii_art);
            free(buffer);
        }
        else
        {
            printw("Failed to print message");
        }

        refresh();

        fclose(file);

        /**/

        free(codename);
        free(file_path);

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
        free(hero->id);
        free(hero->accessCode);
        free(hero->username);
        free(hero);
    }
}
