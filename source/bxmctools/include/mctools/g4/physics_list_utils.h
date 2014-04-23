/// \file mctools/g4/physics_list_utils.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-07-04
 * Last modified: 2013-07-04
 */

#ifndef MCTOOLS_G4_PHYSICS_LIST_UTILS_H
#define MCTOOLS_G4_PHYSICS_LIST_UTILS_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
// - Boost :
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/properties.h>
#include <datatools/handle.h>
// - Boost :
#include <G4VPhysicsConstructor.hh>
#include <G4VModularPhysicsList.hh>
#include <G4BuilderType.hh>

namespace mctools {

  namespace g4 {

    class base_physics_constructor;

    std::string get_builder_type_label(G4BuilderType);

    G4BuilderType get_builder_type(const std::string & label_);

    /// \brief Entry class for the dictionnary of physics constructors
    struct physics_constructor_entry {

    public:

      enum status_type {
        STATUS_CREATED     = 0x1,
        STATUS_INITIALIZED = 0x2
      };

      physics_constructor_entry();

      const base_physics_constructor & get_physics_constructor() const;

      base_physics_constructor & grab_physics_constructor();

    public:

      std::string           name;
      std::string           id;
      datatools::properties config;
      uint32_t              status;
      datatools::handle<base_physics_constructor> handle;

    };

    /// \brief Dictionnary of physics constructors
    typedef std::map<std::string, physics_constructor_entry> physics_constructor_dict_type;

    /// \brief Physics constructor proxy class with weak referencing
    struct physics_constructor_proxy : public G4VPhysicsConstructor {
      physics_constructor_proxy(base_physics_constructor & pc_);
      virtual ~physics_constructor_proxy();
      virtual void ConstructParticle();
      virtual void ConstructProcess();
      base_physics_constructor * pc;
    };

    /// \brief Physics list proxy class with weak referencing
    struct physics_list_proxy : public G4VModularPhysicsList {
      physics_list_proxy(G4VModularPhysicsList & pl_);
      virtual ~physics_list_proxy();
      virtual void ConstructParticle();
      virtual void ConstructProcess();
      virtual void SetCuts();
      G4VModularPhysicsList * pl;
    };

    /// \brief User limits data class
    struct user_limits_info {
    public:
      user_limits_info();
      void reset();
      void make_step_limitation(double step_max_length_);
      void make_track_limitation(double track_max_length_,
                                 double track_max_time_,
                                 double track_min_kinetic_energy_,
                                 double track_min_range_);

    public:
      double step_max_length;
      double track_max_length;
      double track_max_time;
      double track_min_kinetic_energy;
      double track_min_range;
    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_PHYSICS_LIST_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
