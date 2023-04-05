#include "LAB_builtin_structures.h"

#include "LAB_builtin_blocks.h"
#include "LAB_builtin_overworld.h"
#include "LAB_game_structure_util.h"
#include "LAB_game_surface_dimension_placement.h"

bool LAB_BuiltinStructures_Init(LAB_SurfaceDimension* dim)
{
    #define REGISTER_IN_BIOME(biome) \
        if(!LAB_SurfaceDimension_AddStructure(dim, biome, salt, &s)) return false;

    #define REGISTER_COMMON() \
        if(!LAB_SurfaceDimension_AddStructure(dim, LAB_SurfaceBiomeID_NULL, salt, &s)) return false;

    #define ProbAndCount LAB_Game_StructureDensity_ProbAndCount
    #define PlaceOnSurface() LAB_SurfaceDimension_PlaceOnSurface(dim)
    #define SetYRange(min, max) (s = LAB_Game_Structure_RestrictYRange_Move(s, min, max))

    #define SINGLE_BLOCK (LAB_Game_StructureSpan) { .cx=1, .cy=1, .cz=1 }
    #define SMALL (LAB_Game_StructureSpan) { .cx=1, .cy=1, .cz=1 }
    #define VERTICAL2 (LAB_Game_StructureSpan) { .cx=1, .cy=2, .cz=1 }

    LAB_Game_Structure s;
    uint64_t salt;


    // rock
    {
        salt = 0x91827364;
        s.span = SMALL;
        s.structure = LAB_Builtin_Rock(LAB_BLOCK_STONE.cobble);
        s.density = ProbAndCount(1., 0, 1);
        s.placement = PlaceOnSurface();
        //SetYRange(52, INT_MAX);
        //REGISTER_COMMON();
        REGISTER_IN_BIOME(LAB_SURFACE_BIOME_MOUNTAIN);
        LAB_Structure_Destroy(&s);
    }


    // bushes
    {
        salt = 0x32547698;
        s.span = SMALL;
        s.structure = LAB_Builtin_Bush(LAB_BLOCK_LEAVES);
        s.density = ProbAndCount(1, 0, 4);
        s.placement = PlaceOnSurface();
        //SetYRange(0, 92);
        SetYRange(0, INT_MAX);
        REGISTER_IN_BIOME(LAB_SURFACE_BIOME_PLAINS);
        REGISTER_IN_BIOME(LAB_SURFACE_BIOME_FOREST);
        REGISTER_IN_BIOME(LAB_SURFACE_BIOME_BIRCH_FOREST);
        REGISTER_IN_BIOME(LAB_SURFACE_BIOME_TAIGA);
        LAB_Structure_Destroy(&s);
    }


    // plants
    {
        static const struct
        {
            uint64_t salt;
            const LAB_BlockID* block;
            double probability;
            int min_count, max_count;
            bool is_flower;
        } plants[] =
        {
            { 0x56789abc, &LAB_BLOCK_TALLGRASS,    1.0, 3, 35, false},
            { 0xd8f8e945, &LAB_BLOCK_TALLERGRASS,  1.0, 3, 35, false},
            { 0xfd874567, &LAB_BLOCK_RED_TULIP,    0.5, 1, 10, false},
            { 0xfbc90356, &LAB_BLOCK_YELLOW_TULIP, 0.5, 1, 10, false},
        };

        for(size_t i = 0; i < LAB_LEN(plants); ++i)
        {
            salt = plants[i].salt;
            s.span = SINGLE_BLOCK;
            s.structure = LAB_Builtin_Plant(*plants[i].block);
            s.density = ProbAndCount(plants[i].probability, plants[i].min_count, plants[i].max_count);
            s.placement = PlaceOnSurface();
            //SetYRange(0, 102);
            SetYRange(0, INT_MAX);
            REGISTER_IN_BIOME(LAB_SURFACE_BIOME_FOREST);
            REGISTER_IN_BIOME(LAB_SURFACE_BIOME_BIRCH_FOREST);
            REGISTER_IN_BIOME(LAB_SURFACE_BIOME_PLAINS);
            REGISTER_IN_BIOME(LAB_SURFACE_BIOME_MEADOW);
            if(!plants[i].is_flower)
                REGISTER_IN_BIOME(LAB_SURFACE_BIOME_TAIGA);
            LAB_Structure_Destroy(&s);
        }
    }


    // fallen leaves
    {
        salt = 0x12636478;
        s.span = SINGLE_BLOCK;
        s.structure = LAB_Builtin_Plant(LAB_BLOCK_FALLEN_LEAVES);
        s.density = ProbAndCount(1, 64, 192);
        s.placement = PlaceOnSurface();
        SetYRange(0, 65);
        REGISTER_IN_BIOME(LAB_SURFACE_BIOME_BIRCH_FOREST);
        LAB_Structure_Destroy(&s);
    }


    // dirt patch
    {
        salt = 0x93475493,
        s.span = SMALL;
        s.structure = LAB_Builtin_Patch(LAB_BLOCK_DIRT, LAB_BLOCK_GRASS);
        s.density = ProbAndCount(1./8., 1, 5);
        s.placement = PlaceOnSurface();
        SetYRange(0, 112);
        REGISTER_IN_BIOME(LAB_SURFACE_BIOME_FOREST);
        LAB_Structure_Destroy(&s);
    }


    // trees
    {

        LAB_Builtin_TreePalette oak_tree = {
            .wood = LAB_BLOCK_WOOD,
            .leaves = LAB_BLOCK_LEAVES,
            .ground = LAB_BLOCK_DIRT
        };
        LAB_Builtin_TreePalette birch_tree = {
            .wood = LAB_BLOCK_BIRCH_WOOD,
            .leaves = LAB_BLOCK_BIRCH_LEAVES,
            .ground = LAB_BLOCK_DIRT
        };
        LAB_Builtin_TreePalette spruce_tree = {
            .wood = LAB_BLOCK_SPRUCE_WOOD,
            .leaves = LAB_BLOCK_SPRUCE_LEAVES,
            .ground = LAB_BLOCK_DIRT
        };

        
        struct
        {
            uint64_t salt;
            LAB_Builtin_TreePalette* palette;
            bool large;
            LAB_SurfaceBiomeID biome;
            double probability; int min_count, max_count;
        } trees[] =
        {
            { 0x13579bdf, &oak_tree,   false, LAB_SURFACE_BIOME_FOREST,                 1.0, 3, 5 },
            { 0x13579bdf, &oak_tree,   false, LAB_SURFACE_BIOME_PLAINS,             1.0/32., 1, 1 },
            { 0x85484857, &birch_tree, false, LAB_SURFACE_BIOME_FOREST,                 0.5, 0, 2 },
            { 0x85484857, &birch_tree, false, LAB_SURFACE_BIOME_BIRCH_FOREST,           1.0, 3, 6 },
            { 0x85484857, &birch_tree, false, LAB_SURFACE_BIOME_MEADOW,             1.0/16., 1, 1 },
            { 0x85484857, &spruce_tree, true, LAB_SURFACE_BIOME_TAIGA,                  1.0, 4, 5 },
        };


        for(size_t i = 0; i < LAB_LEN(trees); ++i)
        {
            salt = trees[i].salt;
            s.span = trees[i].large ? VERTICAL2 : SMALL;
            s.structure = (trees[i].large ? LAB_Builtin_LargeTree : LAB_Builtin_Tree)(*trees[i].palette);
            s.density = ProbAndCount(trees[i].probability, trees[i].min_count, trees[i].max_count);
            s.placement = PlaceOnSurface();
            //SetYRange(0, 62);
            SetYRange(0, INT_MAX);
            REGISTER_IN_BIOME(trees[i].biome);
            LAB_Structure_Destroy(&s);
        }
    }


    // buildings
    {
        LAB_Builtin_BuildingPalette stone_palette = {
            .corner  = LAB_BLOCK_STONE.smooth,
            .wall    = LAB_BLOCK_STONE.bricks,
            .floor   = LAB_BLOCK_STONE.cobble,
            .ceiling = LAB_BLOCK_WOOD_PLANKS_DARK,
            .roof    = LAB_BLOCK_CLAY.bricks,
        };
        LAB_Builtin_BuildingPalette basalt_palette = {
            .corner  = LAB_BLOCK_BASALT.smooth,
            .wall    = LAB_BLOCK_BASALT.bricks,
            .floor   = LAB_BLOCK_BASALT.cobble,
            .ceiling = LAB_BLOCK_WOOD_PLANKS,
            .roof    = LAB_BLOCK_CLAY.bricks,
        };
        LAB_Builtin_BuildingPalette marble_palette = {
            .corner  = LAB_BLOCK_MARBLE.smooth,
            .wall    = LAB_BLOCK_MARBLE.bricks,
            .floor   = LAB_BLOCK_MARBLE.cobble,
            .ceiling = LAB_BLOCK_WOOD_PLANKS_DARK,
            .roof    = LAB_BLOCK_CLAY.bricks,
        };
        LAB_Builtin_BuildingPalette desert_palette = {
            .corner  = LAB_BLOCK_SANDSTONE.smooth,
            .wall    = LAB_BLOCK_SANDSTONE.bricks,
            .floor   = LAB_BLOCK_SANDSTONE.cobble,
            .ceiling = LAB_BLOCK_WOOD_PLANKS_DARK,
            .roof    = LAB_BLOCK_CLAY.bricks,
        };


        struct
        {
            uint64_t salt;
            LAB_Builtin_BuildingPalette* palette;
            LAB_SurfaceBiomeID biome;
            bool in_cave;
            double probability;
        } towers[] =
        {
            { 0xfdb97531, &stone_palette,  LAB_SURFACE_BIOME_PLAINS, false, 1./256. },
            { 0x7845fdf3, &marble_palette, LAB_SURFACE_BIOME_PLAINS, false, 1./512. },
            { 0x783f45df, &desert_palette, LAB_SURFACE_BIOME_DESERT, false,  1./64. },
            { 0x783f45df, &basalt_palette, LAB_SurfaceBiomeID_NULL,  true,   1./64. },
        };

        for(size_t i = 0; i < LAB_LEN(towers); ++i)
        {
            salt = towers[i].salt;
            s.span = SMALL;
            s.structure = LAB_Builtin_Tower(*towers[i].palette);
            s.density = ProbAndCount(towers[i].probability, 1, 1);
            if(towers[i].in_cave)
            {
                s.placement = LAB_SurfaceDimension_PlaceOnCaveWall(dim, LAB_I_DOWN);
                SetYRange(INT_MIN, 0);
            }
            else
            {
                s.placement = PlaceOnSurface();
                SetYRange(0, 42);
            }
            REGISTER_IN_BIOME(towers[i].biome);
            LAB_Structure_Destroy(&s);
        }


        // house
        salt = 0xf89df80f;
        s.span = SMALL;
        s.structure = LAB_Builtin_House(marble_palette);
        s.density = ProbAndCount(1./128., 1, 1);
        s.placement = PlaceOnSurface();
        SetYRange(0, 42);
        REGISTER_IN_BIOME(LAB_SURFACE_BIOME_PLAINS);
        LAB_Structure_Destroy(&s);
    }


    // cave crystals
    {
        salt = 0x21436587;
        s.span = SMALL;
        s.structure = LAB_Builtin_CeilingCrystal();
        s.density = ProbAndCount(1., 0, 5);
        s.placement = LAB_SurfaceDimension_PlaceOnCaveWall(dim, LAB_I_UP);
        SetYRange(INT_MIN, -18);
        REGISTER_COMMON();
        LAB_Structure_Destroy(&s);
    }

    // ores
    {
        static const struct
        {
            uint64_t salt;
            const LAB_BlockID* block;
            double probability; int min_count, max_count;
        } ores[] =
        {
            {0x78578947, &LAB_BLOCK_IRON_ORE,    0.7, 1, 4},
            {0x67856465, &LAB_BLOCK_COPPER_ORE,  0.7, 1, 4},
            {0x08765324, &LAB_BLOCK_GOLD_ORE,    0.5, 1, 4},
            {0x21567890, &LAB_BLOCK_SILVER_ORE,  0.6, 1, 4},
            {0x67565456, &LAB_BLOCK_URANIUM_ORE, 0.4, 1, 4},
        };

        for(size_t i = 0; i < LAB_LEN(ores); ++i)
        {
            salt = ores[i].salt;
            s.span = SMALL;
            s.structure = LAB_Builtin_Ore(*ores[i].block, LAB_BLOCK_STONE.raw);
            s.density = ProbAndCount(ores[i].probability, ores[i].min_count, ores[i].max_count);
            s.placement = LAB_Game_PlaceAnywhere();
            REGISTER_COMMON();
            LAB_Structure_Destroy(&s);
        }
    }

    return true;
}