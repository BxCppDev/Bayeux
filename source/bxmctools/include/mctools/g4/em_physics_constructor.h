/// \file mctools/g4/em_physics_constructor.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-07-03
 * Last modified: 2013-07-04
 *
 * License:
 *
 * Description:
 *
 *   G4 electro-magnetic physics constructor
 *
 *   http://geant4.in2p3.fr/IMG/pdf_Lecture-LowEnergyEMPhysics.pdf
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_EM_PHYSICS_CONSTRUCTOR_H
#define MCTOOLS_G4_EM_PHYSICS_CONSTRUCTOR_H 1

// This project:
#include <mctools/g4/base_physics_constructor.h>

namespace mctools {

  namespace g4 {

    class em_physics_constructor : public base_physics_constructor
    {
    public:

      // http://geant4.in2p3.fr/IMG/pdf_Lecture-LowEnergyEMPhysics.pdf
      static const std::string EM_MODEL_STANDARD;
      static const std::string EM_MODEL_LOW_ENERGY_LIVERMORE;
      static const std::string EM_MODEL_LOW_ENERGY_PENELOPE;
      static const std::string EM_PIXE_MODEL_EMPIRICAL;
      static const std::string EM_PIXE_MODEL_ECPSSR_FORMFACTOR;
      static const std::string EM_PIXE_MODEL_ECPSSR_ANALYTICAL;
      static const std::string EM_PIXE_ELECTRON_MODEL_LIVERMORE;
      static const std::string EM_PIXE_ELECTRON_MODEL_PROTON_ANALYTICAL;
      static const std::string EM_PIXE_ELECTRON_MODEL_PROTON_EMPIRICAL;
      static const std::string EM_PIXE_ELECTRON_MODEL_PENELOPE;

      struct region_deexcitation_type {
        bool fluorescence;
        bool auger;
        bool pixe;
        region_deexcitation_type();
        region_deexcitation_type(bool,bool,bool);
        bool is_activated() const;
        bool is_fluorescence() const;
        bool is_auger() const;
        bool is_pixe() const;
      };

      /// Check the standard EM processes
      bool is_em_standard () const;

      /// Check the low energy Livermore EM processes
      bool is_em_low_energy_livermore () const;

      /// Check the low energy Penelope EM processes
      bool is_em_low_energy_penelope () const;

      /// Return the name of the EM processes
      const std::string & get_em_model () const;

      /// Default constructor
      em_physics_constructor ();

      /// Destructor
      virtual ~em_physics_constructor ();

      /// Initialization
      virtual void initialize (const datatools::properties & config_,
                               physics_constructor_dict_type & dict_);

      /// Reset
      virtual void reset ();

      /// Smart print
      virtual void tree_dump (std::ostream      & out_    = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;


      /// Particles construction Geant4 interface :
      virtual void ConstructParticle ();

      /// Processes construction Geant4 interface :
      virtual void ConstructProcess ();

    protected:

      // these methods construct processes
      void _ConstructEMProcess ();
      void _ConstructEMDeexcitation();

      void _set_defaults();

    private:

      // EM processes:
      std::string _em_model_;
      bool        _em_gamma_rayleigh_scattering_;
      bool        _em_gamma_photo_electric_;
      bool        _em_gamma_compton_scattering_;
      bool        _em_gamma_conversion_;
      bool        _em_gamma_conversion_to_muons_;
      bool        _em_gamma_step_limiter_;
      bool        _em_gamma_user_special_cuts_;
      bool        _em_electron_ionisation_;
      bool        _em_electron_bremsstrahlung_;
      bool        _em_electron_multiple_scattering_;
      bool        _em_electron_ms_use_distance_to_boundary_;
      double      _em_electron_ms_range_factor_;
      bool        _em_positron_annihilation_;
      bool        _em_electron_step_limiter_;
      bool        _em_electron_user_special_cuts_;
      // Atomic deexcitation:
      bool        _em_fluorescence_;
      bool        _em_auger_;
      bool        _em_pixe_;
      std::string _em_pixe_cross_section_model_;
      std::string _em_pixe_electron_cross_section_model_;
      std::map<std::string, region_deexcitation_type> _em_regions_deexcitation_;
      bool        _em_ion_ionisation_;
      bool        _em_ion_multiple_scattering_;
      bool        _em_ion_step_limiter_;
      bool        _em_ion_user_special_cuts_;
      bool        _em_muon_multiple_scattering_;
      bool        _em_muon_ionisation_;
      bool        _em_muon_bremsstrahlung_;
      bool        _em_muon_pair_production_;
      bool        _em_muon_step_limiter_;
      bool        _em_muon_user_special_cuts_;

      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (base_physics_constructor,
                                                            em_physics_constructor);

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::em_physics_constructor)

#endif // MCTOOLS_G4_EM_PHYSICS_CONSTRUCTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
