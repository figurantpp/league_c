
#include "stream.h"

#include "config/config.h"
#include "zalloc/zalloc.h"

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ncurses.h>

#define EXIT_IO 63
#define EXIT_EXEC 62

#define FIGLET_PATH "/usr/bin/figlet"

static char *read_whole_stream(int file, size_t *size_ptr)
{
    size_t total_size = 0;
    size_t position = 0;

    const size_t buffer_size = 256;

    char *result = NULL;

    int bytes_read = 0;

    // required to preserve errno
    int status;

    do
    {
        if (bytes_read + position == total_size)
        {
            result = zrealloc(result, total_size + buffer_size);

            total_size += buffer_size;
        }

        position += bytes_read;
    } while ((bytes_read = read(file, result + position, total_size - position)) > 0);

    if (bytes_read == -1)
    {
        status = errno;
        free(result);
        errno = status;
        return NULL;
    }

    result =  zrealloc(result, position + 1);

    result[position] = '\0';

    if (size_ptr)
    {
        *size_ptr = position;
    }

    return result;
}

char *read_whole_file(FILE *file, size_t *size_ptr)
{
    long old_pos = ftell(file);

    if (old_pos == -1)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);

    size_t size = ftell(file);
    fseek(file, old_pos, SEEK_SET);

    if (size_ptr)
    {
        *size_ptr = size;
    }

    char *result = zmalloc(size + 1);

    fread(result, size, 1, file);

    result[size] = '\0';

    return result;
}


char *figlet(char *message)
{
    if (!message)
    {
        errno = EINVAL;
        return NULL;
    }

    int files[2];

    if (pipe(files) == -1)
    {
        return NULL;
    }

    int read_file = files[0];
    int write_file = files[1];

    char *output;

    switch (fork())
    {
        case -1: return NULL;
        case 0:

            close(read_file);

            close(STDOUT_FILENO);

            if (!dup2(write_file, STDOUT_FILENO))
            {
                exit(EXIT_IO);
            }


            execl(FIGLET_PATH, "figlet", "-f", "slant", message, NULL);

            perror("Execution of '" FIGLET_PATH "' failed.");

            _exit(EXIT_EXEC);

        default:

            close(write_file);

            output = read_whole_stream(read_file, NULL);

            close(read_file);


            return output;
    }
}

void find_size_of_text(char *text, int result[2])
{
    int size_x = 0;
    int size_y = 0;
    int line_size = 0;

    char *ptr, chr;

    for (ptr = text; (chr = *ptr) ; ptr++)
    {
        if (chr == '\n')
        {
            size_y++;

            if (line_size > size_x)
            {
                size_x = line_size;
            }

            line_size = 0;
        }
        else
        {
            line_size++;
        }
    }

    result[0] = size_x;
    result[1] = size_y;
}

/**
 * Reads a single line from STDBUF until '\n' is found or CONFIG_MAX_INPUT_SIZE characters are read.
 * @return The malloc allocated always null terminated string containing the read data.
 */
char *read_single_line()
{
    refresh();

    char *buffer = zmalloc(CONFIG_MAX_INPUT_SIZE + 1);

    if (getnstr(buffer, CONFIG_MAX_INPUT_SIZE) != OK)
    {
        return NULL;
    }

    buffer[CONFIG_MAX_INPUT_SIZE] = '\0';

    return buffer;
}

void write_center(char *text)
{
    int window_width, window_height;

    getmaxyx(stdscr, window_height, window_width);

    struct {
        int width;
        int height;
    } text_size;

    find_size_of_text(text, (int*) &text_size);

    int position_x = window_width / 2;
    int position_y = window_height / 2;

    position_x -= text_size.width / 2;
    position_y -= text_size.height / 2;

    move(position_y, position_x);

    char c, *iterator;

    for (iterator = text; (c = *iterator) ; iterator++)
    {
        if (c == '\n')
        {
            move(++position_y, position_x);
        }
        else
        {
            addch(c);
        }
        
    }


}

/**
 * Converts a va_list of a NULL terminated pointers to a single buffer containing the given pointers.
 * @param list
 * @note The list needs to have at least one NULL terminated pointer, therefore it must not be empty.
 * @return THe buffer containing the list's data.
 */
void **pointer_va_list_to_buffer(va_list list)
{

    void *allocation;
    void *result;
    size_t offset;
    size_t total_allocated;
    size_t buffer_size;




}