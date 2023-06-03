#pragma once

// export LAB_MIN LAB_MAX LAB_MIN3 LAB_MAX3
// export LAB_CLAMP LAB_CLAMP_EQ
// export LAB_SELECT_MIN LAB_SELECT_MAX LAB_SELECT_MIN3 LAB_SELECT_MAX3
// export LAB_SELECT LAB_SELECT0
// export LAB_COND_MOVE
// export LAB_MIN_BL LAB_MAX_BL LAB_MIN3_BL LAB_MAX3_BL
// export LAB_CLAMP_BL
// export LAB_BOUND0
// export LAB_COND_SWAP_T

#define LAB_MIN(a, b) ((a)<(b) ? (a) : (b))
#define LAB_MAX(a, b) ((a)>(b) ? (a) : (b))

#define LAB_MIN3(a, b, c) (  (a)<(b) ? ( (a)<(c) ? (a) : (c) ) : ( (b)<(c) ? (b) : (c) )  )
#define LAB_MAX3(a, b, c) (  (a)>(b) ? ( (a)>(c) ? (a) : (c) ) : ( (b)>(c) ? (b) : (c) )  )

#define LAB_CLAMP(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define LAB_CLAMP_EQ(x, a, b) ((x) < (a) ? ((x)=(a)) : (x) > (b) ? ((x)=(b)) : (x))

#define LAB_SELECT_MIN(a,va, b,vb) ((a)<(b) ? (va) : (vb))
#define LAB_SELECT_MAX(a,va, b,vb) ((a)>(b) ? (va) : (vb))

#define LAB_SELECT_MIN3(a,va, b,vb, c,vc) (  (a)<(b) ? ( (a)<(c) ? (va) : (vc) ) : ( (b)<(c) ? (vb) : (vc) )  )
#define LAB_SELECT_MAX3(a,va, b,vb, c,vc) (  (a)>(b) ? ( (a)>(c) ? (va) : (vc) ) : ( (b)>(c) ? (vb) : (vc) )  )


// Branchless conditionals, only use with integral types

// LAB_SELECT(cond, a, b) cond ? a : b, but branchless
// GCC: switches between cmov* and imul,
// imul is pretty optimized in current cpus, it is not as a problem, compared
// to a pipeline flush
//#define LAB_SELECT(cond, a, b) ((b) + (!!(cond))*((a)-(b)))
#define LAB_SELECT(cond, a, b) ((b) ^ (!!(cond))*((a)^(b)))
//#define LAB_SELECT(cond, a, b) ((!!(cond))*(a) | (!(cond))*(b))
//#define LAB_SELECT(cond, b, a) ((a) ^ (-!!(cond) & ((a) ^ (b))))
//#define LAB_SELECT(cond, b, a) ((-!(cond) & (a)) | (-!!(cond) & (b)))
//#define LAB_SELECT(cond, b, a) ((a) - (-!!(cond) & ((a) + (b))))

#define LAB_SELECT0(cond, a) ((!!(cond))*(a))

#define LAB_COND_MOVE(cond, dst, src) ((dst) = LAB_SELECT(cond, src, dst))

// Branchless min/max, only use with integers
#define LAB_MIN_BL(a, b) LAB_SELECT((a) < (b), a, b)
#define LAB_MAX_BL(a, b) LAB_SELECT((a) > (b), a, b)

#define LAB_MIN3_BL(a, b, c) LAB_MIN_BL(LAB_MIN_BL(a, b), c)
#define LAB_MAX3_BL(a, b, c) LAB_MAX_BL(LAB_MAX_BL(a, b), c)

#define LAB_CLAMP_BL(x, a, b) ((a) ^ LAB_SELECT0((x) >= (a), (x)^(a)) ^ LAB_SELECT0((b) <= (x), (b)^(x)))

#define LAB_BOUND0(x) LAB_SELECT0((x) >= 0, x)



#define LAB_COND_SWAP_T(type, cond, a, b) do { \
    type LAB_COND_SWAP_delta = LAB_SELECT0(cond, (a)^(b)); \
    (a) ^= LAB_COND_SWAP_delta; \
    (b) ^= LAB_COND_SWAP_delta; \
} while(0)
