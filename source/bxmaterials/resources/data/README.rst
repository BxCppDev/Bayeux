Bayeux/materials resource files
===============================

This directory contains some data resource files used by the
Bayeux/materials modules:

 * ``mass.mas03``: masses and binding energies for all known isotopes (2003 edition)
 * ``mass.mas12``: masses and binding energies for all known isotopes (2012 edition)
 * ``isotopic_compositions_nist.dat``: isotopic composition of natural
   chemical elements (obtained from the following URL http://physics.nist.gov/cgi-bin/Compositions/stand_alone.pl?ele=&all=all&ascii=ascii&isotype=all).
 * A catalog of materials can be found at: http://www.nist.gov/srm/upload/SRM-Catalog-2010-Final.pdf
 * Data files:

    * ``basic/`` : definitions of basic materials (created: 2016-02-15)
    * ``tissue/`` : histology database (created: 2016-02-09)

 * Deprecated files (2016-02-15):

    * The original definitions of so-called *standard* materials are stored in the following files. You should probably prefer to use
      the definitions of *basic* materials above because we do not expect to maintain these deprecated files now.
    * ``std_isotopes.def`` : definition of isotopes of interest (original release, you should now use the definition files from the ``basics`` folders)
    * ``simple_elements.def`` : definition of elements of interest from atomic number Z and average atomic mass (no isotopic composition)
    * ``simple_materials.def`` : definition of materials of interest from elements (no isotopic composition)
    * ``std_elements.def`` : definition of elements of interest from natural abundance isotopic composition
    * ``std_materials.def`` : definition of materials of interest from elements with natural abundance isotopic composition

      * 2016-02-09: fix ``std::pfte`` to ``std::ptfe``
      * 2016-02-09: remove ``std::teflon``
      * 2016-02-09: move ``std::PVT`` to ``std::polyvinyltoluene``
      * 2016-02-09: move ``std::delrin`` to ``std::polyoxymethylene``
      * 2016-02-09: add ``std::air_stp`` (Particle Physics Booklet 2002 input)
      * 2016-02-09: ``std::air`` fix density to 1.205 kg/m3 at *domestic* temperature/pression conditions
      * 2016-02-09: add ``std::graphite``

    * ``std_material_aliases.def`` : definition of useful material aliases

      * 2016-02-09: first release
      * add ``std::teflon``
      * add ``std::delrin``
      * add ``std::PVT``
