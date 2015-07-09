GvgAmpClient   {#mainpage}
==========================

[TOC]

This library implements certain parts of the GVG AMP protocol specification.
See \ref public_api "Public API" for the details of the already implemented functions.

To browse the source in the svn repository [click here.] (http://10.28.98.14/viewvc/mtva/GVG_AMP/GvgAmpClient)

The two core documents used for the implementation were:

   1. [GVG AMP Specification V2.15](http://10.28.98.14/viewvc/mtva/GVG_AMP/GvgAmpClient/doc/AMP_Spec.pdf?view=co)
   2. [K2 Protocol Developers Guide](http://10.28.98.14/viewvc/mtva/GVG_AMP/GvgAmpClient/doc/K2_Protocol_Developers_Guide.pdf?view=co)

To make the library usable on different programing languages it is wrapped with swig.
Right now the code is available as a CPP, Java and PHP class. There is an ongoing effort 
to provide Python bindings in the near future. To automate the building process cmake is used. 
On top of the different language wrappers, the code has been documented with doxygen. 
Further readings on the technologies are available on the links below:

   * [SWIG] (http://www.swig.org)
   * [cmake] (http://www.cmake.org)
   * [doxygen] (http://www.doxygen.org)

Source code structure
=====================

There are 4 directories in the source tree:

   * src:     The src directory contains the core library implementation as a CPP class definition.
   * include: The header file that declairs the GvgAmpClient class.
   * java:    The swig interface file and the cmake config file for the java wrapper.
   * php:     The swig interface file and the cmake config file for the php wrapper.
   * doc:     The Doxygen ini file and the cmake config file for the documentation.
   * test:    Test server and code samples on the different programing languages.

Build instructions
==================

The long term goal is to make the library compilable under Windows, as of today you can build it
under Linux only.

To build the library you need to have the following packages installed on your computer:
   * apt-get install subversion
   * apt-get install sudo
   * apt-get install cmake
   * apt-get install swig
   * apt-get install openjdk-7-jdk
       * export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64
   * apt-get install doxygen
   * apt-get install php-dev
   * apt-get install php-config

To build the library:

1. Get the source: `svn export http://10.28.98.14/svn/MTVA/GVG_AMP/GvgAmpClient GvgAmpClient`
2. Create a build directory: `mkdir GvgAmpClient_release`
3. Change to the build directory: `cd GvgAmpClient_release`
4. Run Cmake to create the build structure: `cmake ../GvgAmpClient`
5. Build the release: `make`
6. Optional build the documentation: `make doc`   
7. Optional install:  `sudo make install`

If there are errors in the build process, enable verbose_make in the main CMakeLists.txt.
In 'set(CMAKE_VERBOSE_MAKEFILE OFF)` change OFF to ON and recreate the build tree. (You can simply 
delete the whole build folder.)

 

Howto use the library
=====================

C++
---

The static (libGvgAmpClient.a) and the dynamic (libGvgAmpClientShared.so) libraries are available
in the src directory of the build tree. The include file is in the include directory of the source 
tree. 

You can find a C++ source sample [here.] (http://10.28.98.14/doxygen/GVG_AMP/test_gvgampclient_8cpp-example.html)


PHP
---

1. Copy libGvgAmpClientPhp.so from the php folder to an appropriate location. `
 /usr/local/php/lib` is a good choice - make install does that - but you need root privileges.

2. Add the library to php.ini (/etc/php5/cli/php.ini) by adding the following line to it:
      extension=/usr/local/php/lib/libGvgAmpClientPhp.so

3. Copy the pGvgAmpClient.php from the php/src folder of the build tree to your working directory.

4. See the [example] (http://10.28.98.14/doxygen/GVG_AMP/test_gvgampclient_8php-example.html) code for a test run.

