===================================
datatools installation instructions
===================================

Dependencies
============

The datatools_ C++ library package uses the CMake build system.
  .. _datatools: https://nemo.lpc-caen.in2p3.fr/wiki/Software/Bayeux/datatools

It uses the following tools :

  - CMake_ >= 2.8

.. _CMake: http://www.cmake.org/

  - doxygen_

.. _doxygen: http://www.doxygen.org/‎

  - a C++ compiler (`g++`_ or clang_)

.. _g++: http://gcc.gnu.org/
.. _clang: http://clang.llvm.org/

  - pandoc_

.. _pandoc: http://johnmacfarlane.net/pandoc/


It depends on the following third-party software :

  - Boost_ >= 1.47

.. _Boost: http://www.boost.org/

  - CLHEP_ >= 2.1.0.1

.. _CLHEP: http://proj-clhep.web.cern.ch/proj-clhep

  - CAMP_  >= 0.7.1

.. _CAMP: https://github.com/tegesoft/camp


datatools is a component of the Bayeux_ aggregator meta-package.
We advise you to install datatools through Bayeux. However it is
possible to install a standalone version of datatools.

.. _Bayeux: https://nemo.lpc-caen.in2p3.fr/wiki/Bayeux



Third-party software setup
==========================

CLHEP, Boost and CAMP libraries must be installed and setup first.
We advise you to install this packages through the Cadfael_ aggregator package.

.. _Cadfael: https://nemo.lpc-caen.in2p3.fr/wiki/Software/Cadfael


Installation through CMake
==========================

1. Change to the datatools source base directory::

     shell> cd <datatools source base dir>

2. Create a ``__build`` directory and make it the working directory::

     shell> mkdir __build
     shell> cd __build

3. Configure the installation to the ``<datatools installation base directory>`` directory::

     shell> cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=<datatools installation base directory> \
        -DCMAKE_BUILD_TYPE:STRING=Release \
        -DCMAKE_MODULE_PATH=<Cadfael installation directory>/share/cmake/Modules \
        ..

4. Build::

     shell> make

5. Generate the documentation::

     shell> make doc

6. Run tests::

     shell> make test

7. Installation (here in the ``<datatools installation base directory>`` directory)::

     shell> make install

8. You may safely remove the temporary build directory::

     shell> cd ..
     shell> rm -fr __build


Using datatools
===============

Through CMake
-------------

You must define the ``datatools_DIR`` path variable and make it
point to the datatools installation base directory. You may use ``cmake`` like this::

     shell> cmake -Ddatatools_DIR:PATH=<datatools installation base directory> ...

where your project's ``CMakeLists.txt`` file contains::

     find_package(datatools 5.0.0 REQUIRED)

Through the ``datatools-config`` script
---------------------------------------

datatools comes with the ``datatools-config`` helper script. You should
setup your ``PATH`` environment variable to contain the ``<datatools installation base directory>/bin``
directory::

    shell> export PATH=<datatools installation base directory>/bin:${PATH}
    shell> export LD_LIBRARY_PATH=<datatools installation base directory>/lib:${LD_LIBRARY_PATH}

Then you will be able to use the ``datatools-config`` to properly set compilation flags with a makefile::

    CPPFLAGS=$(shell datatools-config --cflags)
    LDFLAGS=$(shell datatools-config --ldflags)

Online help::

    shell> datatools-config --help

Examples
========

The ``<datatools installation base directory>/share/datatools/examples/`` contains some standalone
examples that illustrates the use of some datatools classes and concepts.


----

`F. Mauger`_

.. _F. Mauger: mailto:mauger@lpccaen.in2p3.fr

Updated: 2013-06-11






