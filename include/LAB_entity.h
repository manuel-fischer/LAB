#pragma once

#include "LAB_stdinc.h"

/**
 * entities are composed of components, which should
 * able to be stored in a more spread out way, eg. positions
 * of all entities are stored in an array. Then
 * other data, that is dependent on the individual
 * object(class) is stored in another array
 * components should be relocatable!
 *
 * The EntitySet is the base component that holds these arrays together
 * and creates the collection for the entities. It gets stored
 * into chunk regions, it should also provide an optimized way to query
 * all the entities in a given bounding box, by implementing a simple
 * 3D navigation structure, like a octtree (TODO)
 */

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

    // do an action on an array of components, destroy, serialize etc...
    //int(*func)(size_t count, void* array, ...);

    void(*destroy)(size_t count, void* array);
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
    //const LAB_EntityComponent* components[1]; // variadic

    // handle events
    int(*func)(size_t count, void*, ...);
} LAB_EntityClass;



/**
 *  Provides a collection of entities
 *
 *  An entity is constructed in multiple steps by creating an
 *  entity id (LAB_EntitySet_NewEntity) and then adding
 *  multiple components to that entity (LAB_EntitySet_AddComponent)
 *
 *
 */
typedef struct LAB_EntitySet
{
    size_t component_count; // Is redundant, if all types are fixed
    // all have component_count elements
    // the first entries are fixed and always have the same component type
    const LAB_EntityComponent** component_types; // Is redundant, if all types are fixed
    size_t* sizes; // sizes of subarray
    void** array_data;
    // TODO add id to every component itself to determine the entity it belongs to
    // if it is -1, the element is not used

    size_t entity_count;
    const LAB_EntityClass* vtables; // callbacks/hooks
    // (entity_id, type) ->
    // two dimensional array size: component_count*entity_count
    // -1 if entry does not have a specific component
    // TODO: better alternative: bitset
    size_t* component_indices;

} LAB_EntitySet

enum
{
    LAB_ENTITY_COMPONENT_POS,
    LAB_ENTITY_COMPONENT_HEALTH,
};


// bundles the components together, it is
// used, whenever all components should compose a complete object
// stable until some entity or entity component is removed from the collection?
// OR if elements are removed, the gap that is produced is marked to be one, by
// setting the id to -1
typedef struct LAB_EntityReference
{
    LAB_EntitySet* set
    size_t index;
}

/**
 * creates a new entity set by zeroing out most of the
 * fields and allocating space for the fixed component types, sizes and array_data arrays
 */
void LAB_EntitySet_Create(LAB_EntitySet* set);

/**
 * clean up ressources: - call component destructors and arrays
 */
void LAB_EntitySet_Destroy(LAB_EntitySet* set);

/**
 *  Creates an entity, that has no components associated with it
 *  Return index/id of new entity
 */
size_t LAB_EntitySet_NewEntity(LAB_EntitySet* set, const LAB_EntityClass* clazz);


/**
 *  Add an entity by copying src
 *  TODO: do not copy if entity gets moved from one entity set to another
 */
size_t LAB_EntitySet_AddEntity(LAB_EntitySet* set, LAB_EntityReference* src);

/**
 *  Remove entity and all associated components
 */
size_t LAB_EntitySet_RemoveEntity(LAB_EntitySet* set, size_t entity_id);

/**
 *  Allocate space for the component and link it up in the component_indices array
 *
 *
 *  Return index in the component array to the component, same as set->component_indices[id*set->component_count + type_index]
 */
size_t LAB_EntitySet_AddComponent(LAB_EntitySet* set, size_t entity_id, const LAB_EntityComponent* type);


/**
 *  Unlink component (and reset component memory to zero)
 */
size_t LAB_EntitySet_RemoveComponent(LAB_EntitySet* set, size_t entity_id, const LAB_EntityComponent* type);




 /**
  * Removes gaps between components and
  * reduces array capacities (to the minimal possible power of 2, always kept at a power of 2)
  * invalidates all entity references
  */
 size_t LAB_EntitySet_Optimize(LAB_EntitySet* set, size_t id, const LAB_EntityComponent* type);











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
