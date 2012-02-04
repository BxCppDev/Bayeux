# - Determine number of processors/cores on build host
# Uses ideas from a blog post by David Cole:
#
#  http://www.kitware.com/blog/home/post/63
#  
# and further tips from the comments.
#
# This module provides the following functions:
#
#  function GET_NUMBER_OF_CORES(result)
#           determine number of cores on the host on which the CMake process
#           is running, setting the value of result to this number.
#           On Linux, getconf is used if available.
#           On Mac OS X(?), sysctl is used if available.
#           On Windows, the environment variable NUMBER_OF_PROCESSORS is used.
#

#----------------------------------------------------------------------------
# Copyright (c) 2011, Ben Morgan <Ben.Morgan@warwick.ac.uk>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the University of Warwick nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
# ARE DISCLAIMED. IN NO EVENT SHALL BEN MORGAN BE LIABLE FOR ANY 
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# function GET_NUMBER_OF_CORES(result)
#          determine number of cores on the host this function is run on
#          setting the value of the result variable to the determined number
#
function(get_number_of_cores _output)
  set(_processor_count 1)

  # Linux...
  # We use getconf if available (which it should be in most cases).
  if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    find_program(GETCONF_EXECUTABLE getconf "getconf program")
    mark_as_advanced(GETCONF_EXECUTABLE)

    if(GETCONF_EXECUTABLE)
      execute_process(COMMAND ${GETCONF_EXECUTABLE} _NPROCESSORS_ONLN 
        OUTPUT_VARIABLE _processor_count
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
  endif()

  # Mac OS
  # We use sysctl if available (which it should be).
  if(APPLE)
    find_program(SYSCTL_EXECUTABLE sysctl "sysctl program")
    mark_as_advanced(SYSCTL_EXECUTABLE)

    if(SYSCTL_EXECUTABLE)
      execute_process(COMMAND ${SYSCTL_EXECUTABLE} -n hw.ncpu
        OUTPUT_VARIABLE _processor_count
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
  endif()

  # Windows
  # Environment information should be sufficient
  if(WIN32)
    set(_processor_count "$ENV{NUMBER_OF_PROCESSORS}")
  endif()

  set(${_output} ${_processor_count} PARENT_SCOPE)
endfunction()

