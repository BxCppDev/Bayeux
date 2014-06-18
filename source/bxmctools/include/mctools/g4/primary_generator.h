/// \file mctools/g4/primary_generator.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 primary event generator action
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_PRIMARY_GENERATOR_H
#define MCTOOLS_G4_PRIMARY_GENERATOR_H 1

// Standard library:
#include <string>

// Third party:
// - Geant4
#include <G4VUserPrimaryGeneratorAction.hh>

// This project:
#include <mctools/g4/loggable_support.h>

class G4ParticleGun;
class G4Event;

namespace genbb {
  class primary_particle;
  class i_genbb;
}

namespace datatools {
  class properties;
}

namespace genvtx {
  class i_vertex_generator;
}

namespace mctools {

  namespace g4 {

    class run_action;
    class event_action;

    /// \brief Generator of primary particles
    class primary_generator : public G4VUserPrimaryGeneratorAction,
                              public loggable_support
    {
    public:

      bool is_initialized() const;

      void set_run_action(mctools::g4::run_action & run_action_);

      void set_event_action(mctools::g4::event_action & event_action_);

      bool has_vertex_generator() const;

      ///
      void set_vertex_generator(genvtx::i_vertex_generator & vertex_generator_);

      void set_event_generator(genbb::i_genbb & event_generator_);

      size_t get_event_counter() const;

      void reset_event_counter();

      primary_generator();

      virtual ~primary_generator();

      void initialize(const datatools::properties & config_);

      void reset();

      std::string get_g4_particle_name_from_genbb_particle(const ::genbb::primary_particle & p_) const;

      // G4 interface:
      void GeneratePrimaries(G4Event *);

    protected:

      void _set_defaults();

      void _check();

      void _generate_vertex();

      void _generate_event(G4Event * g4_event_);

    private:

      bool                 _initialized_; /// Initialization flag
      run_action        *  _run_action_; /// The Geant4 run action
      event_action      *  _event_action_; /// The Geant4 event action
      ::genvtx::i_vertex_generator * _vertex_generator_; /// The external vertex generator
      ::genbb::i_genbb  *  _event_generator_; /// The external event generator
      G4ParticleGun *      _particle_gun_; /// The Geant4 particle gun
      geomtools::vector_3d _current_vertex_; /// The current generated vertex
      size_t               _event_counter_; /// The event counter
      std::map<std::string, std::string> _particle_names_map_; /// A dictionary of particle names

    };

  } // namespace g4

} // namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::primary_generator)

#endif // MCTOOLS_G4_PRIMARY_GENERATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
