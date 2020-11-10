
#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>

void database_setup();

void database_cleanup();

MYSQL *database_get_connection();


#endif