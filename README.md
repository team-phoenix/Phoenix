Phoenix
=======

A super-slick libretro client powered by Qt5

Environment
-----------

Currently set up to build on Linux against Qt 5.3. Check the paths I've set in main.cpp to find out what files need to be present. You can get the libretro-super project by reading these directions:

http://www.libretro.com/index.php/wiki/compilation/linux/

and checking out the section "Building libretro cores". It might take a while to build all those cores, so maybe you should just pass the name of whatever core I'm using to the libretro-build.sh script.

Building
--------

```
qmake
make
```