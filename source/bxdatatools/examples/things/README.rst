=============================
datatools ``examples/things``
=============================

Introduction
============

 * Description:

   This example illustrates how to  use some of the functionalities of
   the ``datatools::things`` class.  A ``datatools::things`` object is
   a generic  container that can  host and  handle (in term  of memory
   management)  arbitrary objects  stored  in  so-called *banks*.   It
   implements a dictionary  of banks each associated to  an object and
   identified by an unique name/label.  The objects that can be stored
   in      a     *things*      container     must      inherit     the
   ``datatools::i_serializable``    interface,    being    explicitely
   registered in  some internal  database within  the Bayeux/datatools
   library.  That  makes any  *things* mother object  serializable via
   the Boost/Serialization library.

   *Things*  objects are  typically used  to represent  an event  data
   model in particle/nuclear physics  where different stored banks are
   dedicated  to  the  representation  of  some  sub  data  models  of
   interest: event  header, collection  of raw hits,  calibrated data,
   track reconstruction data...

   A *things*  objects is  dynamically extensible in  such a  way some
   dedicated process can  add and fill new banks of  data, even if one
   does not know in advance their  type. Of course some banks can also
   be  deleted  if not  useful  anymore  at  some  step of  some  data
   processing. Note  that a  *things* object  is not  copyable. Client
   libraries can implement its own serializable classe, register it in
   the Bayeux/datatools  library.  This  will automatically  allow the
   storage of object of this new class in *things* objects.

 * Source files :

   * ``ex_things_1.cxx`` : the example main program source

 * Configuration file: none
 * Build method: CMake
 * Built object(s) :

   * ``ex_things_1`` : executable linked with the ``Bayeux`` DLL

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/datatools/examples/things /tmp/datatools_ex_things
      shell> cd /tmp/datatools_ex_things

3. Build and install the example::

      shell> mkdir _build.d
      shell> cd _build.d
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> ./ex_things_1 --debug

5. Clean::

      shell> rm ex_things_1
      shell> rm -fr _build.d
