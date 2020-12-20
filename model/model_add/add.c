//
// Created by figurantpp on 14/12/2020.
//

#include "add.h"
#include <ncurses.h>
#include <string.h>

#include <stream/stream.h>
#include <fullmacro/deconstruct.h>
#include <c-iterator/implementations/track/track.h>
#include <database/operations/operations.h>

#define end(message, ...) \
    ({printw("Insertion failed.\n"); printw(message, ##__VA_ARGS__); refresh(); getch(); return NULL;})

/**
 * Inserts an entity with the given data and return its ID.
 * @param connection The connection to operate on
 * @param species_id  The species ID of the entity
 * @param name The entity name
 * @param codename The entity codename
 * @return The entity's ID or NULL if the insertion failed
 */
static char *insert_entity(MYSQL *connection, const char *species_id, const char *name, const char *codename)
{
    const char *format =
            "insert into Entity (SpeciesID, Name, Codename) "
            " values (?, ?, ?);";

    const char *arguments[4] = {species_id, name, codename, NULL};

    AUTO_FREE const char *message = database_execute_command(connection, format, arguments);

    if (message)
    {
        end("Error: %s", message);
    }

    // After we insert the entity, we need to grab its id


    UNIQUE_POINTER(MYSQL_STMT)statement = mysql_stmt_init(connection);

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

    MYSQL_BIND parameters[1] = {};

    char id[64] = {};

    parameters->buffer_type = MYSQL_TYPE_STRING;
    parameters->buffer = id;
    parameters->buffer_length = sizeof(id) / sizeof(char);
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

    return strdup(id);
}


#undef end
#define end(message, ...) \
    ({printw("Error during insertion execution.\n"); printw(message, ##__VA_ARGS__); refresh(); getch(); return;})

void add_hero()
{
    AUTO_FREE char *name = NULL;
    AUTO_FREE char *codename = NULL;
    AUTO_FREE char *species_id = NULL;
    AUTO_FREE char *access_level = NULL;
    AUTO_FREE char *username = NULL;
    AUTO_FREE char *password = NULL;

    printw("Hero Name:");
    name = read_single_line();

    printw("Hero Codename:");
    codename = read_single_line();

    printw("Hero Species ID:");
    species_id = read_single_line();

    printw("Hero Access Level:");
    access_level = read_single_line();

    printw("Hero Username:");
    username = read_single_line();

    printw("Hero Password:");
    password = read_single_line();

    // TODO: Implement Traits
    // printw("Traits:");

    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    AUTO_FREE char *entity_id = insert_entity(connection, species_id, name, codename);

    if (entity_id == NULL)
    {
        return;
    }

    mysql_close(connection);

    connection = database_connect();

    char *command = "insert into Hero (EntityID, AccessCode, Username, Password) values (?, ?, ?, ?)";

    const char *arguments[5] = {entity_id, access_level, username, password, NULL};

    AUTO_FREE const char *error = database_execute_command(connection, command, arguments);

    if (error)
    {
        end(error);
    }

}


void add_villain()
{
    AUTO_FREE char *name;
    AUTO_FREE char *codename;
    AUTO_FREE char *species_id;
    AUTO_FREE char *rival_hero_id;

    printw("Hero Name:");
    name = read_single_line();

    printw("Hero Codename:");
    codename = read_single_line();

    printw("Hero Species ID:");
    species_id = read_single_line();

    printw("Rival Hero ID:");
    rival_hero_id = read_single_line();

    AUTO_FREE MYSQL *connection = database_connect();

    AUTO_FREE char *entity_id = insert_entity(connection, species_id, name, codename);

    const char *command = "insert into Villain(EntityID, RivalHeroID) values (?, ?)";

    const char *arguments[3] = {entity_id, rival_hero_id, NULL};

    AUTO_FREE const char *error = database_execute_command(connection, command, arguments);

    if (error)
    {
        end(error);
    }
}

static void perform_insertion(const char *query, const char **labels)
{
    size_t count = 0;

    for (const char **i = labels; *i++; count++);

    AUTO_FREE char **arguments = zmalloc((count + 1) * sizeof(const char *));

    for (size_t i = 0; i < count; i++)
    {
        printw("%s", labels[i]);
        arguments[i] = read_single_line();
    }

    arguments[count] = NULL;

    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    AUTO_FREE const char *error = database_execute_command(connection, query, (const char **) arguments);

    for (size_t i = 0; i < count; i++)
    {
        free(arguments[i]);
    }

    if (error)
    {
        end("%s", error);
    }
}

void add_trait()
{

    printw("Trait Name:");
    AUTO_FREE char *name = read_single_line();

    printw("IsAdvantage (yes/no):");
    AUTO_FREE char *is_advantage_input = read_single_line();
    bool is_advantage = is_advantage_input[0] == 'y' || is_advantage_input[0] == 'Y';

    const char *command = "insert into Trait (Name, IsAdvantage) values (?,?)";

    const char *arguments[3] = {name, is_advantage ? "1" : "0", NULL};

    DATABASE_AUTO_CLOSE MYSQL *connection = database_connect();

    AUTO_FREE const char *error = database_execute_command(connection, command, arguments);

    if (error)
    {
        end(error);
    }
}

void add_attack()
{
    // TODO: Associated Villains
    const char *labels[3] = {"Date (yyyy-MM-dd):", "Location:", NULL};

    perform_insertion("insert into Attack (EventDate, Location) values (?,?)", labels);
}

void add_equipment()
{
    const char *command = "Insert into Equipment (ID, Name, Description, Utility, EntityID) values (?, ?, ?, ?, ?);";

    const char *labels[5] = {
            "Name:",
            "Description:",
            "Utility:",
            "Entity ID:",
            NULL
    };

    perform_insertion(command, labels);
}

void add_species()
{
    char *command = "insert into Species (Name, ScientificName) values (?, ?);";

    const char *labels[3] = {
            "Name:",
            "Scientific Name:",
            NULL
    };

    perform_insertion(command, labels);
}

void add_hiding()
{
    char *command = "insert into Hiding (VillainID, Name, Location) values (?, ?, ?);";

    const char *labels[] = {
            "Villain ID: ",
            "Name: ",
            "Location: ",
            NULL
    };

    perform_insertion(command, labels);
}
