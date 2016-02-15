# - Find realine
#
#  Readline_INCLUDE_DIR - where to find readline headers
#  Readline_LIBRARIES   - List of libraries when using readline
#  Readline_FOUND       - True if readline is found

find_path(Readline_INCLUDE_DIR readline/readline.h readline.h)
find_library(Readline_LIBRARY readline)
mark_as_advanced(Readline_LIBRARY Readline_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Readline
  REQUIRED_VARS Readline_INCLUDE_DIR Readline_LIBRARY
  )

if(Readline_FOUND)
  set(Readline_INCLUDE_DIRS ${Readline_INCLUDE_DIR})
  set(Readline_LIBRARIES ${Readline_LIBRARY})

  if(NOT TARGET Readline::Readline)
    add_library(Readline::Readline UNKNOWN IMPORTED)
    set_target_properties(Readline::Readline
      PROPERTIES
        IMPORTED_LOCATION "${Readline_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Readline_INCLUDE_DIRS}"
      )
  endif()
endif()

