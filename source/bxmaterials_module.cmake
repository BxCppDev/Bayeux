# - Module definition for materials submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

foreach(_modulevar MODULE_HEADER_ROOT MODULE_RESOURCE_ROOT)
  if(NOT ${_modulevar})
    message(FATAL_ERROR "${_modulevar} not specified")
  endif()
endforeach()

# - Module
set(module_name materials)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")
set(module_examples_dir "${module_root_dir}/examples")

# Store explicitely this resource dir for further usage during other modules' tests
set(bxmaterials_resource_dir ${module_resource_dir})

foreach(dir root_dir include_dir source_dir test_dir app_dir resource_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(materials_VERSION_MAJOR 6)
set(materials_VERSION_MINOR 2)
set(materials_VERSION_PATCH 0)
set(materials_VERSION "${materials_VERSION_MAJOR}.${materials_VERSION_MINOR}.${materials_VERSION_PATCH}")

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/chemical_symbol.h
  ${module_include_dir}/${module_name}/detail/tools.h
  ${module_include_dir}/${module_name}/element.h
  ${module_include_dir}/${module_name}/factory.h
  ${module_include_dir}/${module_name}/isotope.h
  ${module_include_dir}/${module_name}/manager.h
  ${module_include_dir}/${module_name}/material.h
  ${module_include_dir}/${module_name}/materials_driver.h
  ${module_include_dir}/${module_name}/materials_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/resource.h
  ${module_include_dir}/${module_name}/refractive_index.h
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/chemical_symbol.cc
  ${module_source_dir}/element.cc
  ${module_source_dir}/factory.cc
  ${module_source_dir}/isotope.cc
  ${module_source_dir}/material.cc
  ${module_source_dir}/manager.cc
  ${module_source_dir}/materials_driver.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/resource.cc
  ${module_source_dir}/refractive_index.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "MATERIALS_RESOURCE_DIR=${module_resource_dir};MATERIALS_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_chemical_symbol.cxx
  ${module_test_dir}/test_element.cxx
  ${module_test_dir}/test_factory.cxx
  ${module_test_dir}/test_isotope.cxx
  ${module_test_dir}/test_manager_2.cxx
  ${module_test_dir}/test_manager.cxx
  ${module_test_dir}/test_material.cxx
  ${module_test_dir}/test_materials.cxx
  ${module_test_dir}/test_resource.cxx
  ${module_test_dir}/test_version.cxx
  ${module_test_dir}/test_refractive_index.cxx
  )

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/materials_inspector.cxx
  ${module_app_dir}/materials_diagnose.cxx
  )

# - Resource files
set(${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/data/README.rst
  ${module_resource_dir}/data/mass.mas03
  ${module_resource_dir}/data/mass.mas12
  ${module_resource_dir}/data/isotopic_compositions_nist.dat
  ${module_resource_dir}/data/simple_elements.def
  ${module_resource_dir}/data/simple_materials.def
  ${module_resource_dir}/data/std_elements.def
  ${module_resource_dir}/data/std_isotopes.def
  ${module_resource_dir}/data/std_materials.def
  ${module_resource_dir}/data/std_material_aliases.def
  ${module_resource_dir}/data/basic/README.rst
  ${module_resource_dir}/data/basic/1.0/elements.def
  ${module_resource_dir}/data/basic/1.0/isotopes.def
  ${module_resource_dir}/data/basic/1.0/materials.def
  ${module_resource_dir}/data/basic/1.0/material_aliases.def
  ${module_resource_dir}/data/tissue/README.rst
  ${module_resource_dir}/data/tissue/1.0/README.rst
  ${module_resource_dir}/data/optical/1.0/Gas/Air/Air.conf
  ${module_resource_dir}/data/optical/1.0/Glass/FusedQuartz/FusedQuartz.conf
  ${module_resource_dir}/data/optical/1.0/Glass/N-BK7/N-BK7.conf
  ${module_resource_dir}/data/optical/1.0/Glass/N-BK7/N-BK7_k.data
  ${module_resource_dir}/data/optical/1.0/Glass/N-BK7/N-BK7_n.data
  ${module_resource_dir}/data/optical/1.0/Glass/N-BK7/N-BK7_n_2.data
  ${module_resource_dir}/data/optical/1.0/Glass/N-BK7/README.rst
  ${module_resource_dir}/data/optical/1.0/Liquid/Water/Water.conf
  ${module_resource_dir}/data/optical/1.0/Liquid/Water/Water_k.data
  ${module_resource_dir}/data/optical/1.0/Liquid/Water/Water_n.data
  ${module_resource_dir}/data/optical/1.0/Liquid/Water/Water_n_k.data
  ${module_resource_dir}/data/optical/1.0/Miscellaneous/CsI/CsI.conf
  ${module_resource_dir}/data/optical/1.0/Miscellaneous/Ice/Ice.conf
  ${module_resource_dir}/data/optical/1.0/Miscellaneous/Ice/Ice_n.data
  ${module_resource_dir}/data/optical/1.0/Miscellaneous/Ice/Ice_k.data
  ${module_resource_dir}/data/optical/1.0/Miscellaneous/Ice/README.rst
  ${module_resource_dir}/data/optical/1.0/Miscellaneous/NaI/NaI.conf
  ${module_resource_dir}/data/optical/1.0/Miscellaneous/Vacuum/Vacuum.conf
  ${module_resource_dir}/data/optical/1.0/Organic/PMMA/PMMA2.conf
  ${module_resource_dir}/data/optical/1.0/Organic/PMMA/PMMA3.conf
  ${module_resource_dir}/data/optical/1.0/Organic/PMMA/PMMA.conf
  ${module_resource_dir}/data/optical/1.0/Organic/Polycarbonate/Polycarbonate.conf
  ${module_resource_dir}/data/optical/1.0/Organic/Polystyrene/Polystyrene.conf
  ${module_resource_dir}/data/optical/1.0/README.rst
  ${module_resource_dir}/urn/db/basic_materials_infos.def
  ${module_resource_dir}/urn/resolvers/bxmaterials_data_urn_map.conf
)

# - Publish resource files
foreach(_rfin ${${module_name}_MODULE_RESOURCES})
  string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
  string(REGEX REPLACE "^${module_resource_dir}" "${MODULE_RESOURCE_ROOT}" _rfout "${_rfout}")
  configure_file(${_rfin} ${_rfout} @ONLY)
endforeach()

# - Examples dir
set(${module_name}_MODULE_EXAMPLES
  ${module_examples_dir}
  )
