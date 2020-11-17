
#include <mysql/mysql.h>
#include <string.h>
#include <ncurses.h>

#include <zalloc/zalloc.h>
#include "database/database.h"

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

void hero_display(struct HeroData *data)
{
#define SHOW(type, name) \
    printw(#name ": %s\n", data->name);

    HERO_DATA(SHOW)
#undef SHOW
}

void hero_from_row(struct HeroData *data, MYSQL_ROW* row)
{
    int i = 0;

#define GET(type, name) data->name = strdup(*row[i++]);
    HERO_DATA(GET)
#undef GET

}

void show_heroes()
{
    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    if (mysql_query(connection, "select * from HeroData;") != 0)
    {
        printw("Execution failed: %s\n", mysql_error(connection) );
    }


    MYSQL_RES *result = mysql_store_result(connection);

    struct HeroData *data = new_hero();

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        hero_from_row(data, &row);

        hero_display(data);
    }

    mysql_free_result(result);

    refresh();

    getch();

    hero_delete(data);


}