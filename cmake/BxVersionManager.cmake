# CMake tools for version management
# Author: F.Mauger
# Date: 2017-09-26
# Copyright 2017 (c) François Mauger, Université de Caen Normandie

function(bx_version_build_version_number prefix_)
  set(${prefix_}_VERSION
    "${${prefix_}_VERSION_MAJOR}.${${prefix_}_VERSION_MINOR}.${${prefix_}_VERSION_PATCH}"
    PARENT_SCOPE)
endfunction()

function(bx_version_extract_patch_from_db major_ minor_ patch_)
  set(_version_patch 0)
  if (EXISTS ${PROJECT_SOURCE_DIR}/cmake/BxPackageVersions.db)
    file(STRINGS
      ${PROJECT_SOURCE_DIR}/cmake/BxPackageVersions.db
      _bx_package_versions_lines
      REGEX "^([0-9]+)\\.([0-9]+)\\.([0-9+])"
      )
    foreach(line ${_bx_package_versions_lines})
      string(REPLACE "." ";" _version_list ${line})
      list(GET _version_list 0 _version_major)
      list(GET _version_list 1 _version_minor)
      list(GET _version_list 2 _version_patch)
      if(${_version_major} EQUAL ${major_})
	if(${_version_minor} EQUAL ${minor_})
	  set(${patch_} ${_version_patch} PARENT_SCOPE)
	endif()
      endif()
    endforeach()
  else()
    set(${patch_} ${_version_patch} PARENT_SCOPE)
  endif()
endfunction()

macro(bx_version_set prefix_ major_ minor_)
  set(_version_major ${major_})
  set(_version_minor ${minor_})
  set(_version_patch 0)
  set(${prefix_}_VERSION_MAJOR ${_version_major})
  set(${prefix_}_VERSION_MINOR ${_version_minor})
  set(${prefix_}_VERSION_PATCH ${_version_patch})
  bx_version_extract_patch_from_db(
    ${${prefix_}_VERSION_MAJOR}
    ${${prefix_}_VERSION_MINOR}
    ${prefix_}_VERSION_PATCH
    )
  bx_version_build_version_number(${prefix_})
  message(STATUS "[info] bx_version_set: Major   = ${${prefix_}_VERSION_MAJOR}")
  message(STATUS "[info] bx_version_set: Minor   = ${${prefix_}_VERSION_MINOR}")
  message(STATUS "[info] bx_version_set: Patch   = ${${prefix_}_VERSION_PATCH}")
  message(STATUS "[info] bx_version_set: Version = ${${prefix_}_VERSION}")
  if (NOT PROJECT_VERSION)
    message(STATUS "[info] bx_version_set: Setting project version...")
    set(PROJECT_VERSION "${${prefix_}_VERSION}")
  endif()
  if (NOT PROJECT_VERSION_MAJOR)
    message(STATUS "[info] bx_version_set: Setting project version major...")
    set(PROJECT_VERSION_MAJOR "${${prefix_}_VERSION_MAJOR}")
  endif()
  if (NOT PROJECT_VERSION_MINOR)
    message(STATUS "[info] bx_version_set: Setting project version minor...")
    set(PROJECT_VERSION_MINOR_VERSION "${${prefix_}_VERSION_MINOR}")
  endif()
  if (NOT PROJECT_VERSION_PATCH)
    message(STATUS "[info] bx_version_set: Setting project version patch...")
    set(PROJECT_VERSION_PATCH "${${prefix_}_VERSION_MINOR}")
  endif()
  message(STATUS "[info] bx_version_set: Project version = '${PROJECT_VERSION}'")
endmacro()
