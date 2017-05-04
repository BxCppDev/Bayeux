=================================
datatools ``examples/variants``
=================================

Introduction
============

 * Description:

   This  example illustrates  how to  use the  *configuration variant*
   mechanism  to configure  objects through  ``datatools::properties``
   containers with  dynamic possibility to  switch the values  of some
   properties, typically at program start.

   The  ``application`` class  represents  an algorithm  that must  be
   configured  and  initialized  before   to  execute.  The  algorithm
   expected to  be given a  sequence of configuration  parameters that
   are classified following three categories:

    * ``core`` : the general parameters of the algorithm object.
    * ``algo`` : the parameters related to the tuning of the algorithm
      itself.   Such  parameters  use  a property  key  prefixed  with
      ``algo.``.
    * ``gui`` : the parameters related  to some possible GUI companion
      tool  (not provided  in this  example).  Such  parameters use  a
      property key prefixed with ``gui.``.

 * Source files :

   * ``ex_variants.cxx`` : the example main program source.
   * ``config``   :  the   directory  for   configuration  files   and
     description of the variant parameters.

 * Configuration file:

   * ``config/foo.conf`` : This static  configuration file records the
     configuration parameters  used to initialize  the ``application``
     object and its components in  the main program.  No configuration
     variant  parameters are  used. This  is a  standard configuration
     file that uses the  ASCII format of the ``datatools::properties``
     class.
   * ``config/foo_with_variants.conf``  : This  configuration file  is
     similar to  the ``config/foo.conf`` file with  additional support
     for *variants*.  Some special directives  are added to allow some
     of  the  parameters to  be  modified  using the  Bayeux/datatools
     *configuration  variant*  mechanism.   This file  also  uses  the
     ``datatools::properties`` ASCII format with additional *variants*
     directives.
   * ``config/variants/foo_variants.conf``   :   This  is   the   main
     configuration  file  of  the   variant  repository  used  by  the
     application.        This      file       refers      to       the
     ``config/variants/registries/foo_core_registry.conf``,
     ``config/variants/registries/foo_algo_registry.conf``         and
     ``config/variants/registries/foo_gui_registry.conf`` files below.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/registries/foo_core_registry.conf``    :   This
     configuration file describes the  variant registry for parameters
     used by the *core* part of the ``application`` object in the main
     program.  The  set of resulting  variants stored in  the registry
     can be  modified on  the fly through  the datatools  interface in
     order to change the behaviour of  the program with respect to the
     default  ``config/foo_with_variants.conf``   configuration  file.
     This            file           refers            to           the
     ``config/variants/models/foo_core_variants.def``    file   below.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/registries/foo_gui_registry.conf``    :    This
     configuration file describes the  variant registry for parameters
     used by the *GUI* part of  the ``application`` object in the main
     program.  The  set of resulting  variants stored in  the registry
     can be  modified on  the fly through  the datatools  interface in
     order to change the behaviour of  the program with respect to the
     default  ``config/foo_with_variants.conf``   configuration  file.
     This            file           refers            to           the
     ``config/variants/models/foo_gui_variants.def`` file below.  This
     file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/registries/foo_algo_registry.conf``    :   This
     configuration file describes the  variant registry for parameters
     used by the *algorithm* part of the ``application`` object in the
     main  program.   The set  of  resulting  variants stored  in  the
     registry  can  be  modified  on the  fly  through  the  datatools
     interface in  order to change  the behaviour of the  program with
     respect   to    the   default   ``config/foo_with_variants.conf``
     configuration     file.     This     file    refers     to    the
     ``config/variants/models/foo_algo_variants.def``    file   below.
     This file uses the ``datatools::properties`` ASCII format.
   * ``config/variants/models/foo_core_variants.def``   :  This   file
     defines the configuration variants  and parameters models used to
     build the  *core* variant registry of  the ``application`` object
     in    the     main    program..      This    file     uses    the
     ``datatools::multi_properties`` ASCII format.
   * ``config/variants/models/foo_gui_variants.def``   :   This   file
     defines the configuration variants  and parameters models used to
     build the *GUI* variant registry of the ``application`` object in
     the      main     program..       This     file      uses     the
     ``datatools::multi_properties`` ASCII format.
   * ``config/variants/models/foo_algo_variants.def``   :  This   file
     defines the configuration variants  and parameters models used to
     build  the *algorithm*  variant registry  of the  ``application``
     object   in   the   main    program..    This   file   uses   the
     ``datatools::multi_properties`` ASCII format.


 * Build method: CMake
 * Built object :

   * ``ex_variants`` : executable linked with the ``Bayeux`` shared library.

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

.. code:: sh

   shell$ cp -a $(bxquery --exampledir)/datatools/variants /tmp/datatools_ex_variants
   shell$ cd /tmp/datatools_ex_variants
..

3. Build and install the example::

.. code:: sh

   shell$ mkdir _build.d
   shell$ cd _build.d
   shell$ cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
        ..
   shell$ make
   shell$ make install
   shell$ cd ..
..

4. Run the variant inspector:

   Generate a ReST formatted documentation about the variant repository: ::

.. code:: sh
   shell$ export FOO_CONFIG_DIR=$(pwd)/config
   shell$ bxvariant_inspector \
	  --variant-config "${FOO_CONFIG_DIR}/variants/foo_variants.conf" \
	  --variant-gui \
	  --variant-store "my_foo.profile" \
	  --action "doc" > foo.rst
   shell$ pandoc -r rst foo.rst > foo.html
   shell$ xdg-open foo.html &
..

   Print the current profile associated to the variant repository: ::

.. code:: sh
   shell$ bxvariant_inspector \
	--variant-config "${FOO_CONFIG_DIR}/variants/foo_variants.conf" \
	--variant-load "my_foo.profile" \
	--variant-gui \
	--action "profile"
..

5. Run the example::

.. code:: sh

   shell$ export FOO_CONFIG_DIR=./config
   shell$ ./_install.d/ex_variants
..

6. Clean::

.. code:: sh

      shell$ rm -fr _build.d
      shell$ rm -fr _install.d
..
