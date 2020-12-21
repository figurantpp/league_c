//
// Created by figurantpp on 16/12/2020.
//

#ifndef LEAGUE_OPERATIONS_H
#define LEAGUE_OPERATIONS_H

#include <database/database.h>
#include <fullmacro/deconstruct.h>
#include <string.h>
#include <zalloc/zalloc.h>

const char* database_execute_command(MYSQL *connection, const char *query, const char ** arguments);

const char *database_get_last_insertion_id(MYSQL *connection, char ** id_pointer);

#endif //LEAGUE_OPERATIONS_H
