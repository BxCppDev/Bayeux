/// \file mctools/g4/base_physics_constructor.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-07-03
 * Last modified: 2013-07-03
 *
 * License:
 *
 * Description:
 *
 *   G4 electro-magnetic physics list
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_BASE_PHYSICS_CONSTRUCTOR_H
#define MCTOOLS_G4_BASE_PHYSICS_CONSTRUCTOR_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_tree_dump.h>
#include <datatools/factory_macros.h>
// - Geant4
#include <G4VPhysicsConstructor.hh>
#include <globals.hh>

// This project:
#include <mctools/g4/loggable_support.h>
#include <mctools/g4/physics_list_utils.h>

namespace mctools {

  namespace g4 {

    class physics_list;

    class base_physics_constructor :
      public G4VPhysicsConstructor,
      public datatools::i_tree_dumpable,
      public loggable_support
    {
    public:

      bool is_initialized() const;

      void set_name(const std::string & name_);

      const std::string & get_name() const;

      void set_class_id(const std::string & class_id_);

      const std::string & get_class_id() const;

      base_physics_constructor();

      virtual ~base_physics_constructor();

      virtual void initialize(const datatools::properties & config_,
                              physics_constructor_dict_type & dict_) = 0;

      void initialize_standalone(const datatools::properties & config_);

      virtual void reset() = 0;

      // G4 mandatory interface: construct particle and physics
      virtual void ConstructParticle(); // Default empty implementation

      virtual void ConstructProcess(); // Default empty implementation

      bool has_mother_physics_list() const;

      const physics_list & get_mother_physics_list() const;

      virtual void tree_dump (std::ostream      & out_    = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;

    protected:

      void _set_initialized(bool);

      void _set_mother_physics_list(physics_list &);

      void _reset();

    private:

      std::string _name_;        //!< Name of the physics constructor
      std::string _class_id_;    //!< Class ID of the physics constructor
      bool        _initialized_; //!< Initialization flag
      physics_list * _mother_physics_list_; //! Reference to the mother physics list

      // Factory stuff :
      DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_physics_constructor);

      friend class physics_list;

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_BASE_PHYSICS_CONSTRUCTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
