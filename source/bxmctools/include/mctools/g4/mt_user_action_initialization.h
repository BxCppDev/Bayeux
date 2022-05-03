/// \file mctools/g4/mt_user_action_initialization.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-19
 * Last modified: 2022-04-19
 *
 * License: GPL 3.0
 *
 * Description:
 *
 *   G4 user action initialization
 *
 */

#ifndef MCTOOLS_G4_MT_USER_ACTION_INITIALIZATION_H
#define MCTOOLS_G4_MT_USER_ACTION_INITIALIZATION_H 1

// Third party:
// - Geant4:
#include <G4VUserActionInitialization.hh>

// - Bayeux/datatools :
#include <datatools/multi_properties.h>

namespace mctools {

  namespace g4 {

    class manager;
    class run_action;
    class event_action;
    class primary_generator;
    class tracking_action;
    class stepping_action;
    class stacking_action;

    class mt_user_action_initialization
      : public G4VUserActionInitialization
    {
    public:
      
      mt_user_action_initialization(manager & mgr_);
      
      virtual ~mt_user_action_initialization() = default;
      
      virtual void Build() const override;

      virtual void BuildForMaster() const override;

    private:

      void _init_run_action_();

      void _init_event_action_();

      void _init_primary_generator_action_();

      void _init_tracking_action_();

      void _init_stepping_action_();

      void _init_stacking_action_();

    private:

      manager * _manager_ = nullptr;
      const datatools::multi_properties * _multi_config_ = nullptr; 
      mctools::g4::run_action * _user_run_action_ = nullptr;
      mctools::g4::event_action * _user_event_action_ = nullptr;
      mctools::g4::primary_generator * _user_primary_generator_ = nullptr;
      mctools::g4::tracking_action * _user_tracking_action_ = nullptr;
      mctools::g4::stepping_action * _user_stepping_action_ = nullptr;
      mctools::g4::stacking_action * _user_stacking_action_ = nullptr;

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_MT_USER_ACTION_INITIALIZATION_H 1
