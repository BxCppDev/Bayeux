# - Cpack setup for datatools
#
#-----------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
#
# Copyright (C) 2011 François Mauger, Université de Caen Basse-Normandie
# Contact: mauger@lpccaen.in2p3.fr
#
# This file is part of the datatools library.
#
# datatools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# datatools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with datatools.  If not, see <http://www.gnu.org/licenses/>.
#

#-----------------------------------------------------------------------
# General Configuration
#
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "François Mauger, LPC Caen - University of Caen Basse-Normandie")

foreach(_vtag VERSION VERSION_MAJOR VERSION_MINOR VERSION_PATCH)
  set(CPACK_PACKAGE_${_vtag} "${${PROJECT_NAME}_${_vtag}}")
endforeach()

set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/README.txt)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "datatools is a multi-purpose C++ library for data storage and manipulation")

set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING.txt)
set(CPACK_RESOURCE_FILE_README  ${PROJECT_SOURCE_DIR}/README.txt)
set(CPACK_RESOURCE_FILE_WELCOME ${PROJECT_SOURCE_DIR}/README.txt)

#-----------------------------------------------------------------------
# Source Package Configuration
#
set(CPACK_SOURCE_GENERATOR TBZ2;ZIP)
set(CPACK_SOURCE_IGNORE_FILES
  ${PROJECT_BINARY_DIR}
  "~$"
  "/CVS/"
  "/\\\\.svn/"
  "/.git/"
  "/\\\\.git/"
  "\\\\\\\\.swp$"
  "\\\\\\\\.swp$"
  "\\\\.swp"
  "\\\\\\\\.#"
  "/#"
  )

#-----------------------------------------------------------------------
# Binary Package Configuration
#
if(UNIX AND NOT APPLE)
  set(CPACK_GENERATOR STGZ)
elseif(APPLE)
  set(CPACK_SET_DESTDIR TRUE)
  set(CPACK_GENERATOR STGZ;PackageMaker)
elseif(WIN32 AND NOT UNIX)
  set(CPACK_GENERATOR ZIP;NSIS)
endif()

include(CPack)

