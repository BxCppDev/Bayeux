
Bayeux example 01
=================

The Bayeux bxexample01 program illustrates the
use of some tools provided by Bayeux.


Installation through CMake
--------------------------

1. Change to the bxexample01/post_install source base directory::

     $ cd <bxexample01 source base dir>/post_install

2. Create a ``__build`` directory and make it the working directory::

     $ mkdir __build
     $ cd __build

3. Configure: ::

     $ cmake \
       -DCMAKE_INSTALL_PREFIX:PATH=../__install \
       -DCMAKE_FIND_ROOT_PATH:PATH="<Bayeux install dir>;<Cadfael install dir>" \
       ..

4. Build: ::

     $ make
     $ ./bxexample01 --interactive

5. Installation: ::

     $ make install
     $ cd ..
     $ ./__install/bin/bxexample01 --interactive

Automated bash script
---------------------

This is only tested on Ubuntu 12.04 LTS): ::

     $ ./process.sh

