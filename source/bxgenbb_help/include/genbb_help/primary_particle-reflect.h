// -*- mode: c++; -*-
//! \file primary_particle-reflect.h
#ifndef GENBB_HELP_PRIMARY_PARTICLE_REFLECT_H
#define GENBB_HELP_PRIMARY_PARTICLE_REFLECT_H

// Standard Library

// Third Party

// Datatools

// Genbb_help
#include <genbb_help/primary_particle.h>

// Export the introspection code for the 'genbb::primary_particle' class:
DR_CLASS_EXPORT (::genbb::primary_particle, "genbb::primary_particle")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::genbb::primary_particle, /*tag_*/)
        {
          // Declare and populate the reflection metaclass for the 'genbb::primary_particle' class:
          DR_CLASS_DECLARE (::genbb::primary_particle)
            .DR_CLASS_BASE(::datatools::i_serializable)
            .DR_CLASS_BASE(::datatools::i_tree_dumpable)
            .DR_CLASS_CONSTRUCTOR()
            .DR_CLASS_CONSTRUCTOR3(int,double,::geomtools::vector_3d)
            .DR_CLASS_PROPERTY_GETTER_SETTER("type",
                                             ::genbb::primary_particle::get_type,
                                             ::genbb::primary_particle::set_type)
            .DR_CLASS_PROPERTY_GETTER_SETTER("pdg_code",
                                             ::genbb::primary_particle::get_pdg_code,
                                             ::genbb::primary_particle::set_pdg_code)
            .DR_CLASS_PROPERTY_GETTER_SETTER("particle_label",
                                             ::genbb::primary_particle::get_particle_label,
                                             ::genbb::primary_particle::set_particle_label)
            .DR_CLASS_PROPERTY_GETTER_SETTER("mass",
                                             ::genbb::primary_particle::get_mass,
                                             ::genbb::primary_particle::set_mass)
            .DR_CLASS_PROPERTY_GETTER_SETTER("time",
                                             ::genbb::primary_particle::get_time,
                                             ::genbb::primary_particle::set_time)
            .DR_CLASS_PROPERTY_GETTER_SETTER("momentum",
                                             ::genbb::primary_particle::get_momentum,
                                             ::genbb::primary_particle::set_momentum)
            .DR_CLASS_PROPERTY_GETTER_SETTER("vertex",
                                             ::genbb::primary_particle::get_vertex,
                                             ::genbb::primary_particle::set_vertex)
            .DR_CLASS_PROPERTY_GETTER("auxiliaries",
                                      ::genbb::primary_particle::get_auxiliaries)
            .DR_CLASS_PROPERTY_GETTER("charge",
                                      ::genbb::primary_particle::get_charge)
            .DR_CLASS_PROPERTY_GETTER("total_energy",
                                      ::genbb::primary_particle::get_total_energy)
            .DR_CLASS_PROPERTY_GETTER("kinetic_energy",
                                      ::genbb::primary_particle::get_kinetic_energy)
            .DR_CLASS_PROPERTY_GETTER("beta",
                                      ::genbb::primary_particle::get_beta)
            .DR_CLASS_METHOD_CONST("has_type",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::has_type,
                                   bool)
            .DR_CLASS_METHOD_CONST("has_pdg_code",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::has_pdg_code,
                                   bool)
            .DR_CLASS_METHOD_CONST("has_particle_label",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::has_particle_label,
                                   bool)
            .DR_CLASS_METHOD_CONST("has_vertex",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::has_vertex,
                                   bool)
            .DR_CLASS_VOID_METHOD("invalidate_vertex",
                                  ::genbb::primary_particle,
                                  ::genbb::primary_particle::invalidate_vertex)
            .DR_CLASS_METHOD_CONST("is_valid",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_valid,
                                   bool)
            .DR_CLASS_METHOD_CONST("charge_is_known",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::charge_is_known,
                                   bool)
            .DR_CLASS_METHOD_CONST("mass_is_known",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::mass_is_known,
                                   bool)
            .DR_CLASS_VOID_METHOD("reset",
                                  ::genbb::primary_particle,
                                  ::genbb::primary_particle::reset)
            .DR_CLASS_METHOD_CONST("is_gamma",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_gamma,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_positron",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_positron,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_electron",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_electron,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_alpha",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_alpha,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_neutron",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_neutron,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_muon_plus",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_muon_plus,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_muon_minus",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_muon_minus,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_nucleus",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_nucleus,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_ion",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::is_ion,
                                   bool)
            .DR_CLASS_METHOD_CONST("get_charge",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::get_charge,
                                   double)
            .DR_CLASS_METHOD_CONST("mass_is_known",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::mass_is_known,
                                   bool)
            // .DR_CLASS_METHOD_CONST("get_mass",
            //                        ::genbb::primary_particle,
            //                        ::genbb::primary_particle::get_mass,
            //                        double)
            .DR_CLASS_METHOD_CONST("get_total_energy",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::get_total_energy,
                                   double)
            .DR_CLASS_METHOD_CONST("get_kinetic_energy",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::get_kinetic_energy,
                                   double)
            .DR_CLASS_METHOD_CONST("get_beta",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::get_beta,
                                   double)
            .DR_CLASS_VOID_METHOD1("shift_time",
                                   ::genbb::primary_particle,
                                   ::genbb::primary_particle::shift_time,
                                   double)
            ;
          return;
        }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GENBB_HELP_PRIMARY_PARTICLE_REFLECT_H
