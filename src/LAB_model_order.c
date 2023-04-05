#include "LAB_model_order.h"

#include "LAB_debug.h"

/*void LAB_BuildModelOrder(LAB_TriangleOrder* order, size_t triangle_count)
{
    // interpret as array of indices, because
    // LAB_TriangleOrder consists of three uint32_t which are
    // layed out after each other
    uint32_t* v = &order->v[0];
    LAB_ASSUME((3*triangle_count)/3 == triangle_count); // no overflow
    for(size_t i = 0; i < 3*triangle_count; ++i)
    {
        v[i] = i;
    }
}*/

// fill `order` with distance data
LAB_STATIC void LAB_PrepareSortModelOrder(LAB_TriangleOrder* order, LAB_Triangle const* mesh,
                                      size_t triangle_count, LAB_Vec3F cam2)
{
    for(size_t i = 0; i < triangle_count; ++i, ++order)
    {
        int index3 = order->v[0];
        LAB_ASSUME(index3%3 == 0); // possible hint to the compiler to
                                   // do not division followed by a multiplication by three
        int index = order->v[0]/3;
        float mx2, my2, mz2;
        mx2 = (mesh[index].v[0].x+mesh[index].v[1].x)-cam2.x;
        my2 = (mesh[index].v[0].y+mesh[index].v[1].y)-cam2.y;
        mz2 = (mesh[index].v[0].z+mesh[index].v[1].z)-cam2.z;
        float dist = mx2*mx2+my2*my2+mz2*mz2;
        // alternatively
        //order->v[1] = (uint32_t)(256*dist);

        uint32_t dist_i;
        memcpy(&dist_i, &dist, sizeof dist);
        order->v[1] = ~dist_i;
        //order->v[1] = ~*(uint32_t*)&dist; // bit cast of the floating point value
                                          // it is assumed that the floating point
                                          // variable is represented as an integer
                                          // in the following way (IEEE 754):
                                          // 0bSEEEEEEEEMMMMMMMMMMMMMMMMMMMMMMM
                                          //   |\__  __/\_________  __________/
                                          //   |   \/             \/
                                          // SIGN EXPONENT    MANTISSA
                                          //
                                          // Other representations may also work
                                          // where the exponent is in higher bits
                                          // than the mantissa bits and the, byte
                                          // order of the float is the same as for
                                          // the integers. Where the sign bit is
                                          // located does not matter much, because
                                          // every value is positive anyway
                                          //
                                          // the ~ to invert order, rendering from back to front
    }
    // an other possible way to implement the algorithm is to write
    // the distance of a triangle into order[order[i].v[0]/3].v[1]
    // instead of order[i].v[1], this would remove the need of
    // reordering the distance values. But it would lead to an
    // additional indirection and the need of a division by 3
    // NOTE: multiplication by three is not a big problem
    // because it is probably implemented as the LEA instruction on x86
    // it happens all the time, because sizeof(order[i]) is divisible
    // by 3
}

// I -> position in order[i].v that contains the index
LAB_STATIC void LAB_FinishSortModelOrder(LAB_TriangleOrder* order, size_t triangle_count, int I)
{
    LAB_TriangleOrder* end = order+triangle_count;
    for(; order != end; ++order)
    {
        int index = order->v[I];

        order->v[0] = index;
        order->v[1] = index+1;
        order->v[2] = index+2;
    }
}

void LAB_SortModelOrder2(LAB_TriangleOrder* order, LAB_Triangle const* mesh,
                        size_t triangle_count, LAB_Vec3F cam2)
{
    // Radix sort with radix 2**8 -> 256 counters
    // it does not use additional memory by using
    // the fact that the indices in a triangle
    // are consecutive. So the second element
    // is used for a cached distance to the camera
    // and the third element is used as the output
    // array.
    //
    // these indices are swapped around to simulate
    // the copying back from the temporary buffer to the
    // input array
    #define RADIX_BITS 8
    #define RADIX_COUNT (1<<RADIX_BITS)
    #define RADIX_MASK (RADIX_COUNT-1)
    // fill order[i].v[1] with distances
    LAB_PrepareSortModelOrder(order, mesh, triangle_count, cam2);

    // sort the triangles
    // no floating point operation happens here
    size_t count1[RADIX_COUNT];
    size_t count2[RADIX_COUNT];

    LAB_TriangleOrder* itr;
    LAB_TriangleOrder* end = order+triangle_count;

    // "index renaming" to reduce copying
    // indices into order[i].v
    // I -> current position of first index value
    // D -> current position of distance value
    // T -> current position of target/temporary (simulated output array)
    int I = 0, D = 1, T = 2;
    for(int dig = 0; dig < 32; dig += RADIX_BITS)
    {
        int is_last = 0;//dig+RADIX_BITS >= 32;
        // reset counters
        for(int i = 0; i < RADIX_COUNT; ++i) count1[i] = 0;

        // fill counters
        for(itr = order; itr != end; ++itr)
        {
            int bucket = itr->v[D]>>dig & RADIX_MASK;
            count1[bucket]++;
        }

        // compute prefix sum
        for(int i = 1; i < RADIX_COUNT; ++i) count1[i] += count1[i-1];

        // reorder distance values part 1
        if(!is_last)
        {
            // copy it into count2 -> used twice
            for(int i = 0; i < RADIX_COUNT; ++i) count2[i] = count1[i];
        }


        // reorder index values
        {
            // fill reordered indices to v[T]
            // v[D] keeps untouched so v[D] can be
            // copied later
            for(itr = end; itr != order;)
            {
                --itr;
                int bucket = itr->v[D]>>dig & RADIX_MASK;
                int new_pos = --count1[bucket];
                order[new_pos].v[T] = itr->v[I];
            }
            // old v[T] now contains the index values
            // old v[I] can be overwritten now (by distances)
            int tmp = I;
            I = T;
            T = tmp;
        }

        // reorder distance values part 2
        if(!is_last)
        {
            // fill reordered distances to v[T]
            for(itr = end; itr != order;)
            {
                --itr;
                int bucket = itr->v[D]>>dig & RADIX_MASK;
                int new_pos = --count2[bucket];
                order[new_pos].v[T] = itr->v[D];
            }
            // old v[T] now contains the distance values
            // old v[D] can be overwritten now (by indices)
            int tmp = D;
            D = T;
            T = tmp;
        }
    }

    /*size_t last_dist = 0;
    for(size_t i = 0; i < triangle_count; ++i)
    {
        LAB_ASSUME(last_dist <= order[i].v[D]);
        last_dist = order[i].v[D];
    }*/

    LAB_FinishSortModelOrder(order, triangle_count, I);
}
