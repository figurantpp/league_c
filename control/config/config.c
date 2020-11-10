

#include "config.h"

#include <stdlib.h>
#include <ncurses.h>

#include "zalloc/zalloc.h"
#include "database/database.h"

static void config_at_exit()
{
    refresh();
    endwin();
    database_cleanup();
}

static void config_alloc_abort()
{
    endwin();
    database_cleanup();
    abort();
}

void config_setup()
{
    atexit(config_at_exit);

    z_alloc_set_abort_function(config_alloc_abort);

    database_setup();

    initscr();

    if (has_colors())
    {
        start_color();
    }
}
