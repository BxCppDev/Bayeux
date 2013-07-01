# - Package Bayeux
# Configure CPack to create Source and Binary packages.

#-----------------------------------------------------------------------
# Copyright 2012,2013 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2013 University of Warwick
#
# This file is part of Bayeux.
#
# Bayeux is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Bayeux is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------

# - Include guard
if(__bxcpack_isloaded)
  return()
endif()
set(__bxcpack_isloaded YES)

#-----------------------------------------------------------------------
# General Settings
#
set(CPACK_PACKAGE_DESCRIPTION "Bayeux C++ Foundation Library")
set(CPACK_PACKAGE_VENDOR      "SuperNEMO Collaboration")

foreach(_vtag VERSION VERSION_MAJOR VERSION_MINOR VERSION_PATCH)
  set(CPACK_PACKAGE_${_vtag} "${${PROJECT_NAME}_${_vtag}}")
endforeach()

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README  "${PROJECT_SOURCE_DIR}/README.rst")

#-----------------------------------------------------------------------
# Source Package
#
set(CPACK_SOURCE_GENERATOR "TBZ2" "ZIP")
set(CPACK_SOURCE_IGNORE_FILES 
  ${PROJECT_BINARY_DIR}
  "/pkgtools.d/"
  "/bx.*/devel/"
  "~$"
  "/CVS/"
  "/\\\\.svn/"
  "/.git/"
  "/\\\\\\\\.git/"
  "\\\\\\\\.swp$"
  "\\\\\\\\.swp$"
  "\\\\.swp"
  "\\\\\\\\.#"
  "/#"
  )

#-----------------------------------------------------------------------
# Binary Package(s)
#

#-----------------------------------------------------------------------
# Finalize
#
include(CPack)

