[&#8592; Prefetch Queue](topic-prefetch-queue.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; Table](table.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `LAB`](LAB.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/topic-template-headers.xmd#L1)</small>
***

# Template Headers/Template sources
<small>*Topic*</small>  


In combination with the C-Preprocessor you can define Generic Datastructures
in C. That is done by special header files, which I call template headers
and template sources. Before including these you provide a set of parameters,
that describe the content type and other configuration of the datastructure.
A template header and a template source should only be included once per given
parameter set. Eg. the template header is included in a header file, the
template source is included in the corresponding source file. The header file
should be guarded (`#ifndef #define #endif` or `#pragma once`), so that the
ODR (One definition rule) is not broken. The macro `HTL_DEF` could be defined
with `inline` so that the template source can be included in a header. Note
that both the template header and the template source need to be included for
a given parameter set. A parameter set is selected by defining `HTL_PARAM`
with the corresponding identifier prefix of the defined parameters. After
the include it is recommended to undefine `HTL_PARAM` again. The identifier
prefix of the parameters could be the name of the structure to be defined.
It should not be a macro that maps to something other than itself.

