
#include <mysql/mysql.h>
#include <stdlib.h>
#include <ncurses.h>

#include "database.h"
#include "config/config.h"
#include "fullmacro/deconstruct.h"

SET_POINTER_DECONSTRUCTOR(MYSQL_RES, mysql_free_result)
SET_POINTER_DECONSTRUCTOR(MYSQL_STMT, mysql_stmt_close)

void database_setup()
{}

void database_close(MYSQL **connection)
{
    mysql_close(*connection);
}


void database_cleanup()
{
    mysql_library_end();
}

MYSQL *database_connect()
{
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        printw("Not enough memory to start mysql connection");

        refresh();

        exit(CONFIG_EXIT_DATABASE_ERROR);
    }

    if (mysql_real_connect(connection, "localhost", "figurantpp", "beep", "leagueDB", 3600, NULL, 0))
    {
        return connection;
    }
    else
    {
        printw("Connection to database failed: %s", mysql_error(connection));

        refresh();

        mysql_close(connection);

        exit(CONFIG_EXIT_DATABASE_ERROR);
    }
}