#pragma once


// export LAB_REDUCE_*
// export LAB_LREDUCE_*
// export LAB_RREDUCE_*


#define LAB_REDUCE_3(func, a, b, c) ( \
    func(func(a, b), c) \
)

#define LAB_REDUCE_4(func, a, b, c, d) ( \
    func(func(a, b), func(c, d)) \
)

#define LAB_REDUCE_5(func, a, b, c, d, e) ( \
    func(func(func(a, b), func(c, d)), e) \
)



#define LAB_LREDUCE_3(func, a, b, c) ( \
    func(func(a, b), c) \
)

#define LAB_LREDUCE_4(func, a, b, c, d) ( \
    func(func(func(a, b), c), d) \
)

#define LAB_LREDUCE_5(func, a, b, c, d, e) ( \
    func(func(func(func(a, b), c), d), e) \
)



#define LAB_RREDUCE_3(func, a, b, c) ( \
    func(a, func(b, c)) \
)

#define LAB_RREDUCE_4(func, a, b, c, d) ( \
    func(a, func(b, func(c, d))) \
)

#define LAB_RREDUCE_5(func, a, b, c, d, e) ( \
    func(a, func(b, func(c, func(d, e)))) \
)



