
#include <mysql/mysql.h>
#include <stdlib.h>
#include <ncurses.h>

#include "database.h"
#include "config/config.h"

static MYSQL* global_connection;

void database_setup()
{
    MYSQL *connection = mysql_init(NULL);

    if (mysql_real_connect(connection, "localhost", "figurantpp", "beep", "leagueDB", 3600, NULL, 0))
    {
        global_connection = connection;
    }
    else
    {
        printw("Connection to database failed: %s", mysql_error(connection));

        refresh();

        mysql_close(connection);

        exit(CONFIG_EXIT_DATABASE_ERROR);

    }
}

void database_cleanup()
{
    mysql_close(global_connection);
    mysql_library_end();
}

MYSQL *database_get_connection()
{
    return global_connection;
}