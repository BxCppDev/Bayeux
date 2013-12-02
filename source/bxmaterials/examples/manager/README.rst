==============================
materials ``examples/manager``
==============================

Introduction
============

 * Description :

   This example illustrates the use of the ``materials::manager`` class
   to describe some arbitrary *isotopes*, *elements* and *materials* objects.

 * Source files :

   * ``ex_manager.cxx`` : the main program

 * Configuration files :

   * ``config/manager.conf`` : main configuration file of the materials manager
   * ``config/isotopes.conf`` : definition of registered isotopes
   * ``config/elements.def`` : definition of registered elements
   * ``config/materials.def`` : definition of registered materials
   * ``config/material_aliases.def`` : definition of registered material aliases
   * ``config/material_aliases.redef`` : redefinition of some registered material
     aliases (supersedes some previous definitions)

 * Build method : CMake

 * Binary files :

  * ``ex_manager`` : the example executable linked to the ``Bayeux`` DLL
  * ``materials_inspector`` : the official *inspector* executable from the ``Bayeux`` library.



Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

     $ cp -a $(bxquery --exampledir)/materials/examples/manager /tmp/materials_ex_manager
     $ cd /tmp/materials_ex_manager

3. Build and install the example::

     $ mkdir __build
     $ cd __build
     $ cmake \
       -DCMAKE_INSTALL_PREFIX=.. \
       -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
       ..
     $ make
     $ make install
     $ cd ..

4. Inspect the material manager configuration::

     $ bxmaterials_inspector --manager-config "config/manager.conf" --with-decoration -M

5. Run the example program::

     $ ./ex_manager

6. Clean::

     $ rm -f ex_manager
     $ rm -fr __build
