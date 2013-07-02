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

      // http://geant4.in2p3.fr/IMG/pdf_Lecture-LowEnergyEMPhysics.pdf
      static const std::string EM_MODEL_STANDARD;
      static const std::string EM_MODEL_LOW_ENERGY_LIVERMORE;
      static const std::string EM_MODEL_LOW_ENERGY_PENELOPE;

    public:

      bool is_em_standard () const;

      bool is_em_low_energy_livermore () const;

      bool is_em_low_energy_penelope () const;

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

      void _set_defaults();

      void _initialize_deexcitation();

      // these methods Construct particles
      void _ConstructBosons ();
      void _ConstructLeptons ();
      void _ConstructMesons ();
      void _ConstructBaryons ();
      void _ConstructIons ();

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

      // http://geant4.in2p3.fr/IMG/pdf_Lecture-LowEnergyEMPhysics.pdf
      bool        _em_fluo_;
      bool        _em_auger_;
      bool        _em_pixe_;
      std::string _em_pixe_cross_section_model_;
      static const std::string EM_PIXE_MODEL_EMPIRICAL;
      static const std::string EM_PIXE_MODEL_ECPSSR_FORMFACTOR;
      static const std::string EM_PIXE_MODEL_ECPSSR_ANALYTICAL;
      struct region_deexcitation_type {
        bool fluo;
        bool auger;
        bool pixe;
        region_deexcitation_type();
        region_deexcitation_type(bool,bool,bool);
        bool is_activated() const;
        bool is_fluo() const;
        bool is_auger() const;
        bool is_pixe() const;
      };
      std::map<std::string, region_deexcitation_type> _em_regions_deexcitation_;

      bool        _use_geantinos_;
      bool        _use_optical_photons_;

      bool        _use_muon_leptons_;
      bool        _use_tau_leptons_;

      bool        _use_light_mesons_;
      bool        _use_charm_mesons_;
      bool        _use_bottom_mesons_;

      bool        _use_nucleons_;
      bool        _use_strange_baryons_;
      bool        _use_charm_baryons_;
      bool        _use_bottom_baryons_;

      bool        _use_light_nuclei_;
      bool        _use_light_anti_nuclei_;
      bool        _use_generic_ion_;

      bool        _using_cuts_;
      double      _default_cut_value_;
      double      _cuts_min_energy_;
      double      _cuts_max_energy_;
      std::map<std::string, double> _region_cuts_;
      std::map<std::string, double> _particle_cuts_;

      double _electron_cut_;
      double _positron_cut_;
      double _gamma_cut_;
      double _proton_cut_;

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::physics_list)

#endif // MCTOOLS_G4_PHYSICS_LIST_H_

// end of physics_list.h
