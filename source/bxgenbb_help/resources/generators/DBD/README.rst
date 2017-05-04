==================================
 DBD Generators from Bayeux/genbb
 ==================================

:Author: F.Mauger <mauger@lpccaen.in2p3.fr>
:Date: 2017-02-07

Contents
--------

Sample DBD generator definition files:

* ``Ca48.def`` : a few DBD processes for the Ca-48 isotope
* ``Ge76.def`` : a few DBD processes for the Ge-76 isotope


Documentation
-------------

The  ``genbb::wdecay0`` class  is a  C++ port  of the  original Decay0
fortran code. You can generate  some documentation about its usage and
configuration through the following command (with Bayeux setup on your
system):

.. code: sh

   $ bxocd_manual --class-id "genbb::wdecay0" --action "show" > wdecay0.rst
   $ rst2html wdecay0.rst > wdecay0.html
   $ xdg-open wdecay0.html &
..


This should help you to create the DBD generators definitions you need
for your application.
