
#include <mysql/mysql.h>
#include <ncurses.h>

#include <zalloc/zalloc.h>
#include <fullmacro/deconstruct.h>
#include "database/database.h"

#include "util/util.h"

#define HERO_SOURCE_FILE

#include "display.h"

#undef HERO_SOURCE_FILE


static struct HeroData *new_hero()
{
    return zcalloc(1, sizeof(struct HeroData));
}

static void hero_delete(struct HeroData *data)
{
    if (data)
    {

#define FREE(type, name) free(data->name);
        HERO_DATA(FREE)
#undef FREE

        free(data);
    }
}

SET_POINTER_DECONSTRUCTOR_STATIC(StructHeroData, hero_delete)

void hero_display(struct HeroData *data)
{
#define SHOW(type, name) \
    printw(#name ": %s", data->name); \
    addch('\n');         \
    refresh();

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

static void hero_copy(struct HeroData *target, struct HeroData *source)
{
#define COPY(t, name) { target->name = source->name; }
    HERO_DATA(COPY)
#undef COPY
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


void display_data(char *table, struct Iterator *iterator)
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

        int i;
        char *column;

        FOR_EACH(column, iterator)
        {
            printw("%10s: %s\n", column, row[i++]);
        }

        for (size_t i = 0; i < total_columns; i++)
        {
        }
    }

    refresh();

    getch();
}

void args_display_data_(char *table, ...)
{
    va_list args;
    va_start(args, table);
}

void show_attacks()
{
    const char *labels[] = {
            "ID",
            "Date",
            "Location",
            NULL
    };

    display_data("AttackData", labels);
}

void show_villains()
{
    const char *labels[] = {
            "ID",
            "Name",
            "Codename",
            "Species",
            "Species",
            "Rival",
            NULL
    };

    display_data("VillainData", labels);
}

void show_traits()
{
    const char *labels[] = {
            "Name",
            "Kind",
            NULL
    };

    display_data("TraitData", labels);
}

void show_equipments()
{

    const char *labels[] = {
            "Name",
            "Description",
            "Utility",
            NULL
    };
    display_data("EquipmentData", labels);
}

void show_species()
{
    const char *labels[] = {
            "Name",
            "ScientificName",
            NULL
    };

    display_data("SpeciesData", labels);
}

void show_hidings()
{
    const char *labels[] = {
            "ID",
            "Owner",
            "Location",
            NULL
    };


    display_data("Hiding", labels);
}
