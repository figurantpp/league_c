
#include <mysql/mysql.h>
#include <ncurses.h>

#include <zalloc/zalloc.h>
#include <fullmacro/deconstruct.h>
#include "database/database.h"

#include "util/util.h"

#define HERO_SOURCE_FILE
#include "Hero.h"
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
        printw("Execution failed: %s\n", mysql_error(connection) );
    }


    UNIQUE_POINTER(MYSQL_RES) result = mysql_store_result(connection);

    UNIQUE_POINTER(StructHeroData) data = new_hero();

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        printw("--- --- ---\n");

        hero_from_row(data, row);

        hero_display(data);
    }

    memset(data, 0, sizeof(struct HeroData));

    refresh();

    getch();



}