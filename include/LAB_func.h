#pragma once

/**
 * Define functional interfaces
 */

#include "LAB_poly_buffer.h"
#include "LAB_stdinc.h" // alignof
#include "LAB_memory_util.h"
#include "LAB_intmathlib.h"
#include "LAB_ptr.h"
#include "LAB_select.h"


#define LAB_DEF_FUNC_OBJECT_TYPE(type, signature_type) \
    typedef struct type { \
        signature_type func; \
        void* ctx; \
        size_t size, alignment; \
    } type


#define LAB_DEF_FUNC_BUF_TYPE(type, signature_type) \
    typedef struct type { \
        signature_type func; \
        LAB_PolyBufferPtr ctx; \
    } type


#define LAB_DEF_FUNC_TYPES(type_prefix, signature_type) \
    LAB_DEF_FUNC_OBJECT_TYPE(type_prefix, signature_type); \
    LAB_DEF_FUNC_BUF_TYPE(type_prefix ## _Opt, signature_type)


#define LAB_CREATE_EMPTY_FUNC(func_type, func_ptr) ((func_type) { \
    .func = (func_ptr), \
    .ctx = NULL, .size = 0, .alignment = 0, \
})

/**
 * Check return.ctx for success
 * 
 * @param ... context_object
 */
#define LAB_CREATE_FUNC(func_type, func_ptr, ...) ((func_type) { \
    .func = (func_ptr), \
    .ctx = LAB_MemDup(&(__VA_ARGS__), sizeof(__VA_ARGS__), alignof(__VA_ARGS__)), \
    .size = sizeof(__VA_ARGS__), \
    .alignment = alignof(__VA_ARGS__) \
})

#define LAB_DUP_FUNC(func_type, func_obj) ((func_type) { \
    .func = (func_obj).func, \
    .ctx = LAB_MemDup((func_obj).ctx, (func_obj).size, (func_obj).alignment), \
    .size = (func_obj).size, \
    .alignment = (func_obj).alignment \
})

#define LAB_FREE_FUNC(func_obj) LAB_Free((func_obj).ctx)

#define LAB_REPLACE_FUNC(dst, src) ( \
    LAB_Free((dst).ctx), \
    (dst) = (src) \
)

/**
 * Return true on success
 */
#define LAB_OPTIMIZE_FUNC(poly_buffer, func_obj, out_buf_func) ( \
    (out_buf_func)->func = (func_obj).func, \
    (func_obj).ctx \
      ? LAB_PolyBuffer_AppendCopy(poly_buffer, (func_obj).ctx, (func_obj).size, (func_obj).alignment, &(out_buf_func)->ctx) \
      : ((out_buf_func)->ctx = LAB_POLY_BUFFER_NULLPTR, true) \
)


#define LAB_CALL_FUNC0(poly_buffer, default_expr, buf_func) ( \
    (buf_func).func \
      ? (buf_func).func(LAB_PolyBuffer_GetOrNull(poly_buffer, (buf_func).ctx)) \
      : (default_expr) \
)

#define LAB_CALL_FUNC(poly_buffer, default_expr, buf_func, ...) ( \
    (buf_func).func \
      ? (buf_func).func(LAB_PolyBuffer_GetOrNull(poly_buffer, (buf_func).ctx), __VA_ARGS__) \
      : (default_expr) \
)




typedef void (*LAB_FuncPtr)(void);

typedef struct LAB_DecoratorFooter
{
    LAB_FuncPtr func_ptr;
    size_t ctx_offset;
} LAB_DecoratorFooter;




struct LAB_DecorateFuncContext_Layout
{
    size_t complete_align,
           footer_pos,
           footer_end,
           inner_pos,
           complete_used,
           complete_size;
};

LAB_INLINE
size_t LAB_DecorateFuncContext_Align(
    const void* outer, size_t outer_size, size_t outer_align,
    const void* inner, size_t inner_size, size_t inner_align)
{
    if(!inner) inner_align = 1;
    return LAB_MAX3(outer_align, alignof(LAB_DecoratorFooter), inner_align);
}


LAB_INLINE LAB_ALWAYS_INLINE
struct LAB_DecorateFuncContext_Layout LAB_DecorateFuncContext_Layout(
    const void* outer, size_t outer_size, size_t outer_align,
    const void* inner, size_t inner_size, size_t inner_align)
{
    (void)outer;
    if(!inner) { inner_size = 0; inner_align = 1; }
    struct LAB_DecorateFuncContext_Layout layout;
    layout.complete_align = LAB_DecorateFuncContext_Align(
        outer, outer_size, outer_align,
        inner, inner_size, inner_align
    );
    layout.footer_pos = LAB_RoundUpAlign(outer_size, alignof(LAB_DecoratorFooter));
    layout.footer_end = layout.footer_pos + sizeof(LAB_DecoratorFooter);
    layout.inner_pos = LAB_RoundUpAlign(layout.footer_end, inner_align);
    layout.complete_used = layout.inner_pos + inner_size;
    layout.complete_size = LAB_RoundUpAlign(layout.complete_used, layout.complete_align);
    return layout;
}

LAB_INLINE
size_t LAB_DecorateFuncContext_Size(
    const void* outer, size_t outer_size, size_t outer_align,
    const void* inner, size_t inner_size, size_t inner_align)
{
    struct LAB_DecorateFuncContext_Layout layout = LAB_DecorateFuncContext_Layout(
        outer, outer_size, outer_align,
        inner, inner_size, inner_align
    );
    return layout.complete_size;
}

LAB_INLINE
void* LAB_DecorateFuncContext(
    const void* outer, size_t outer_size, size_t outer_align,
    const void* inner, size_t inner_size, size_t inner_align,
    LAB_FuncPtr func_ptr)
{
    struct LAB_DecorateFuncContext_Layout layout = LAB_DecorateFuncContext_Layout(
        outer, outer_size, outer_align,
        inner, inner_size, inner_align
    );

    LAB_DecoratorFooter footer = {
        .func_ptr = func_ptr,
        .ctx_offset = inner ? layout.inner_pos : 0,
    };


    void* mem = LAB_Malloc(layout.complete_size);
    if(!mem) return NULL;

    memcpy(mem, outer, outer_size);
    memset(LAB_PTR_OFFSET(mem, outer_size, 1), 0, layout.footer_pos - outer_size);
    memcpy(LAB_PTR_OFFSET(mem, layout.footer_pos, 1), &footer, sizeof footer);
    memset(LAB_PTR_OFFSET(mem, layout.footer_end, 1), 0, layout.inner_pos - layout.footer_end);
    if(inner)
        memcpy(LAB_PTR_OFFSET(mem, layout.inner_pos, 1), inner, inner_size);
    memset(LAB_PTR_OFFSET(mem, layout.complete_used, 1), 0, layout.complete_size - layout.complete_used);

    return mem;
}



#define LAB_DECORATE_FUNC_LAYOUT_PARAMS(context_object, old_func_obj) \
    &(context_object), sizeof(context_object), alignof(context_object), \
    (old_func_obj).ctx, (old_func_obj).size, (old_func_obj).alignment
/**
 * context object does not hold pointer to other function
 */
#define LAB_DECORATE_FUNC(func_type, func_ptr, context_object, old_func_obj) ((func_type) { \
    .func = (func_ptr), \
    .ctx = LAB_DecorateFuncContext(LAB_DECORATE_FUNC_LAYOUT_PARAMS(context_object, old_func_obj), \
                                   (LAB_FuncPtr)(old_func_obj).func), \
    .size = LAB_DecorateFuncContext_Size(LAB_DECORATE_FUNC_LAYOUT_PARAMS(context_object, old_func_obj)), \
    .alignment = LAB_DecorateFuncContext_Align(LAB_DECORATE_FUNC_LAYOUT_PARAMS(context_object, old_func_obj)) \
})

#define LAB_GET_DECORATOR_FOOTER(typed_context) ( \
    (LAB_DecoratorFooter*)LAB_PTR_OFFSET(typed_context, LAB_RoundUpAlign(sizeof(typed_context), alignof(LAB_DecoratorFooter)), 1) \
)
#define LAB_GET_DECORATED_FUNC(signature_type, typed_context) ( \
    (signature_type)(LAB_GET_DECORATOR_FOOTER(typed_context)->func_ptr) \
)
#define LAB_GET_DECORATED_CONTEXT(typed_context) ( \
    LAB_GET_DECORATOR_FOOTER(typed_context)->ctx_offset \
      ? LAB_PTR_OFFSET(typed_context, LAB_GET_DECORATOR_FOOTER(typed_context)->ctx_offset, 1) \
      : NULL \
)

#define LAB_CALL_DECORATED0(signature_type, typed_context, default_expr) ( \
    LAB_GET_DECORATED_FUNC(signature_type, typed_context) \
      ? LAB_GET_DECORATED_FUNC(signature_type, typed_context)(LAB_GET_DECORATED_CONTEXT(typed_context)) \
      : (default_expr) \
)

#define LAB_CALL_DECORATED(signature_type, typed_context, default_expr, ...) ( \
    LAB_GET_DECORATED_FUNC(signature_type, typed_context) \
      ? LAB_GET_DECORATED_FUNC(signature_type, typed_context)(LAB_GET_DECORATED_CONTEXT(typed_context), __VA_ARGS__) \
      : (default_expr) \
)
