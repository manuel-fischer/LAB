#include "LAB_gl.h"

#define LAB_GL_ERRORS_X(X) \
    X(GL_NO_ERROR, "No error") \
    X(GL_INVALID_ENUM, "An unacceptable value is specified for an enumerated argument") \
    X(GL_INVALID_VALUE, "A numeric argument is out of range") \
    X(GL_INVALID_OPERATION, "The specified operation is not allowed in the current state") \
    /*X(GL_INVALID_FRAMEBUFFER_OPERATION, "The framebuffer object is not complete")*/ \
    X(GL_OUT_OF_MEMORY, "There is not enough memory left to execute the command") \
    X(GL_STACK_UNDERFLOW, "An attempt has been made to perform an operation that would cause an internal stack to underflow") \
    X(GL_STACK_OVERFLOW, "An attempt has been made to perform an operation that would cause an internal stack to overflow")


const char* LAB_GL_GetError(GLenum errorid)
{
    switch(errorid)
    {
#define X(id, description) case id: return description;
        LAB_GL_ERRORS_X(X)
#undef X
        default: return "Unknown error";
    }
}

const char* LAB_GL_GetCurrentError(void)
{
    return LAB_GL_GetError(glGetError());
}
