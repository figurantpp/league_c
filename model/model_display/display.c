
#include <mysql/mysql.h>
#include <ncurses.h>

#include <zalloc/zalloc.h>
#include <fullmacro/deconstruct.h>
#include "database/database.h"

#include <c-iterator/implementations/variadic/variadic.h>
#include <c-iterator/implementations/track/track.h>
#include <model_utils/util.h>
#include <string.h>

#include "util/util.h"

#define HERO_SOURCE_FILE

#include "display.h"

#undef HERO_SOURCE_FILE



void hero_display(struct HeroData *data)
{
    AUTO_FREE char *art = hero_image_from_codename(data->codename, NULL);

    printw("%s", art ?: "");

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
            printw("%-10s: %s\n", *column++, row[i++]);
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

static void show_attack_villains(MYSQL *connection, char *id)
{

#define end(message) ({printw("Error while showing villains: %s\n", message); refresh(); return;})

    printw("Villains:\n");

    const char *query = "select * from AttackVillains where ID = ?";

    UNIQUE_POINTER(MYSQL_STMT) statement = mysql_stmt_init(connection);

    if (mysql_stmt_prepare(statement, query, strlen(query)))
    {
        end( mysql_stmt_error(statement));
    }
    size_t count = mysql_stmt_param_count(statement);

    if (count != 1)
    {
        printw("Invalid statement count (query expected %zu, got %zu)\n", count, 1);
        refresh();
        return;
    }

    MYSQL_BIND binding[2] = {};

    MYSQL_BIND *parameters = binding;

    parameters->buffer_type = MYSQL_TYPE_STRING;
    parameters->buffer_length = strlen(id);
    parameters->buffer = id;

    if (mysql_stmt_bind_param(statement, parameters))
    {
        end( mysql_stmt_error(statement));
    }

    if (mysql_stmt_execute(statement))
    {
        end( mysql_stmt_error(statement));
    }

    my_bool is_null[2] = {};
    char villain_id[255] = {};
    char villain_name[255] = {};

    binding[0].buffer_type = binding[1].buffer_type = MYSQL_TYPE_STRING;

    binding[0].buffer = villain_id;
    binding[0].buffer_length = sizeof(villain_id);
    binding[0].is_null = is_null;

    binding[1].buffer = villain_name;
    binding[1].buffer_length = sizeof(villain_name);
    binding[1].is_null = is_null + 1;

    if (mysql_stmt_bind_result(statement, binding))
    {
        end(mysql_stmt_error(statement));
    }

    if (mysql_stmt_store_result(statement))
    {
        end(mysql_stmt_error(statement));
    }

    while (!mysql_stmt_fetch(statement))
    {
        printw("%-10s: %s\n", "ID", is_null[0] ? "NULL" : villain_id);
        printw("%-10s: %s\n", "Codename", is_null[1] ? "NULL" : villain_name);
    }

#undef end

}

void show_attacks()
{
    char *message = "select * from AttackData;";

    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    if (mysql_query(connection, message) != 0)
    {
        printw("Execution failed: %s\n", mysql_error(connection));
    }

    UNIQUE_POINTER(MYSQL_RES) result = mysql_store_result(connection);

    for (MYSQL_ROW row; (row = mysql_fetch_row(result));)
    {
        printw("--- --- ---\n");

        printw("%-10s: %s\n", "ID", row[0]);
        printw("%-10s: %s\n", "Date", row[1]);
        printw("%-10s: %s\n", "Location", row[2]);

        show_attack_villains(connection, row[0]);
    }

    refresh();

    getch();
}


void show_traits()
{
    display_data("TraitData", "ID", "Name", "Kind", NULL);
}

void show_equipments()
{
    display_data("EquipmentData", "ID", "Name", "Description", "Utility", NULL);
}

void show_species()
{
    display_data("SpeciesData", "ID", "Name", "ScientificName", NULL);
}

void show_hidings()
{
    display_data("Hiding", "ID", "Owner", "Location", NULL);
}
