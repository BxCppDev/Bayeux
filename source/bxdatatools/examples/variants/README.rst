=================================
datatools ``examples/variants``
=================================

Introduction
============

 * Description:
   this example illustrates how to use the configuration variant
   mechanism to configure object through ``datatools::properties`` containers.

 * Source files :

   * ``ex_variants.cxx`` : the example main program source.
   * ``config`` : the directory for configuration  files

 * Configuration file:

   * ``config/foo.conf`` :
     This static configuration file records the configuration parameters
     used to initialize the ``application`` object and its components in the main program.
     No configuration variant parameters are used.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/foo_with_variants.conf`` :
     This configuration file is similar to the ``config/foo.conf`` file with support for variants.
     Some special directives are added to allow some of the parameters to be modified using the datatools variant mechanism.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/foo_variance.conf`` :
     This is the main configuration file of the variant repository used by the application.
     This file refers to the ``config/variants/registries/foo_core_registry.conf``,
     ``config/variants/registries/foo_algo_registry.conf`` and ``config/variants/registries/foo_gui_registry.conf`` files below.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/registries/foo_core_registry.conf`` :
     This configuration file describes the variant registry
     for parameters used by the *core* part of the ``application`` object in the main program.
     The set of resulting variants stored in the registry can be modified on the fly through the datatools
     interface in order to change the behaviour of the program with respect to the default
     ``config/foo_with_variants.conf`` configuration file.
     This file refers to the ``config/variants/models/foo_core_variants.def`` file below.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/registries/foo_gui_registry.conf`` :
     This configuration file describes the variant registry
     for parameters used by the *GUI* part of the ``application`` object in the main program.
     The set of resulting variants stored in the registry can be modified on the fly through the datatools
     interface in order to change the behaviour of the program with respect to the default
     ``config/foo_with_variants.conf`` configuration file.
     This file refers to the ``config/variants/models/foo_gui_variants.def`` file below.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/registries/foo_algo_registry.conf`` :
     This configuration file describes the variant registry
     for parameters used by the *algorithm* part of the ``application`` object in the main program.
     The set of resulting variants stored in the registry can be modified on the fly through the datatools
     interface in order to change the behaviour of the program with respect to the default
     ``config/foo_with_variants.conf`` configuration file.
     This file refers to the ``config/variants/models/foo_algo_variants.def`` file below.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/models/foo_core_variants.def`` :
     This file defines the configuration variants and parameters models
     used to build the  *core* variant registry of the ``application`` object in the main program..
     This file uses the ``datatools::multi_properties`` ASCII format.
   * ``config/variants/models/foo_gui_variants.def`` :
     This file defines the configuration variants and parameters models
     used  to build the *GUI* variant registry of the ``application`` object in the main program..
     This file uses the ``datatools::multi_properties`` ASCII format.
   * ``config/variants/models/foo_algo_variants.def`` :
     This file defines the configuration variants and parameters models
     used  to build the *algorithm* variant registry of the ``application`` object in the main program..
     This file uses the ``datatools::multi_properties`` ASCII format.


 * Build method: CMake
 * Built object :

   * ``ex_variants`` : executable linked with the ``Bayeux`` shared library.

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/datatools/examples/variants /tmp/datatools_ex_variants
      shell> cd /tmp/datatools_ex_variants

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

4. Run the example::

      shell> ./ex_variants

6. Clean::

      shell> rm ex_variants
      shell> rm -fr __build
