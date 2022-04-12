====================================
Tools for Bayeux developpers only
====================================

This directory hosts dedicated scripts for quick build test
of the Bayeux library for Ubuntu Linux 18.04/20.04 and CentOS 7.5:

* ``build.sh``



Usage
================


Build and install
-----------------

* From the Bayeux base source directory, use the build/installation script:

  .. code:: bash
	    
     $ linuxbrew_setup # Please use here your own command(s) to setup Bayeux's dependencies
     $ ./tools/build.sh --help
     $ rm -fr /scratch/sw/Bayeux/build-develop
     $ ./tools/build.sh \
	    --nprocs 6 \
	    --build-base-dir /scratch/sw/Bayeux/build-develop \
	    --install-base-dir /scratch/sw/Bayeux/install-develop 
  ..

Special build
================

* Build:

  .. code:: bash

     $ bayeux_build_env
     $ ./tools/build.sh \
	    --nprocs 6 \
	    --build-base-dir /scratch/sw/Bayeux/build-test \
	    --install-base-dir /scratch/sw/Bayeux/install-test \
	    -with-geant4 \
	    -with-qt 
	    
  ..
  
* Select a specific Boost prefix:

  .. code:: bash

     $ bayeux_build_g49_setup
     $ ./tools/build.sh \
	    --boost-root /scratch/ubuntu18.04/BxInstall/boost-1.69.0 \
	    --bayeux-prefix "-boost169-g49"
  ..


* Run a specific test:
  
  From the build directory:

  .. code:: bash

     $ ctest -VV -I 41,41
  ..


Special build
================


./tools/build.sh \
	    --nprocs 6 \
	    --build-base-dir /scratch/sw/Bayeux/build-minimal \
	    --install-base-dir /scratch/sw/Bayeux/install-minimal \
	    --minimal-build





.. end
   

  
