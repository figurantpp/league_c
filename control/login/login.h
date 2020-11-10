
#ifndef LOGIN_H
#define LOGIN_H

#include <stdbool.h>

struct HeroLogin
{
    char *id;
    char *accessCode;
    char *username;
};

void hero_login_delete(struct HeroLogin* hero);

void *login_perform(char *username, char *password);

#endif 