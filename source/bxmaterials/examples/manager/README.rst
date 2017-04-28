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
   * ``config/isotopes.conf`` : definitions of registered isotopes
   * ``config/elements.def`` : definitions of registered elements
   * ``config/materials.def`` : definitions of registered materials
   * ``config/material_aliases.def`` : definitions of registered material aliases
   * ``config/material_aliases.redef`` : redefinitions of some registered material
     aliases (supersedes some previous material alias definitions)

 * Build method : CMake

 * Binary files :

  * ``ex_manager`` : the example executable linked to the ``Bayeux`` DLL
  * ``bxmaterials_inspector`` : the official *inspector* executable from the ``Bayeux`` library.



Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory:

.. code:: sh

   $ cp -a $(bxquery --exampledir)/materials/manager /tmp/materials_ex_manager
   $ cd /tmp/materials_ex_manager
..

3. Build and install the example:

.. code:: sh

   $ mkdir _build.d
   $ cd _build.d
   $ cmake \
	  -DCMAKE_INSTALL_PREFIX=../_install.d \
	  -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
	  ..
   $ make
   $ make install
..

4. Inspect the material manager configuration:

.. code:: sh

   $ cd ..
   $ bxmaterials_inspector --manager-config "config/manager.conf" --with-decoration -M
..

5. Run the example program:

.. code:: sh

   $ ./_install.d/ex_manager
..

6. Clean:

.. code:: sh

   $ rm -fr _build.d
   $ rm -fr _install.d
..
