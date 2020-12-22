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
#include <util/util.h>

#define end(message, ...) \
    ({printw("Insertion failed.\n"); printw(message, ##__VA_ARGS__); refresh(); getch(); return NULL;})

// AUTO: Fix
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

    const char *message = database_execute_command(connection, format, arguments);

    if (message)
    {
        end("Error: %s", message);
    }

    // After we insert the entity, we need to grab its id

    char *id;

    const char *error = database_get_last_insertion_id(connection, &id);

    if (error)
    {
        end("Error: %s", error);
    }

    return id;

}


#undef end
#define end(message, ...) \
    ({printw("Error during insertion execution.\n"); printw(message, ##__VA_ARGS__); refresh(); getch(); return;})

void add_hero()
{
#define clear_input() \
    ({                    \
        free(name); \
        free(codename); \
        free(species_id); \
        free(access_level); \
        free(username); \
        free(password);       \
    })


    char *name = NULL;
    char *codename = NULL;
    char *species_id = NULL;
    char *access_level = NULL;
    char *username = NULL;
    char *password = NULL;

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

    MYSQL *connection = database_connect();

    char *entity_id = insert_entity(connection, species_id, name, codename);

    mysql_close(connection);

    if (entity_id == NULL)
    {
        clear_input();
        return;
    }


    connection = database_connect();

    char *command = "insert into Hero (EntityID, AccessCode, Username, Password) values (?, ?, ?, ?)";

    const char *arguments[5] = {entity_id, access_level, username, password, NULL};

    const char *error = database_execute_command(connection, command, arguments);

    mysql_close(connection);

    clear_input();

    free(entity_id);

    if (error)
    {
        end(error);
    }


#undef clear_input

}


void add_villain()
{

#define clear_input() \
    ({ free(name); \
    free(codename); \
    free(species_id); \
    free(rival_hero_id); }) \

    char *name;
    char *codename;
    char *species_id;
    char *rival_hero_id;

    printw("Villain Name:");
    name = read_single_line();

    printw("Villain Codename:");
    codename = read_single_line();

    printw("Villain Species ID:");
    species_id = read_single_line();

    printw("Rival Hero ID:");
    rival_hero_id = read_single_line();

    MYSQL *connection = database_connect();

    char *entity_id = insert_entity(connection, species_id, name, codename);

    const char *command = "insert into Villain(EntityID, RivalHeroID) values (?, ?)";

    const char *arguments[3] = {entity_id, rival_hero_id, NULL};

    const char *error = database_execute_command(connection, command, arguments);

    free(entity_id);
    mysql_close(connection);

    clear_input();

    if (error)
    {
        end(error);
    }

#undef clear_input
}


static int perform_insertion_connected(MYSQL *connection, const char *query, const char **labels)
{
    size_t count = 0;

    for (const char **i = labels; *i++; count++);

    char **arguments = zmalloc((count + 1) * sizeof(const char *));

    for (size_t i = 0; i < count; i++)
    {
        printw("%s", labels[i]);
        arguments[i] = read_single_line();
    }

    arguments[count] = NULL;


    const char *error = database_execute_command(connection, query, (const char **) arguments);

    for (size_t i = 0; i < count; i++)
    {
        free(arguments[i]);
    }

    free(arguments);

    if (error)
    {
        printw("%s", error);
        refresh();
        getch();
        return 1;
    }
    else
    {
        return 0;
    }
}

static inline void perform_insertion(const char *query, const char **labels)
{
    MYSQL *connection = database_connect();
    perform_insertion_connected(connection, query, labels);
    mysql_close(connection);
}

void add_trait()
{

    printw("Trait Name:");
    char *name = read_single_line();

    printw("IsAdvantage (yes/no):");
    char *is_advantage_input = read_single_line();
    bool is_advantage = is_advantage_input[0] == 'y' || is_advantage_input[0] == 'Y';

    const char *command = "insert into Trait (Name, IsAdvantage) values (?,?)";

    const char *arguments[3] = {name, is_advantage ? "1" : "0", NULL};

    MYSQL *connection = database_connect();

    const char *error = database_execute_command(connection, command, arguments);

    mysql_close(connection);
    free(is_advantage_input);
    free(name);

    if (error)
    {
        end(error);
    }
}

void add_attack()
{
    const char *labels[3] = {"Date (yyyy-MM-dd):", "Location:", NULL};

    MYSQL *connection = database_connect();

    if (perform_insertion_connected(connection, "insert into Attack (EventDate, Location) values (?,?)", labels))
    {
        mysql_close(connection);
        return;
    }

    printw("Note: Dash separated e.g \"10-20-30\", leave empty for none.\n");
    printw("Associated Villains: ");

    char *line = read_single_line();

    char *trim = full_trim_string(line);

    if (*trim)
    {
        char *id = NULL;

        const char *error = database_get_last_insertion_id(connection, &id);

        if (error)
        {
            free(line);
            mysql_close(connection);
            end("Failed to insert villains: %s", error);
        }

        char *token = strtok(trim, "-");

        while (token != NULL)
        {
            char *message = "insert into AttackAggregation(VillainID, AttackID) values (?, ?);";

            const char *arguments[] = {
                    token,
                    id,
                    NULL
            };

            error = database_execute_command(connection, message, arguments);

            if (error)
            {
                mysql_close(connection);
                free(line);
                free(id);
                end("Failed to insert villain %s : %s", token, error);
            }

            token = strtok(NULL, "-");
        }

        free(line);
        free(id);
        mysql_close(connection);
    }



}

void add_equipment()
{
    const char *command = "Insert into Equipment (Name, Description, Utility, EntityID) values (?, ?, ?, ?);";

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
