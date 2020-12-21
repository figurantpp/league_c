
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

const char *database_get_last_insertion_id(MYSQL *connection, char **id_pointer)
{
    if (connection == NULL)
    {
        return "database_get_last_insertion_id: NULL MYSQL* given";
    }
    else if (id_pointer == NULL)
    {
        return "database_get_last_insertion_id: NULL parameter \"id_pointer\" given";
    }

    UNIQUE_POINTER(MYSQL_STMT)statement = mysql_stmt_init(connection);

    if (!statement)
    {
        return "Not enough memory to initialize selection";
    }

    char *format = "select last_insert_id() as 'ID'";

    if (mysql_stmt_prepare(statement, format, strlen(format)) != 0)
    {
        return mysql_stmt_error(statement);
    }

    if (mysql_stmt_param_count(statement) != 0)
    {
        return "MySql Invalid Argument count";
    }

    // We first execute then grab the results

    if (mysql_stmt_execute(statement) != 0)
    {
        return mysql_stmt_error(statement);
    }

    MYSQL_BIND parameters[1] = {};

    char id[64] = {};

    parameters->buffer_type = MYSQL_TYPE_STRING;
    parameters->buffer = id;
    parameters->buffer_length = sizeof(id) / sizeof(char);
    parameters->is_unsigned = true;

    if (mysql_stmt_bind_result(statement, parameters) != 0)
    {
        return "Error while binding: %s\n";
    }

    if (mysql_stmt_store_result(statement) != 0)
    {
        return "Failed to store statement result: ";
    }

    if (mysql_stmt_fetch(statement) != 0)
    {
        return "Failed to get insertion ID";
    }

    *id_pointer = strdup(id);

    if (*id_pointer == NULL)
    {
        return "Not enough memory";
    }

    return NULL;
}
