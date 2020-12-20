
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

#include <string.h>
#include <zalloc/zalloc.h>

const char* database_execute_command(MYSQL *connection, const char *query, const char ** const arguments)
{
    size_t count = 0;

    if (arguments)
    {
        for (const char** i = arguments; *i++; count++);
    }

    UNIQUE_POINTER(MYSQL_STMT) statement = mysql_stmt_init(connection);

    if (!statement)
    {
        return "Statement Allocation Failed";
    }

    if (mysql_stmt_prepare(statement, query, strlen(query)))
    {
        return mysql_stmt_error(statement);
    }

    size_t expected = mysql_stmt_param_count(statement);

    if (expected > count)
    {
        char *format = "MYSQL query has more statements then the provided amount (expected: %lu, provided: %lu)";

        size_t size = strlen(format) + (sizeof(unsigned long) / 3) * 2;

        char *buffer = zmalloc(size);

        snprintf(buffer, size, format, expected, count);

        return buffer;
    }

    if (arguments)
    {
        AUTO_FREE MYSQL_BIND *binding = zcalloc(count, sizeof(MYSQL_BIND));

        for (size_t i = 0; i < count; i++)
        {
            binding[i].buffer_type = MYSQL_TYPE_STRING;
            binding[i].buffer = (char *) arguments[i];
            binding[i].buffer_length = strlen(arguments[i]);
        }

        if (mysql_stmt_bind_param(statement, binding))
        {
            return mysql_stmt_error(statement);
        }
    }

    if (mysql_stmt_execute(statement))
    {
        const char *result = strdup(mysql_stmt_error(statement));
        return result ?: "Out of memory to allocate statement execution error message";
    }

    return NULL;
}

