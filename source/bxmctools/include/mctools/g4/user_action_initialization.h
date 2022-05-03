
#ifndef MCTOOLS_G4_USER_ACTION_INITIALIZATION_H
#define MCTOOLS_G4_USER_ACTION_INITIALIZATION_H 1

#include <G4VUserActionInitialization.hh>

namespace mctools {

  namespace g4 {

    class user_action_initialization
      : public G4VUserActionInitialization
    {
    public:

      user_action_initialization() = default;

      virtual ~user_action_initialization() = default;

      void Build() const;

      void BuildForMaster() const;
      
    };
    
  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_USER_ACTION_INITIALIZATION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
