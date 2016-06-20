#.rst:
# BXCMakeSettings
# ---------------
#
# Global CMake settings for the Bayeux project.
#
# Stored here to avoid clutter in main CMake script.
#
#

#-----------------------------------------------------------------------
# Include need modules
#
include(BXCMakeUtilities)

#-----------------------------------------------------------------------
# Global CMake/target properties
#
# - Though we check for some absolute paths, ensure there are no others
set(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION ON)

# - Never export to or seach in user/system package registry
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set(CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY ON)
set(CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY ON)

# - Force project directories to appear first in any list of includes
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# - Only report newly installed files
set(CMAKE_INSTALL_MESSAGE LAZY)

#-----------------------------------------------------------------------
# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
include(GNUInstallDirs)

# - Validate that certain paths are relative, otherwise relocation may fail
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

# Add a path for CMake config files
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
set(CMAKE_INSTALL_FULL_CMAKEDIR ${CMAKE_INSTALL_FULL_LIBDIR}/cmake)

# Resources directories based on name-version subdirs
set(BAYEUX_TAG "Bayeux-${Bayeux_VERSION}")
set(BAYEUX_RESOURCE_DIR "${BAYEUX_TAG}/resources")
set(BAYEUX_EXAMPLE_DIR "${BAYEUX_TAG}/examples")
set(BAYEUX_DOCUMENTATION_DIR "${BAYEUX_TAG}/Documentation")

#-----------------------------------------------------------------------
# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
set(BAYEUX_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")

# - Note that at present, this layout only supports single mode
# generators like make/ninja...
file(RELATIVE_PATH BAYEUX_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}"  "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${BAYEUX_TAG}")

file(RELATIVE_PATH BAYEUX_LIBDIR_TO_PREFIX      "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_PREFIX}")
file(RELATIVE_PATH BAYEUX_LIBDIR_TO_BINDIR      "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_FULL_BINDIR}")
file(RELATIVE_PATH BAYEUX_LIBDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_LIBDIR}"  "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${BAYEUX_TAG}")

file(RELATIVE_PATH BAYEUX_CMAKEDIR_TO_INCLUDEDIR "${CMAKE_INSTALL_FULL_CMAKEDIR}/${BAYEUX_TAG}"  "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BAYEUX_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")

#-----------------------------------------------------------------------
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to RelWithDebInfo
#
if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    # Default to a RelWithDebInfo build if nothing else...
    set(CMAKE_BUILD_TYPE RelWithDebInfo
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo MinSizeRel."
      FORCE
      )
  else()
    # Force to the cache, but use existing value.
    set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo MinSizeRel."
      FORCE
      )
  endif()
endif()

#-----------------------------------------------------------------------
# Compiler/Linker configuration
# - Enforce all warnings, but ensure other flags are retained
# Note that the added flags *won't* show up in the cache.
# TODO : better way to do this with the "Make Override" system?
option(BAYEUX_COMPILER_ERROR_ON_WARNING "Turn warnings into errors" ON)
mark_as_advanced(BAYEUX_COMPILER_ERROR_ON_WARNING)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|(Apple)+Clang")
  set(BAYEUX_CXX_FLAGS "-Wall -Wextra -Wshadow -pedantic")

  # Disable specific warnings on GCC (-Wlong-long/pedantic) because Root source code (Rtypes.h)
  # makes use of unsupported type by ISO C++ 98 ([unsigned] long long int) and
  # the 'pragma GCC diagnostic ignored "-Wlong-long"' does not work
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(BAYEUX_CXX_FLAGS "${BAYEUX_CXX_FLAGS} -Wno-long-long")
  endif()

  # Disable C99 extension warnings on Clang
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
    set(BAYEUX_CXX_FLAGS "${BAYEUX_CXX_FLAGS} -Wno-c99-extensions")
  endif()

  if(BAYEUX_COMPILER_ERROR_ON_WARNING)
    # Use fatal-errors to fall over at first sign of trouble
    set(BAYEUX_CXX_FLAGS "${BAYEUX_CXX_FLAGS} -Werror -Wfatal-errors")
  endif()

  # On clang, need larger template depth
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
    set(BAYEUX_CXX_FLAGS "${BAYEUX_CXX_FLAGS} -ftemplate-depth=512")
  endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  # Needs more testing
  set(BAYEUX_CXX_FLAGS "-w2")
endif()

# - Prepend Bayeux's flags to any preexisting ones, allowing user
#   to supply additional ones or override if required
set(CMAKE_CXX_FLAGS "${BAYEUX_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

# - On OS X, default exe/shared linking to dynamic lookup to match GNU
#   linker behaviour
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup ${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "-undefined dynamic_lookup ${CMAKE_EXE_LINKER_FLAGS}")
endif()

# - Could set RPATHs here, but difficult to pass genexps in variables,
#   and we need that to handle exe/lib differences

#-----------------------------------------------------------------------
# C++ Standard support
# - Do not want extensions
set(CMAKE_CXX_EXTENSIONS OFF)

# - Allow choice of standard
enum_option(BAYEUX_CXX_STANDARD
  VALUES "11" "14"
  TYPE   STRING
  DOC    "Choose C++ Standard to compile against"
  )

# - Default is 98, add an arbitrary feature here so that appropriate
#   flags/requirement is added and propagated
set(BAYEUX_CXX_COMPILE_FEATURES cxx_template_template_parameters)

# - For C++11, add all features supported in GCC 4.9 and above, except
#   thread_local because that needs a workaround on OS X
set(BAYEUX_CXX11_COMPILE_FEATURES
  cxx_alias_templates
  cxx_alignas
  cxx_alignof
  cxx_attributes
  cxx_auto_type
  cxx_constexpr
  cxx_decltype
  cxx_decltype_incomplete_return_types
  cxx_default_function_template_args
  cxx_defaulted_functions
  cxx_defaulted_move_initializers
  cxx_delegating_constructors
  cxx_deleted_functions
  cxx_enum_forward_declarations
  cxx_explicit_conversions
  cxx_extended_friend_declarations
  cxx_extern_templates
  cxx_final
  cxx_func_identifier
  cxx_generalized_initializers
  cxx_inheriting_constructors
  cxx_inline_namespaces
  cxx_lambdas
  cxx_local_type_template_args
  cxx_long_long_type
  cxx_noexcept
  cxx_nonstatic_member_init
  cxx_nullptr
  cxx_override
  cxx_range_for
  cxx_raw_string_literals
  cxx_reference_qualified_functions
  cxx_right_angle_brackets
  cxx_rvalue_references
  cxx_sizeof_member
  cxx_static_assert
  cxx_strong_enums
  cxx_trailing_return_types
  cxx_unicode_literals
  cxx_uniform_initialization
  cxx_unrestricted_unions
  cxx_user_literals
  cxx_variadic_macros
  cxx_variadic_templates
  )

# - For C++14, add all features supported by current compiler
set(BAYEUX_CXX14_COMPILE_FEATURES "${CMAKE_CXX14_COMPILE_FEATURES}")

# - Process compile features based on selected standard
if(BAYEUX_CXX_STANDARD EQUAL 11)
  list(APPEND BAYEUX_CXX_COMPILE_FEATURES ${BAYEUX_CXX11_COMPILE_FEATURES})
endif()

# - If C++14 is requested, add all features supported by current
#   compiler. If no 14 features are supported, fail under assumption
#   compiler does not support it.
if(BAYEUX_CXX_STANDARD EQUAL 14)
  if(NOT BAYEUX_CXX14_COMPILE_FEATURES)
    message(FATAL_ERROR "C++14 requested, but no support for any C++14 features for compiler:\n"
      "'${CMAKE_CXX_COMPILER_ID}', '${CMAKE_CXX_COMPILER_VERSION}'"
      )
  endif()

  list(APPEND BAYEUX_CXX_COMPILE_FEATURES
    ${BAYEUX_CXX11_COMPILE_FEATURES}
    ${BAYEUX_CXX14_COMPILE_FEATURES}
    )
endif()
