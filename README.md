# LAB
 
A Voxel-Engine, *infinite* world in all three spacial dimensions. Currently
work in progress. After the main engine has been written, custom modding
support gets added, for custom blocks etc... (Thats how the content should be
added).

# The language choice: C

Why do I use C for this project? For me its clean compared to other commonly
used languages. It forces you to write simpler code, that does not hide parts
of the code, that are important.  
In combination with the C-Preprocessor you can define Generic Datastructures
in C.

**Even if GitHub comes up with C++ or Objective C, the project is completely C.**
GitHub might be fooled by the use of template-headers, which involves a special
syntax of declaring functions, that is completely based on the C-Preprocessor.
The only thing that is not pure C is the use of GCCs attributes and builtins,
which can even be used with Clang.
