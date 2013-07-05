// -*- mode: c++ ; -*-
/* physics_list.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-06-03
 *
 * License:
 *
 * Description:
 *
 *   G4 physics list
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_PHYSICS_LIST_H_
#define MCTOOLS_G4_PHYSICS_LIST_H_ 1

#include <string>
#include <map>

#include <boost/scoped_ptr.hpp>

#include <datatools/i_tree_dump.h>

//#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
//#include "globals.hh"

#include <mctools/g4/loggable_support.h>
#include <mctools/g4/physics_list_utils.h>
#include <mctools/g4/base_physics_constructor.h>

namespace datatools {
  class properties;
}

namespace mctools {

  namespace g4 {

    class physics_list : public G4VModularPhysicsList,
                         public datatools::i_tree_dumpable,
                         public loggable_support
    {
    public:
      enum g4_verbosity_type
        {
          VERBOSITY_SILENT  = 0,
          VERBOSITY_WARNING = 1,
          VERBOSITY_MORE    = 2
        };

      struct production_cuts_info {
      public:

        production_cuts_info();

        production_cuts_info(double all_value_);

        production_cuts_info(double gamma_value_, double electron_value_,
                             double positron_value_, double proton_value_);

        void initialize(double default_cut_gamma_, double default_cut_electron_,
                        double default_cut_positron_, double default_cut_proton_);

      public:

        double gamma;
        double electron;
        double positron;
        double proton;

      };

      bool has_geant4_physics_list () const;

      const G4VModularPhysicsList & get_geant4_physics_list() const;

      G4VModularPhysicsList & grab_geant4_physics_list();

      bool has_physics_constructor (const std::string & pc_name_) const;

      const base_physics_constructor & get_physics_constructor(const std::string & pc_name_);

      const physics_constructor_dict_type  & get_physics_constructors() const;

      physics_constructor_dict_type  & grab_physics_constructors();

      physics_list ();

      virtual ~physics_list ();

      void initialize (const datatools::properties & config_);

      void reset ();

      virtual void tree_dump (std::ostream      & out_    = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;

      // G4 mandatory interface: construct particle and physics
      virtual void ConstructParticle();

      virtual void ConstructProcess();

      virtual void SetCuts ();

    protected:

      void _set_defaults();

      void _register_physics_constructors();

      void _SetCuts();

    private:

      bool _initialized_; //!< Initialization flag

      std::string                              _geant4_physics_list_name_;
      boost::scoped_ptr<G4VModularPhysicsList> _geant4_physics_list_; //!< Wrapped official Geant4 physics list


      // Physics constructors:
      base_physics_constructor::factory_register_type  _factory_register_;  //!< The embedded factory register for physics constructors
      physics_constructor_dict_type                    _physics_constructors_; //!< The embedded dictionnary of physics constructors

      // Production cuts for secondary particles (only gamma, electron, positron, proton):
      bool        _using_production_cuts_;
      double      _production_cuts_low_energy_;
      double      _production_cuts_high_energy_;
      double      _production_cuts_default_value_;
      production_cuts_info _production_cuts_values_;
      std::map<std::string, production_cuts_info> _production_cuts_per_region_;

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::physics_list)

#endif // MCTOOLS_G4_PHYSICS_LIST_H_

// end of physics_list.h
