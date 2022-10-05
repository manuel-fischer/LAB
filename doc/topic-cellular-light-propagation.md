[&#8592; Spatial Cache-efficient Linked Lists](topic-cache-efficient-linked-lists.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; Table](table.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; Prefetch Queue](topic-prefetch-queue.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/topic-cellular-light-propagation.xmd#L1)</small>
***

# Cellular Light Propagation
<small>*Topic*</small>  


# Drafts

The lighting algorithm changed multiple times during the developement to be more realistic and efficient.
Some algorithms were not implemented but are also described here.

0 in the working, 1 is currently implemented, 2-3 were implemented, 4-7 either were implemented partially or are just drafts.
3 is the most basic lighting model, 2 extends this model by using octants.

**Overview**

0. [Vector Octant Lighting](#0-vector-octant-lighting)
1. [HDR Octant Manhattan Distance Lighting](#1-hdr-octant-manhattan-distance-lighting)
2. [Manhattan Distance Lighting](#2-manhattan-distance-lighting)
3. [Octant Manhattan Distance Lighting](#3-octant-manhattan-distance-lighting)
4. [Directional Kernel Lighting](#4-directional-kernel-lighting)
5. [Directional Bitset Lighting](#5-directional-bitset-lighting)
6. [Complex Number Lighting](#6-complex-number-lighting)
7. [Two Stage Octant Lighting](#7-two-stage-octant-lighting)



## 0. Vector Octant Lighting

Instead of just storing luminance values per lit up cell, an additional
direction vector and a diffusion factor might be stored. The diffusion factor
affects the decay and distribution over distance of the light. If it is low,
then it would propagate further along the direction. If it is higher, it would
be distributed among the other directions.

The values are stored per octant similarily to manhattan octant distance lighting.
By the structure of the octants, components of vectors are each either never
negative or never positive for a given octant. This would mean that it is
sufficient to store the absolute value of the component.

The components of the vector correspond to weights, in which the light propagates
to the neighbors. More specifically it corresponds to the offset from the light
source


The direction vector of a node is computed 

If there are multiple neighbors with light shining into a light node,

For sunlight the direction vector would be downwards and the diffusion factor
is minimal.



$$d' = d - \alpha d^2$$
*Diffusion propagation*

The diffusion propagation in a sequence with a behavior similar to the geometric
sequence proportional ($\frac 1 x$),
because the diffusion is proportional to the angle or radius of diffusion,
it results in the inverse square law, when applied to two dimensions.

## 1. HDR Octant Manhattan Distance Lighting


This technique is based on
[Manhattan Distance Lighting](#2-manhattan-distance-lighting), with the
addition of hdr color values. The difference is that light can propagate much
further, that can be made visible, when increasing the gamma. To avoid too many
updates and allow for light data optimizations, the accuracy is limited to a
minimal hdr-exponent, any light values under this treshold are converted to
complete darkness.



## 2. Manhattan Distance Lighting

**Pros**
- Memory: One color value per block

**Cons**
- Computation intensive algorithm: flood fill algorithm
- Needs careful considerations of chunk borders
- Diamond shaped light, distinctive behavior necessary for sunlight








## 3. Octant Manhattan Distance Lighting

**Pros**
- Simple efficient algorithm: linear pass per octant
- Directional shadows and shading

**Cons**
- Memory: 8 colors per block
- Diamond shaped light, stinctive behavior necessary for sunlight

Linear pass:
- Three nested `for` loops corresponding to each axis
- Each one either counts up or down, according to the octant position in the cube







## 4. Directional Kernel Lighting

Assign light to each face of the cube, propagate it in the direction of the normals of those faces.
This was partially implemented but discarded due to its illumination behavior.

**Pros**
- Smooth round shapes
- Directional shadows and shading
- Far light propagation

**Cons**
- Long illuminated rays: Perpendicular the light does not propagate as far as parallel to the direction of the light cube face
- Memory: 6 colors per block

### Notes


Each block has 6 light values/light fields, for each side/direction.
Light is directional, it keeps the direction/light field, unless it would be reflected from a block.

The light propagates from X in the following way, seen from the side:

```
+-----+-----+-----+  ^
| 1/4 | 1/2 | 1/4 |  |
+-----+-----+-----+  |
      |  X  |        |
      +-----+        |
```

Seen from the direction of the light, there is a 3x3 matrix, that splits up the light among all 9 blocks.

```
+-----+-----+-----+
| 1/16| 1/8 | 1/16|
+-----+-----+-----+
| 1/8 | 1/4 | 1/8 |
+-----+-----+-----+
| 1/16| 1/8 | 1/16|
+-----+-----+-----+
```

The values in the matrix correspond to an angle of the light field of 90° (2*45°), it results in a multinomial distribution, which corresponds to the gaussian blur kernel.

TODO: Is this the widest feasible angle, or is an angle of 180° still efficient enough, which includes more blocks.
If the angle of the light field is narrower, more light is concentrated on the center cell.

There are 4 parameters that describe the field of light, two for each axis (one for each direction) perpendicular
to the direction of the light.
These values describe how much light propagates to the outer column/row, a value of 0 would mean that no light propagates to
that outer column.

TODO: Test light first with a fixed angle parameter of 2*45°.

The shading color in the tesselated model for rendering is calculated as a weighted sum from the 6 light color values.

TODO: What if values from other directions are used in the calculation.





## 5. Directional Bitset Lighting

Store a light intensity color channel value per cube face. For more directional light use a set of 7x7 bits, corresponding to slightly-offset rays through the cube face.



### Notes


```
                x x o o o x x .
                x x o o o x x .
                o o x x x o o .
                o o x x x o o .
                o o x x x o o .
                x x o o o x x .
                x x o o o x x .
                l l l l l l l l
```
*Layout of the bitset(x/o) and the color channel value(l) in a 64 bit integer*

```
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
```
*Each of the 7 directions in an axis propagates do different neighboring blocks in different ways*

```
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
```
*The outer rays are removed when light is propagated in the normal direction to the next plane of blocks.*
*This has the effect that the light propagation is more directional as further it is away from the light source.*

```
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
                  side face         top face of forward block


                            d r
                        -------
                     d|    ^
                     r| >>>^
                      |
```
*Diagonal light propagation*

Light only propagates to the direct following block, corresponding to the direction of the light value.
But it does not need to propagate to the same face again, rays near to the edges of
the face propagate to other faces in the next block.
This would mean, that diagonal blocks are indirectly illuminated through the forward block.

When light propagates to neighboring blocks, only certain rays are propagated to the neighboring faces.
Note the trapezoid shape of the light propagation.

#### C code
```c
typedef uint64_t LAB_LightEdge;

typedef struct LAB_LightNode
{
    LAB_LightEdge edges[6];
} LAB_LightNode;


#define LAB_LIGHT_FORWARD(light_edge) ((light_edge)       & 0x0000e3e3e3e3e300ll)
#define LAB_LIGHT_LEFT(light_edge)    ((light_edge) >>  4 & 0x0080c0e0e0e0c080ll)
#define LAB_LIGHT_RIGHT(light_edge)   ((light_edge) <<  4 & 0x0001030707070301ll)
#define LAB_LIGHT_UP(light_edge)      ((light_edge) >> 32 & 0x00ff7e3c00000000ll)
#define LAB_LIGHT_DOWN(light_edge)    ((light_edge) << 32 & 0x00000000003c7effll)
```

Note that for the implementation it might also important to be able to rotate the bitset by multiples of 90°.
Also the bitmasks here are slightly different to the notes above.




## 6. Complex Number Lighting

The light propagation through a face could possibly be encoded by a pair of two numbers.
These could be the vertical and horizontal scatter of the light on the face.
These could also be center point of the distribution of light on the face.
But these could also be complex numbers with specific phases and magnitude.
The precision might not need to be as high, so it could be implemented using a small integer
used with fixed point arithmetic.

```c
typedef struct LAB_LightEdge
{
    // "Complex" number
    int16_t a, b;
} LAB_LightEdge;

typedef struct LAB_LightNode
{
    LAB_LightEdge edges[6];
} LAB_LightNode;
```








## 7. Two Stage Octant Lighting

**Pros**
- Smoother shape
- Far light propagation

**Cons**
- No shadows under trees
- Artefact shadow at axis planes, notable if only one emmiting block is set


This is based on the [Octant Manhattan Distance Lighting](#3-octant-manhattan-distance-lighting).
It only varies in the calculation of the light value and the use of hdr lighting values.


```
     |____              |____              |____ 
    /    /|            /  __/|            /|    |
   /____/ |__   __\   /__/|_||__   __\   / |____|__
   |    | /       /   |  |__//       /   | /    /
   |____|/            |____|/            |/____/ 
   /                  /                  /
```
*Unit cube with a corner at the origin, transformed into the three faces at the three axis planes*



```
         p₂               s₃
          \______________/
          /|             |
         / |             |
        /  |             | λs₃p₁
       /   |             |
   s₁_/    |             |
      |    |_____________|_p₁
      |    /\    λp₁t    /
      |   /  t          /
      |  /             /
      | /             /
      |/_____________/
      /              \
     p₃               s₂
```
*The 7 cube vertices on the axis planes*

|Symbol|Description|
|:-:|-|
|$t$|Target light node|
|$p_i$|Primary light node|
|$s_j$|Secondary light node|
|||
|$l$|Induced luminance value (calculated and stored)|
|$λ$|Propagated light (in the context of $t$)|
|$e$|Block emission (`.lum`)|
|$d$|Diaphaneity/transmittance color (`.dia`)|
|||
|$α$|Primary weight, constant|

*Table of variables*

If $α = 1$, this would exactly behave as the [Octant Manhattan Distance Lighting](#3-octant-manhattan-distance-lighting).


$$λ_{{s_j}{p_i}} = d_{p_i}\cdot l_{s_j}$$
*Formular for the calculation of the propagated light from secondary to primary nodes*


$$λ_{{p_i}t} = d_t\cdot\left(α l_{p_i} + \frac{1-α}2\sum_{j\in[1..3]\setminus\{i\}}{λ_{{s_j}{p_i}}}\right)$$

*Formular for the calculation of the propagated light from primary to the target node*

$$l_t = \max(\left\{λ_{p_i}\ |\ i \in[1..3]\right\}\cup \{e_t\})$$

*Formular for the finally calculated target luminance value*

$$λ_{{p_i}t} = d_t\cdot\left(l_{p_i} - \left\lfloor(1-α) l_{p_i} \right\rfloor + \left\lfloor\frac{1-α}2\sum_{j\in[1..3]\setminus\{i\}}{λ_{{s_j}{p_i}}}\right\rfloor\right)$$

*Formular for propagated light that considers rounding regarding multiplication*

$$λ_{{p_i}t} = d_t\cdot\left(l_{p_i} - \left\lfloor(1-α) l_{p_i} \right\rfloor + \left\lfloor\frac{1-α}2d_{p_i}\sum_{j\in[1..3]\setminus\{i\}}{l_{s_j}}\right\rfloor\right)$$
$$λ_{{p_i}t} = d_t\cdot\left(l_{p_i} - \left\lfloor(1-α) l_{p_i} \right\rfloor + \left\lfloor(1-α)d_{p_i}\frac{\sum_{j\in[1..3]\setminus\{i\}}{l_{s_j}}}2\right\rfloor\right)$$
$$λ_{{p_i}t} = d_t\cdot\left(l_{p_i} - \left\lfloor β l_{p_i} \right\rfloor + \left\lfloor βd_{p_i}\frac{\sum_{j\in[1..3]\setminus\{i\}}{l_{s_j}}}2\right\rfloor\right)$$

*Restructured, $β = 1-α$*

When dealing with parallel light rays of sunlight, the calculated light should not decrease over distance.
This could possibly happen by rounding errors.
This means that if the neighboring light nodes already have the same intensity, the resulting light value should be the same
but could be slightly less due to the rounding.







# Further Ideas

## Reflections

The directional light algorithms often create complete darkness for unilluminated regions.
In reality light rays are reflected from many surfaces, resulting in indirect illumination.

### Iterative Reflection
**Pros**
- No further memory usage

**Cons**
- Needs complex updating algorithm, possible problem with cyclic illumination

### Separated Reflection

Store illumination value per reflection depth.
Treat reflecting surfaces of previous reflection depth as light sources in the current reflection depth.

**Pros**
- Efficient and simple algorithm, based on algorithm without any reflections

**Cons**
- Additional memory cost for each reflection depth, fixed amount of reflection depths



### HDR Lighting Values

Color light values are stored as 24 bit color values.
For alignment reasons, the light values are stored in 32 bit integers.
The 8 left over bits could be used as an exponent, that allows for a higher range
of values. The exponent is shared between the channels, such that there might
be an acceptable precision loss for the channels, that are relatively smaller
than the maximum channel value.

