#include "LAB_filesystem.h"

#include "LAB_error.h"
#include "LAB_loop.h"

#include <stdio.h>
#include <errno.h>
#include <dirent.h>


/**
 *  Extract the filename of a path.
 *
 *  \return Pointer to the first char of the filename
 */
const char* LAB_Filename(const char* path)
{
    size_t i = strlen(path);
    while(i --> 0)
    {
        if(path[i] == '/' || path[i] == '\\')
            return path+i+1;
    }
    return path;
}

#define LAB_ReadFile_INITIAL_CAPACITY 2048

LAB_STATIC
LAB_Err LAB_ReadFile_Loop(FILE* f, LAB_FileContents* contents)
{
    {
        // read first 2048 bytes into buffer first, then allocate fit buffer
        char buffer[LAB_ReadFile_INITIAL_CAPACITY];
        size_t read_count = fread(buffer, 1, sizeof buffer, f);

        char* next = LAB_ARRAY_APPEND_SOME(LAB_FileContents_array(contents), read_count);
        if(next == NULL) return LAB_RAISE_C();

        memcpy(next, buffer, read_count);

        if(read_count < sizeof buffer) return LAB_OK;
    }

    size_t capacity = LAB_ReadFile_INITIAL_CAPACITY;
    while(true)
    {
        char* next = LAB_ARRAY_APPEND_SOME(LAB_FileContents_array(contents), capacity);
        if(next == NULL) return LAB_RAISE_C();

        size_t read_count = fread(next, 1, capacity, f);
        if(read_count < capacity)
        {
            contents->size -= capacity-read_count;
            return LAB_OK;
        }

        capacity *= 2;
    }
}


LAB_STATIC
LAB_Err LAB_ReadFileInto(FILE* f, LAB_FileContents* contents)
{
    if(feof(f))
        // keep contents empty
        return LAB_OK;

    LAB_Err err = LAB_ReadFile_Loop(f, contents);
    if(LAB_FAILED(err)) return err;

    if(ferror(f)) return LAB_RAISE_C();

    char* next = LAB_ARRAY_APPEND_SOME(LAB_FileContents_array(contents), 1);
    if(next == NULL) return LAB_RAISE_C();
    *next = '\0';
    return LAB_OK;
}

LAB_FileContents LAB_ReadFile(const char* filename, const char* mode)
{
    LAB_FileContents result = {0};

    FILE* f = fopen(filename, mode);
    if(f == NULL)
        return (result.err=LAB_RAISE_C(), result);

    LAB_Err err = LAB_ReadFileInto(f, &result);

    fclose(f);

    if(LAB_FAILED(err))
    {
        LAB_Free(result.data);
        result.data = NULL;
        result.size = 0;
        return (result.err=err, result);
    }

    return (result.err=LAB_OK, result);
}


LAB_STATIC
LAB_Err LAB_ListDirInto_Loop(DIR* dir, LAB_DirEntries* entries, bool(* filter)(const char* name, const void* ctx), const void* ctx)
{
    while(true)
    {
        errno = 0;
        struct dirent* direntry = readdir(dir);
        if(direntry == NULL && errno != 0) return LAB_RAISE_C();
        if(direntry == NULL) return LAB_OK;

        if(!filter(direntry->d_name, ctx)) continue;

        char** entry = LAB_ARRAY_APPEND_SOME(LAB_DirEntries_array(entries), 1);
        if(entry == NULL) return LAB_RAISE_C();

        *entry = LAB_StrDup(direntry->d_name);
        if(*entry == NULL)
        {
            entries->count--;
            return LAB_RAISE_C();
        }
    }
}

LAB_DirEntries LAB_ListDir(const char* dirpath, bool(* filter)(const char* name, const void* ctx), const void* ctx)
{
    LAB_DirEntries result = {0};

    DIR* dir = opendir(dirpath);
    if(dir == NULL) return (result.err = LAB_RAISE_C(), result);

    LAB_Err err = LAB_ListDirInto_Loop(dir, &result, filter, ctx);

    closedir(dir);

    if(LAB_FAILED(err))
    {
        LAB_FOREACH_ARRAY(e, LAB_DirEntries_array(&result))
            LAB_Free(*e);
        LAB_Free(result.data);
        result.data = NULL;
        result.count = 0;

        return (result.err=err, result);
    }

    return (result.err=LAB_OK, result);
}

bool LAB_FilterDotFiles(const char* name, const void* ctx)
{
    return name[0] != '.';
}