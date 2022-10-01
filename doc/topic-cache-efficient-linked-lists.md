[&#8592; Spatial Cache-efficient Hash Map](topic-cache-efficient-hash-map.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; Table](table.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; Cellular Light Propagation](topic-cellular-light-propagation.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/topic-cache-efficient-linked-lists.xmd#L1)</small>
***

# Spatial Cache-efficient Linked Lists
<small>*Topic*</small>  


*Status:* Not implemented

Lists are cache inefficient, but useful, when it is important that elements in
a list can be added and removed easily. Here lists are used for the queueing
mechanism. Chumks can be added to a queue without the need to allocate
additional memory, instead the memory associated with the list node is the
chunk itself, that means the previous and next pointers are stored in the
chunk itself.


```
list head
    │                    ╭───────────╮
    v                    │           v 
 ╭─>┌──────────────────┬─┼─┐      ╭─>┌──────────────────────┐
 │  │        Header    │ ╵ │      │  │        Header        │
 │  ├──────────────────┴───┤      │  ├──────────────────────┤
 │  │ ┌───┬──────────┬───┐ │    ╭────┼>┌───┬──────────┬───┐ │
 ├──┼─┼─╴ │   Data   │ ╶─┼─┼─╮  │ ├──┼─┼─╴ │   Data   │ ╶─┼─┼─╮
 │  │ ├───┼──────────┼───┤<┼─╯  │ │  │ ├───┼──────────┼───┤ │ │
 ├──┼─┼─╴ │   Data   │ ╶─┼─┼─╮  │ ├──┼─┼─╴ │    --    │ - │ │ │
 │  │ ├───┼──────────┼───┤ │ │  │ │  │ ├───┼──────────┼───┤<┼─╯
 ├──┼─┼─╴ │    --    │ - │ │ │  │ ├──┼─┼─╴ │   Data   │ 0 │ │
 │  │ ├───┼──────────┼───┤<┼─╯  │ │  │ ├───┼──────────┼───┤ │
 ├──┼─┼─╴ │   Data   │ ╶─┼─┼────╯ ├──┼─┼─╴ │    --    │ - │ │
 │  │ ├───┼──────────┼───┤ │      │  │ ├───┼──────────┼───┤ │
 ╰──┼─┼─╴ │    --    │ - │ │      ╰──┼─┼─╴ │    --    │ - │ │
    │ └───┴──────────┴───┘ │         │ └───┴──────────┴───┘ │
    └──────────────────────┘         └──────────────────────┘
```
*Figure 1:* Two allocation blocks linked together, previous pointers left out
for brevity


## Insertion, Allocate with Hints

To improve cache efficiency, near chunks are tried to be allocated in the
same block. A near chunk is used as a hint in the allocation, that means an
empty slot is searched first in the allocation block of the hint chunk.
If there is no empty slot, the next (or previous) allocation block is tested,
because it might also be in cache when the previous is in cache, by list
iteration. This is repeated by a fixed amount, the most optimal fixed amout
might be dependent on the specific hardware.



## Allocation Avoidance

If chunks are too far apart, chunks are not allocated in the same block. instead
a new allocation block is created.



## Header Pointer

The header pointer inside block elements can be optimized to just a few bits,
namely the index of the node in the current block. Using this index the address
of the header can be calculated easily. Similarly, the next and previous
pointer can also be optimized to few bits (1 bit more). If the additional bit
is set, a next pointer would point into the next allocation block. For a
previous pointer it would point into the previous allocation block.



## Multiple Queues/Lists

If multiple queues or lists need to be stored, either multiple next or previous
pointers are created or if a chunk/node is only associated with a single
queue/list, an identifier of the queue is stored additionally to a single next
and previous pointer.


