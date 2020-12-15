
#include <mysql/mysql.h>
#include <ncurses.h>

#include <zalloc/zalloc.h>
#include <fullmacro/deconstruct.h>
#include "database/database.h"

#include <c-iterator/implementations/variadic/variadic.h>
#include <c-iterator/implementations/track/track.h>

#include "util/util.h"

#define HERO_SOURCE_FILE

#include "display.h"

#undef HERO_SOURCE_FILE



void hero_display(struct HeroData *data)
{
#define SHOW(type, name) \
    printw(#name ": %s", data->name); \
    addch('\n');         \

    HERO_DATA(SHOW)
#undef SHOW

}

static void hero_from_row(struct HeroData *data, MYSQL_ROW row)
{
    int i = 0;

#define GET(type, name) \
    { data->name = (row[i++]); }

    HERO_DATA(GET)
#undef GET
}


void show_heroes()
{
    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    if (mysql_query(connection, "select * from HeroData;") != 0)
    {
        printw("Execution failed: %s\n", mysql_error(connection));
    }


    UNIQUE_POINTER(MYSQL_RES)result = mysql_store_result(connection);

    struct HeroData data = {};

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        printw("--- --- ---\n");

        hero_from_row(&data, row);

        hero_display(&data);
    }

    memset(&data, 0, sizeof(struct HeroData));

    refresh();

    getch();


}


// We don't use an iterator here cause we need to go through more than once
void display_data_buffer(char *table, const char **labels)
{
    char *format = "select * from %s;";

    // size - "%s" + '\0'
    size_t size = strlen(format) + strlen(table) - 2 + 1;

    AUTO_FREE char *message = zmalloc(size);

    snprintf(message, size, format, table);


    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    if (mysql_query(connection, message) != 0)
    {
        printw("Execution failed: %s\n", mysql_error(connection));
    }


    UNIQUE_POINTER(MYSQL_RES)result = mysql_store_result(connection);

    MYSQL_ROW row;


    while ((row = mysql_fetch_row(result)))
    {
        printw("--- --- ---\n");

        int i = 0;
        const char **column = labels;

        while (*column)
        {
            printw("%10s: %s\n", *column++, row[i++]);
        }

    }

    refresh();

    getch();
}

// limit: 20 non null arguments
void display_data(char *table, ...)
{
    va_list args;
    va_start(args, table);


    AUTO_FREE const char **buffer = store_iterator(pointer_va_list_iterator(&args), sizeof(char*), NULL);

    display_data_buffer(table, buffer);
}

void show_villains()
{
    display_data("VillainData", "ID", "Name", "Codename", "Species", "Species", "Rival", NULL);
}

void show_attacks()
{
    display_data("AttackData", "ID", "Date", "Location", NULL);
}


void show_traits()
{
    display_data("TraitData", "Name", "Kind", NULL);
}

void show_equipments()
{
    display_data("EquipmentData", "Name", "Description", "Utility", NULL);
}

void show_species()
{
    display_data("SpeciesData", "Name", "ScientificName", NULL);
}

void show_hidings()
{
    display_data("Hiding", "ID", "Owner", "Location", NULL);
}
