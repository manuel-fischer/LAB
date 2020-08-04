#pragma once

const char* LAB_GetError();
void LAB_SetError(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
