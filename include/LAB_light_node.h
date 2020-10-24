#pragma once

#include <stdlib.h>

/** V2 Directional Light
 *
 *  Each block has 6 light values/light fields, for each side/direction
 *
 *  Light is directional, it keeps the direction/light field, unless it is reflected from a block
 *
 *  The light propagates from X in the following way, seen from the side:
 *
 *  +---+---+---+  ^
 *  |25%|50%|25%|  |
 *  +---+---+---+  |
 *      | X |      |
 *      +---+      |
 */
/** V1
                x x o o o x x .
                x x o o o x x .
                o o x x x o o .
                o o x x x o o .
                o o x x x o o .
                x x o o o x x .
                x x o o o x x .
                l l l l l l l l






                0 1 2 3 4 5 6
                x x o o o x x


          |                       |
          |                       |
          |                       |
  ________|                       |________
              __      ^      __
              |\      |      /|
                \     |     /
          __     \    |    /     __
          \-_     \   |   /     _-/
              -_   2  3  4   _-
                 1  \ | /  5
        <------ 0 1 2 3 4 5 6 ------>




        FORWARD

                  v v v v v
              +---------------+
              | a b c d e f g |          0 0 0 0 0 0 0
            > | h i j k l m n | <        0 i j k l m 0
            > | o p q r s t u | <        0 p q r s t 0
            > | v w x y z A B | < --->   0 w x y z A 0
            > | C D E F G H I | <        0 D E F G H 0
            > | J K L M N O P | <        0 K L M N O 0
              | Q R S T U V W |          0 0 0 0 0 0 0
              +---------------+
                  ^ ^ ^ ^ ^


        TO THE SIDE (HERE: UP)

                v v v v v v v
              +---------------+
            > | a b c d e f g | <        0 0 0 0 0 0 0
            > | h i j k l m n | <        0 0 0 0 0 0 0
            > | o p q r s t u | <        0 0 0 0 0 0 0
              | v w x y z A B |   --->   0 0 0 0 0 0 0
              | C D E F G H I |          0 0 c d e 0 0
              | J K L M N O P |          0 i j k l m 0
              | Q R S T U V W |          o p q r s t u
              +---------------+
                ^ ^ ^ ^ ^ ^ ^

**/

typedef uint64_t LAB_LightEdge;

typedef struct LAB_LightNode
{
    // "Complex" number
    //int16_t a, b;

    LAB_LightEdge edges[6];

} LAB_LightNode;


#define LAB_LIGHT_FORWARD(light_edge) ((light_edge)       & 0x0000e3e3e3e3e300ll)
#define LAB_LIGHT_LEFT(light_edge)    ((light_edge) >>  4 & 0x0080c0e0e0e0c080ll)
#define LAB_LIGHT_RIGHT(light_edge)   ((light_edge) <<  4 & 0x0001030707070301ll)
#define LAB_LIGHT_UP(light_edge)      ((light_edge) >> 32 & 0x00ff7e3c00000000ll)
#define LAB_LIGHT_DOWN(light_edge)    ((light_edge) << 32 & 0x00000000003c7effll)

