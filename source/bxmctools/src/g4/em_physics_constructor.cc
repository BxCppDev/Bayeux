// em_physics_constructor.cc
/*
 * https://twiki.cern.ch/twiki/bin/view/Geant4/LoweMigratedLivermore
 * https://twiki.cern.ch/twiki/bin/view/Geant4/LoweAtomicDeexcitation
 */

// Ourselves:
#include <mctools/g4/em_physics_constructor.h>

// Standard library:
#include <stdexcept>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/ioutils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>
// - Geant4:
#include <globals.hh>
#include <G4Version.hh>
#include <G4UnitsTable.hh>
#include <G4ProcessManager.hh>
// Particles:
#include <G4ParticleDefinition.hh>
#include <G4LossTableManager.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleTable.hh>
#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4MuonPlus.hh>
#include <G4MuonMinus.hh>
//#include <G4RegionStore.hh>
// EM processes :
#include <G4RadioactiveDecay.hh>
#include <G4EmProcessOptions.hh>
#include <G4ComptonScattering.hh>
#include <G4RayleighScattering.hh>
#include <G4GammaConversion.hh>
#include <G4GammaConversionToMuons.hh>
#include <G4PhotoElectricEffect.hh>
#include <G4eMultipleScattering.hh>
#include <G4eIonisation.hh>
#include <G4eBremsstrahlung.hh>
#include <G4eplusAnnihilation.hh>
#include <G4MuMultipleScattering.hh>
#include <G4MuIonisation.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>
#include <G4hMultipleScattering.hh>
#include <G4hIonisation.hh>
#include <G4ionIonisation.hh>
/* **** LIVERMORE **** */
// Gammas :
#include <G4LivermoreComptonModel.hh>
#include <G4LivermoreRayleighModel.hh>
#include <G4LivermoreGammaConversionModel.hh>
#include <G4LivermorePhotoElectricModel.hh>
// Electrons :
#include <G4LivermoreIonisationModel.hh>
#include <G4LivermoreBremsstrahlungModel.hh>
/* **** PENELOPE **** */
// Gammas :
#include <G4PenelopeComptonModel.hh>
#include <G4PenelopeRayleighModel.hh>
#include <G4PenelopeGammaConversionModel.hh>
#include <G4PenelopePhotoElectricModel.hh>
// Electrons/positrons :
#include <G4PenelopeIonisationModel.hh>
#include <G4PenelopeBremsstrahlungModel.hh>
// Positrons :
#include <G4PenelopeAnnihilationModel.hh>
#include "G4UAtomicDeexcitation.hh"
#include <G4RegionStore.hh>
#include <G4StepLimiter.hh>
#include <G4UserSpecialCuts.hh>

namespace mctools {

  namespace g4 {

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (base_physics_constructor,
                                                               em_physics_constructor,
                                                               "mctools::g4::em_physics_constructor");

    // *** em_physics_constructor::region_deexcitation_type *** //

    em_physics_constructor::region_deexcitation_type::region_deexcitation_type()
    {
      fluorescence = false;
      auger = false;
      pixe = false;
    }

    em_physics_constructor::region_deexcitation_type::region_deexcitation_type(bool fluo_, bool auger_, bool pixe_)
    {
      fluorescence = fluo_;
      if (auger_) {
        fluorescence = true;
      }
      auger = auger_;
      pixe = pixe_;
    }

    bool em_physics_constructor::region_deexcitation_type::is_activated() const
    {
      return fluorescence || auger || pixe;
    }

    bool em_physics_constructor::region_deexcitation_type::is_fluorescence() const
    {
      return fluorescence;
    }

    bool em_physics_constructor::region_deexcitation_type::is_auger() const
    {
      return auger;
    }

    bool em_physics_constructor::region_deexcitation_type::is_pixe() const
    {
      return pixe;
    }

    // *** em_physics_constructor *** //

    const std::string em_physics_constructor::EM_MODEL_STANDARD               = "standard";
    const std::string em_physics_constructor::EM_MODEL_LOW_ENERGY_LIVERMORE   = "livermore";
    const std::string em_physics_constructor::EM_MODEL_LOW_ENERGY_PENELOPE    = "penelope";

    const std::string em_physics_constructor::EM_PIXE_MODEL_EMPIRICAL         = "Empirical";
    const std::string em_physics_constructor::EM_PIXE_MODEL_ECPSSR_FORMFACTOR = "ECPSSR_FormFactor";
    const std::string em_physics_constructor::EM_PIXE_MODEL_ECPSSR_ANALYTICAL = "ECPSSR_Analytical";

    const std::string em_physics_constructor::EM_PIXE_ELECTRON_MODEL_LIVERMORE         = "Livermore";
    const std::string em_physics_constructor::EM_PIXE_ELECTRON_MODEL_PENELOPE          = "Penelope";
    const std::string em_physics_constructor::EM_PIXE_ELECTRON_MODEL_PROTON_ANALYTICAL = "Analytical";
    const std::string em_physics_constructor::EM_PIXE_ELECTRON_MODEL_PROTON_EMPIRICAL  = "Empirical";

    const std::string & em_physics_constructor::get_em_model () const
    {
      return _em_model_;
    }

    bool em_physics_constructor::is_em_standard () const
    {
      return _em_model_ == EM_MODEL_STANDARD;
    }

    bool em_physics_constructor::is_em_low_energy_livermore () const
    {
      return _em_model_ == EM_MODEL_LOW_ENERGY_LIVERMORE;
    }

    bool em_physics_constructor::is_em_low_energy_penelope () const
    {
      return _em_model_ == EM_MODEL_LOW_ENERGY_PENELOPE;
    }

    em_physics_constructor::em_physics_constructor () : base_physics_constructor ()
    {
      this->em_physics_constructor::_set_defaults ();
      SetPhysicsName("electromagnetic");
      SetPhysicsType(1);
      return;
    }

    em_physics_constructor::~em_physics_constructor ()
    {
      if (is_initialized()) {
        this->em_physics_constructor::reset();
      }
      return;
    }

    void em_physics_constructor::initialize (const datatools::properties & config_,
                                             physics_constructor_dict_type & /*dict_*/)
    {
      DT_LOG_TRACE(_logprio(), "Entering...");
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

      /*** parsing properties from 'config_'  ***/
      loggable_support::_initialize_logging_support(config_);


      // **** Electro-magnetic processes **** //

      if (config_.has_key ("em.model")) {
        _em_model_ = config_.fetch_string ("em.model");
      }
      DT_THROW_IF (_em_model_ != EM_MODEL_STANDARD
                   && _em_model_ != EM_MODEL_LOW_ENERGY_LIVERMORE
                   && _em_model_ != EM_MODEL_LOW_ENERGY_PENELOPE,
                   std::logic_error,
                   "Invalid electro-magnetic interaction model '" << _em_model_ << "' !");

      DT_LOG_DEBUG(_logprio(),"Electro-magnetic model set to: '" << _em_model_ << "'");

      // Gammas :
      if (config_.has_key ("em.gamma.rayleigh_scattering")) {
        _em_gamma_rayleigh_scattering_ = config_.fetch_boolean ("em.gamma.rayleigh_scattering");
      }
      DT_LOG_DEBUG(_logprio(), "Gamma Rayleigh scattering process set to: " << _em_gamma_rayleigh_scattering_);

      if (config_.has_key ("em.gamma.photo_electric")) {
        _em_gamma_photo_electric_ = config_.fetch_boolean ("em.gamma.photo_electric");
      }
      DT_LOG_DEBUG(_logprio(), "Gamma photo-electric process set to: " << _em_gamma_photo_electric_);

      if (config_.has_key ("em.gamma.compton_scattering")) {
        _em_gamma_compton_scattering_ = config_.fetch_boolean ("em.gamma.compton_scattering");
      }
      DT_LOG_DEBUG(_logprio(), "Gamma Compton scattering process set to: " << _em_gamma_compton_scattering_);

      if (config_.has_key ("em.gamma.conversion")) {
        _em_gamma_conversion_ = config_.fetch_boolean ("em.gamma.conversion");
      }
      DT_LOG_DEBUG(_logprio(), "Gamma conversion process set to: " << _em_gamma_conversion_);


      if (config_.has_key ("em.gamma.conversion_to_muons")) {
        _em_gamma_conversion_to_muons_ = config_.fetch_boolean ("em.gamma.conversion_to_muons");
      }
      DT_LOG_DEBUG(_logprio(), "Gamma conversion to muons process set to: " << _em_gamma_conversion_to_muons_);

      if (config_.has_key ("em.gamma.step_limiter")) {
        _em_gamma_step_limiter_ = config_.fetch_boolean ("em.gamma.step_limiter");
      }
      DT_LOG_DEBUG(_logprio(),"Gamma step limiter set to: " << _em_gamma_step_limiter_);

      // Electrons/positrons :
      if (config_.has_key ("em.electron.ionisation")) {
        _em_electron_ionisation_ = config_.fetch_boolean ("em.electron.ionisation");
      }
      DT_LOG_DEBUG(_logprio(), "Electron/positron energy loss set to: " << _em_electron_ionisation_);

      if (config_.has_key ("em.electron.multiple_scattering")) {
        _em_electron_multiple_scattering_ = config_.fetch_boolean ("em.electron.multiple_scattering");
      }
      DT_LOG_DEBUG(_logprio(),"Electron/positron multiple scattering set to: " << _em_electron_multiple_scattering_);

      if (config_.has_key ("em.electron.multiple_scattering.use_distance_to_boundary")) {
        _em_electron_ms_use_distance_to_boundary_ = config_.fetch_boolean ("em.electron.multiple_scattering.use_distance_to_boundary");
      }
      DT_LOG_DEBUG(_logprio(),"Electron/positron multiple scattering use distance to boundary: " << _em_electron_ms_use_distance_to_boundary_);

      if (config_.has_key ("em.electron.multiple_scattering.range_factor")) {
        _em_electron_ms_range_factor_ = config_.fetch_real ("em.electron.multiple_scattering.range_factor");
        DT_THROW_IF(_em_electron_ms_range_factor_ <= 0.0,
                    std::domain_error,
                    "Invalid multiple scattering range factor for electron/positron ("
                    << _em_electron_ms_range_factor_ << ") !");
      }
      DT_LOG_DEBUG(_logprio(),"Electron/positron multiple scattering range factor: " << _em_electron_ms_range_factor_);


      if (config_.has_key ("em.electron.bremsstrahlung")) {
        _em_electron_bremsstrahlung_ = config_.fetch_boolean ("em.electron.bremsstrahlung");
      }
      DT_LOG_DEBUG(_logprio(),"Electron/positron bremsstrahlung set to: " << _em_electron_bremsstrahlung_);

      // Positrons :
      if (config_.has_key ("em.positron.annihilation")) {
        _em_positron_annihilation_ = config_.fetch_boolean ("em.positron.annihilation");
      }
      DT_LOG_DEBUG(_logprio(),"Positron annihilation set to: " << _em_positron_annihilation_);

      if (config_.has_key ("em.electron.step_limiter")) {
        _em_electron_step_limiter_ = config_.fetch_boolean ("em.electron.step_limiter");
      }
      DT_LOG_DEBUG(_logprio(),"Electron/positron step limiter set to: " << _em_electron_step_limiter_);


      /* **** Atomic deexcitation **** */

      if (config_.has_key("em.deexcitation.fluorescence")) {
        _em_fluorescence_ = config_.fetch_boolean("em.deexcitation.fluorescence");
      }
      DT_LOG_DEBUG(_logprio(),"EM fluorescence set to : " << _em_fluorescence_);

      if (_em_fluorescence_) {
        if (config_.has_key("em.deexcitation.auger")) {
          _em_auger_ = config_.fetch_boolean("em.deexcitation.auger");
          // if (_em_auger_) {
          //   _em_fluorescence_ = true;
          // }
        }
        DT_LOG_DEBUG(_logprio(),"EM Auger set to : " << _em_auger_);
      }

      if (config_.has_key("em.deexcitation.pixe")) {
        _em_pixe_ = config_.fetch_boolean("em.deexcitation.pixe");
      }
      DT_LOG_DEBUG(_logprio(),"EM PIXE set to : " << _em_pixe_);

      if (config_.has_key ("em.deexcitation.pixe.cross_section_model")) {
        _em_pixe_cross_section_model_ = config_.fetch_string ("em.deexcitation.pixe.cross_section_model");
        DT_THROW_IF (_em_pixe_cross_section_model_ != EM_PIXE_MODEL_EMPIRICAL
                     && _em_pixe_cross_section_model_ != EM_PIXE_MODEL_ECPSSR_FORMFACTOR
                     && _em_pixe_cross_section_model_ != EM_PIXE_MODEL_ECPSSR_ANALYTICAL,
                     std::logic_error,
                     "Invalid EM PIXE cross-section model '" << _em_pixe_cross_section_model_ << "' !");
        DT_LOG_DEBUG(_logprio(),"EM PIXE cross-section model set to : " << _em_pixe_cross_section_model_);
      }

      if (config_.has_key ("em.deexcitation.pixe.electron_cross_section_model")) {
        _em_pixe_electron_cross_section_model_ = config_.fetch_string ("em.deexcitation.pixe.electron_cross_section_model");
        DT_THROW_IF (   _em_pixe_electron_cross_section_model_ != EM_PIXE_ELECTRON_MODEL_LIVERMORE
                     && _em_pixe_electron_cross_section_model_ != EM_PIXE_ELECTRON_MODEL_PROTON_ANALYTICAL
                     && _em_pixe_electron_cross_section_model_ != EM_PIXE_ELECTRON_MODEL_PROTON_EMPIRICAL
                     && _em_pixe_electron_cross_section_model_ != EM_PIXE_ELECTRON_MODEL_PENELOPE,
                     std::logic_error,
                     "Invalid EM PIXE e+/e- cross-section model '" << _em_pixe_electron_cross_section_model_ << "' !");
        DT_LOG_DEBUG(_logprio(),"EM PIXE e+/e- cross-section model set to : " << _em_pixe_electron_cross_section_model_);
      }

      /* **** Atomic deexcitation per region **** */
      std::vector<std::string> deexcitation_regions;
      if (config_.has_key ("em.deexcitation.regions")) {
        config_.fetch("em.deexcitation.regions", deexcitation_regions);
      }

      for (size_t i  = 0; i < deexcitation_regions.size(); i++) {
        const std::string & region_name = deexcitation_regions[i];
        region_deexcitation_type rd(false,false,false);
        std::ostringstream region_fluo_key;
        region_fluo_key << "em.deexcitation.regions." << region_name << ".fluorescence";
        if (config_.has_key(region_fluo_key.str())) {
          rd.fluorescence = config_.fetch_boolean(region_fluo_key.str());
          if (rd.fluorescence) {
            _em_fluorescence_ = true;
          }
        }
        if (rd.fluorescence) {
          std::ostringstream region_auger_key;
          region_auger_key << "em.deexcitation.regions." << region_name << ".auger";
          if (config_.has_key(region_auger_key.str())) {
            rd.auger = config_.fetch_boolean(region_auger_key.str());
            if (rd.auger) {
              _em_auger_ = true;
            }
          }
        }
        std::ostringstream region_pixe_key;
        region_pixe_key << "em.deexcitation.regions." << region_name << ".pixe";
        if (config_.has_key(region_pixe_key.str())) {
          rd.pixe = config_.fetch_boolean(region_pixe_key.str());
          if (rd.pixe) {
            _em_pixe_ = true;
          }
        }
        _em_regions_deexcitation_[region_name] = rd;
      }

      // Ion :
      if (config_.has_key ("em.ion.ionisation")) {
        _em_ion_ionisation_ = config_.fetch_boolean ("em.ion.ionisation");
      }
      DT_LOG_DEBUG(_logprio(),"Ion ionisation set to: " << _em_ion_ionisation_);

      if (config_.has_key ("em.ion.multiple_scattering")) {
        _em_ion_multiple_scattering_ = config_.fetch_boolean ("em.ion.multiple_scattering");
      }
      DT_LOG_DEBUG(_logprio(),"Ion multiple scattering set to: " << _em_ion_multiple_scattering_);

      if (config_.has_key ("em.ion.step_limiter")) {
        _em_ion_step_limiter_ = config_.fetch_boolean ("em.ion.step_limiter");
      }
      DT_LOG_DEBUG(_logprio(),"Ion step limiter set to: " << _em_ion_step_limiter_);

      // Muons :
      if (config_.has_key ("em.muon.ionisation")) {
        _em_muon_ionisation_ = config_.fetch_boolean ("em.muon.ionisation");
      }
      DT_LOG_DEBUG(_logprio(),"Muon ionisation set to: " << _em_muon_ionisation_);

      if (config_.has_key ("em.muon.bremsstrahlung")) {
        _em_muon_bremsstrahlung_ = config_.fetch_boolean ("em.muon.bremsstrahlung");
      }
      DT_LOG_DEBUG(_logprio(),"Muon bremsstrahlung set to: " << _em_muon_bremsstrahlung_);

      if (config_.has_key ("em.muon.multiple_scattering")) {
        _em_muon_multiple_scattering_ = config_.fetch_boolean ("em.muon.multiple_scattering");
      }
      DT_LOG_DEBUG(_logprio(),"Muon multiple scattering set to: " << _em_muon_multiple_scattering_);

      if (config_.has_key ("em.muon.pair_production")) {
        _em_muon_pair_production_ = config_.fetch_boolean ("em.muon.pair_production");
      }
      DT_LOG_DEBUG(_logprio(),"Muon pair production set to: " << _em_muon_pair_production_);

      if (config_.has_key ("em.muon.step_limiter")) {
        _em_muon_step_limiter_ = config_.fetch_boolean ("em.muon.step_limiter");
      }
      DT_LOG_DEBUG(_logprio(),"Muon step limiter set to: " << _em_muon_step_limiter_);

      // End

      _set_initialized(true);

      if (_logprio() >= datatools::logger::PRIO_INFORMATION) {
        DT_LOG_INFORMATION(_logprio(), "EM Physics Constructor: ");
        this->tree_dump(std::clog, "", "");
      }

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }


    void em_physics_constructor::_set_defaults ()
    {
      // Process:
      _em_model_                        = EM_MODEL_STANDARD;

      // gamma:
      _em_gamma_rayleigh_scattering_    = false;
      _em_gamma_photo_electric_         = true;
      _em_gamma_compton_scattering_     = true;
      _em_gamma_conversion_             = true;
      _em_gamma_conversion_to_muons_    = false;
      _em_gamma_step_limiter_           = false;
      _em_gamma_user_special_cuts_      = false;

      // electron/positron:
      _em_electron_ionisation_          = true;
      _em_electron_bremsstrahlung_      = true;
      _em_electron_multiple_scattering_ = true;
      _em_electron_ms_use_distance_to_boundary_  = true;
      _em_electron_ms_range_factor_              = 0.005;
      _em_positron_annihilation_        = true;
      _em_electron_step_limiter_        = true;
      _em_electron_user_special_cuts_   = false;

      // atomic deexcitation
      _em_fluorescence_                 = false;
      _em_auger_                        = false;
      _em_pixe_                         = false;
      _em_pixe_cross_section_model_          = EM_PIXE_MODEL_EMPIRICAL; // clear();
      _em_pixe_electron_cross_section_model_ = EM_PIXE_ELECTRON_MODEL_LIVERMORE; // clear();
      _em_regions_deexcitation_.clear();

      // ion:
      _em_ion_multiple_scattering_      = true;
      _em_ion_ionisation_               = true;
      _em_ion_step_limiter_             = false;
      _em_ion_user_special_cuts_        = false;

      // muon:
      _em_muon_multiple_scattering_     = true;
      _em_muon_ionisation_              = true;
      _em_muon_bremsstrahlung_          = true;
      _em_muon_pair_production_         = true;
      _em_muon_step_limiter_            = false;
      _em_muon_user_special_cuts_       = false;

      return;
    }

    void em_physics_constructor::reset ()
    {
      DT_LOG_TRACE(_logprio(), "Entering...");

      DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");

      _set_initialized(false);

      _em_regions_deexcitation_.clear();
      _set_defaults ();

      this->base_physics_constructor::_reset();

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }

    void em_physics_constructor::tree_dump (std::ostream & out_,
                                            const std::string & title_,
                                            const std::string & indent_,
                                            bool inherit_) const
    {
      this->base_physics_constructor::tree_dump(out_, title_, indent_, true);
      std::string indent;
      if (! indent_.empty ()) indent = indent_;

      // EM model:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electro-magnetic model       : '" << _em_model_ << "'" << std::endl;

      // Gamma:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Gamma Rayleigh scattering    : "
           << (_em_gamma_rayleigh_scattering_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Gamma photo-electric         : "
           << (_em_gamma_photo_electric_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Gamma Compton scattering     : "
           << (_em_gamma_compton_scattering_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Gamma conversion             : "
           << (_em_gamma_conversion_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Gamma conversion to muons    : "
           << (_em_gamma_conversion_to_muons_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Gamma step limiter : "
           << (_em_gamma_step_limiter_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Gamma user special cuts : "
           << (_em_gamma_user_special_cuts_ ? "Yes" : "No") << std::endl;

      // Electron/positron:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electron/positron ionisation : "
           << (_em_electron_ionisation_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electron/positron  bremsstrahlung : "
           << (_em_electron_bremsstrahlung_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electron/positron multiple scattering : "
           << (_em_electron_multiple_scattering_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electron/positron multiple scattering uses distance to boundary : "
           << (_em_electron_ms_use_distance_to_boundary_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electron/positron multiple scattering range factor : "
           << (_em_electron_ms_range_factor_) << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Positron annihilation : "
           << (_em_positron_annihilation_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electron/positron step limiter : "
           << (_em_electron_step_limiter_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electron/positron user special cuts : "
           << (_em_electron_user_special_cuts_ ? "Yes" : "No") << std::endl;

      // Atomic deexcitation:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Atomic deexcitation : " << std::endl;
      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::tag
           << "Fluorescence : "
           << (_em_fluorescence_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::tag
           << "Auger : "
           << (_em_auger_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::tag
           << "PIXE : "
           << (_em_pixe_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::tag
           << "PIXE cross section model : "
           << "'" << _em_pixe_cross_section_model_ << "'" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::tag
           << "PIXE e+/e- cross section model : "
           << "'" << _em_pixe_electron_cross_section_model_ << "'" << std::endl;

      // Atomic deexcitation per region :
      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::last_tag
           << "Atomic deexcitation per region : " << std::endl;

      for (std::map<std::string, region_deexcitation_type>::const_iterator i
             = _em_regions_deexcitation_.begin();
           i != _em_regions_deexcitation_.end();
           i++) {
        out_ << indent << datatools::i_tree_dumpable::skip_tag
             << datatools::i_tree_dumpable::last_skip_tag;
        std::map<std::string, region_deexcitation_type>::const_iterator j = i;
        j++;
        if (j++ == _em_regions_deexcitation_.end()) {
          out_ << datatools::i_tree_dumpable::last_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
        }
        out_ << "Region '" << i->first << "' : "
             << "  fluorescence=" << (i->second.fluorescence? "Yes" : "No")
             << "  Auger=" << (i->second.auger? "Yes" : "No")
             << "  PIXE=" << (i->second.pixe? "Yes" : "No")
             << std::endl;
      }

      // Ion:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Ion ionisation : "
           << (_em_ion_ionisation_ ? "Yes" : "No") << std::endl;


      out_ << indent << datatools::i_tree_dumpable::tag
           << "Ion multiple scattering : "
           << (_em_ion_multiple_scattering_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Ion step limiter : "
           << (_em_ion_step_limiter_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Ion user special cuts : "
           << (_em_ion_user_special_cuts_ ? "Yes" : "No") << std::endl;

      // Muon:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Muon ionisation : "
           << (_em_muon_ionisation_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Muon bremsstrahlung : "
           << (_em_muon_bremsstrahlung_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Muon pair production : "
           << (_em_muon_pair_production_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Muon multiple scattering : "
           << (_em_muon_multiple_scattering_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Muon step limiter : "
           << (_em_muon_step_limiter_ ? "Yes" : "No") << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Muon user special cuts : "
           << (_em_muon_user_special_cuts_ ? "Yes" : "No") << std::endl;

      return;
    }

    void em_physics_constructor::ConstructParticle ()
    {
      DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");

      G4Gamma::GammaDefinition();
      G4Electron::ElectronDefinition();
      G4Positron::PositronDefinition();
      if (_em_gamma_conversion_to_muons_) {
        G4MuonPlus::MuonPlusDefinition();
        G4MuonMinus::MuonMinusDefinition();
      }

      return;
    }

    void em_physics_constructor::ConstructProcess ()
    {
      DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");

      this->em_physics_constructor::_ConstructEMProcess();

      if (_em_fluorescence_ || _em_auger_ || _em_pixe_ || _em_regions_deexcitation_.size() > 0) {
        DT_LOG_NOTICE(_logprio(), "Activating EM deexcitation...");
        this->em_physics_constructor::_ConstructEMDeexcitation();
      }

      return;
    }

    void em_physics_constructor::_ConstructEMDeexcitation ()
    {
      // First setup the static atomic deexcitation:
      // G4RadioactiveDecay* radioactiveDecay = new G4RadioactiveDecay();
      // radioactiveDecay->SetHLThreshold(nanosecond);
      // radioactiveDecay->SetICM(true); // Internal Conversion
      // radioactiveDecay->SetARM(true); // Atomic Rearangement

      G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
      G4LossTableManager::Instance()->SetAtomDeexcitation(de);

      // Activate deexcitation processes through the EM options:
      G4EmProcessOptions emOptions;
      int verbose = 1;
      emOptions.SetVerbose(verbose);
      /*
      emOptions.SetMinEnergy(10*eV);     //default=100eV
      emOptions.SetMaxEnergy(10*TeV);    //default=100TeV
      emOptions.SetDEDXBinning(12*10);   //default=12*7
      emOptions.SetLambdaBinning(12*10); //default=12*7
      emOptions.SetMscStepLimitation(fUseDistanceToBoundary); //default=fUseSafety
      emOptions.SetPolarAngleLimit(CLHEP::pi);
      */

      emOptions.SetFluo(_em_fluorescence_);
      if (_em_fluorescence_) {
        emOptions.SetAuger(_em_auger_);
      }
      emOptions.SetPIXE(_em_pixe_);
      if (_em_pixe_) {
        if (! _em_pixe_cross_section_model_.empty()) {
          emOptions.SetPIXECrossSectionModel(_em_pixe_cross_section_model_);
        }
      }

      // Activate deexcitation processes per region :
      for (std::map<std::string, region_deexcitation_type>::const_iterator i
             = _em_regions_deexcitation_.begin();
           i != _em_regions_deexcitation_.end();
           i++) {
        const std::string & region_name = i->first;
        const region_deexcitation_type & rd = i->second;
        G4Region * a_region = G4RegionStore::GetInstance ()->GetRegion (region_name);
        DT_THROW_IF(a_region == 0, std::logic_error,
                    "Cannot find region named '" << region_name
                    << "' to apply de-excitation processes !");
        DT_LOG_NOTICE(_logprio(), "Activating EM deexcitation for region '" << region_name << "' :"
                      << " Fluorescence=" << rd.fluorescence
                      << " Auger=" << rd.auger
                      << " PIXE=" << rd.pixe
                      << "..."
                      );
        emOptions.SetDeexcitationActiveRegion(region_name, rd.fluorescence, rd.auger, rd.pixe);
      }

     // de->SetFluo(true);
     //  //de->SetFluo(_em_fluorescence_);
     //  //de->SetFluo(true);
     // de->SetAuger(true);
     // de->SetPIXE(true);

      return;
    }

    void em_physics_constructor::_ConstructEMProcess ()
    {
      theParticleIterator->reset ();
      while ((*theParticleIterator) ()) {
        G4ParticleDefinition * particle = theParticleIterator->value ();
        G4ProcessManager     * pmanager = particle->GetProcessManager ();
        G4String particle_name = particle->GetParticleName ();

        DT_LOG_NOTICE (_logprio(), "Set electromagnetic processes for '"
                       << particle_name << "'");

        if (particle_name == "gamma") {
          /***********
           *  Gamma  *
           ***********/
          int process_rank = 0;

          G4RayleighScattering * the_rayleigh_scattering = 0;
          if (_em_gamma_rayleigh_scattering_) {
            the_rayleigh_scattering = new G4RayleighScattering();
          }
          G4PhotoElectricEffect * the_photoelectric_effect = 0;
          if (_em_gamma_photo_electric_) {
            the_photoelectric_effect = new G4PhotoElectricEffect();
          }
          G4ComptonScattering   * the_compton_scattering = 0;
          if (_em_gamma_compton_scattering_) {
            the_compton_scattering = new G4ComptonScattering();
          }
          G4GammaConversion     * the_gamma_conversion = 0;
          if (_em_gamma_conversion_) {
            the_gamma_conversion = new G4GammaConversion();
          }
          G4GammaConversionToMuons * the_gamma_conversion_to_muons = 0;
          if (_em_gamma_conversion_to_muons_) {
            the_gamma_conversion_to_muons = new G4GammaConversionToMuons();
          }

          if (is_em_low_energy_livermore ()) {
            // Livermore:

            if (the_rayleigh_scattering) {
              G4LivermoreRayleighModel * the_livermore_rayleigh_model
                = new G4LivermoreRayleighModel ();
              the_rayleigh_scattering->SetEmModel (the_livermore_rayleigh_model);
            }

            if (the_photoelectric_effect) {
              G4LivermorePhotoElectricModel* the_livermore_photoelectric_model
                = new G4LivermorePhotoElectricModel ();
              the_photoelectric_effect->SetEmModel (the_livermore_photoelectric_model);
            }

            if (the_compton_scattering) {
              G4LivermoreComptonModel* the_livermore_compton_model
                = new G4LivermoreComptonModel ();
              the_compton_scattering->SetEmModel (the_livermore_compton_model);
            }

            if (the_gamma_conversion) {
              G4LivermoreGammaConversionModel* the_livermore_gamma_conversion_model
                = new G4LivermoreGammaConversionModel ();
              the_gamma_conversion->SetEmModel (the_livermore_gamma_conversion_model);
            }

          } else if (is_em_low_energy_penelope ()) {
            // Penelope:

            if (the_rayleigh_scattering) {
              G4PenelopeRayleighModel * the_penelope_rayleigh_model
                = new G4PenelopeRayleighModel ();
              the_rayleigh_scattering->SetEmModel (the_penelope_rayleigh_model);
            }

            if (the_photoelectric_effect) {
              G4PenelopePhotoElectricModel* the_penelope_photoelectric_model
                = new G4PenelopePhotoElectricModel ();
              the_photoelectric_effect->SetEmModel (the_penelope_photoelectric_model);
            }

            if (the_compton_scattering) {
              G4PenelopeComptonModel* the_penelope_compton_model
                = new G4PenelopeComptonModel ();
              the_compton_scattering->SetEmModel (the_penelope_compton_model);
            }

            if (the_gamma_conversion) {
              G4PenelopeGammaConversionModel* the_penelope_gamma_conversion_model
                = new G4PenelopeGammaConversionModel ();
              the_gamma_conversion->SetEmModel (the_penelope_gamma_conversion_model);
            }

          }

          if (the_rayleigh_scattering) {
            pmanager->AddDiscreteProcess (the_rayleigh_scattering);
          }
          if (the_photoelectric_effect) {
            pmanager->AddDiscreteProcess (the_photoelectric_effect);
          }
          if (the_compton_scattering) {
            pmanager->AddDiscreteProcess (the_compton_scattering);
          }
          if (the_gamma_conversion) {
            pmanager->AddDiscreteProcess (the_gamma_conversion);
          }
          if (the_gamma_conversion_to_muons) {
            pmanager->AddDiscreteProcess (the_gamma_conversion_to_muons);
          }

          if (_em_gamma_step_limiter_) {
            ++process_rank;
            pmanager->AddProcess (new G4StepLimiter, -1, -1, process_rank);
          }

          //pmanager->AddProcess (new G4StepLimiter (), -1, -1, 4);

        } else if (particle_name == "e-" || particle_name == "e+") {
          /***********************
           *  electron/positron  *
           ***********************/
          int process_rank = 0;

          if (_em_electron_multiple_scattering_) {
            // Multiple scattering:
            G4eMultipleScattering * the_electron_multiple_scattering = new G4eMultipleScattering ();
            // Setting the FacRange to 0.005 instead of default value 0.2
            the_electron_multiple_scattering->SetRangeFactor(_em_electron_ms_range_factor_);
            //??? the_electron_multiple_scattering->SetStepLimitType (fUseDistanceToBoundary);
            ++process_rank;
            pmanager->AddProcess(the_electron_multiple_scattering, -1, process_rank, process_rank);
          }

          if (_em_electron_ionisation_) {
            // Ionisation:
            G4eIonisation * the_e_ionisation = new G4eIonisation ();
            if (is_em_low_energy_livermore ()) {
              // Livermore:
              if (particle_name == "e-"){
                G4LivermoreIonisationModel* the_livermore_ionisation_model
                  = new G4LivermoreIonisationModel ();
                the_e_ionisation->SetEmModel(the_livermore_ionisation_model);
              } else if (particle_name == "e+"){
                G4PenelopeIonisationModel* the_penelope_ionisation_model
                  = new G4PenelopeIonisationModel ();
                the_e_ionisation->SetEmModel(the_penelope_ionisation_model);
              }
            } else if (is_em_low_energy_penelope ()) {
              // Penelope:
              G4PenelopeIonisationModel* the_penelope_ionisation_model
                = new G4PenelopeIonisationModel ();
              the_e_ionisation->SetEmModel(the_penelope_ionisation_model);
            }
            ++process_rank;
            pmanager->AddProcess (the_e_ionisation, -1, process_rank, process_rank);
          }

          if (_em_electron_bremsstrahlung_) {
            // Bremsstrahlung:
            G4eBremsstrahlung * the_e_bremsstrahlung = new G4eBremsstrahlung ();
            if (is_em_low_energy_livermore ()) {
              // Livermore:
              if (particle_name == "e-"){
                G4LivermoreBremsstrahlungModel * the_livermore_bremsstrahlung_model
                  = new G4LivermoreBremsstrahlungModel ();
                the_e_bremsstrahlung->SetEmModel(the_livermore_bremsstrahlung_model);
              } else if (particle_name == "e+"){
                G4PenelopeBremsstrahlungModel* the_penelope_bremsstrahlung_model
                  = new G4PenelopeBremsstrahlungModel ();
                the_e_bremsstrahlung->SetEmModel(the_penelope_bremsstrahlung_model);
              }
            } else if (is_em_low_energy_penelope ()) {
              // Penelope:
              G4PenelopeBremsstrahlungModel* the_penelope_bremsstrahlung_model
                = new G4PenelopeBremsstrahlungModel ();
              the_e_bremsstrahlung->SetEmModel(the_penelope_bremsstrahlung_model);
            }
            ++process_rank;
            pmanager->AddProcess (the_e_bremsstrahlung, -1, process_rank, process_rank);
          }

          if (particle_name == "e+" && _em_positron_annihilation_) {
            // e+ annihilation:
            G4eplusAnnihilation * the_positron_annihilation = new G4eplusAnnihilation ();
            if (is_em_low_energy_penelope ()) {
              G4PenelopeAnnihilationModel * the_penelope_annihilation_model
                = new G4PenelopeAnnihilationModel ();
              the_positron_annihilation->SetEmModel(the_penelope_annihilation_model);
            }
            ++process_rank;
            pmanager->AddProcess(the_positron_annihilation, 0, -1, process_rank);
          }

          if (_em_electron_step_limiter_) {
            ++process_rank;
            pmanager->AddProcess (new G4StepLimiter, -1, -1, process_rank);
          }

        } else if (   particle_name == "alpha"    || particle_name == "anti_alpha"
                      || particle_name == "deuteron" || particle_name == "anti_deuteron"
                      || particle_name == "triton"   || particle_name == "anti_triton"
                      || particle_name == "He3"      || particle_name == "anti_He3"
                      || particle_name == "GenericIon") {
          int process_rank = 0;
          /************
           *   Ions   *
           ************/
          if (_em_ion_multiple_scattering_) {
            G4hMultipleScattering * the_ion_multiple_scattering = new G4hMultipleScattering();
            ++process_rank;
            pmanager->AddProcess (the_ion_multiple_scattering , -1, process_rank, process_rank);
          }

          if (_em_ion_ionisation_) {
            G4ionIonisation       * the_ion_ionisation = new G4ionIonisation();
            ++process_rank;
            pmanager->AddProcess (the_ion_ionisation, -1, process_rank, process_rank);
          }

          if (_em_ion_step_limiter_) {
            ++process_rank;
            pmanager->AddProcess (new G4StepLimiter, -1, -1, process_rank);
          }

        } else if (particle_name == "mu+" ||
                   particle_name == "mu-") {
          int process_rank = 0;
          /*************
           *   Muons   *
           *************/
          ++process_rank;
          pmanager->AddProcess (new G4MuMultipleScattering, -1, process_rank, process_rank);
          ++process_rank;
          pmanager->AddProcess (new G4MuIonisation,         -1, process_rank, process_rank);
          ++process_rank;
          pmanager->AddProcess (new G4MuBremsstrahlung,     -1, process_rank, process_rank);
          ++process_rank;
          pmanager->AddProcess (new G4MuPairProduction,     -1, process_rank, process_rank);

          if (_em_muon_step_limiter_) {
            ++process_rank;
            pmanager->AddProcess (new G4StepLimiter, -1, -1, process_rank);
          }
          // ++process_rank;
          // pmanager->AddProcess (new G4UserSpecialCuts,      -1, -1, process_rank);
        } else {
          int process_rank = 0;
          /***********************
           *   other particles   *
           ***********************/
          // all others charged particles
          if ((!particle->IsShortLived ()) &&
              (particle->GetPDGCharge () != 0.0)) {
            ++process_rank;
            pmanager->AddProcess (new G4hMultipleScattering, -1, process_rank, process_rank);
            ++process_rank;
            pmanager->AddProcess (new G4hIonisation,         -1, process_rank, process_rank);
            //      ++process_rank;
            //      pmanager->AddProcess(new G4StepLimiter,       -1,-1, process_rank);
            //      ++process_rank;
            //      pmanager->AddProcess(new G4UserSpecialCuts,   -1,-1, process_rank);
          }
        }
      }

      return;
    }

  } // end of namespace g4

} // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::em_physics_constructor, ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::em_physics_constructor");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation manager embedded electro-magnetic physics list");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The Geant4 simulation manager class embedded \n"
                                "electromagnetic physics list.                \n"
                                );


  logger::declare_ocd_logging_configuration(ocd_, "warning");

  {
    // Description of the 'em.model' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.model")
      .set_terse_description("Name of the EM model")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("standard")
      .set_long_description("Default: ``\"standard\"``                              \n"
                            "                                                       \n"
                            "Allowed values:                                        \n"
                            "                                                       \n"
                            " * ``standard``  : Standard EM processes               \n"
                            " * ``livermore`` : Livermore low-energy model          \n"
                            " * ``penelope``  : Penelope low-energy model           \n"
                            )
      .add_example("Use the *Livermore* EM model: ::   \n"
                   "                                   \n"
                   " em.model : string = \"livermore\" \n"
                   "                                   \n"
                   )
      ;
  }

  {
    // Description of the 'em.gamma.rayleigh_scattering' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.rayleigh_scattering")
      .set_terse_description("Flag to activate the gamma Rayleigh scattering")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate Rayleigh scattering : ::           \n"
                   "                                            \n"
                   " em.gamma.rayleigh_scattering : boolean = 1 \n"
                   "                                            \n"
                   )
      ;
  }

  {
    // Description of the 'em.gamma.photo_electric' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.photo_electric")
      .set_terse_description("Flag to activate the gamma photo-electric effect")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate photoelectric effect : ::     \n"
                   "                                       \n"
                   " em.gamma.photo_electric : boolean = 1 \n"
                   "                                       \n"
                   )
      ;
  }

  {
    // Description of the 'em.gamma.compton_scattering' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.compton_scattering")
      .set_terse_description("Flag to activate the gamma Compton scattering")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate Compton scattering : ::           \n"
                   "                                           \n"
                   " em.gamma.compton_scattering : boolean = 1 \n"
                   "                                           \n"
                   )
      ;
  }

  {
    // Description of the 'em.gamma.conversion' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.conversion")
      .set_terse_description("Flag to activate the gamma conversion")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate gamma conversion : ::     \n"
                   "                                   \n"
                   " em.gamma.conversion : boolean = 1 \n"
                   "                                   \n"
                   )
      ;
  }

  {
    // Description of the 'em.gamma.conversion_to_muons' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.gamma.conversion_to_muons")
      .set_terse_description("Flag to activate the gamma conversion to muons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate gamma conversion to muons : ::     \n"
                   "                                            \n"
                   " em.gamma.conversion_to_muons : boolean = 1 \n"
                   "                                            \n"
                   )
      ;
  }


  {
    // Description of the 'em.electron.ionisation' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.ionisation")
      .set_terse_description("Flag to activate the electron/positron ionisation")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate electron/positron ionisation : :: \n"
                   "                                           \n"
                   " em.electron.ionisation : boolean = 1      \n"
                   "                                           \n"
                   )
      ;
  }

  {
    // Description of the 'em.electron.bremsstrahlung' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.bremsstrahlung")
      .set_terse_description("Flag to activate the bremsstrahlung effect for electron/positron")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate electron/positron bremsstrahlung : :: \n"
                   "                                               \n"
                   " em.electron.bremsstrahlung : boolean = 1      \n"
                   "                                               \n"
                   )
      ;
  }

  {
    // Description of the 'em.electron.multiple_scattering' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.multiple_scattering")
      .set_terse_description("Flag to activate the electron/positron multiple scattering")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate electron/positron multiple scattering : :: \n"
                   "                                                    \n"
                   " em.electron.multiple_scattering : boolean = 1      \n"
                   "                                                    \n"
                   )
      ;
  }

  {
    // Description of the 'em.electron.multiple_scattering.use_distance_to_boundary' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.multiple_scattering.use_distance_to_boundary")
      .set_terse_description("Flag to use distance to boundary in the electron/positron multiple scattering")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Use the distance to boundary algorithm for electron/positron multiple scattering : :: \n"
                   "                                                                         \n"
                   " em.electron.multiple_scattering.use_distance_to_boundary : boolean = 1  \n"
                   "                                                                         \n"
                   )
      ;
  }

  {
    // Description of the 'em.electron.multiple_scattering.range_factor' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.multiple_scattering.range_factor")
      .set_terse_description("The range factor in the electron/positron multiple scattering")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_default_value_real(0.005)
      .add_example("Set the electron/positron multiple scattering range factor : :: \n"
                   "                                                                \n"
                   " em.electron.multiple_scattering.range_factor : real = 0.005    \n"
                   "                                                                \n"
                   )
      ;
  }

  {
    // Description of the 'em.electron.step_limiter' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.electron.step_limiter")
      .set_terse_description("Flag to activate the electron/positron step limiter")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Set the electron/positron step limiter technique : ::  \n"
                   "                                                       \n"
                   " em.electron.step_limiter : boolean = 1                \n"
                   "                                                       \n"
                   )
      ;
  }

  {
    // Description of the 'em.positron.annihilation' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.positron.annihilation")
      .set_terse_description("Flag to activate the positron annihilation")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(true)
      .add_example("Activate positron annihilation : ::     \n"
                   "                                        \n"
                   " em.positron.annihilation : boolean = 1 \n"
                   "                                        \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.fluorescence' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.fluorescence")
      .set_terse_description("Flag to activate the atomic deexcitation and fluorescence")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate deexcitation by fluorescence : ::  \n"
                   "                                            \n"
                   " em.deexcitation.fluorescence : boolean = 1 \n"
                   "                                            \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.auger' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.auger")
      .set_terse_description("Flag to activate Auger process")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description(
                            "Auger effect is only activated if ``em.deexcitation.fluorescence``\n"
                            "is also activated.                                                \n"
                            )
      .add_example("Activate deexcitation by Auger effect : :: \n"
                   "                                           \n"
                   " em.deexcitation.auger : boolean = 1       \n"
                   "                                           \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.pixe' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.pixe")
      .set_terse_description("Flag to activate PIXE process (Particle Induced X-ray Emission)")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate deexcitation by PIXE process : :: \n"
                   "                                           \n"
                   " em.deexcitation.pixe : boolean = 1        \n"
                   "                                           \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.pixe.cross_section_model' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.pixe.cross_section_model")
      .set_terse_description("The name of the PIXE cross-section model")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("empirical")
      .set_long_description("Allowed values are:                                    \n"
                            "                                                       \n"
                            " * ``\"empirical\"``                                   \n"
                            " * ``\"ecpssr_formfactor\"``                           \n"
                            " * ``\"ecpssr_analytical\"``                           \n"
                            )
      .add_example("Use the *empirical* PIXE model : :: \n"
                   "                                                       \n"
                   " em.deexcitation.pixe.cross_section_model : string = \"empirical\"  \n"
                   "                                                       \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.pixe.cross_section_model' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.pixe.electron_cross_section_model")
      .set_terse_description("The name of the PIXE e+/e- cross-section model")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("empirical")
      .set_long_description("Allowed values are:                                    \n"
                            "                                                       \n"
                            " * ``\"livermore\"``                                   \n"
                            " * ``\"proton_analytical\"``                           \n"
                            " * ``\"proton_empirical\"``                            \n"
                            " * ``\"penelope\"``                                    \n"
                            )
      .add_example("Use the *empirical* PIXE model : :: \n"
                   "                                                       \n"
                   " em.deexcitation.pixe.electron_cross_section_model : string = \"livermore\"  \n"
                   "                                                       \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.regions' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.regions")
      .set_terse_description("The name of the regions with special parameters for deexcitation processes")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .add_example("Define two regions with activated deexcitation: :: \n"
                   "                                                   \n"
                   " em.deexcitation.regions : string[2] = \"A\" \"B\" \n"
                   "                                                   \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.regions.XXX.fluorescence' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.regions.${em.deexcitation.regions}.fluorescence")
      .set_terse_description("Flag to activate fluorecence in some given region")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate fluorescence in two *deexcitation* regions: :: \n"
                   "                                                        \n"
                   " em.deexcitation.regions : string[2] = \"A\" \"B\"      \n"
                   " em.deexcitation.regions.A.fluorescence : boolean = 1   \n"
                   " em.deexcitation.regions.B.fluorescence : boolean = 1   \n"
                   "                                                        \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.regions.XXX.auger' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.regions.${em.deexcitation.regions}.auger")
      .set_terse_description("Flag to activate Auger effect in some given region")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate Auger effect in two *deexcitation* regions: :: \n"
                   "                                                        \n"
                   " em.deexcitation.regions : string[2] = \"A\" \"B\"      \n"
                   " em.deexcitation.regions.A.auger : boolean = 1          \n"
                   " em.deexcitation.regions.B.auger : boolean = 1          \n"
                   "                                                        \n"
                   )
      ;
  }

  {
    // Description of the 'em.deexcitation.regions.XXX.pixe' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("em.deexcitation.regions.${em.deexcitation.regions}.pixe")
      .set_terse_description("Flag to activate PIXE effect in some given region")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate PIXE effect in two *deexcitation* regions: :: \n"
                   "                                                       \n"
                   " em.deexcitation.regions : string[2] = \"A\" \"B\"     \n"
                   " em.deexcitation.regions.A.pixe : boolean = 1          \n"
                   " em.deexcitation.regions.B.pixe : boolean = 1          \n"
                   "                                                       \n"
                   )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Not available yet                    \n"
                               );

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::em_physics_constructor' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::em_physics_constructor,"mctools::g4::em_physics_constructor")
