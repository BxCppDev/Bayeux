===========================
geomtools ``examples/ex01``
===========================

Introduction
============

 * Description:

   This  example illustrates  how to configure and use the event
   generator manager.
   Also it is shown how to use the ``bxgenbb_inspector`` utility.

 * Configuration files :

   * ``config/manager.conf`` : the main configuration file of the geometry
     manager.
   * Event generators' configuration files :

     * ``config/generators/dbd.conf`` : some DBD event generators

Quick start
===========

1. Setup the Bayeux library within a Cadfaelbrew shell.
2. Make a copy of the example directory:

.. code:: sh

      $ cp -a $(bxquery --exampledir)/genbb_help/ex04 /tmp/genbb_help_ex04
      $ cd /tmp/genbb_help_ex04
..

Run the ``bxgenbb_inspector``
=============================

1. Print the list of available generators:

.. code:: sh

      $ bxgenbb_inspector \
	  --configuration config/manager.conf \
          --action list
..

2. Generate 10000 DBD decay events, build and save histograms in a ROOT file:

.. code:: sh

      $ bxgenbb_inspector \
	  --configuration config/manager.conf \
          --action shoot \
          --generator "bb2nu_Ge76_HE" \
          --prng-seed 314159 \
          --number-of-events 10000 \
          --modulo 1000 \
          --prompt \
          --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
          --output-file "histos_test.root"
..

3. Output histograms browsable from ROOT via the ``histos_test.root`` file:

.. code:: sh

      $ root histos_test.root
      root [1] TBrowser b;
      root [2] .q
..

4. Generate 100 DBD decay events, save them in a data file using the Boost
   serialization portable binary format:

.. code:: sh

      $ bxgenbb_inspector \
	  --configuration config/manager.conf \
          --action shoot \
          --generator "bb2nu_Ge76_HE" \
          --prng-seed 314159 \
          --number-of-events 100 \
          --modulo 10 \
          --output-mode "plain" \
          --output-file "test_DBD_100.xml"
..
