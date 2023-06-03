#include "LAB_filesystem.h"

#include "LAB_array.h"
#include "LAB_error.h"

#include <stdio.h>
#include <errno.h>


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


LAB_STATIC
bool LAB_ReadFileInto(FILE* f, LAB_FileContents* contents)
{
    #define LAB_content_array LAB_FileContents_array(contents)

    if(feof(f))
        // keep contents empty
        return true;

    while(true)
    {
        char buffer[2048];
        size_t read_count = fread(buffer, 1, sizeof buffer, f);
        if(read_count == 0) break;

        char* next = LAB_ARRAY_APPEND_SOME(LAB_content_array, read_count);
        if(!next)
            return (LAB_SetCError(), false);

        memcpy(next, buffer, read_count);
    }

    if(ferror(f))
        return (LAB_SetCError(), false);

    char* next = LAB_ARRAY_APPEND_SOME(LAB_content_array, 1);
    if(!next)
        return (LAB_SetCError(), false);
    *next = '\0';
    return true;
}

LAB_FileContents LAB_ReadFile(const char* filename, const char* mode)
{
    LAB_FileContents result = {0};

    FILE* f = fopen(filename, mode);
    if(f == NULL)
        return (LAB_SetError("Error reading file: %s: %s", strerror(errno), filename), result.success=false, result);

    bool success = LAB_ReadFileInto(f, &result);

    fclose(f);

    if(!success)
    {
        LAB_Free(result.data);
        return (result.success=false, result);
    }

    return (result.success=true, result);
}