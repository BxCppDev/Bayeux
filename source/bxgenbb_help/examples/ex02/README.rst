===========================
geomtools ``examples/ex02``
===========================

Introduction
============

 * Description:

   This  example illustrates  how to  create a new event generator
   class belonging to a shared library (plugin) and usable both from
   standalone mode (from a simple executable program) or from the
   ``bxgenbb_inspector`` utility.

 * Source files :

   * ``crazy_gun.hh`` : the header file of the ``crazy_gun`` class,
   * ``crazy_gun.cc`` : the source file of the ``crazy_gun`` class,
   * ``ex02.cxx`` : the main program.

 * Configuration files :

   * ``config/manager.conf`` : the main configuration file of the geometry
     manager.
   * Event generators' configuration files :

     * ``config/generators.def`` : some event generators.

 * Built object(s) :

     * ``libgenbb_help_ex02[.so|.dylib]`` : the plugin DLL that
       contains the new ''crazy_gun'' generator class.
     * ``ex02`` : the example executable linked to the ``libgenbb_help_ex02`` and
       ``Bayeux`` library.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/genbb_help/examples/ex02 /tmp/genbb_help_ex02
      shell> cd /tmp/genbb_help_ex02

3. Build and install the example::

      shell> mkdir __build
      shell> cd __build
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..
      shell> ls -1 ./lib/
      libgenbb_help_ex02.so
      shell> ls -1 ./bin/
      ex02

4. Run the example::

      shell> ./bin/ex02

5. Run the ``bxgenbb_inspector`` ::

      shell> bxgenbb_inspector \
               --configuration config/manager.conf \
               --action list

      shell> bxgenbb_inspector \
               --configuration config/manager.conf \
               --action shoot \
               --generator "Fool" \
               --prng-seed 314159 \
               --number-of-events 10000 \
               --modulo 1000 \
               --prompt \
               --delayed \
               --prompt-time-limit 1 \
               --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
               --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf" \
               --output-file "histos_Fool.root"

6. Check the output file:

   Output histograms browsable from ROOT via the ``histos_Co60.root`` file ::

      shell> root histos_Co60.root
      root [1] TBrowser b;
      root [2] .q


8. Clean::

      shell> rm ex02
      shell> rm -fr ./__build ./bin ./lib


9. Note:

   Run an automated test script: ::

      shell> ./process.sh
