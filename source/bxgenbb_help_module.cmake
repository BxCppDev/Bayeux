# - Module definition for genbb_help submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name genbb_help)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")
set(module_examples_dir "${module_root_dir}/examples")

foreach(dir root_dir include_dir source_dir app_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(genbb_help_VERSION_MAJOR 6)
set(genbb_help_VERSION_MINOR 2)
set(genbb_help_VERSION_PATCH 0)
set(genbb_help_VERSION "${genbb_help_VERSION_MAJOR}.${genbb_help_VERSION_MINOR}.${genbb_help_VERSION_PATCH}")

# - Boost I/O, CAMP Reflection, Gnuplot pipe interface
set(GENBB_HELP_WITH_BIO 1)
set(GENBB_HELP_WITH_REFLECTION 1)
message(STATUS "Bayeux_WITH_BXDECAY0=${Bayeux_WITH_BXDECAY0}")

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/genbb_macros.h
  ${module_include_dir}/${module_name}/genbb_mgr.h
  ${module_include_dir}/${module_name}/genbb_utils.h
  ${module_include_dir}/${module_name}/genbb_writer.h
  ${module_include_dir}/${module_name}/i_genbb.h
  ${module_include_dir}/${module_name}/kinematics.h
  ${module_include_dir}/${module_name}/two_body_decay.h
  ${module_include_dir}/${module_name}/manager.h
  ${module_include_dir}/${module_name}/primary_event.h
  ${module_include_dir}/${module_name}/primary_particle.h
  ${module_include_dir}/${module_name}/single_particle_generator.h
  ${module_include_dir}/${module_name}/combined_particle_generator.h
  ${module_include_dir}/${module_name}/time_slicer_generator.h
  ${module_include_dir}/${module_name}/save_to_file_wrapper.h
  ${module_include_dir}/${module_name}/lorentz_boost_wrapper.h
  ${module_include_dir}/${module_name}/from_file_generator.h
  ${module_include_dir}/${module_name}/fermi_function.h
  ${module_include_dir}/${module_name}/nuclear_level.h
  ${module_include_dir}/${module_name}/nuclear_decay.h
  ${module_include_dir}/${module_name}/nuclear_decay_manager.h
  ${module_include_dir}/${module_name}/nuclear_decay_generator.h
  ${module_include_dir}/${module_name}/base_decay_driver.h
  ${module_include_dir}/${module_name}/nuclear_transition.h
  ${module_include_dir}/${module_name}/beta_decay.h
  ${module_include_dir}/${module_name}/alpha_decay.h
  ${module_include_dir}/${module_name}/wdecay0.h
  ${module_include_dir}/${module_name}/pdg_particle_tools.h
  ${module_include_dir}/${module_name}/primary_event.ipp
  ${module_include_dir}/${module_name}/primary_particle.ipp
  ${module_include_dir}/${module_name}/genbb_help_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/detail/pg_tools.h
  ${module_include_dir}/${module_name}/primary_particle-reflect.h
  ${module_include_dir}/${module_name}/primary_event-reflect.h
  ${module_include_dir}/${module_name}/resource.h
  )

if(Bayeux_WITH_BXDECAY0)
  include_directories(${BxDecay0_INCLUDE_DIRS})
endif()

if(NOT Bayeux_WITH_BXDECAY0)
  list(APPEND ${module_name}_MODULE_HEADERS
    ${module_include_dir}/${module_name}/decay0/Ac228.h
    ${module_include_dir}/${module_name}/decay0/alpha.h
    ${module_include_dir}/${module_name}/decay0/Am241.h
    ${module_include_dir}/${module_name}/decay0/Ar39.h
    ${module_include_dir}/${module_name}/decay0/Ar42.h
    ${module_include_dir}/${module_name}/decay0/As79.h
    ${module_include_dir}/${module_name}/decay0/At214.h
    ${module_include_dir}/${module_name}/decay0/At214low.h
    ${module_include_dir}/${module_name}/decay0/Ba136low.h
    ${module_include_dir}/${module_name}/decay0/Ba138low.h
    ${module_include_dir}/${module_name}/decay0/bb.h
    ${module_include_dir}/${module_name}/decay0/beta_1fu.h
    ${module_include_dir}/${module_name}/decay0/beta1.h
    ${module_include_dir}/${module_name}/decay0/beta2.h
    ${module_include_dir}/${module_name}/decay0/beta.h
    ${module_include_dir}/${module_name}/decay0/Bi207.h
    ${module_include_dir}/${module_name}/decay0/Bi208.h
    ${module_include_dir}/${module_name}/decay0/Bi210.h
    ${module_include_dir}/${module_name}/decay0/Bi212.h
    ${module_include_dir}/${module_name}/decay0/Bi214.h
    ${module_include_dir}/${module_name}/decay0/C14.h
    ${module_include_dir}/${module_name}/decay0/Ca48.h
    ${module_include_dir}/${module_name}/decay0/Cd112low.h
    ${module_include_dir}/${module_name}/decay0/Cd113.h
    ${module_include_dir}/${module_name}/decay0/Co60.h
    ${module_include_dir}/${module_name}/decay0/common.h
    ${module_include_dir}/${module_name}/decay0/compton1.h
    ${module_include_dir}/${module_name}/decay0/compton.h
    ${module_include_dir}/${module_name}/decay0/Cs136.h
    ${module_include_dir}/${module_name}/decay0/Cs137.h
    ${module_include_dir}/${module_name}/decay0/dgmlt1.h
    ${module_include_dir}/${module_name}/decay0/dgmlt2.h
    ${module_include_dir}/${module_name}/decay0/divdif.h
    ${module_include_dir}/${module_name}/decay0/dshelp1.h
    ${module_include_dir}/${module_name}/decay0/dshelp2.h
    ${module_include_dir}/${module_name}/decay0/electron.h
    ${module_include_dir}/${module_name}/decay0/Eu147.h
    ${module_include_dir}/${module_name}/decay0/Eu152.h
    ${module_include_dir}/${module_name}/decay0/Eu154.h
    ${module_include_dir}/${module_name}/decay0/fe12_mods.h
    ${module_include_dir}/${module_name}/decay0/fe1_mods.h
    ${module_include_dir}/${module_name}/decay0/fe2_mods.h
    ${module_include_dir}/${module_name}/decay0/Fe58low.h
    ${module_include_dir}/${module_name}/decay0/fermi.h
    ${module_include_dir}/${module_name}/decay0/funbeta_1fu.h
    ${module_include_dir}/${module_name}/decay0/funbeta1.h
    ${module_include_dir}/${module_name}/decay0/funbeta2.h
    ${module_include_dir}/${module_name}/decay0/funbeta.h
    ${module_include_dir}/${module_name}/decay0/gamma.h
    ${module_include_dir}/${module_name}/decay0/gauss.h
    ${module_include_dir}/${module_name}/decay0/Gd146.h
    ${module_include_dir}/${module_name}/decay0/Gd156low.h
    ${module_include_dir}/${module_name}/decay0/Gd158low.h
    ${module_include_dir}/${module_name}/decay0/gdrot.h
    ${module_include_dir}/${module_name}/decay0/Ge70low.h
    ${module_include_dir}/${module_name}/decay0/Ge74low.h
    ${module_include_dir}/${module_name}/decay0/genbbsub.h
    ${module_include_dir}/${module_name}/decay0/gfang.h
    ${module_include_dir}/${module_name}/decay0/Hf180low.h
    ${module_include_dir}/${module_name}/decay0/Hf182.h
    ${module_include_dir}/${module_name}/decay0/Hg198low.h
    ${module_include_dir}/${module_name}/decay0/I126.h
    ${module_include_dir}/${module_name}/decay0/I133.h
    ${module_include_dir}/${module_name}/decay0/I134.h
    ${module_include_dir}/${module_name}/decay0/I135.h
    ${module_include_dir}/${module_name}/decay0/K40.h
    ${module_include_dir}/${module_name}/decay0/K42.h
    ${module_include_dir}/${module_name}/decay0/Kr81.h
    ${module_include_dir}/${module_name}/decay0/Kr82low.h
    ${module_include_dir}/${module_name}/decay0/Kr84low.h
    ${module_include_dir}/${module_name}/decay0/Kr85.h
    ${module_include_dir}/${module_name}/decay0/Mn54.h
    ${module_include_dir}/${module_name}/decay0/Mo94low.h
    ${module_include_dir}/${module_name}/decay0/Mo96low.h
    ${module_include_dir}/${module_name}/decay0/moller1.h
    ${module_include_dir}/${module_name}/decay0/moller.h
    ${module_include_dir}/${module_name}/decay0/Na22.h
    ${module_include_dir}/${module_name}/decay0/Nb96.h
    ${module_include_dir}/${module_name}/decay0/Nd142low.h
    ${module_include_dir}/${module_name}/decay0/Ni64low.h
    ${module_include_dir}/${module_name}/decay0/nucltransK.h
    ${module_include_dir}/${module_name}/decay0/nucltransKL.h
    ${module_include_dir}/${module_name}/decay0/nucltransKLM.h
    ${module_include_dir}/${module_name}/decay0/nucltransKLM_Pb.h
    ${module_include_dir}/${module_name}/decay0/Os186low.h
    ${module_include_dir}/${module_name}/decay0/Os190low.h
    ${module_include_dir}/${module_name}/decay0/P32.h
    ${module_include_dir}/${module_name}/decay0/Pa231.h
    ${module_include_dir}/${module_name}/decay0/Pa234m.h
    ${module_include_dir}/${module_name}/decay0/pairext1.h
    ${module_include_dir}/${module_name}/decay0/pairext.h
    ${module_include_dir}/${module_name}/decay0/pair.h
    ${module_include_dir}/${module_name}/decay0/particle.h
    ${module_include_dir}/${module_name}/decay0/Pb210.h
    ${module_include_dir}/${module_name}/decay0/Pb211.h
    ${module_include_dir}/${module_name}/decay0/Pb212.h
    ${module_include_dir}/${module_name}/decay0/Pb214.h
    ${module_include_dir}/${module_name}/decay0/PbAtShell.h
    ${module_include_dir}/${module_name}/decay0/Pd104low.h
    ${module_include_dir}/${module_name}/decay0/Pd106low.h
    ${module_include_dir}/${module_name}/decay0/Pd108low.h
    ${module_include_dir}/${module_name}/decay0/plog69.h
    ${module_include_dir}/${module_name}/decay0/Po212.h
    ${module_include_dir}/${module_name}/decay0/Po214.h
    ${module_include_dir}/${module_name}/decay0/Po214low.h
    ${module_include_dir}/${module_name}/decay0/positron.h
    ${module_include_dir}/${module_name}/decay0/Ra222.h
    ${module_include_dir}/${module_name}/decay0/Ra222low.h
    ${module_include_dir}/${module_name}/decay0/Ra226.h
    ${module_include_dir}/${module_name}/decay0/Ra228.h
    ${module_include_dir}/${module_name}/decay0/Rb87.h
    ${module_include_dir}/${module_name}/decay0/Rh106.h
    ${module_include_dir}/${module_name}/decay0/Rn218.h
    ${module_include_dir}/${module_name}/decay0/Rn218low.h
    ${module_include_dir}/${module_name}/decay0/Ru100low.h
    ${module_include_dir}/${module_name}/decay0/Sb125.h
    ${module_include_dir}/${module_name}/decay0/Sb126.h
    ${module_include_dir}/${module_name}/decay0/Sb133.h
    ${module_include_dir}/${module_name}/decay0/Sc48.h
    ${module_include_dir}/${module_name}/decay0/Se76low.h
    ${module_include_dir}/${module_name}/decay0/Sm148low.h
    ${module_include_dir}/${module_name}/decay0/Sm150low.h
    ${module_include_dir}/${module_name}/decay0/Sn114low.h
    ${module_include_dir}/${module_name}/decay0/Sn116low.h
    ${module_include_dir}/${module_name}/decay0/Sn120low.h
    ${module_include_dir}/${module_name}/decay0/Sr90.h
    ${module_include_dir}/${module_name}/decay0/Ta182.h
    ${module_include_dir}/${module_name}/decay0/Te122low.h
    ${module_include_dir}/${module_name}/decay0/Te124low.h
    ${module_include_dir}/${module_name}/decay0/Te133.h
    ${module_include_dir}/${module_name}/decay0/Te133m.h
    ${module_include_dir}/${module_name}/decay0/Te134.h
    ${module_include_dir}/${module_name}/decay0/tgold.h
    ${module_include_dir}/${module_name}/decay0/Th234.h
    ${module_include_dir}/${module_name}/decay0/Ti48low.h
    ${module_include_dir}/${module_name}/decay0/Tl207.h
    ${module_include_dir}/${module_name}/decay0/Tl208.h
    ${module_include_dir}/${module_name}/decay0/tsimpr.h
    ${module_include_dir}/${module_name}/decay0/Xe128low.h
    ${module_include_dir}/${module_name}/decay0/Xe129m.h
    ${module_include_dir}/${module_name}/decay0/Xe130low.h
    ${module_include_dir}/${module_name}/decay0/Xe131m.h
    ${module_include_dir}/${module_name}/decay0/Xe133.h
    ${module_include_dir}/${module_name}/decay0/Xe135.h
    ${module_include_dir}/${module_name}/decay0/Y88.h
    ${module_include_dir}/${module_name}/decay0/Y90.h
    ${module_include_dir}/${module_name}/decay0/Zn65.h
    ${module_include_dir}/${module_name}/decay0/Zr92low.h
    ${module_include_dir}/${module_name}/decay0/Zr96.h
    )
endif()

# - NB Order of sources appears to be important - taken from genbb_help
#   listing. Note that the_serializable.cc is added manually - not
#   totally clear this is in the right place...
set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/genbb_utils.cc
  ${module_source_dir}/genbb_writer.cc
  ${module_source_dir}/kinematics.cc
  ${module_source_dir}/two_body_decay.cc
  ${module_source_dir}/pg_tools.cc
  ${module_source_dir}/i_genbb.cc
  ${module_source_dir}/pdg_particle_tools.cc
  ${module_source_dir}/primary_event.cc
  ${module_source_dir}/primary_particle.cc
  ${module_source_dir}/wdecay0.cc
  ${module_source_dir}/genbb_mgr.cc
  ${module_source_dir}/single_particle_generator.cc
  ${module_source_dir}/combined_particle_generator.cc
  ${module_source_dir}/time_slicer_generator.cc
  ${module_source_dir}/save_to_file_wrapper.cc
  ${module_source_dir}/lorentz_boost_wrapper.cc
  ${module_source_dir}/from_file_generator.cc
  ${module_source_dir}/fermi_function.cc
  ${module_source_dir}/nuclear_level.cc
  ${module_source_dir}/nuclear_decay.cc
  ${module_source_dir}/nuclear_decay_manager.cc
  ${module_source_dir}/nuclear_decay_generator.cc
  ${module_source_dir}/base_decay_driver.cc
  ${module_source_dir}/beta_decay.cc
  ${module_source_dir}/alpha_decay.cc
  ${module_source_dir}/nuclear_transition.cc
  ${module_source_dir}/manager.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/the_serializable.cc
  #${module_source_dir}/the_introspectable.cc
  ${module_source_dir}/resource.cc
  )

if(NOT Bayeux_WITH_BXDECAY0)
  list(APPEND ${module_name}_MODULE_SOURCES
    ${module_source_dir}/decay0/Ac228.cc
    ${module_source_dir}/decay0/alpha.cc
    ${module_source_dir}/decay0/Am241.cc
    ${module_source_dir}/decay0/Ar39.cc
    ${module_source_dir}/decay0/Ar42.cc
    ${module_source_dir}/decay0/As79.cc
    ${module_source_dir}/decay0/At214.cc
    ${module_source_dir}/decay0/At214low.cc
    ${module_source_dir}/decay0/Ba136low.cc
    ${module_source_dir}/decay0/Ba138low.cc
    ${module_source_dir}/decay0/bb.cc
    ${module_source_dir}/decay0/beta1_1fu.cc
    ${module_source_dir}/decay0/beta1.cc
    ${module_source_dir}/decay0/beta2.cc
    ${module_source_dir}/decay0/beta.cc
    ${module_source_dir}/decay0/Bi207.cc
    ${module_source_dir}/decay0/Bi208.cc
    ${module_source_dir}/decay0/Bi210.cc
    ${module_source_dir}/decay0/Bi212.cc
    ${module_source_dir}/decay0/Bi214.cc
    ${module_source_dir}/decay0/C14.cc
    ${module_source_dir}/decay0/Ca48.cc
    ${module_source_dir}/decay0/Cd112low.cc
    ${module_source_dir}/decay0/Cd113.cc
    ${module_source_dir}/decay0/Co60.cc
    ${module_source_dir}/decay0/common.cc
    ${module_source_dir}/decay0/compton1.cc
    ${module_source_dir}/decay0/compton.cc
    ${module_source_dir}/decay0/Cs136.cc
    ${module_source_dir}/decay0/Cs137.cc
    ${module_source_dir}/decay0/dgmlt1.cc
    ${module_source_dir}/decay0/dgmlt2.cc
    ${module_source_dir}/decay0/divdif.cc
    ${module_source_dir}/decay0/dshelp1.cc
    ${module_source_dir}/decay0/dshelp2.cc
    ${module_source_dir}/decay0/electron.cc
    ${module_source_dir}/decay0/Eu147.cc
    ${module_source_dir}/decay0/Eu152.cc
    ${module_source_dir}/decay0/Eu154.cc
    ${module_source_dir}/decay0/fe12_mods.cc
    ${module_source_dir}/decay0/fe1_mods.cc
    ${module_source_dir}/decay0/fe2_mods.cc
    ${module_source_dir}/decay0/Fe58low.cc
    ${module_source_dir}/decay0/fermi.cc
    ${module_source_dir}/decay0/funbeta1.cc
    ${module_source_dir}/decay0/funbeta_1fu.cc
    ${module_source_dir}/decay0/funbeta2.cc
    ${module_source_dir}/decay0/funbeta.cc
    ${module_source_dir}/decay0/gamma.cc
    ${module_source_dir}/decay0/gauss.cc
    ${module_source_dir}/decay0/Gd146.cc
    ${module_source_dir}/decay0/Gd156low.cc
    ${module_source_dir}/decay0/Gd158low.cc
    ${module_source_dir}/decay0/gdrot.cc
    ${module_source_dir}/decay0/Ge70low.cc
    ${module_source_dir}/decay0/Ge74low.cc
    ${module_source_dir}/decay0/genbbsub.cc
    ${module_source_dir}/decay0/gfang.cc
    ${module_source_dir}/decay0/Hf180low.cc
    ${module_source_dir}/decay0/Hf182.cc
    ${module_source_dir}/decay0/Hg198low.cc
    ${module_source_dir}/decay0/I126.cc
    ${module_source_dir}/decay0/I133.cc
    ${module_source_dir}/decay0/I134.cc
    ${module_source_dir}/decay0/I135.cc
    ${module_source_dir}/decay0/K40.cc
    ${module_source_dir}/decay0/K42.cc
    ${module_source_dir}/decay0/Kr81.cc
    ${module_source_dir}/decay0/Kr82low.cc
    ${module_source_dir}/decay0/Kr84low.cc
    ${module_source_dir}/decay0/Kr85.cc
    ${module_source_dir}/decay0/Mn54.cc
    ${module_source_dir}/decay0/Mo94low.cc
    ${module_source_dir}/decay0/Mo96low.cc
    ${module_source_dir}/decay0/moller1.cc
    ${module_source_dir}/decay0/moller.cc
    ${module_source_dir}/decay0/Na22.cc
    ${module_source_dir}/decay0/Nb96.cc
    ${module_source_dir}/decay0/Nd142low.cc
    ${module_source_dir}/decay0/Ni64low.cc
    ${module_source_dir}/decay0/nucltransK.cc
    ${module_source_dir}/decay0/nucltransKL.cc
    ${module_source_dir}/decay0/nucltransKLM.cc
    ${module_source_dir}/decay0/nucltransKLM_Pb.cc
    ${module_source_dir}/decay0/Os186low.cc
    ${module_source_dir}/decay0/Os190low.cc
    ${module_source_dir}/decay0/P32.cc
    ${module_source_dir}/decay0/Pa231.cc
    ${module_source_dir}/decay0/Pa234m.cc
    ${module_source_dir}/decay0/pair.cc
    ${module_source_dir}/decay0/pairext1.cc
    ${module_source_dir}/decay0/pairext.cc
    ${module_source_dir}/decay0/particle.cc
    ${module_source_dir}/decay0/Pb210.cc
    ${module_source_dir}/decay0/Pb211.cc
    ${module_source_dir}/decay0/Pb212.cc
    ${module_source_dir}/decay0/Pb214.cc
    ${module_source_dir}/decay0/PbAtShell.cc
    ${module_source_dir}/decay0/Pd104low.cc
    ${module_source_dir}/decay0/Pd106low.cc
    ${module_source_dir}/decay0/Pd108low.cc
    ${module_source_dir}/decay0/plog69.cc
    ${module_source_dir}/decay0/Po212.cc
    ${module_source_dir}/decay0/Po214.cc
    ${module_source_dir}/decay0/Po214low.cc
    ${module_source_dir}/decay0/positron.cc
    ${module_source_dir}/decay0/Ra222.cc
    ${module_source_dir}/decay0/Ra222low.cc
    ${module_source_dir}/decay0/Ra226.cc
    ${module_source_dir}/decay0/Ra228.cc
    ${module_source_dir}/decay0/Rb87.cc
    ${module_source_dir}/decay0/Rh106.cc
    ${module_source_dir}/decay0/Rn218.cc
    ${module_source_dir}/decay0/Rn218low.cc
    ${module_source_dir}/decay0/Ru100low.cc
    ${module_source_dir}/decay0/Sb125.cc
    ${module_source_dir}/decay0/Sb126.cc
    ${module_source_dir}/decay0/Sb133.cc
    ${module_source_dir}/decay0/Sc48.cc
    ${module_source_dir}/decay0/Se76low.cc
    ${module_source_dir}/decay0/Sm148low.cc
    ${module_source_dir}/decay0/Sm150low.cc
    ${module_source_dir}/decay0/Sn114low.cc
    ${module_source_dir}/decay0/Sn116low.cc
    ${module_source_dir}/decay0/Sn120low.cc
    ${module_source_dir}/decay0/Sr90.cc
    ${module_source_dir}/decay0/Ta182.cc
    ${module_source_dir}/decay0/Te122low.cc
    ${module_source_dir}/decay0/Te124low.cc
    ${module_source_dir}/decay0/Te133.cc
    ${module_source_dir}/decay0/Te133m.cc
    ${module_source_dir}/decay0/Te134.cc
    ${module_source_dir}/decay0/tgold.cc
    ${module_source_dir}/decay0/Th234.cc
    ${module_source_dir}/decay0/Ti48low.cc
    ${module_source_dir}/decay0/Tl207.cc
    ${module_source_dir}/decay0/Tl208.cc
    ${module_source_dir}/decay0/tsimpr.cc
    ${module_source_dir}/decay0/Xe128low.cc
    ${module_source_dir}/decay0/Xe129m.cc
    ${module_source_dir}/decay0/Xe130low.cc
    ${module_source_dir}/decay0/Xe131m.cc
    ${module_source_dir}/decay0/Xe133.cc
    ${module_source_dir}/decay0/Xe135.cc
    ${module_source_dir}/decay0/Y88.cc
    ${module_source_dir}/decay0/Y90.cc
    ${module_source_dir}/decay0/Zn65.cc
    ${module_source_dir}/decay0/Zr92low.cc
    ${module_source_dir}/decay0/Zr96.cc
    )
endif()

# - Publish public headers only
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - add private headers
# list(APPEND ${module_name}_MODULE_HEADERS ${${module_name}_PRIVATE_MODULE_HEADERS})

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "GENBB_HELP_RESOURCE_DIR=${module_resource_dir};GENBB_HELP_TESTING_DIR=${module_test_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_genbb_help.cxx
  ${module_test_dir}/test_bb0nu_channel_2.cxx
  ${module_test_dir}/test_bb0nu_channel.cxx
  ${module_test_dir}/test_bb0nu_cos_theta.cxx
  ${module_test_dir}/test_genbb_help_serialization.cxx
  ${module_test_dir}/test_genbb_mgr_2.cxx
  ${module_test_dir}/test_genbb_mgr_3.cxx
  ${module_test_dir}/test_genbb_mgr_4.cxx
  ${module_test_dir}/test_genbb_mgr_5.cxx
  ${module_test_dir}/test_genbb_mgr.cxx
  ${module_test_dir}/test_genbb_writer.cxx
  ${module_test_dir}/test_pdg_particle_tools.cxx
  ${module_test_dir}/test_primary_event.cxx
  ${module_test_dir}/test_single_particle_generator_2.cxx
  ${module_test_dir}/test_single_particle_generator_3.cxx
  ${module_test_dir}/test_single_particle_generator_4.cxx
  ${module_test_dir}/test_single_particle_generator_5.cxx
  ${module_test_dir}/test_single_particle_generator.cxx
  ${module_test_dir}/test_lorentz_boost_wrapper.cxx
  ${module_test_dir}/test_from_to_file_generator.cxx
  ${module_test_dir}/test_fermi_function_2.cxx
  ${module_test_dir}/test_nuclear_level.cxx
  ${module_test_dir}/test_nuclear_decay.cxx
  ${module_test_dir}/test_nuclear_transition.cxx
  ${module_test_dir}/test_beta_decay.cxx
  ${module_test_dir}/test_alpha_decay.cxx
  ${module_test_dir}/test_nuclear_decay_manager.cxx
  ${module_test_dir}/test_nuclear_decay_generator.cxx
  ${module_test_dir}/test_wdecay0.cxx
  ${module_test_dir}/test_manager.cxx
  ${module_test_dir}/test_reflection_0.cxx
  )

if(NOT Bayeux_WITH_BXDECAY0)
  list(APPEND ${module_name}_MODULE_TESTS
    ${module_test_dir}/decay0/test_decay0_alpha.cxx
    ${module_test_dir}/decay0/test_decay0_bb.cxx
    ${module_test_dir}/decay0/test_decay0_beta1.cxx
    ${module_test_dir}/decay0/test_decay0_beta_1fu.cxx
    ${module_test_dir}/decay0/test_decay0_beta2.cxx
    ${module_test_dir}/decay0/test_decay0_beta.cxx
    ${module_test_dir}/decay0/test_decay0_electron.cxx
    ${module_test_dir}/decay0/test_decay0_fermi.cxx
    ${module_test_dir}/decay0/test_decay0_gamma.cxx
    ${module_test_dir}/decay0/test_decay0_gfang.cxx
    ${module_test_dir}/decay0/test_decay0_plog69.cxx
    ${module_test_dir}/decay0/test_decay0_tgold.cxx
    )
else()
  list(APPEND ${module_name}_MODULE_TESTS ${module_test_dir}/test_wdecay0_2.cxx)
endif()

# - Applications
set(${module_name}_MODULE_APPS
  ${module_app_dir}/genbb_inspector.cxx
  )

# - Resource files
set(${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/manager/config/pro-1.0/README.rst
  ${module_resource_dir}/manager/config/pro-1.0/misc.conf
  ${module_resource_dir}/manager/config/pro-1.0/calibrations.conf
  ${module_resource_dir}/manager/config/pro-1.0/manager.conf
  ${module_resource_dir}/manager/config/pro-1.0/backgrounds.conf
  ${module_resource_dir}/manager/config/pro-1.0/dbd.conf
  ${module_resource_dir}/inspector/config/le_nuphy-1.0/README.rst
  ${module_resource_dir}/inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf
  ${module_resource_dir}/inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf
  ${module_resource_dir}/generators/Co60/README.rst
  ${module_resource_dir}/generators/Co60/manager.conf
  ${module_resource_dir}/generators/Co60/generators.def
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_levels.def
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_decays.def
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_level_scheme.jpeg
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_decay_scheme.jpeg
  ${module_resource_dir}/generators/Co60/data/Co60/Co60_levels.jpeg
  ${module_resource_dir}/generators/Co60/data/Ni60/Ni60_levels.def
  ${module_resource_dir}/generators/Co60/data/Ni60/Ni60_decays.def
  ${module_resource_dir}/generators/Co60/data/Ni60/Ni60_level_scheme.jpeg
  ${module_resource_dir}/generators/Co60/data/Ni60/Ni60_levels.jpeg
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

# - Utility script:
if(BAYEUX_WITH_DEVELOPER_TOOLS)
  configure_file(${module_app_dir}/genbb_mkskelcfg.in
    ${BAYEUX_BUILD_BINDIR}/bxgenbb_mkskelcfg @ONLY)

  install(FILES
    ${BAYEUX_BUILD_BINDIR}/bxgenbb_mkskelcfg
    DESTINATION
    ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS
    OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )
endif()
