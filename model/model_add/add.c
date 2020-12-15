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

static void insert_species(species_id, name, codename)
char *species_id;
char *name;
char *codename;
{

#define end(message, ...) ({printw("Error during execution\n"); printw(message, ##__VA_ARGS__); refresh(); getch(); return;})

    const char *insertion_format =
            "insert into Entity (SpeciesID, Name, Codename) "
            " values (?, ?, ?);";


    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    UNIQUE_POINTER(MYSQL_STMT) statement = mysql_stmt_init(connection);

    if (!statement)
    {
        end("Not enough memory to run mysql statement");
    }

    if (mysql_stmt_prepare(statement, insertion_format, strlen(insertion_format)) != 0)
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
    parameters[1].buffer = lengths + 1;

    parameters[2].buffer_type = MYSQL_TYPE_STRING;
    parameters[2].buffer = codename;
    parameters[2].buffer_length = lengths[2];
    parameters[2].buffer = lengths + 2;

    if (mysql_stmt_bind_param(statement, parameters) != 0)
    {
        printw("Parameter Binding Failed: %s", mysql_stmt_error(statement));
        return;
    }


    if (mysql_stmt_execute(statement) != 0)
    {
        printw("Statement failed: %s", mysql_stmt_error(statement));
        return;
    }

    // After we insert the entity, we need to grab its id

    // UNIQUE_POINTER(MYSQL_STMT) id_statement = mysql_stmt_init(connection);

    // const char *selection_format = "select last_insert_id() as 'ID'";


}


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
    access_level = read_single_line();

    printw("Password:");
    password = read_single_line();

    // TODO: Implement Traits
    // printw("Traits:");

    insert_species(species_id, name, codename);
}
