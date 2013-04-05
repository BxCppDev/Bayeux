# - Cpack setup for genvtx
#
#-----------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
#
# Copyright (C) 2011 François Mauger, Université de Caen Basse-Normandie
# Contact: mauger@lpccaen.in2p3.fr
#
# This file is part of the genvtx library.
#
# genvtx is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# genvtx is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with genvtx.  If not, see <http://www.gnu.org/licenses/>.
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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "genvtx is a multi-purpose C++ library for data storage and manipulation")

set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING.txt)
set(CPACK_RESOURCE_FILE_README  ${PROJECT_SOURCE_DIR}/README.txt)
set(CPACK_RESOURCE_FILE_WELCOME ${PROJECT_SOURCE_DIR}/README.txt)

#-----------------------------------------------------------------------
# Component Configuration
#
#set(CPACK_COMPONENTS_ALL bin devel apidoc utils)
#set(CPACK_ALL_INSTALL_TYPES Runtime Development)

#set(CPACK_COMPONENT_BIN_DISPLAY_NAME "genvtx library")
#set(CPACK_COMPONENT_BIN_DESCRIPTION "The genvtx library binaries")
#set(CPACK_COMPONENT_BIN_INSTALL_TYPES Runtime Development)

#set(CPACK_COMPONENT_DEVEL_DISPLAY_NAME "genvtx development files")
#set(CPACK_COMPONENT_DEVEL_DESCRIPTION "C++ header files, import library")
#set(CPACK_COMPONENT_DEVEL_DEPENDS bin)
#set(CPACK_COMPONENT_DEVEL_GROUP "Development")
#set(CPACK_COMPONENT_DEVEL_INSTALL_TYPES Development)

#set(CPACK_COMPONENT_APIDOC_DISPLAY_NAME "genvtx API documentation")
#set(CPACK_COMPONENT_APIDOC_DESCRIPTION "Documentation for developers")
#set(CPACK_COMPONENT_APIDOC_GROUP "Development")
#set(CPACK_COMPONENT_APIDOC_INSTALL_TYPES Development)

#set(CPACK_COMPONENT_UTILS_DISPLAY_NAME "genvtx utilities")
#set(CPACK_COMPONENT_UTILS_DESCRIPTION
#    "Provides a FindGenvtx.cmake file to be used with project based on CMake"
#)
#set(CPACK_COMPONENT_UTILS_GROUP "Development")
#set(CPACK_COMPONENT_UTILS_DISABLED TRUE)

#set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "Components related to software development based on genvtx")
#set(CPACK_COMPONENT_GROUP_DEVELOPMENT_EXPANDED TRUE)

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

