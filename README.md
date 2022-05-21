DICE
====

Matt Dillon's [DICE 68000 C compiler suite](http://apollo.backplane.com/FreeSrc/) and [DXMake](http://apollo.backplane.com/FreeBSDPorts/)

The trick is to use Matt Dillon's DXMake utility to compile the DICE sources.
I have ported it to macOS 14, changed the DMakefiles to use dxmake.

I have added to the  include/ directory the Amiga 3.1 and Amiga 3.9 clib 
and fd files. The sources are modified to generate Amiga 3.1 and Amiga 
3.9 lib files.

I have attached a compiled binary, with all the generated libs.

The Amiga includes themselves are not included in the source and in the
binary.


**NOTE**: Good luck!
