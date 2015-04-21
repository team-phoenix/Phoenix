Phoenix
=======

A super-slick libretro frontend powered by Qt5. Designed to be dead simple to use, it features both a powerful library system to organize your game collection and a high-performance frontend to play your games within the application!

<b>Phoenix is still under early development. Don't expect everything to work!<b>
-----------


Building
--------

[![Build Status](https://secure.travis-ci.org/team-phoenix/Phoenix.png)](http://travis-ci.org/team-phoenix/Phoenix)

Dependencies: SDL2 for input, libsamplerate for audio resampling, Qt 5.4+ for OpenGL and UI design. All of these should be available on your favorite package manager (OS X, linux), we recommend msys2 on Windows.

1. Load the .pro file into Qt Creator. Compile. 
2. Put libretro cores in the proper place, depending on your platform
  - Windows: Copy a set of the latest cores from the libretro buildbot for your platform (http://buildbot.libretro.com/nightly/) into a folder called "cores" in the directory the Phoenix executable resides. 
  - OS X: Install cores to /usr/local/lib/libretro.
  - Linux: Install cores using your package manager or manually to /usr/lib/libretro.
3. Copy /databases/systemdatabase.db into the directory the Phoenix executable resides. 
4. Run Phoenix from Qt Creator, drag and drop some games into the library, configure your controllers, and go nuts!

Screenshot
------------
![ScreenShot](https://raw.github.com/team-phoenix/Phoenix/master/assets/mockup.png)
