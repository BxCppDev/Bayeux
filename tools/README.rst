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
	    
     $ bayeux_build_env 
     [info] camp_setup: CAMP version 0.8.4 is now setup !
     [info] clhep_2_1_4_2_setup: CLHEP version 2.1.4.2 is now setup !
     [info] g4datasets_9_6_4_setup: Geant4 datasets version 9.6.4 is now setup !
     [info] geant4_9_6_4_setup: GEANT4 version 9.6.4 is now setup !
     [info] bxdecay0_1_1_0_setup: BxDecay0 version 1.1.0 is now setup !
     [info] root_6_16_00_setup: ROOT version 6.16.00 is now setup !
     $ ./tools/build.sh --help
     $ bxsuffix="-352"
     $ rm -fr /scratch/sw/Bayeux/build-develop${bxsuffix}
     $ rm -fr /scratch/sw/Bayeux/install-develop${bxsuffix}
     $ ./tools/build.sh \
	    --nprocs 6 \
	    --build-base-dir /scratch/sw/Bayeux/build-develop${bxsuffix} \
	    --install-base-dir /scratch/sw/Bayeux/install-develop${bxsuffix} 
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
	    --with-geant4 \
	    --with-qt 	    
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
   

  
