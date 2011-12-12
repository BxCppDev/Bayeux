# Locate datatools library
# This module defines
#  DATATOOLS_FOUND, if false, do not try to link to DATATOOLS
#  DATATOOLS_LIBRARIES
#  DATATOOLS_INCLUDE_DIR, where to find datatools/datatools_config.h

FIND_PATH(DATATOOLS_INCLUDE_DIR datatools/datatools_config.h
  HINTS
  $ENV{DATATOOLS_ROOT_DIR}
  PATH_SUFFIXES include
  PATHS
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\datatools]
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

FIND_LIBRARY(DATATOOLS_LIBRARY 
  NAMES datatools datatools_bio
  HINTS
  $ENV{DATATOOLS_ROOT_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\datatools]
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

SET(DATATOOLS_LIBRARIES "${DATATOOLS_LIBRARY}" CACHE STRING "DATATOOLS Libraries")

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DATATOOLS_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DATATOOLS DEFAULT_MSG DATATOOLS_LIBRARIES DATATOOLS_INCLUDE_DIR)

MARK_AS_ADVANCED(DATATOOLS_INCLUDE_DIR DATATOOLS_LIBRARIES DATATOOLS_LIBRARY)

