//
// Created by figurantpp on 20/12/2020.
//

#include "util.h"

#include <fullmacro/deconstruct.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <zalloc/zalloc.h>
#include <stream/stream.h>

#define HERO_IMAGE_FILE_PATH "/home/figurantpp/Desktop/programming/c/league/art"


char *hero_image_from_codename(char *codename, size_t *ascii_art_size)
{
    // The path is defined by imagepath + '/' + name.lower().replace(' ', '_')

    /* Text Processing */

    char *position;

    AUTO_FREE char *file_path = NULL;

    UNIQUE_POINTER(FILE) file = NULL;

    // Replacing spaces with _
    while ((position = strchr(codename, ' ')))
    { *position = '_'; }

    // Lower casing
    for (position = codename; *position; position++)
    { *position = (char) tolower((unsigned char) *position); }

    //  + extra /
    //  + extra '\0'
    size_t image_path_size = strlen(HERO_IMAGE_FILE_PATH);
    size_t file_path_size = image_path_size + strlen(codename) + 2;

    file_path = zmalloc(file_path_size);

    snprintf(file_path, file_path_size, "%s/%s", HERO_IMAGE_FILE_PATH, codename);

    file = fopen(file_path, "r");

    if (file == NULL)
    {
        return NULL;
    }

    return read_whole_file(file, ascii_art_size);
}