#pragma once

#include <stdlib.h>
#include "LAB_color.h"

/** V2 Directional Light
 *
 *  Each block has 6 light values/light fields, for each side/direction
 *
 *  Light is directional, it keeps the direction/light field, unless it is reflected from a block
 *
 *  The light propagates from X in the following way, seen from the side:
 *
 *  +-----+-----+-----+  ^
 *  | 1/4 | 1/2 | 1/4 |  |
 *  +-----+-----+-----+  |
 *        |  X  |        |
 *        +-----+        |
 *
 *  Seen from the direction of the light, there is a 3x3 matrix, that splits up the light among all 9 blocks
 *
 *  +-----+-----+-----+
 *  | 1/16| 1/8 | 1/16|
 *  +-----+-----+-----+
 *  | 1/8 | 1/4 | 1/8 |
 *  +-----+-----+-----+
 *  | 1/16| 1/8 | 1/16|
 *  +-----+-----+-----+
 *
 *  The values in the matrix correspond to an angle of the light field of 90° (2*45°), it results in a multinomial distribution,
 *  TODO: is this the widest possible angle, or is an angle of 180° (2*90°) possible
 *  If the angle of the light field is narrower, more light is concentrated on the center cell.
 *
 *  There are 4 parameters that describe the field of light, two for each axis (one for each direction) perpendicular
 *  to the direction of the light.
 *  These values describe how much light propagates to the outer column/row, a value of 0 means that no light propagates to
 *  that outer column,
 *
 *  TODO: test light first with a fixed angle parameter of 2*45°
 *
 *  The light for rendering is calculated from all 6 light color values
 */
 #define LAB_DIRECTIONAL_LIGHT 0
#if LAB_DIRECTIONAL_LIGHT == 0
typedef LAB_Color LAB_LightNode;
#else
typedef struct LAB_LightNode
{
    // the corresponding face points to the light source
    LAB_Color faces[6];
} LAB_LightNode;
#endif

 #if 0
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
#endif
