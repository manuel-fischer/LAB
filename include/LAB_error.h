#pragma once

// export LAB_*Error*
// export LAB_ALLOC
// export LAB_CHECK_ALLOC

/**
 *  Returns a message with information about the specific error that occurred,
 *  or an empty string if there hasn't been any error.
 *  Return the latest that was set by LAB_SetError
 *  If there was no error set by LAB_SetError, try to check the libraries
 *  if an error has occurred.
 *  If no error was found, an empty string is returned
 *
 *  The returned string is valid until LAB_SetError or LAB_ClearError is called the
 *  next time
 */
const char* LAB_GetError(void);
void LAB_ClearError(void);
void LAB_SetError(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
void LAB_AppendError(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
void LAB_AddErrorContext(const char* filename, int line, const char* expression);
void LAB_AddErrorContextFmt(const char* filename, int line, const char* fmt, ...);

#define LAB_ADD_ERROR_MESSAGE(message) \
    LAB_AddErrorContext(__FILE__, __LINE__, message)

#define LAB_ADD_ERROR_MESSAGE_FMT(message, ...) \
    LAB_AddErrorContextFmt(__FILE__, __LINE__, message, __VA_ARGS__)

#define LAB_SetCError() LAB_SetError("Error: %s", strerror(errno))

// TODO
#define LAB_CHECK_ALLOC(ptr, on_error)
#define LAB_ALLOC(lval_ptr, alloc_expr)
