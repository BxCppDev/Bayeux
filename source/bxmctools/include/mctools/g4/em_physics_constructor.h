// -*- mode: c++ ; -*-
/* em_physics_constructor.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-07-03
 * Last modified: 2013-07-04
 *
 * License:
 *
 * Description:
 *
 *   G4 electro-magnetic physics constructor
 *
 *     // http://geant4.in2p3.fr/IMG/pdf_Lecture-LowEnergyEMPhysics.pdf
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_EM_PHYSICS_CONSTRUCTOR_H_
#define MCTOOLS_G4_EM_PHYSICS_CONSTRUCTOR_H_ 1

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

      bool is_em_standard () const;

      bool is_em_low_energy_livermore () const;

      bool is_em_low_energy_penelope () const;

      const std::string & get_em_model () const;

      em_physics_constructor ();

      virtual ~em_physics_constructor ();

      virtual void initialize (const datatools::properties & config_,
                               physics_constructor_dict_type & dict_);

      virtual void reset ();

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
      bool        _em_electron_ionisation_;
      bool        _em_electron_bremsstrahlung_;
      bool        _em_electron_multiple_scattering_;
      bool        _em_electron_ms_use_distance_to_boundary_;
      double      _em_electron_ms_range_factor_;
      bool        _em_positron_annihilation_;
      bool        _em_electron_step_limiter_;
      // Atomic deexcitation:
      bool        _em_fluorescence_;
      bool        _em_auger_;
      bool        _em_pixe_;
      std::string _em_pixe_cross_section_model_;
      static const std::string EM_PIXE_MODEL_EMPIRICAL;
      static const std::string EM_PIXE_MODEL_ECPSSR_FORMFACTOR;
      static const std::string EM_PIXE_MODEL_ECPSSR_ANALYTICAL;
      std::map<std::string, region_deexcitation_type> _em_regions_deexcitation_;
      bool        _em_ion_ionisation_;
      bool        _em_ion_multiple_scattering_;
      bool        _em_ion_step_limiter_;
      bool        _em_muon_multiple_scattering_;
      bool        _em_muon_ionisation_;
      bool        _em_muon_bremsstrahlung_;
      bool        _em_muon_pair_production_;
      bool        _em_muon_step_limiter_;

      DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (base_physics_constructor,
                                                            em_physics_constructor);

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::em_physics_constructor)

#endif // MCTOOLS_G4_EM_PHYSICS_CONSTRUCTOR_H_

// end of em_physics_constructor.h
