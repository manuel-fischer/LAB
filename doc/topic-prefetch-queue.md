[&#8592; Spatial Cache-efficient Linked Lists](topic-cache-efficient-linked-lists.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; Table](table.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; Template Headers/Template sources](topic-template-headers.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/topic-prefetch-queue.xmd#L1)</small>
***

# Prefetch Queue
<small>*Topic*</small>  


*Status:* Not implemented

Simply store an additional pointer, that is some fixed amount of elements in
the queue behind. If an element gets removed, the pointer advances to the next
element and prefetches data associated with the element.

For multithreading, the global queue is split into thread specific queues.
When an element is popped from a thread specific queue, another element gets
popped from the global queue, the element gets pushed into the thread specific
queue and the data gets prefetched.

