[&#8593; Table](table.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; Spatial Cache-efficient Linked Lists](topic-cache-efficient-linked-lists.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/topic-cache-efficient-hash-map.xmd#L1)</small>
***

# Spatial Cache-efficient Hash Map
<small>*Topic*</small>  


*Status:* Not implemented

Group chunks together into chunk clusters or super chunks.
An entry in the hash map corresponds to a super chunk, which is mapped from
the coordinates as a key. Similar to how multiple world coordinates are mapped
to a single chunk by dividing xyz by a factor of 16. Chunks can be associated
with a super chunk.

This technique can be combined with the allocated blocks as described in
cache efficient linked lists.

