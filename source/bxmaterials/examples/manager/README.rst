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
   * ``config/isotopes.conf`` : definition of isotopes
   * ``config/elements.def`` : definition of elements
   * ``config/materials.def`` : definition of materials
   * ``config/material_aliases.def`` : definition of material aliases
   * ``config/material_aliases.redef`` : redefinition of some material
     aliases (superseded some previous definitions)

 * Build method : CMake

 * Objects :

  * ``ex_manager`` : the example executable linked to the ``materials`` DLL



Quick start
===========

1. Build, install and setup the materials library
2. Make a copy of the example directory::

     shell> cp -a [materials install base directory]/share/materials/examples/manager /tmp/materials_ex_manager
     shell> cd /tmp/materials_ex_manager

3. Build and install the example::

     shell> mkdir __build
     shell> cd __build
     shell> cmake \
       -DCMAKE_INSTALL_PREFIX=.. \
       -Dmaterials_DIR=$(materials-config --prefix) \
       ..
     shell> make
     shell> make install
     shell> cd ..

4. Run the example::

     shell> ./ex_manager

5. Check the output file:

6. Clean::

     shell> rm -f ex_manager
     shell> rm -fr __build


