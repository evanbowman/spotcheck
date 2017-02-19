# SpotCheck

## Introduction
SpotCheck is a fast computer vision desktop app designed to identify and quantify eliptical features in three-dimensional height maps.

The application frontend is written with web technologies via nwjs, and the performance critical image processing parts of the program run on a C++ backend implemented as an addon to nodejs. The backend API exposes a javascript class, called backend, which can be used to asynchronously invoke C++ functions that reference OpenCV by delegating them to nodejs' libuv threadpool.

## Build Steps
Build dependencies: 
 * nodejs
 * python 3.0.0 > x >= 2.5.0
 * nwjs
 * nw-gyp
 * OpenCV (Windows build currently requires version 2.4.13 for the pre-compiled static libs)
 * Some version of bash
 * A C++ compiler (try these: Linux: gcc, macOS: clang, Windows: Visual Studio >= 2012)
 
Build Steps:
 1. Compile the backend:
 ```
 cd spotcheck/backend &&
 nw-gyp configure --target=0.18.3 &&
 nw-gyp build --target=0.18.3
 ```
 2. Go to project root and zip everything
 3. Rename the archive to SpotCheck.nw
 4. Now the app can be run with the nw command (assuming you installed nwjs globally with npm)

Note: for compiling with Visual Studio 2013, you may need to do this: http://stackoverflow.com/a/24882281
