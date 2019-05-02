====================================
Tools for Bayeux developpers only
====================================

This directory hosts dedicated scripts for quick build test
of the Bayeux library for Ubuntu Linux 18.04 and CentOS 7.5:

* ``build.sh``


From the Bayeux base source directory, use the build/installation script:

.. code:: bash

   $ linuxbrew_setup # Please use her your own command(s) to setup Bayeux's dependencies
   $ ./tools/build.sh --help   
   $ ./tools/build.sh --nprocs 6	  
..


New build script
================

Build Bayeux with Boost 1.68
----------------------------

.. code:: bash

   $ bxdev_setup
   $ ./tools/build_exp.sh --without-geant4 --boost-root /scratch/ubuntu18.04/BxInstall/boost-1.68.0 --bayeux-prefix "_boost168"
..

It breaks as shown in https://github.com/BxCppDev/Bayeux/issues/39 and,
from the build directory:

.. code:: bash

   $ ctest -VV -I 41,41
..
   
Build Bayeux with Boost 1.69
----------------------------

.. code:: bash

   $ bxdev_setup
   $ ./tools/build_exp.sh --without-geant4 --boost-root /scratch/ubuntu18.04/BxInstall/boost-1.69.0 --bayeux-prefix "_boost169"
..

From the build directory:

.. code:: bash

   $ ctest -VV -I 41,41
..


.. code:: bash

With Geant4
-----------

   $ bxdev_setup
   $ ./tools/build_exp.sh --boost-root /scratch/ubuntu18.04/BxInstall/boost-1.69.0 --bayeux-prefix "_boost169_withg4"
..

From the build directory:
.. code:: bash

   $ ctest -VV -I 41,41
..







.. end
   

  
