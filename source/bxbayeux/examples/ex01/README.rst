===============================
``BxBayeux`` example ``ex01``
===============================

A micro-project that depends on some parts of Bayeux.

Quick start
===========

1. Build, install and setup the Bayeux library:

.. code:: sh

   $ brewsh
   $ do_bayeux_setup
..

2. Build and install the example:

.. code:: sh

   $ mkdir _build.d
   $ cd _build.d
   $ cmake .. \
	  -DCMAKE_INSTALL_PREFIX=../_install.d \
	  -DBayeux_DIR=$(bxquery --cmakedir) \
	  ..
   $ make
   $ make install
..

3. Run the example:

.. code:: sh

   $ ./ex01_user
   $ echo $?
..
