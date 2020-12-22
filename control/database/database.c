
#include <mysql/mysql.h>
#include <stdlib.h>
#include <ncurses.h>

#include "database.h"
#include "config/config.h"

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

static char global_pool[1024];
static const size_t global_pool_size = 1024;

#include <string.h>
#include <zalloc/zalloc.h>

// Not thread safe: uses global_pool
const char* database_execute_command(MYSQL *connection, const char *query, const char ** const arguments)
{
    size_t count = 0;

    if (arguments)
    {
        for (const char** i = arguments; *i++; count++);
    }

    MYSQL_STMT* statement = mysql_stmt_init(connection);

    if (!statement)
    {
        return "Statement Allocation Failed";
    }

    if (mysql_stmt_prepare(statement, query, strlen(query)))
    {
        mysql_stmt_close(statement);
        return mysql_stmt_error(statement);
    }

    size_t expected = mysql_stmt_param_count(statement);

    if (expected > count)
    {
        mysql_stmt_close(statement);

        char *format = "MYSQL query has more statements then the provided amount (expected: %lu, provided: %lu)";
        char *substitute_format = "MYSQL query has more statements then the provided amount.";
        char *result;

        size_t size = strlen(format) + (sizeof(unsigned long) / 3) * 2;

        if (size < global_pool_size)
        {
            result = global_pool;
            snprintf(result, size, format, expected, count);
        }
        else
        {
            result = substitute_format;
        }

        return result;
    }

    if (arguments)
    {
        MYSQL_BIND *binding = zcalloc(count, sizeof(MYSQL_BIND));

        for (size_t i = 0; i < count; i++)
        {
            binding[i].buffer_type = MYSQL_TYPE_STRING;
            binding[i].buffer = (char *) arguments[i];
            binding[i].buffer_length = strlen(arguments[i]);
        }

        if (mysql_stmt_bind_param(statement, binding))
        {
            free(binding);
            return mysql_stmt_error(statement);
        }

        free(binding);
    }

    if (mysql_stmt_execute(statement))
    {
        char *buffer = global_pool;

        strcpy(buffer, mysql_stmt_error(statement));

        mysql_stmt_close(statement);

        return buffer;
    }

    mysql_stmt_close(statement);

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

    MYSQL_STMT* statement = mysql_stmt_init(connection);

    if (!statement)
    {
        return "Not enough memory to initialize selection";
    }

    char *format = "select last_insert_id() as 'ID'";

    if (mysql_stmt_prepare(statement, format, strlen(format)) != 0)
    {
        mysql_stmt_close(statement);
        return mysql_stmt_error(statement);
    }

    if (mysql_stmt_param_count(statement) != 0)
    {
        mysql_stmt_close(statement);
        return "MySql Invalid Argument count";
    }

    // We first execute then grab the results

    if (mysql_stmt_execute(statement) != 0)
    {
        mysql_stmt_close(statement);
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
        mysql_stmt_close(statement);
        return "Error while binding: %s\n";
    }

    if (mysql_stmt_store_result(statement) != 0)
    {
        mysql_stmt_close(statement);
        return "Failed to store statement result: ";
    }

    if (mysql_stmt_fetch(statement) != 0)
    {
        mysql_stmt_close(statement);
        return "Failed to get insertion ID";
    }

    *id_pointer = strdup(id);

    mysql_stmt_close(statement);

    if (*id_pointer == NULL)
    {
        return "Not enough memory";
    }

    return NULL;
}
