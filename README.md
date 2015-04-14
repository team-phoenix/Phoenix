Phoenix
=======

A super-slick libretro frontend powered by Qt5.

<b>Phoenix is still under early development. Don't expect everything to work!<b>
-----------

Environment
-----------

Dependencies: SDL2 for input, Qt5.4 for OpenGL and UI design.


Building
--------

[![Build Status](https://secure.travis-ci.org/team-phoenix/Phoenix.png)](http://travis-ci.org/team-phoenix/Phoenix)

- Load the .pro file into Qt Creator. Compile. 
- Put libretro cores in the proper place, depending on your platform
  - Windows: Copy a set of the latest cores from the libretro buildbot for your platform (http://buildbot.libretro.com/nightly/) into a folder called "cores" in the directory the Phoenix executable resides. 
  - OS X: Install cores to /usr/local/lib/libretro.
  - Linux: Install cores using your package manager or manually to /usr/lib/libretro.
- Copy /databases/systemdatabase.db into the directory the Phoenix executable resides. Run Phoenix from Qt Creator, drag and drop some games into the library, configure your controllers, and go nuts!

Screenshots
------------
![ScreenShot](https://raw.github.com/team-phoenix/Phoenix/master/assets/mockup.png)
