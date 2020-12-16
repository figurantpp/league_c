//
// Created by figurantpp on 14/12/2020.
//

#include "add.h"
#include <ncurses.h>
#include <string.h>

#include <database/database.h>
#include <stream/stream.h>
#include <fullmacro/deconstruct.h>
#include <zalloc/zalloc.h>

#define end(message, ...) \
    ({printw("Error during insertion execution.\n"); printw(message, ##__VA_ARGS__); refresh(); getch(); return -1;})

/**
 * Inserts an entity with the given data and return its ID.
 * @param connection The connection to operate on
 * @param species_id  The species ID of the entity
 * @param name The entity name
 * @param codename The entity codename
 * @return The entity's ID or -1 if the insertion failed
 */
static long insert_entity(MYSQL* connection, char *species_id, char *name, char *codename)
{


    const char *format =
            "insert into Entity (SpeciesID, Name, Codename) "
            " values (?, ?, ?);";



    UNIQUE_POINTER(MYSQL_STMT) statement = mysql_stmt_init(connection);

    if (!statement)
    {
        end("Not enough memory to run mysql statement");
    }

    if (mysql_stmt_prepare(statement, format, strlen(format)) != 0)
    {
        end("Mysql Error: %s", mysql_error(connection));
    }

    unsigned long parameter_count = mysql_stmt_param_count(statement);

    if (parameter_count != 3)
    {
        end("MySql Bad parameter count");
    }


    MYSQL_BIND parameters[3];
    memset(parameters, 0, sizeof(parameters));

    unsigned long lengths[3] = { strlen(species_id), strlen(name), strlen(codename) };

    parameters[0].buffer_type = MYSQL_TYPE_STRING;
    parameters[0].buffer = species_id;
    parameters[0].buffer_length = lengths[0];
    parameters[0].length = lengths;

    parameters[1].buffer_type = MYSQL_TYPE_STRING;
    parameters[1].buffer = name;
    parameters[1].buffer_length = lengths[1];
    parameters[1].length = lengths + 1;

    parameters[2].buffer_type = MYSQL_TYPE_STRING;
    parameters[2].buffer = codename;
    parameters[2].buffer_length = lengths[2];
    parameters[2].length = lengths + 2;

    if (mysql_stmt_bind_param(statement, parameters) != 0)
    {
        end("Parameter Binding Failed: %s", mysql_stmt_error(statement));
    }

    if (mysql_stmt_execute(statement) != 0)
    {
        end("Statement failed: %s", mysql_stmt_error(statement));
    }

    // After we insert the entity, we need to grab its id


    mysql_stmt_close(statement);

    statement = NULL;

    statement = mysql_stmt_init(connection);

    if (!statement)
    {
        end("Not enough memory to initialize selection");
    }

    format = "select last_insert_id() as 'ID'";

    if (mysql_stmt_prepare(statement, format, strlen(format)) != 0)
    {
        end("Formatting failed: %s", mysql_stmt_error(statement));
    }

    if (mysql_stmt_param_count(statement) != 0)
    {
        end("MySql Invalid Argument count");
    }

    // We first execute then grab the results

    if (mysql_stmt_execute(statement) != 0)
    {
        end("Statement Error: %s", mysql_stmt_error(statement));
    }

    memset(parameters, 0, sizeof(*parameters));

    unsigned int id;

    parameters->buffer_type = MYSQL_TYPE_LONG;
    parameters->buffer = &id;
    parameters->is_unsigned = true;

    if (mysql_stmt_bind_result(statement, parameters) != 0)
    {
        end("Error while binding: %s\n", mysql_stmt_error(statement));
    }

    if (mysql_stmt_store_result(statement) != 0)
    {
        end("Failed to store statement result: ", mysql_stmt_error(statement));
    }

    if (mysql_stmt_fetch(statement) != 0)
    {
        end("Failed to get insertion ID");
    }

    return id;
}



int perform_insertion(MYSQL *connection, const char *query, const char ** const arguments)
{
    size_t count = 0;

    for (const char** i = arguments; *i++; count++);


    UNIQUE_POINTER(MYSQL_STMT) statement = mysql_stmt_init(connection);

    if (!statement)
    {
        end("Statement Allocation Failed");
    }

    if (!mysql_stmt_prepare(statement, query, strlen(query)))
    {
        end("Statement Preparation Failed: %s", mysql_stmt_error(statement));
    }

    if (mysql_stmt_param_count(statement) > count)
    {
        end("MYSQL query has more statements then the provided amount");
    }

    AUTO_FREE MYSQL_BIND* binding = zcalloc(count , sizeof(MYSQL_BIND));

    for (size_t i = 0; i < count; i++)
    {
        binding[i].buffer_type = MYSQL_TYPE_STRING;
        binding[i].buffer = (char*) arguments[i];
        binding[i].buffer_length = strlen(arguments[i]);
    }

    if (mysql_stmt_bind_param(statement, binding))
    {
        end("Statement Binding Failed: %s", mysql_stmt_error(statement));
    }

    if (mysql_stmt_execute(statement))
    {
        end("Statement Execution Failed: %s", mysql_stmt_error(statement));
    }

    return 0;
}

#undef end
#define end(message, ...) \
    ({printw("Error during insertion execution.\n"); printw(message, ##__VA_ARGS__); refresh(); getch(); return;})

void add_hero()
{
    AUTO_FREE char *name  = NULL;
    AUTO_FREE char *codename  = NULL;
    AUTO_FREE char *species_id  = NULL;
    AUTO_FREE char *access_level = NULL;
    AUTO_FREE char *username = NULL;
    AUTO_FREE char *password = NULL;

    printw("Name:");
    name = read_single_line();

    printw("Codename:");
    codename = read_single_line();

    printw("Species ID:");
    species_id = read_single_line();

    printw("Access Level:");
    access_level = read_single_line();

    printw("Username:");
    username = read_single_line();

    printw("Password:");
    password = read_single_line();

    // TODO: Implement Traits
    // printw("Traits:");

    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    long entity_id = insert_entity(connection, species_id, name, codename);

    if (entity_id == -1)
    {
        end("Insertion Failed");
    }

    mysql_close(connection);

    connection = database_connect();

    char *format = "insert into Hero (EntityID, AccessCode, Username, Password) values (?, ?, ?, ?)";

    UNIQUE_POINTER(MYSQL_STMT) statement = mysql_stmt_init(connection);

    if (statement == NULL)
    {
        end("Not enough memory");
    }

    if (mysql_stmt_prepare(statement, format, strlen(format)) != 0)
    {
        end("Statement preparation failed: %s", mysql_stmt_error(statement));
    }

     if (mysql_stmt_param_count(statement) != 4)
     {
         end("Invalid State: Illegal Argument Count");
     }

    MYSQL_BIND parameters[4];
    memset(parameters, 0, sizeof(parameters));

    unsigned long sizes[3] = {strlen(access_level), strlen(username), strlen(password)};

    // EntityID: int
    parameters[0].buffer_type = MYSQL_TYPE_LONG;
    parameters[0].buffer = &entity_id;

    // AccessLevel: long
    parameters[1].buffer_type = MYSQL_TYPE_STRING;
    parameters[1].buffer = access_level;
    parameters[1].buffer_length = sizes[0];

    // Username: string
    parameters[2].buffer_type = MYSQL_TYPE_STRING;
    parameters[2].buffer = username;
    parameters[2].buffer_length = sizes[1];

    // Password: string
    parameters[3].buffer_type = MYSQL_TYPE_STRING;
    parameters[3].buffer = password;
    parameters[3].buffer_length = sizes[2];

    if (mysql_stmt_bind_param(statement, parameters) != 0)
    {
        end("Parameter binding failed: %s", mysql_stmt_error(statement));
    }

    if (mysql_stmt_execute(statement) != 0)
    {
        end("Execution failed: %s", mysql_stmt_error(statement));
    }

}
