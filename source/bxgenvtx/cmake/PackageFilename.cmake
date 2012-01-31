###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the genvtx library.
##
## genvtx is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## genvtx is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with genvtx.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################


###############################
# generate package filename
# setup CPACK_PACKAGE_FILE_NAME and CPACK_SOURCE_PACKAGE_FILE_NAME according to
# the current build architecture, system and compiler
# Note that CPACK_PACKAGE_NAME and VERSION_STR must be defined
###############################

# detect architecture
include(CheckTypeSize)
check_type_size(void* SIZEOF_VOID_PTR)
if(${SIZEOF_VOID_PTR} MATCHES "^8$")
    set(NATIVE_ARCH 64)
else()
    set(NATIVE_ARCH 32)
endif()

# detect system
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PACKAGE_SYSTEM_NAME "linux${NATIVE_ARCH}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(PACKAGE_SYSTEM_NAME "win${NATIVE_ARCH}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PACKAGE_SYSTEM_NAME "osx")
else()
    message(WARNING "Unsupported system for package generation")
    return()
endif()

#detect compiler
if(CMAKE_COMPILER_IS_GNUCXX)
    set(PACKAGE_COMPILER_NAME "gcc")
    if(WIN32)
        execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-dumpversion" OUTPUT_VARIABLE GCC_VERSION_OUTPUT)
        string(REGEX REPLACE "([0-9]+\\.[0-9]+).*" "\\1" GCC_VERSION "${GCC_VERSION_OUTPUT}")
        set(PACKAGE_COMPILER_NAME ${PACKAGE_COMPILER_NAME}${GCC_VERSION})
    endif()
elseif(MSVC_VERSION EQUAL 1400)
    set(PACKAGE_COMPILER_NAME "vc2005")
elseif(MSVC_VERSION EQUAL 1500)
    set(PACKAGE_COMPILER_NAME "vc2008")
elseif(MSVC_VERSION EQUAL 1600)
    set(PACKAGE_COMPILER_NAME "vc2010")
else()
    message(WARNING "Unsupported compiler for package generation")
    return()
endif()
