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

#include <datatools/i_tree_dump.h>

#include "G4VUserPhysicsList.hh"
//#include "globals.hh"

#include <mctools/g4/loggable_support.h>

namespace datatools {
  class properties;
}

namespace mctools {

  namespace g4 {

    class physics_list : public G4VUserPhysicsList,
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

      static const std::string EM_MODEL_STANDARD;
      static const std::string EM_MODEL_LOW_ENERGY;
      static const std::string EM_MODEL_PENELOPE;

    public:

      bool is_em_standard () const;

      bool is_em_low_energy () const;

      bool is_em_penelope () const;

      const std::string & get_em_model () const;

      physics_list (bool logging_ = false);

      ~physics_list ();

      void initialize (const datatools::properties & config_);

      virtual void tree_dump (std::ostream      & out_    = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;

       // G4 mandatory interface: construct particle and physics
      void ConstructParticle ();

      void ConstructProcess ();

      void SetCuts ();

    protected:

      void _set_defaults ();

      // these methods Construct particles
      void _ConstructBosons ();
      void _ConstructLeptons ();
      void _ConstructMesons ();
      void _ConstructBaryons ();

      // these methods Construct physics processes and register them
      void _ConstructGeneral ();
      void _ConstructEMProcess ();
      void _ConstructHadronicProcess ();

    private:

      bool _initialized_;
      bool _logging_;
      bool _g4_verbosity_;


      std::string _em_model_;
      bool        _electron_energy_loss_;
      bool        _electron_multiple_scattering_;
      bool        _msc_use_distance_to_boundary_;
      bool        _bremsstrahlung_;

      bool        _use_he_leptons_;
      bool        _use_mesons_;
      bool        _use_neutrons_;

      bool        _using_cuts_;
      double      _default_cut_value_;
      std::map<std::string, double> _region_cuts_;
      std::map<std::string, double> _particle_cuts_;

      double _electron_cut_;
      double _positron_cut_;
      double _gamma_cut_;

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_PHYSICS_LIST_H_

// end of physics_list.h
