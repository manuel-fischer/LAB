#pragma once

#include <stddef.h> // size_t
#include <stdbool.h> // bool
#include "LAB_opt.h"
#include "LAB_error_state.h"
#include "LAB_array.h"

// export LAB_Filename
// export LAB_FileContents*
// export LAB_ReadFile


LAB_POINTER_CONST
const char* LAB_Filename(const char* path);



typedef struct LAB_FileContents
{
    char* data;
    size_t size;
    #define LAB_FileContents_array(file_contents) (char, (file_contents)->data, (file_contents)->size)
    LAB_Err err;
} LAB_FileContents;

LAB_FileContents LAB_ReadFile(const char* filename, const char* mode);


typedef char* LAB_DirEntry;

typedef struct LAB_DirEntries
{
    LAB_DEF_ARRAY_UNNAMED(LAB_DirEntry);
    #define LAB_DirEntries_array(dir_entries) LAB_REF_ARRAY_UNNAMED(LAB_DirEntry, dir_entries)
    LAB_Err err;
} LAB_DirEntries;


LAB_DirEntries LAB_ListDir(const char* dirpath, bool(* filter)(const char* name, const void* ctx), const void* ctx);


bool LAB_FilterDotFiles(const char* name, const void* ctx);