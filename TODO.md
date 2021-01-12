# TODO list

## entity system
  - datastructure per chunk in the world
  - view
    - add render pass for dynamicly rendered entities.
    - triangle allocation in a mesh per chunk for the entities
    - rendering entites to the allocated region in the mesh
    - implement animation to vertices by adding the following fields, a shader vertex program should
      do the interpolation (only linear? rotation?) between the given points
          float x0, y0, z0;
          float x1, y1, z1;
          timepoint firsttime; // timepoint when x0, y0, z0 are reached
          timepoint endtime;   // timepoint when x1, y1, z1 are reached, note: extrapolation is ok
      velocities of entities should be included in this calculation.

  - world sends updates if entities and so also portal planes have changed, some entity changes should not
    be sent to the view - update filtering
    - eg. only velocity changes are sent to the view, whenever an entity moved.

## view redesign, allow reflections
  - meshes of chunks should be reusable -> for portal planes
  - data structure to store the meshes
  - model order independent of meshes -> reuse same mesh but with a different order (different viewing angle)
  - tree data structure to store portal planes
  - world-chunks does contain information about portal planes, should happen in combination with block entities


## chunk regions
  - 16³? or 4³? cube of chunks
    - 4*4*4 = 64 -> 64 bitset
  - index into array: chunkid(x, y, z) = x | y << 2 | z << 4
  - remove neighbors from chunks
    - because these are now easy to find in the chunk region, if
	  they are in the same region
	- add neighbors to regions, like it works now (in the view)
  - are stored as one file: region 1:1 file:
    - a bitset is stored into the file indicating the stored chunks
	- this bitset is iterated to load the chunks.
	- eg. if the bitset only includes the first bit (the index is 0, whole bitset is 1)
	  the coordinates x,y,z = 0,0,0 are used for the chunk,
	  if the first bit was not set but the second, the first read chunk is the chunk
	  at x,y,z = 1,0,0 relative to the region, and so on
	  the x,y,z coordinate are determined by the bitindex by plugging it into the inverse
	  of the chunkid-function.
  - entity environment per region
    - better than per chunk?
	- holds information for every entity, entities are split into
      components, some components are shared among all entity classes, others are uniqe to
	  a specific entity class.
	  - for example a position component is in almost every entity.
	- every entity indexed by an id
	- entity classes not hardcoded
	  - an entity descriptor is stored with (array of pointers to the component descriptors| component type enum bitset) and
	    indices into the component array for a given descriptor
	  - component index array sorted?
	- component types:
	  - hardcoded static structures
	  - could be dynamically created? or not: bitset possible
	  - the hardcoded component types have fixed indices in the array of component arrays:
	- max capacity of entities per chunk region?
	  - 1<<16?




# DONE
- Implement hash-map for chunk-peeking
  - view!
- colored light
- use VBOs
