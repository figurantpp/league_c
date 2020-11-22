
#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>

#include <iterator/iterator.h>

void database_setup();

void database_cleanup();

void database_close(MYSQL **connection);

__attribute__((warn_unused_result))
__attribute__((returns_nonnull))
MYSQL *database_connect();

#define DATABASE_AUTO_CLOSE __attribute__((cleanup(database_close)))

#endif