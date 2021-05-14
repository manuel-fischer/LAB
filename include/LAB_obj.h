#pragma once

// ... -- on success
#define LAB_OBJ(init, defer, ...) do \
{ \
    if(init) \
    { \
        { __VA_ARGS__ } \
        { defer; } \
    } \
} while(0)
