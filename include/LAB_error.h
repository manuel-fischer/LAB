#pragma once

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


// returns
#define LAB_CHECK_ALLOC(ptr, on_error)
#define LAB_ALLOC(lval_ptr, alloc_expr)
