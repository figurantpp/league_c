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

#endif //LEAGUE_OPERATIONS_H
