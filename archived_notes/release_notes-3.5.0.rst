=============================
Bayeux 3.5.0 Release Notes
=============================

Bayeux 3.5.0 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux  3.5.0 requires  Linux (recommended  Ubuntu >=18.04  (preferred
20.04), CentOS >=7.5), or macOS 10.


Additions
=========

* Add ``geomtools::plain_model`` class.
* Add                ``geomtools::i_model::destroy``               and
  ``geomtools::i_model::_at_destroy``  methods  for possible  cleaning
  from within a specific model.
* Add  load/store  methods in  ``geomtools::tessellated_solid``  class
  (using  a  simple  ASCII  format,  alternative  to  the  STL  import
  features).
* Add
  ``geomtools::model_with_internal_items_tools::init_internal_items``
  method  to  enable  automatic  setup of  internal  physical  volumes
  without  using  initialization through  a  ``datatools::properties``
  object                            (like                           in
  ``geomtools::model_with_internal_items_tools::plug_internal_models``).
* Add ranking features in the datatools variant system. Some dedicated
  auxiliary data are parsed from any CSV  file used for large enumeration of
  string values for an enumerated string variant parameter. A few levels of UI
  ranking             directives             are             available
  (``rank=highlight|first|second|third|last``).   The variant  GUI now
  provides  a check  box per  variant  registry panel  to inhibit  the
  display of values of *secondary* interest in combo boxes (for string
  parameters only).
* Add  support for  ``GNUPLOT_DRAWER_DEBUG`` environment  variable for
  debug print from the ``geomtools::gnuplot_drawer``.

  
Removals
=========


Changes
=======

* Better support for model cleaning in ``geomtools::model_factory``.
* The interface of the  ``geomtools::i_model`` abstract class has been
  changed.

  - the ``geomtools::i_model::_at_construct``  protected method  has a
  simpler signature. It does not use the model name parameter anymore,
  because it  was unused  and confusing. All  model classes  have been
  updated to take into account this interface change.
  - the             ``geomtools::i_model::_at_destroy``            and
    ``geomtools::i_model::destroy`` methods have been added to fullfil
    some possible needs at destruction stage in some geometry models.
* The ``geomtools::simple_shaped_model`` class has been refactored for
  a better  management of  some internals for  the tube,  polycone and
  polyhedra shapes  when using  the *filled_by_envelope* mode.   It is
  now  possible  to define  some  envelope  geometrical tolerance  and
  automatically generate an envelope with  some margins along the main
  axis of the  shape (X, Y, Z, radial...),  typically at submicrometer
  scale  in  the  geometry  setup, to  avoid  rounding  issues  during
  tracking  of particle  (Geant4).   The envelope  can  be defined  by
  inflation (default) of  the contained shape, or by  deflation of the
  contained shape. This  experimental feature is rather  tricky to use
  so  it  will  need  to  be checked  in  real  situations  in  Geant4
  (Falaise). Not all shapes can be addressed safely through this mechanism due
  to the simplicity of topological approach used to build the envelope
  by inflation or deflation of the contained shape.

  New properties  are thus available  from the model  configuration. By
  default, none  of these properties  are defined and the  envelope is
  build with  zero margin with  respect to the contained  shape (tube,
  polycone or polyhedra) :
  
  .. code::

     filled_mode : string = "by_envelope"
     envelope.tolerance.r     : real as length = 0.1 um  # Radial distance
     envelope.tolerance.x     : real as length = 0.1 um  # X-axis distance
     envelope.tolerance.y     : real as length = 0.1 um  # Y-axis distance
     envelope.tolerance.z     : real as length = 0.1 um  # Z-axis distance
     envelope.tolerance.phi   : real as angle  = 0.1 degree # Longitude angular tolerance
     envelope.tolerance.theta : real as angle  = 0.1 degree # Colatitude angular tolerance
     envelope.deflated : boolean = false # Envelope defined by an inflated version
                                         # of the contained shape outer bounds (default)
  ..
 
Fixes
=====
    
Bugs
====


.. end
