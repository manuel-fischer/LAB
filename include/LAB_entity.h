#pragma once


// entities are composed of components, which could
// be stored in a more spread out way, eg. positions
// of all entities are stored in an array. Then
// other data, that is dependent on the individual
// object(class) is stored in another array
// components should be relocatable!
//

///// "data type" /////
typedef struct LAB_EntityComponent
{
    int id;
    size_t size;

    // TODO: component vtable:
    // - destroy component
    // - serialize, deserialize
    // - event handlers

    //LAB_EntityComponent* requires[1];
} LAB_EntityComponent;

///// behavior /////
// here static solution
typedef struct LAB_EntityClass
{
    // TODO for more complex classes like a player,
    //      event handlers are here
    // TODO use bitset instead of array, only 32 different components possible
    //   OR store in the actual memory each component array element with an
    //      tag, that identifiers an entity, tag=0 allows for gaps (fast removal)
    const LAB_EntityComponent* components[1]; // variadic
} LAB_EntityClass;





///// data /////

// should be fundamental
typedef struct LAB_EntityPos
{
    double x, y, z;
    double vx, vy, vz; // velocity
} LAB_EntityPos;

const LAB_EntityComponent LAB_EntityPos_component = {
    .id = 0,
    .size = sizeof(LAB_EntityPos),
};





typedef struct LAB_EntityHealth
{
    int healthpoints
} LAB_EntityPos;
