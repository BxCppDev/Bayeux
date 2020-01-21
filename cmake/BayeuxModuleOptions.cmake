#.rst:
# BayeuxModuleOptions
# -------------------
#

#-----------------------------------------------------------------------
# Optional dosimetry module
#
option(BAYEUX_WITH_LAHAGUE "Build Bayeux/lahague library module" OFF)
mark_as_advanced(BAYEUX_WITH_LAHAGUE)

#-----------------------------------------------------------------------
# Option for enabling Geant4 extension library module
#
option(BAYEUX_WITH_GEANT4_MODULE "Build Bayeux/mctools Geant4 Monte-Carlo module" ON)
mark_as_advanced(BAYEUX_WITH_GEANT4_MODULE)
cmake_dependent_option(BAYEUX_WITH_GEANT4_EXPERIMENTAL "Allow experimental Geant4 support" OFF "BAYEUX_WITH_GEANT4_MODULE" OFF)

# #-----------------------------------------------------------------------
# # Option for enabling MCNP extension library module
# #
# option(BAYEUX_WITH_MCNP_MODULE "Build Bayeux/mctools MCNP Monte-Carlo module" OFF)
# mark_as_advanced(BAYEUX_WITH_MCNP_MODULE)

#-----------------------------------------------------------------------
# Only for developers
option(BAYEUX_WITH_CUTS       "Build Bayeux/cuts module" ON)
mark_as_advanced(BAYEUX_WITH_CUTS)

option(BAYEUX_WITH_MATERIALS  "Build Bayeux/materials module" ON)
mark_as_advanced(BAYEUX_WITH_MATERIALS)

option(BAYEUX_WITH_MYGSL      "Build Bayeux/mygsl module" ON)
mark_as_advanced(BAYEUX_WITH_MYGSL)

option(BAYEUX_WITH_BRIO       "Build Bayeux/brio module" ON)
mark_as_advanced(BAYEUX_WITH_BRIO)

option(BAYEUX_WITH_DPP        "Build Bayeux/dpp module" ON)
mark_as_advanced(BAYEUX_WITH_DPP)

option(BAYEUX_WITH_GEOMTOOLS  "Build Bayeux/geomtools module" ON)
mark_as_advanced(BAYEUX_WITH_GEOMTOOLS)

option(BAYEUX_WITH_EMFIELD    "Build Bayeux/emfield module" ON)
mark_as_advanced(BAYEUX_WITH_EMFIELD)

option(BAYEUX_WITH_GENBB "Build Bayeux/genbb_help module" ON)
mark_as_advanced(BAYEUX_WITH_GENBB)

option(BAYEUX_WITH_GENVTX     "Build Bayeux/genvtx module" ON)
mark_as_advanced(BAYEUX_WITH_GENVTX)

option(BAYEUX_WITH_MCTOOLS    "Build Bayeux/mctools module" ON)
mark_as_advanced(BAYEUX_WITH_MCTOOLS)

option(BAYEUX_MINIMAL_BUILD
  "Build minimal Bayeux with only the Bayeux/datatools module, conflicts with BAYEUX_WITH_XXX build module options" OFF)
mark_as_advanced(BAYEUX_MINIMAL_BUILD)

# lahague module
set(Bayeux_WITH_LAHAGUE 0)
if(BAYEUX_WITH_LAHAGUE)
  set(Bayeux_WITH_LAHAGUE 1)
endif()

# geant4 module
set(Bayeux_WITH_GEANT4_MODULE 0)
if(BAYEUX_WITH_GEANT4_MODULE)
  set(Bayeux_WITH_GEANT4_MODULE 1)
endif()

# MCNP module
set(Bayeux_WITH_MCNP_MODULE 0)
# if(BAYEUX_WITH_MCNP_MODULE)
#   set(Bayeux_WITH_MCNP_MODULE 1)
# endif()

#-----------------------------------------------------------------------
# mctools module
set(Bayeux_WITH_MCTOOLS 0)
if(BAYEUX_WITH_MCTOOLS)
  set(Bayeux_WITH_MCTOOLS 1)
endif()

#-----------------------------------------------------------------------
# genbb_help module
set(Bayeux_WITH_GENBB 0)
if(BAYEUX_WITH_GENBB)
  set(Bayeux_WITH_GENBB 1)
endif()

#-----------------------------------------------------------------------
# genvtx module
set(Bayeux_WITH_GENVTX 0)
if(BAYEUX_WITH_GENVTX)
  set(Bayeux_WITH_GENVTX 1)
endif()

#-----------------------------------------------------------------------
# emfield module
set(Bayeux_WITH_EMFIELD 0)
if(BAYEUX_WITH_EMFIELD)
  set(Bayeux_WITH_EMFIELD 1)
endif()

#-----------------------------------------------------------------------
# geomtools module
set(Bayeux_WITH_GEOMTOOLS 0)
if(BAYEUX_WITH_GEOMTOOLS)
  set(Bayeux_WITH_GEOMTOOLS 1)
endif()

#-----------------------------------------------------------------------
# dpp module
set(Bayeux_WITH_DPP 0)
if(BAYEUX_WITH_DPP)
  set(Bayeux_WITH_DPP 1)
endif()

#-----------------------------------------------------------------------
# brio module
set(Bayeux_WITH_BRIO 0)
if(BAYEUX_WITH_BRIO)
  set(Bayeux_WITH_BRIO 1)
endif()

#-----------------------------------------------------------------------
# materials module
set(Bayeux_WITH_MATERIALS 0)
if(BAYEUX_WITH_MATERIALS)
  set(Bayeux_WITH_MATERIALS 1)
  set(Bayeux_WITH_MYGSL 1)
endif()

#-----------------------------------------------------------------------
# mygsl module
set(Bayeux_WITH_MYGSL 0)
if(BAYEUX_WITH_MYGSL)
  set(Bayeux_WITH_MYGSL 1)
endif()

#-----------------------------------------------------------------------
# cuts module
set(Bayeux_WITH_CUTS 0)
if(BAYEUX_WITH_CUTS)
  set(Bayeux_WITH_CUTS 1)
endif()

#-----------------------------------------------------------------------
# Resolve dependencies

if(Bayeux_WITH_LAHAGUE)
  set(Bayeux_WITH_MCTOOLS 1)
endif()

if(Bayeux_WITH_GEANT4_MODULE)
  set(Bayeux_WITH_MCTOOLS 1)
endif()

# if(Bayeux_WITH_MCNP_MODULE)
#   set(Bayeux_WITH_MCTOOLS 1)
# endif()

if(Bayeux_WITH_MCTOOLS)
  set(Bayeux_WITH_GENVTX 1)
  set(Bayeux_WITH_GENBB 1)
  set(Bayeux_WITH_GEOMTOOLS 1)
  set(Bayeux_WITH_EMFIELD 1)
endif()

if(Bayeux_WITH_GENBB)
  set(Bayeux_WITH_MYGSL 1)
  set(Bayeux_WITH_GEOMTOOLS 1)
endif()

if(Bayeux_WITH_GENVTX)
  set(Bayeux_WITH_MYGSL 1)
  set(Bayeux_WITH_GEOMTOOLS 1)
endif()

if(Bayeux_WITH_EMFIELD)
  set(Bayeux_WITH_GEOMTOOLS 1)
endif()

if(Bayeux_WITH_GEOMTOOLS)
  set(Bayeux_WITH_MYGSL 1)
  set(Bayeux_WITH_MATERIALS 1)
endif()

if(Bayeux_WITH_DPP)
  set(Bayeux_WITH_BRIO 1)
  set(Bayeux_WITH_CUTS 1)
  set(Bayeux_WITH_MYGSL 1)
endif()

if (BAYEUX_MINIMAL_BUILD)
  set(Bayeux_WITH_GEANT4_MODULE 0)
  #set(Bayeux_WITH_MCNP_MODULE 0)
  set(Bayeux_WITH_LAHAGUE 0)
  set(Bayeux_WITH_MCTOOLS 0)
  set(Bayeux_WITH_GENVTX 0)
  set(Bayeux_WITH_GENBB 0)
  set(Bayeux_WITH_EMFIELD 0)
  set(Bayeux_WITH_GEOMTOOLS 0)
  set(Bayeux_WITH_DPP 0)
  set(Bayeux_WITH_BRIO 0)
  set(Bayeux_WITH_MYGSL 0)
  set(Bayeux_WITH_MATERIALS 0)
  set(Bayeux_WITH_CUTS 0)
endif()

# end
