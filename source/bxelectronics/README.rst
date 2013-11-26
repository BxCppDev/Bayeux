Bayeux/electronics
==================


Author: F.Mauger <mauger@lpccaen.in2p3.fr>

Introduction
------------

The  Bayeux/electronics  module  aims  to model  the  layout  of  a
front end   electronics   system   for  an   nuclear/particle   physics
experiment: racks, crate, boards and embedded components like ASIC and
FPGA chips.


Note: 2013-11-22:

 * This is a preliminary implementation.
 * It is not integrated in Bayeux but needs it.


TODO
----

 * A component manager with:
   - a component factory
   - a EID manager (alal geomtools with GIDs)
   - a mapping system
 * An electronics service (ala geometry service in geomtools)
 * Concept of component I/O (analog/digital)
   with specific traits and addressing scheme (link to GID)
 * Concept of cables and connectors...

DEVELOPMENT
-----------

We can build Bayeux/electronics as a standalone library
for now.

Steps: ::

1. Setup Cadfael and Bayeux.

2. Cd in the devel directory: ::

    $ cd devel

3. Build the library and test programs: ::

    $ ./bootstrap_standalone.bash

4. Manually test the test programs: ::

    $ ELECTRONICS_TESTING_DIR=../testing ./_build/BuildProducts/bin/tests/test_component_manager


--
