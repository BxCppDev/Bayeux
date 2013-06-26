# - CMake script for building Bayeux from its submodules
#

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

#-----------------------------------------------------------------------
# Just build Bayeux support code at present
# TODO: provide "lpc-module.cmake" file as we'll do for other
# submodules.
include_directories(bxbayeux/include)
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include)

configure_file(bxbayeux/include/bayeux/version.h.in
  ${Bayeux_BUILDPRODUCT_DIR}/include/bayeux/version.h
  @ONLY
  )

set(Bayeux_HEADERS 
  ${Bayeux_BUILDPRODUCT_DIR}/include/bayeux/version.h
  )
set(Bayeux_SOURCES bxbayeux/src/version.cc)

add_library(bayeux SHARED ${Bayeux_HEADERS} ${Bayeux_SOURCES})


