#pragma once

#include <stddef.h> // size_t
#include <stdbool.h> // bool
#include "LAB_opt.h"

// export LAB_Filename
// export LAB_FileContents*
// export LAB_ReadFile


LAB_POINTER_CONST
const char* LAB_Filename(const char* path);



typedef struct LAB_FileContents
{
    char* data;
    size_t size;
    bool success;
    #define LAB_FileContents_array(file_contents) (char, (file_contents)->data, (file_contents)->size)
} LAB_FileContents;

LAB_FileContents LAB_ReadFile(const char* filename, const char* mode);
