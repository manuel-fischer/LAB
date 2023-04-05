#pragma once

#include "LAB_vec.h"
#include "LAB_opt.h"

// TODO: remove usage of constants outside this file

#define LAB_CHUNK_SHIFT  4
#define LAB_CHUNK_SIZE   (1 << LAB_CHUNK_SHIFT)
#define LAB_CHUNK_LENGTH (LAB_CHUNK_SIZE*LAB_CHUNK_SIZE*LAB_CHUNK_SIZE)
#define LAB_CHUNK_MASK   (LAB_CHUNK_SIZE - 1)


#define LAB_CHUNK_X(x) (x)
#define LAB_CHUNK_Y(y) (LAB_CHUNK_SIZE*(y))
#define LAB_CHUNK_Z(z) (LAB_CHUNK_SIZE*LAB_CHUNK_SIZE*(z))
#define LAB_CHUNK_OFFSET(x, y, z) ((x) + LAB_CHUNK_SIZE*((y) + LAB_CHUNK_SIZE*(z)))


#define LAB_DEF LAB_INLINE LAB_VALUE_CONST

LAB_DEF LAB_Vec3I LAB_Pos3D2Block(LAB_Vec3D pos) { return LAB_Vec3D2I_FastFloor(pos); }
LAB_DEF LAB_Vec3I LAB_Pos3F2Block(LAB_Vec3F pos) { return LAB_Vec3F2I_FastFloor(pos); }

LAB_DEF LAB_Vec3I LAB_Block2Chunk(LAB_Vec3I block) { return LAB_Vec3I_Sar(block, LAB_CHUNK_SHIFT); }


LAB_DEF LAB_Vec3I LAB_Pos3D2Chunk(LAB_Vec3D pos) { return LAB_Block2Chunk(LAB_Vec3D2I_FastFloor(pos)); }
LAB_DEF LAB_Vec3I LAB_Pos3F2Chunk(LAB_Vec3F pos) { return LAB_Block2Chunk(LAB_Vec3F2I_FastFloor(pos)); }


LAB_DEF LAB_Vec3I LAB_Chunk2Block(LAB_Vec3I chunk) { return LAB_Vec3I_RMul(chunk, LAB_CHUNK_SIZE); }

LAB_DEF LAB_Vec3D LAB_Block2Pos3D(LAB_Vec3I block) { return LAB_Vec3I2D(block); }
LAB_DEF LAB_Vec3F LAB_Block2Pos3F(LAB_Vec3I block) { return LAB_Vec3I2F(block); }

LAB_DEF LAB_Vec3D LAB_Chunk2Pos3D(LAB_Vec3I chunk) { return LAB_Block2Pos3D(LAB_Chunk2Block(chunk)); }
LAB_DEF LAB_Vec3F LAB_Chunk2Pos3F(LAB_Vec3I chunk) { return LAB_Block2Pos3F(LAB_Chunk2Block(chunk)); }

// TODO: remove, unnecessary types
LAB_DEF LAB_ChunkPos LAB_ChunkVec2ChunkPos(LAB_Vec3I chunk) {
    return (LAB_ChunkPos) { chunk.x, chunk.y, chunk.z };
}

#undef LAB_DEF