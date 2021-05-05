# LAB

![Newest Screenshot](screenshots/024.png)
[More screenshots](screenshots/screenshots.md)

A Voxel-Engine, *infinite* world in all three spacial dimensions. Currently
work in progress. After the main engine has been written, custom modding
support gets added, for custom blocks etc... (Thats how the content should be
added).

## Building
To build LAB follow the instructions [here](setup-dev.md)

## Documentation
The documentation can be found [here](doc/table.md).

## The language choice: C

Why do I use C for this project? For me its clean compared to other commonly
used languages. It forces you to write simpler code, that does not hide parts
of the code, that are important.  

### *"Blog"*
[Template Headers](doc/topic-template-headers.md)

**Even if GitHub comes up with C++ or Objective C, the project is completely C.**
GitHub might be fooled by the use of template-headers, which involves a special
syntax of declaring functions, that is completely based on the C-Preprocessor.
The only thing that is not pure C is the use of GCCs attributes and builtins,
which can even be used with Clang. When compiling with any other compiler,
the attributes and builtins are disabled and replaced by a pure C alternative:
either by nothing or by an (inline) implementation.

The python script [`xmd.py`](xmd.py) comes from one of my other repositories
[XMD](https://github.com/manuel-fischer/XMD) and generates the markdown files
of the [documentation](doc/table.md)
