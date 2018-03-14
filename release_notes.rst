=============================
Bayeux 3.2.0 Release Notes
=============================

Bayeux 3.2 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux 3.2.0 requires Linux (for example Ubuntu 16.04 or later),
or macOS 10.


Additions
=========

* Add a new smart printing interface method in the ``datatools::i_tree_dumpable`` interface class.


  The new method is:

  .. code::

     virtual void print_tree(std::ostream & out_ = std::clog,
                             const boost::property_tree::ptree & options_ = empty_options()) const;
..

  The method is passed a property tree object which hosts formatting rules and/or options
  as key/value pairs (see http://www.boost.org/doc/libs/1_63_0/doc/html/property_tree.html).
  Supported legacy keyed parameters are:

  - "indent" : a character string used to indent each line of the printed output,
  - "title" : a character string that will add an explicit title before the printed output,
  - "inherit" : a boolean flag that indicates that the printed output of a daughter
    class should use an adapted format of the format of its mother class.

  These parameters are the ones historically used by the  "tree_dump" method as normal
  arguments. With the new interface, it is now possible to add more formatting parameters
  depending of the complexity of the instance to be printed.
  For example, one could add a "expand" boolean flag coupled to a "max-level" integer
  to control recursively printed output of an internal hierachical data structure.
  Of course, such additional formatting parameters should be documented.

  The old interface, using plain title and indentation string and inherit flag,
  is preserved :

  .. code::

     virtual void tree_dump(std::ostream & out_ = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const;
..

  But each new classes inheriting the ``datatools::i_tree_dumpable`` should now favour
  the new interface method.

* Add explicit release notes and archived notes.

* Add ``cmake/BayeuxDependencies.cmake`` file for the description of Bayeux's dependencies.

Removals
=========

None.

Changes
=======

* The  ``datatools::properties::fetch_real_with_explicit_unit`` method
  now uses  a more strict approach  to parse the unit  associated to a
  real  parameter with  its  expected explicit  unit (commit  5d71f78,
  merged PR #15)

* A new initialization  interface from the ``datatools::base_service``
  class.      The     (protected)      method     is     now     named
  ``datatools::base_service::_common_initialize(...)``  and should  be
  used  by  daughter  service  classes  in  place  of  the  deprecated
  ``datatools::base_service::common_initialize(...)``. See  an example
  in the ``cuts::cut_service::initialize`` method.

Fixes
=====

* Fix issue #11: remove a cyclic dependency (ea99c5a, merged from the release 3.1.X).
