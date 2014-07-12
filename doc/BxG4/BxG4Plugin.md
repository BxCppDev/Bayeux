Bayeux Geant4 interface {#bxgeant4interface}
=======================

\tableofcontents

Components {#bxgeant4interface_components}
==========

**libBayeux_mctools_geant4**
:     This is the Bayeux/mctools Geant4 plugin that embeds the Geant4
      interface for the simulation of nuclear and particle physics
      experiments. It links against the Geant4 library (preferably
      provided by the Cadfael SDK).

**bxmctools_g4_mkskelcfg**
:     This script generates a skeleton configuration directory
      for a full simulation setup. This implies a hierarchical
      repository for the modeling of the geometry,
      the generation of vertexes and primary particles, the
      Geant4 session control.

**bxg4_production**
:     Given the proper set of configuration files,
      this executable is able to run a full Geant4 session.