// -*- mode: c++ ; -*-
/* physics_list.cc
 */

/*
 *
 *
 * https://twiki.cern.ch/twiki/bin/view/Geant4/LoweMigratedLivermore
 *
 *
 */

#include <mctools/g4/physics_list.h>

#include <stdexcept>

#include <datatools/units.h>
#include <datatools/ioutils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/exception.h>

// G4 stuff :
#include <globals.hh>
#include <G4Version.hh>
#include <G4UnitsTable.hh>
#include <G4ProcessManager.hh>

// Particles:
#include <G4ParticleDefinition.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleTable.hh>

// Bosons:
//#include <G4BosonConstructor.hh>
#include <G4ChargedGeantino.hh>
#include <G4Geantino.hh>
#include <G4Gamma.hh>
#include <G4OpticalPhoton.hh>

// Leptons
//#include <G4LeptonConstructor.hh>
#include <G4MuonPlus.hh>
#include <G4MuonMinus.hh>
#include <G4TauMinus.hh>
#include <G4TauPlus.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4NeutrinoTau.hh>
#include <G4AntiNeutrinoTau.hh>
#include <G4NeutrinoMu.hh>
#include <G4AntiNeutrinoMu.hh>
#include <G4NeutrinoE.hh>
#include <G4AntiNeutrinoE.hh>

// Mesons:
//#include <G4MesonConstructor.hh>
#include <G4PionPlus.hh>
#include <G4PionMinus.hh>
#include <G4PionZero.hh>
#include <G4Eta.hh>
#include <G4EtaPrime.hh>

#include <G4KaonPlus.hh>
#include <G4KaonMinus.hh>
#include <G4KaonZero.hh>
#include <G4AntiKaonZero.hh>
#include <G4KaonZeroLong.hh>
#include <G4KaonZeroShort.hh>

#include <G4DMesonPlus.hh>
#include <G4DMesonMinus.hh>
#include <G4DMesonZero.hh>
#include <G4AntiDMesonZero.hh>
#include <G4DsMesonPlus.hh>
#include <G4DsMesonMinus.hh>
#if G4VERSION_NUMBER == 960
#include <G4Etac.hh>
#endif // G4VERSION_NUMBER == 960
#include <G4JPsi.hh>

#include <G4BMesonPlus.hh>
#include <G4BMesonMinus.hh>
#include <G4BMesonZero.hh>
#include <G4AntiBMesonZero.hh>
#include <G4BsMesonZero.hh>
#include <G4AntiBsMesonZero.hh>
#if G4VERSION_NUMBER == 960
#include <G4BcMesonPlus.hh>
#include <G4BcMesonMinus.hh>
#include <G4Upsiron.hh>
#endif // G4VERSION_NUMBER == 960

// Baryons:
//#include <G4BaryonConstructor.hh>
#include <G4Proton.hh>
#include <G4AntiProton.hh>
#include <G4Neutron.hh>
#include <G4AntiNeutron.hh>

#include <G4Lambda.hh>
#include <G4SigmaPlus.hh>
#include <G4SigmaZero.hh>
#include <G4SigmaMinus.hh>
#include <G4XiMinus.hh>
#include <G4XiZero.hh>
#include <G4OmegaMinus.hh>

#include <G4AntiLambda.hh>
#include <G4AntiSigmaPlus.hh>
#include <G4AntiSigmaZero.hh>
#include <G4AntiSigmaMinus.hh>
#include <G4AntiXiMinus.hh>
#include <G4AntiXiZero.hh>
#include <G4AntiOmegaMinus.hh>

#include <G4LambdacPlus.hh>
#include <G4SigmacPlusPlus.hh>
#include <G4SigmacPlus.hh>
#include <G4SigmacZero.hh>
#include <G4XicPlus.hh>
#include <G4XicZero.hh>
#include <G4OmegacZero.hh>

#include <G4AntiLambdacPlus.hh>
#include <G4AntiSigmacPlusPlus.hh>
#include <G4AntiSigmacPlus.hh>
#include <G4AntiSigmacZero.hh>
#include <G4AntiXicPlus.hh>
#include <G4AntiXicZero.hh>
#include <G4AntiOmegacZero.hh>

#if G4VERSION_NUMBER == 960
#include <G4Lambdab.hh>
#include <G4SigmabPlus.hh>
#include <G4SigmabZero.hh>
#include <G4SigmabMinus.hh>
#include <G4XibZero.hh>
#include <G4XibMinus.hh>
#include <G4OmegabMinus.hh>
#endif // G4VERSION_NUMBER == 960

#if G4VERSION_NUMBER == 960
#include <G4AntiLambdab.hh>
#include <G4AntiSigmabPlus.hh>
#include <G4AntiSigmabZero.hh>
#include <G4AntiSigmabMinus.hh>
#include <G4AntiXibZero.hh>
#include <G4AntiXibMinus.hh>
#include <G4AntiOmegabMinus.hh>
#endif // G4VERSION_NUMBER == 960


// Ions:
//#include <G4IonConstructor.hh>
// Nuclei
#include <G4Alpha.hh>
#include <G4Deuteron.hh>
#include <G4Triton.hh>
#include <G4He3.hh>
#include <G4GenericIon.hh>
// AntiNuclei
#include <G4AntiAlpha.hh>
#include <G4AntiDeuteron.hh>
#include <G4AntiTriton.hh>
#include <G4AntiHe3.hh>

// EM processes :

#include <G4EmProcessOptions.hh>
#include <G4ComptonScattering.hh>
#include <G4RayleighScattering.hh>
#include <G4GammaConversion.hh>
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


/* **** Hadronic processes **** */

//
// Elastic processes:
#include <G4HadronElasticProcess.hh>

// Inelastic processes:
#include <G4ProtonInelasticProcess.hh>
#include <G4AntiProtonInelasticProcess.hh>
#include <G4NeutronInelasticProcess.hh>
#include <G4AntiNeutronInelasticProcess.hh>
#include <G4PionPlusInelasticProcess.hh>
#include <G4PionMinusInelasticProcess.hh>
#include <G4KaonPlusInelasticProcess.hh>
#include <G4KaonMinusInelasticProcess.hh>
#include <G4KaonZeroSInelasticProcess.hh>
#include <G4KaonZeroLInelasticProcess.hh>
#include <G4DeuteronInelasticProcess.hh>
#include <G4TritonInelasticProcess.hh>
#include <G4AlphaInelasticProcess.hh>
#include <G4HadronCaptureProcess.hh>

// Low-energy Models: < 20GeV
#include <G4LElastic.hh>
#include <G4LEProtonInelastic.hh>
#include <G4LEAntiProtonInelastic.hh>
#include <G4LENeutronInelastic.hh>
#include <G4LEAntiNeutronInelastic.hh>
#include <G4LEPionPlusInelastic.hh>
#include <G4LEPionMinusInelastic.hh>
#include <G4LEKaonPlusInelastic.hh>
#include <G4LEKaonMinusInelastic.hh>
#include <G4LEKaonZeroSInelastic.hh>
#include <G4LEKaonZeroLInelastic.hh>
#include <G4LEDeuteronInelastic.hh>
#include <G4LETritonInelastic.hh>
#include <G4LEAlphaInelastic.hh>
// High-energy Models: >20 GeV
#include <G4HEProtonInelastic.hh>
#include <G4HEAntiProtonInelastic.hh>
#include <G4HEPionPlusInelastic.hh>
#include <G4HEPionMinusInelastic.hh>
#include <G4HEKaonPlusInelastic.hh>
#include <G4HEKaonMinusInelastic.hh>
#include <G4HEKaonZeroInelastic.hh>
#include <G4HEAntiNeutronInelastic.hh>

// Neutron high-precision models: <20 MeV
#include <G4NeutronHPElastic.hh>
#include <G4NeutronHPElasticData.hh>
#include <G4NeutronHPInelastic.hh>
#include <G4NeutronHPInelasticData.hh>
#include <G4NeutronHPCapture.hh>
#include <G4NeutronHPCaptureData.hh>
#include <G4LCapture.hh>

// Stopping processes:
#include <G4PiMinusAbsorptionAtRest.hh>
#include <G4KaonMinusAbsorptionAtRest.hh>

#include <G4Decay.hh>

#include <G4UserSpecialCuts.hh>
#include <G4StepLimiter.hh>

#include <G4ProductionCuts.hh>
#include <G4RegionStore.hh>

namespace mctools {

  namespace g4 {

    using namespace std;

    const std::string physics_list::EM_MODEL_STANDARD   = "standard";
    const std::string physics_list::EM_MODEL_LOW_ENERGY_LIVERMORE = "livermore";
    const std::string physics_list::EM_MODEL_LOW_ENERGY_PENELOPE  = "penelope";

    const std::string & physics_list::get_em_model () const
    {
      return _em_model_;
    }

    bool physics_list::is_em_standard () const
    {
      return _em_model_ == EM_MODEL_STANDARD;
    }

    bool physics_list::is_em_low_energy_livermore () const
    {
      return _em_model_ == EM_MODEL_LOW_ENERGY_PENELOPE;
    }

    bool physics_list::is_em_low_energy_penelope () const
    {
      return _em_model_ == EM_MODEL_LOW_ENERGY_LIVERMORE;
    }

    // ctor:
    physics_list::physics_list (bool logging_) : G4VUserPhysicsList ()
    {
      _initialized_ = false;
      _logging_ = logging_;
      this->_set_defaults ();
      return;
    }

    // dtor:
    physics_list::~physics_list ()
    {
      return;
    }

    void physics_list::initialize (const datatools::properties & config_)
    {
      DT_LOG_TRACE(_logprio(), "Entering...");
      DT_THROW_IF (_initialized_, logic_error, "Already initialized !");

      // *********************** Processes activation *************************** //

      if (config_.has_key ("em.model")) {
        _em_model_ = config_.fetch_string ("em.model");
      }
      DT_THROW_IF (_em_model_ != EM_MODEL_STANDARD
                   && _em_model_ != EM_MODEL_LOW_ENERGY_LIVERMORE
                   && _em_model_ != EM_MODEL_LOW_ENERGY_PENELOPE,
                   std::logic_error,
                   "Invalid electro-magnetic interaction model '" << _em_model_ << "' !");

      DT_LOG_DEBUG(_logprio(),"Electro-magnetic model set to: '" << _em_model_ << "'");

      if (config_.has_key ("electron.energy_loss")) {
        _electron_energy_loss_ = config_.fetch_boolean ("electron.energy_loss");
      }
      DT_LOG_DEBUG(_logprio(), "Electrons energy Loss set to: "
                   <<  _electron_energy_loss_);

      if (config_.has_key ("electron.multiple_scattering")) {
        _electron_multiple_scattering_ = config_.fetch_boolean ("electron.multiple_scattering");
      }
      DT_LOG_DEBUG(_logprio(),"Electrons multiple scattering set to: "
                   << _electron_multiple_scattering_);

      if (config_.has_key ("bremsstrahlung")) {
        _bremsstrahlung_ = config_.fetch_boolean ("bremsstrahlung");
      }
      DT_LOG_DEBUG(_logprio(),"Bremsstrahlung set to: " <<  _bremsstrahlung_);


      /* **** deexcitation **** */

      if (config_.has_flag ("em.deexcitation.fluo")) {
        _em_fluo_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"EM fluroescence set to : " << _em_fluo_);

      if (_em_fluo_) {
        if (config_.has_flag ("em.deexcitation.auger")) {
          _em_auger_ = true;
        }
        DT_LOG_DEBUG(_logprio(),"EM Auger set to : " << _em_auger_);
      }

      if (config_.has_flag ("em.deexcitation.pixe")) {
        _em_pixe_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"EM PIXE set to : " << _em_pixe_);

      if (config_.has_key ("em.deexcitation.pixe.model")) {
        _em_pixe_cross_section_model_ = config_.fetch_string ("em.deexcitation.pixe.model");
        DT_THROW_IF (_em_pixe_cross_section_model_ != EM_PIXE_MODEL_EMPIRICAL
                     && _em_pixe_cross_section_model_ != EM_PIXE_MODEL_ECPSSR_FORMFACTOR
                     && _em_pixe_cross_section_model_ != EM_PIXE_MODEL_ECPSSR_ANALYTICAL,
                     std::logic_error,
                     "Invalid EM PIXE cross-section model '" << _em_pixe_cross_section_model_ << "' !");
        DT_LOG_DEBUG(_logprio(),"EM PIXE cross-section model set to : " << _em_pixe_cross_section_model_);
      }

      /* **** deexcitation per region **** */
      std::vector<std::string> deexcitation_regions;
      if (config_.has_key ("em.deexcitation.regions")) {
        config_.fetch("em.deexcitation.regions", deexcitation_regions);
      }

      for (int i  = 0; i < deexcitation_regions.size(); i++) {
        const std::string & region_name = deexcitation_regions[i];
        region_deexcitation_type rd(false,false,false);
        if (_em_fluo_) {
          std::ostringstream region_fluo_key;
          region_fluo_key << "em.deexcitation.regions." << region_name << ".fluo";
          if (config_.has_flag (region_fluo_key.str())) {
            rd.fluo = true;
          }
          if (rd.fluo) {
            std::ostringstream region_auger_key;
            region_auger_key << "em.deexcitation.regions." << region_name << ".auger";
            if (config_.has_flag (region_auger_key.str())) {
              rd.auger = true;
            }
          }
          std::ostringstream region_pixe_key;
          region_pixe_key << "em.deexcitation.regions." << region_name << ".pixe";
          if (config_.has_flag (region_pixe_key.str())) {
            rd.pixe = true;
          }
        }
        _em_regions_deexcitation_[region_name] = rd;
      }


      // *********************** Particles *************************** //

      // Bosons:
      if (config_.has_flag ("use_geantinos")) {
        _use_geantinos_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use geantinos set to: " << _use_geantinos_);

      if (config_.has_flag ("use_optical_photons")) {
        _use_optical_photons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use optical photons set to: " << _use_optical_photons_);

      // Leptons:
      if (config_.has_flag ("use_muon_leptons")) {
        _use_muon_leptons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use muon leptons set to: " << _use_muon_leptons_);

      if (config_.has_flag ("use_tau_leptons")) {
        _use_tau_leptons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use tau leptons set to: " << _use_tau_leptons_);


      // Mesons:
      if (config_.has_flag ("use_light_mesons")) {
        _use_light_mesons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use light mesons set to: " << _use_light_mesons_);

      if (config_.has_flag ("use_charm_mesons")) {
        _use_charm_mesons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use charm mesons set to: " << _use_charm_mesons_);

      if (config_.has_flag ("use_bottom_mesons")) {
        _use_bottom_mesons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use bottom mesons set to: " << _use_bottom_mesons_);

      // Baryons :
      if (config_.has_flag ("use_nucleons")) {
        _use_nucleons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use nucleons set to: " << _use_nucleons_);

      if (config_.has_flag ("use_strange_baryons")) {
        _use_strange_baryons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use strange baryons set to: " << _use_strange_baryons_);

      if (config_.has_flag ("use_charm_baryons")) {
        _use_charm_baryons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use charm baryons set to: " << _use_charm_baryons_);

      if (config_.has_flag ("use_bottom_baryons")) {
        _use_bottom_baryons_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use bottom baryons set to: " << _use_bottom_baryons_);

      // Nuclei:
      if (config_.has_flag ("use_light_nuclei")) {
        _use_light_nuclei_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use light nuclei set to: " << _use_light_nuclei_);

      if (config_.has_flag ("use_light_anti_nuclei")) {
        _use_light_anti_nuclei_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use light anti-nuclei set to: " << _use_light_anti_nuclei_);

      if (config_.has_flag ("use_generic_ion")) {
        _use_generic_ion_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use generic ion set to: " << _use_generic_ion_);

      // *********************** Cuts *************************** //
      if (config_.has_flag ("using_cuts")) {
        _using_cuts_ = true;
      }
      DT_LOG_DEBUG(_logprio(),"Use cuts set to: " << _using_cuts_);

      if (_using_cuts_) {
        // Production cuts:
        std::vector<std::string> regions;
        double lunit = CLHEP::mm;
        if (config_.has_key ("production_cuts.regions")) {
          config_.fetch ("production_cuts.regions", regions);

          if (config_.has_key ("production_cuts.length_unit")) {
            string lunit_str = config_.fetch_string ("production_cuts.length_unit");
            lunit = datatools::units::get_length_unit_from (lunit_str);
          }

          for (int i = 0; i < (int) regions.size (); i++) {
            const string & region_label = regions[i];
            ostringstream key_ss;
            key_ss << "production_cuts." << region_label;
            std::string key = key_ss.str();
            DT_THROW_IF (! config_.has_key(key),
                         logic_error,
                         "Missing production cuts for '"
                         << key << "' !");
            double production_cut = config_.fetch_real (key);
            if (! config_.has_explicit_unit (key)) {
              production_cut *= lunit;
            }
            _region_cuts_[region_label] = production_cut;
          }
        }

        DT_LOG_NOTICE(_logprio(), "Production cuts for regions : ");

        for (map<string, double>::const_iterator i = _region_cuts_.begin ();
             i != _region_cuts_.end ();
             i++) {
          map<string, double>::const_iterator j = i;
          j++;
          if (j == _region_cuts_.end ()) {
            clog << "`-- ";
          } else {
            clog << "|-- ";
          }
          clog << "Region '" << i->first << "'  :  Production cuts = "
               << i->second/ CLHEP::mm << " mm" << endl;
        }

        // Electron cut :
        if (config_.has_key ("electron_cut")) {
          _electron_cut_ = config_.fetch_real ("electron_cut");
          if (! config_.has_explicit_unit("electron_cut")) _electron_cut_ *= lunit;
        }
        DT_LOG_NOTICE(_logprio(), "Electron cut set to: " << _electron_cut_ / CLHEP::mm << " mm");

        // Positron cut :
        if (config_.has_key ("positron_cut")) {
          _positron_cut_ = config_.fetch_real ("positron_cut");
          if (! config_.has_explicit_unit("positron_cut")) _positron_cut_ *= lunit;
        }
        DT_LOG_NOTICE(_logprio(), "Positron cut set to: " << _positron_cut_ / CLHEP::mm << " mm");

        // Gamma cut :
        if (config_.has_key ("gamma_cut")) {
          _gamma_cut_ = config_.fetch_real ("gamma_cut");
          if (! config_.has_explicit_unit("gamma_cut")) _gamma_cut_ *= lunit;
        }
        DT_LOG_NOTICE(_logprio(), "Gamma cut set to: " << _gamma_cut_ / CLHEP::mm << " mm");

        // Proton cut :
        if (config_.has_key ("proton_cut")) {
          _proton_cut_ = config_.fetch_real ("proton_cut");
          if (! config_.has_explicit_unit("proton_cut")) _proton_cut_ *= lunit;
        }
        DT_LOG_NOTICE(_logprio(), "Proton cut set to: " << _proton_cut_ / CLHEP::mm << " mm");
      }

      SetVerboseLevel (_g4_verbosity_);
      clog << datatools::io::notice
           << "mctools::g4::physics_list::initialize: "
           << "Original default cut Length : " << G4BestUnit (defaultCutValue, "Length") << endl;
      SetDefaultCutValue (_default_cut_value_);
      DT_LOG_NOTICE(_logprio(), "Default cut Length : " << G4BestUnit (defaultCutValue, "Length"));

      // Initialize deexcitation :
      _initialize_deexcitation();

      _initialized_ = true;

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }

    void physics_list::_set_defaults ()
    {
      _g4_verbosity_ = VERBOSITY_SILENT;

      _em_model_                     = EM_MODEL_STANDARD;
      _electron_energy_loss_         = true;
      _electron_multiple_scattering_ = true;
      // _msc_use_distance_to_boundary_ = true;
      _bremsstrahlung_               = true;
      _em_fluo_                      = false;
      _em_auger_                     = false;
      _em_pixe_                      = false;
      _em_pixe_cross_section_model_.clear();

      // Bosons:
      _use_geantinos_                = false;
      _use_optical_photons_          = false;

      // Leptons:
      _use_muon_leptons_             = false;
      _use_tau_leptons_              = false;

      // Mesons:
      _use_light_mesons_             = false;
      _use_charm_mesons_             = false;
      _use_bottom_mesons_            = false;

      // Baryons:
      _use_nucleons_                 = false;
      _use_strange_baryons_          = false;
      _use_charm_baryons_            = false;
      _use_bottom_baryons_           = false;

      // Ions:
      _use_light_nuclei_             = false;
      _use_light_anti_nuclei_        = false;
      _use_generic_ion_              = false;

      // Cuts:
      _using_cuts_        = false;
      _default_cut_value_ = 1.0 * CLHEP::micrometer;
      datatools::invalidate(_cuts_min_energy_); //250*eV
      datatools::invalidate(_cuts_max_energy_); //1*GeV
      _electron_cut_ = 1.0 * CLHEP::mm;
      _positron_cut_ = 1.0 * CLHEP::mm;
      _gamma_cut_    = 1.0 * CLHEP::mm;
      _proton_cut_   = 1.0 * CLHEP::mm;

      return;
    }


    void physics_list::ConstructParticle ()
    {
      DT_THROW_IF (! _initialized_, std::logic_error, "Not initialized !");

      // In this method, static member functions should be called
      // for all particles which you want to use.
      // This ensures that objects of these particle types will be
      // created in the program.

      // bosons
      _ConstructBosons ();

      // leptons
      _ConstructLeptons ();

      // mesons
      _ConstructMesons ();

      // baryons
      _ConstructBaryons ();

      // ions
      _ConstructIons ();

      return;
    }

    void physics_list::_ConstructIons ()
    {
      if (_use_light_nuclei_) {
        G4Alpha::AlphaDefinition();
        G4Deuteron::DeuteronDefinition();
        G4Triton::TritonDefinition();
        G4He3::He3Definition();
      }
      if (_use_light_anti_nuclei_) {
        G4AntiAlpha::AntiAlphaDefinition();
        G4AntiDeuteron::AntiDeuteronDefinition();
        G4AntiTriton::AntiTritonDefinition();
        G4AntiHe3::AntiHe3Definition();
      }
      if (_use_generic_ion_) {
        G4GenericIon::GenericIonDefinition();
      }
      return;
    }


    void physics_list::_ConstructBosons ()
    {
      // gammas
      G4Gamma::GammaDefinition ();

      if (_use_geantinos_) {
        G4Geantino::GeantinoDefinition ();
        G4ChargedGeantino::ChargedGeantinoDefinition ();
      }
      if (_use_optical_photons_) {
        G4OpticalPhoton::OpticalPhotonDefinition ();
      }

      return;
    }

    void physics_list::_ConstructLeptons ()
    {
      // Light leptons e+/e- :
      //G4LeptonConstructor::ConstructELeptons();
      G4Electron::ElectronDefinition();
      G4Positron::PositronDefinition();
      G4NeutrinoE::NeutrinoEDefinition();
      G4AntiNeutrinoE::AntiNeutrinoEDefinition();

      // Muon (anti-)leptons :
      if (_use_muon_leptons_) {
        //G4LeptonConstructor::ConstructMuLeptons();
        G4MuonPlus::MuonPlusDefinition();
        G4MuonMinus::MuonMinusDefinition();
        G4NeutrinoMu::NeutrinoMuDefinition();
        G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
      }

      // Tau (anti-)leptons :
      if (_use_tau_leptons_) {
        //G4LeptonConstructor::ConstructTauLeptons();
        G4TauMinus::TauMinusDefinition();
        G4TauPlus::TauPlusDefinition();
        G4NeutrinoTau::NeutrinoTauDefinition();
        G4AntiNeutrinoTau::AntiNeutrinoTauDefinition();
      }

      return;
    }


    void physics_list::_ConstructMesons ()
    {
      if (_use_light_mesons_) {
        //G4MesonConstructor::ConstructLightMesons();
        G4PionPlus::PionPlusDefinition();
        G4PionMinus::PionMinusDefinition();
        G4PionZero::PionZeroDefinition();
        G4Eta::EtaDefinition();
        G4EtaPrime::EtaPrimeDefinition();
        G4KaonPlus::KaonPlusDefinition();
        G4KaonMinus::KaonMinusDefinition();
        G4KaonZero::KaonZeroDefinition();
        G4AntiKaonZero::AntiKaonZeroDefinition();
        G4KaonZeroLong::KaonZeroLongDefinition();
        G4KaonZeroShort::KaonZeroShortDefinition();
      }
      if (_use_charm_mesons_) {
        //G4MesonConstructor::ConstructCharmMesons();
        G4DMesonPlus::DMesonPlusDefinition();
        G4DMesonMinus::DMesonMinusDefinition();
        G4DMesonZero::DMesonZeroDefinition();
        G4AntiDMesonZero::AntiDMesonZeroDefinition();
        G4DsMesonPlus::DsMesonPlusDefinition();
        G4DsMesonMinus::DsMesonMinusDefinition();
#if G4VERSION_NUMBER == 960
        G4Etac::EtacDefinition();
#endif // G4VERSION_NUMBER == 960
        G4JPsi::JPsiDefinition();
      }
      if (_use_bottom_mesons_) {
        //G4MesonConstructor::ConstructBottomMesons();
        G4BMesonPlus::BMesonPlusDefinition();
        G4BMesonMinus::BMesonMinusDefinition();
        G4BMesonZero::BMesonZeroDefinition();
        G4AntiBMesonZero::AntiBMesonZeroDefinition();
        G4BsMesonZero::BsMesonZeroDefinition();
        G4AntiBsMesonZero::AntiBsMesonZeroDefinition();
#if G4VERSION_NUMBER == 960
        G4BcMesonPlus::BcMesonPlusDefinition();
        G4BcMesonMinus::BcMesonMinusDefinition();
        G4Upsiron::UpsironDefinition();
#endif // G4VERSION_NUMBER == 960

      }
      return;
    }


    void physics_list::_ConstructBaryons ()
    {
      if (_use_nucleons_) {
        //G4BaryonConstructor::ConstructNucleons();
        G4Proton::ProtonDefinition();
        G4Neutron::NeutronDefinition();
        G4AntiProton::AntiProtonDefinition();
        G4AntiNeutron::AntiNeutronDefinition();
      }
      if (_use_strange_baryons_) {
        //G4BaryonConstructor::ConstructStrangeBaryons();
        G4Lambda::LambdaDefinition();
        G4AntiLambda::AntiLambdaDefinition();
        G4SigmaZero::SigmaZeroDefinition();
        G4AntiSigmaZero::AntiSigmaZeroDefinition();
        G4SigmaPlus::SigmaPlusDefinition();
        G4AntiSigmaPlus::AntiSigmaPlusDefinition();
        G4SigmaMinus::SigmaMinusDefinition();
        G4AntiSigmaMinus::AntiSigmaMinusDefinition();
        G4XiZero::XiZeroDefinition();
        G4AntiXiZero::AntiXiZeroDefinition();
        G4XiMinus::XiMinusDefinition();
        G4AntiXiMinus::AntiXiMinusDefinition();
        G4OmegaMinus::OmegaMinusDefinition();
        G4AntiOmegaMinus::AntiOmegaMinusDefinition();
      }
      if (_use_charm_baryons_) {
        //G4BaryonConstructor::ConstructCharmBaryons();
        G4LambdacPlus::LambdacPlusDefinition();
        G4SigmacPlusPlus::SigmacPlusPlusDefinition();
        G4SigmacPlus::SigmacPlusDefinition();
        G4SigmacZero::SigmacZeroDefinition();
        G4XicPlus::XicPlusDefinition();
        G4XicZero::XicZeroDefinition();
        G4OmegacZero::OmegacZeroDefinition();

        G4AntiLambdacPlus::AntiLambdacPlusDefinition();
        G4AntiSigmacPlusPlus::AntiSigmacPlusPlusDefinition();
        G4AntiSigmacPlus::AntiSigmacPlusDefinition();
        G4AntiSigmacZero::AntiSigmacZeroDefinition();
        G4AntiXicPlus::AntiXicPlusDefinition();
        G4AntiXicZero::AntiXicZeroDefinition();
        G4AntiOmegacZero::AntiOmegacZeroDefinition();
      }
      if (_use_bottom_baryons_) {
        //G4BaryonConstructor::ConstructBottomBaryons();
#if G4VERSION_NUMBER == 960
        G4Lambdab::LambdabDefinition();
        G4SigmabPlus::SigmabPlusDefinition();
        G4SigmabZero::SigmabZeroDefinition();
        G4SigmabMinus::SigmabMinusDefinition();
        G4XibZero::XibZeroDefinition();
        G4XibMinus::XibMinusDefinition();
        G4OmegabMinus::OmegabMinusDefinition();
#else // G4VERSION_NUMBER == 960
        DT_LOG_WARNING(_logprio(), "Geant4 version " << G4VERSION_NUMBER << " does not support bottom baryons.");
#endif // G4VERSION_NUMBER == 960

#if G4VERSION_NUMBER == 960
        G4AntiLambdab::AntiLambdabDefinition();
        G4AntiSigmabPlus::AntiSigmabPlusDefinition();
        G4AntiSigmabZero::AntiSigmabZeroDefinition();
        G4AntiSigmabMinus::AntiSigmabMinusDefinition();
        G4AntiXibZero::AntiXibZeroDefinition();
        G4AntiXibMinus::AntiXibMinusDefinition();
        G4AntiOmegabMinus::AntiOmegabMinusDefinition();
#else // G4VERSION_NUMBER == 960
        DT_LOG_WARNING(_logprio(), "Geant4 version " << G4VERSION_NUMBER << " does not support bottom anti-baryons.");
#endif // G4VERSION_NUMBER == 960
      }
      return;
    }


    void physics_list::ConstructProcess ()
    {
      DT_THROW_IF (! _initialized_, std::logic_error, "Not initialized !");

      AddTransportation ();
      _ConstructEMProcess ();
      _ConstructGeneral ();

      if (_use_nucleons_) {
        _ConstructHadronicProcess ();
      }

      return;
    }

    void physics_list::_ConstructEMProcess ()
    {
      double ms_electron_range_factor = 0.005;
      theParticleIterator->reset ();
      while ((*theParticleIterator) ()) {
        G4ParticleDefinition * particle = theParticleIterator->value ();
        G4ProcessManager     * pmanager = particle->GetProcessManager ();
        G4String particle_name = particle->GetParticleName ();

        DT_LOG_NOTICE (_logprio(), "Set electromagnetic processes for '"
                       << particle_name << "'");


        if (particle_name == "gamma") {
          /***********
           *  gamma  *
           ***********/
          G4PhotoElectricEffect * the_photoelectric_effect = new G4PhotoElectricEffect ();
          G4ComptonScattering   * the_compton_scattering   = new G4ComptonScattering   ();
          G4GammaConversion     * the_gamma_conversion     = new G4GammaConversion     ();
          if (is_em_low_energy_livermore ()) {
            // Livermore:
            G4LivermorePhotoElectricModel* the_livermore_photoelectric_model
              = new G4LivermorePhotoElectricModel ();
            the_photoelectric_effect->SetModel (the_livermore_photoelectric_model);

            G4LivermoreComptonModel* the_livermore_compton_model
              = new G4LivermoreComptonModel ();
            the_compton_scattering->SetModel (the_livermore_compton_model);

            G4LivermoreGammaConversionModel* the_livermore_gamma_conversion_model
              = new G4LivermoreGammaConversionModel ();
            the_gamma_conversion->SetModel (the_livermore_gamma_conversion_model);
          } else if (is_em_low_energy_penelope ()) {
            // Penelope:
            G4PenelopePhotoElectricModel* the_penelope_photoelectric_model
              = new G4PenelopePhotoElectricModel ();
            the_photoelectric_effect->SetModel (the_penelope_photoelectric_model);

            G4PenelopeComptonModel* the_penelope_compton_model
              = new G4PenelopeComptonModel ();
            the_compton_scattering->SetModel (the_penelope_compton_model);

            G4PenelopeGammaConversionModel* the_penelope_gamma_conversion_model
              = new G4PenelopeGammaConversionModel ();
            the_gamma_conversion->SetModel (the_penelope_gamma_conversion_model);
          }

          pmanager->AddDiscreteProcess (the_photoelectric_effect);
          pmanager->AddDiscreteProcess (the_compton_scattering);
          pmanager->AddDiscreteProcess (the_gamma_conversion);
          //pmanager->AddProcess (new G4StepLimiter (), -1, -1, 4);

        } else if (particle_name == "e-") {
          /**************
           *  electron  *
           **************/
          if (_electron_multiple_scattering_) {
            // Multiple scattering:
            G4eMultipleScattering * emuls = new G4eMultipleScattering ();
            // Setting the FacRange to 0.005 instead of default value 0.2
            emuls->SetRangeFactor (ms_electron_range_factor);
            /*
              #ifdef G4_VER_9
              emuls->SetRangeFactor (0.005);
              #else
              emuls->SetFacrange (0.005);
              #endif
            */
            //??? emuls->SetStepLimitType (fUseDistanceToBoundary);
            pmanager->AddProcess (emuls,                   -1, 1, 1);
          }
          if (_electron_energy_loss_) {
            // Ionisation:
            G4eIonisation * the_ionisation = new G4eIonisation ();
            if (is_em_low_energy_livermore ()) {
              // Livermore:
              G4LivermoreIonisationModel* the_livermore_ionisation_model
                = new G4LivermoreIonisationModel ();
              the_ionisation->SetEmModel(the_livermore_ionisation_model);
            } else if (is_em_low_energy_penelope ()) {
              // Penelope:
              G4PenelopeIonisationModel* the_penelope_ionisation_model
                = new G4PenelopeIonisationModel ();
              the_ionisation->SetEmModel(the_penelope_ionisation_model);
            }

            // Bremsstrahlung:
            G4eBremsstrahlung * the_bremsstrahlung = new G4eBremsstrahlung ();
            if (is_em_low_energy_livermore ()) {
              // Livermore:
              G4LivermoreBremsstrahlungModel * the_livermore_bremsstrahlung_model
                = new G4LivermoreBremsstrahlungModel ();
              the_bremsstrahlung->SetEmModel(the_livermore_bremsstrahlung_model);
            } else if (is_em_low_energy_penelope ()) {
              // Penelope:
              G4PenelopeBremsstrahlungModel* the_penelope_bremsstrahlung_model
                = new G4PenelopeBremsstrahlungModel ();
              the_bremsstrahlung->SetEmModel(the_penelope_bremsstrahlung_model);
            }
            pmanager->AddProcess (the_ionisation,       -1, 2, 2);
            pmanager->AddProcess (the_bremsstrahlung,   -1, 3, 3);
            pmanager->AddProcess (new G4StepLimiter,    -1,-1, 4);
          }
        } else if (particle_name == "e+") {

          /**************
           *  positron  *
           **************/
          if (_electron_multiple_scattering_) {
            // Multiple scattering:
            G4eMultipleScattering * emuls = new G4eMultipleScattering ();
            // Setting the FacRange to 0.005 instead of default value 0.2
            emuls->SetRangeFactor (ms_electron_range_factor);
            // #ifdef G4_VER_9
            //                emuls->SetRangeFactor (0.005);
            // #else
            //                emuls->SetFacrange (0.005);
            // #endif
            pmanager->AddProcess (emuls, -1, 1, 1);
          }
          if (_electron_energy_loss_) {
            // Ionisation:
            G4eIonisation * the_ionisation = new G4eIonisation ();
            if (is_em_low_energy_livermore ()) {
              // Livermore:
              G4LivermoreIonisationModel * the_livermore_ionisation_model
                = new G4LivermoreIonisationModel ();
              the_ionisation->SetEmModel(the_livermore_ionisation_model);
            } else if (is_em_low_energy_penelope ()) {
              // Penelope:
              G4PenelopeIonisationModel * the_penelope_ionisation_model
                = new G4PenelopeIonisationModel ();
              the_ionisation->SetEmModel(the_penelope_ionisation_model);
            }
            // Bremsstrahlung:
            G4eBremsstrahlung * the_bremsstrahlung = new G4eBremsstrahlung ();
            if (is_em_low_energy_livermore ()) {
              // Livermore:
              G4LivermoreBremsstrahlungModel * the_livermore_bremsstrahlung_model
                = new G4LivermoreBremsstrahlungModel ();
              the_bremsstrahlung->SetEmModel(the_livermore_bremsstrahlung_model);
            } else if (is_em_low_energy_penelope ()) {
              // Penelope:
              G4PenelopeBremsstrahlungModel * the_penelope_bremsstrahlung_model
                = new G4PenelopeBremsstrahlungModel ();
              the_bremsstrahlung->SetEmModel(the_penelope_bremsstrahlung_model);
            }
            // e+ annihilation:
            G4eplusAnnihilation * the_eplus_annihilation = new G4eplusAnnihilation ();
            if (is_em_low_energy_penelope ()) {
              G4PenelopeAnnihilationModel * the_penelope_annihilation_model
                = new G4PenelopeAnnihilationModel ();
              the_eplus_annihilation->SetModel(the_penelope_annihilation_model);
            }
            pmanager->AddProcess (the_ionisation,         -1, 2, 2);
            pmanager->AddProcess (the_bremsstrahlung,     -1, 3, 3);
            pmanager->AddProcess (the_eplus_annihilation,  0,-1, 4);
            pmanager->AddProcess (new G4StepLimiter,      -1,-1, 5);
          }
        } else if (particle_name == "alpha" ||
                   particle_name == "GenericIon") {
          /************
           *   ions   *
           ************/
          G4hMultipleScattering * the_ion_multiple_scattering = new G4hMultipleScattering ();
          G4ionIonisation       * the_ion_ionisation          = new G4ionIonisation ();
          pmanager->AddProcess (the_ion_multiple_scattering , -1, 1, 1);
          pmanager->AddProcess (the_ion_ionisation,           -1, 2, 2);
          pmanager->AddProcess (new G4StepLimiter,            -1,-1, 3);
        } else if (particle_name == "mu+" ||
                   particle_name == "mu-") {
          /*************
           *   muons   *
           *************/
          if (_use_muon_leptons_) {
            pmanager->AddProcess (new G4MuMultipleScattering, -1,  1, 1);
            pmanager->AddProcess (new G4MuIonisation,         -1,  2, 2);
            pmanager->AddProcess (new G4MuBremsstrahlung,     -1,  3, 3);
            pmanager->AddProcess (new G4MuPairProduction,     -1,  4, 4);
            // pmanager->AddProcess (new G4StepLimiter,          -1, -1, 5);
            // pmanager->AddProcess (new G4UserSpecialCuts,      -1, -1, 5);
          }
        } else {
          /***********************
           *   other particles   *
           ***********************/
          // all others charged particles
          if ((!particle->IsShortLived ()) &&
              (particle->GetPDGCharge () != 0.0)) {
            pmanager->AddProcess (new G4hMultipleScattering,-1, 1, 1);
            pmanager->AddProcess (new G4hIonisation,       -1, 2, 2);
            //      pmanager->AddProcess(new G4StepLimiter,       -1,-1, 3);
            //      pmanager->AddProcess(new G4UserSpecialCuts,   -1,-1, 3);
          }
        }
      }
      return;
    }

    void physics_list::_ConstructHadronicProcess ()
    {
      // Taken from ConstructHad() of
      // $G4INSTALL/examples/advanced/underground_physics
      // Makes discrete physics processes for the hadrons, at present
      // limited to those particles with GHEISHA interactions (INTRC >
      // 0).  The processes are: Elastic scattering and Inelastic
      // scattering and Capture (neutron).  F.W.Jones 09-JUL-1998

      // Elastic process
      G4HadronElasticProcess * the_elastic_process = new G4HadronElasticProcess ();
      G4LElastic             * the_elastic_model   = new G4LElastic ();
      the_elastic_process->RegisterMe (the_elastic_model);

      theParticleIterator->reset();
      while ((*theParticleIterator)()) {
        G4ParticleDefinition * particle = theParticleIterator->value ();
        G4ProcessManager     * pmanager = particle->GetProcessManager ();
        G4String particle_name = particle->GetParticleName ();
        DT_LOG_NOTICE(_logprio(),"Set hadronic processes for '" << particle_name << "'");

        if (particle_name == "neutron") {
          {
            // elastic scattering
            G4HadronElasticProcess * the_elastic_process
              = new G4HadronElasticProcess ();
            G4LElastic * the_elastic_model = new G4LElastic ();
            the_elastic_model->SetMinEnergy (19*CLHEP::MeV);
            the_elastic_process->RegisterMe (the_elastic_model);
            G4NeutronHPElastic * the_low_energy_elastic_model = new G4NeutronHPElastic ();
            the_elastic_process->RegisterMe (the_low_energy_elastic_model);
            G4NeutronHPElasticData * the_neutron_data = new G4NeutronHPElasticData ();
            the_elastic_process->AddDataSet (the_neutron_data);
            pmanager->AddDiscreteProcess (the_elastic_process);
          }

          {
            // inelastic scattering
            G4NeutronInelasticProcess * the_inelastic_process
              = new G4NeutronInelasticProcess ("inelastic");
            G4LENeutronInelastic * the_inelastic_model = new G4LENeutronInelastic ();
            the_inelastic_model->SetMinEnergy (19*CLHEP::MeV);
            the_inelastic_process->RegisterMe (the_inelastic_model);
            G4NeutronHPInelastic * the_low_energy_inelastic_model
              = new G4NeutronHPInelastic ();
            the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
            G4NeutronHPInelasticData * the_neutron_data
              = new G4NeutronHPInelasticData ();
            the_inelastic_process->AddDataSet (the_neutron_data);
            pmanager->AddDiscreteProcess (the_inelastic_process);
          }

          {
            // capture
            G4HadronCaptureProcess * the_capture_process
              = new G4HadronCaptureProcess ();
            G4LCapture * the_capture_model = new G4LCapture ();
            the_capture_model->SetMinEnergy (19*CLHEP::MeV);
            the_capture_process->RegisterMe (the_capture_model);
            G4NeutronHPCapture * the_low_energy_capture_model = new G4NeutronHPCapture ();
            the_capture_process->RegisterMe (the_low_energy_capture_model);
            G4NeutronHPCaptureData * the_neutron_data = new G4NeutronHPCaptureData ();
            the_capture_process->AddDataSet (the_neutron_data);
            pmanager->AddDiscreteProcess (the_capture_process);
          }
          //  G4ProcessManager* pmanager = G4Neutron::Neutron->GetProcessManager();
          //  pmanager->AddProcess(new G4UserSpecialCuts(),-1,-1,1);
        } else if (particle_name == "anti_neutron") {
          G4HadronElasticProcess * the_elastic_process
            = new G4HadronElasticProcess ();
          G4LElastic * the_elastic_model = new G4LElastic ();
          the_elastic_process->RegisterMe (the_elastic_model);
          pmanager->AddDiscreteProcess (the_elastic_process);

          G4AntiNeutronInelasticProcess * the_inelastic_process
            = new G4AntiNeutronInelasticProcess ("inelastic");
          G4LEAntiNeutronInelastic * the_low_energy_inelastic_model
            = new G4LEAntiNeutronInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEAntiNeutronInelastic * the_high_energy_inelastic_model =
            new G4HEAntiNeutronInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "pi+") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4PionPlusInelasticProcess * the_inelastic_process
            = new G4PionPlusInelasticProcess ("inelastic");
          G4LEPionPlusInelastic * the_low_energy_inelastic_model
            = new G4LEPionPlusInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEPionPlusInelastic * the_high_energy_inelastic_model
            = new G4HEPionPlusInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "pi-") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4PionMinusInelasticProcess * the_inelastic_process
            = new G4PionMinusInelasticProcess ("inelastic");
          G4LEPionMinusInelastic * the_low_energy_inelastic_model
            = new G4LEPionMinusInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEPionMinusInelastic * the_high_energy_inelastic_model
            = new G4HEPionMinusInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
          pmanager->AddRestProcess (new G4PiMinusAbsorptionAtRest (), ordDefault);
        } else if (particle_name == "kaon+") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4KaonMinusInelasticProcess * the_inelastic_process
            = new G4KaonMinusInelasticProcess ("inelastic");
          G4LEKaonMinusInelastic * the_low_energy_inelastic_model
            = new G4LEKaonMinusInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEKaonMinusInelastic * the_high_energy_inelastic_model
            = new G4HEKaonMinusInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "kaon-") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4KaonMinusInelasticProcess * the_inelastic_process
            = new G4KaonMinusInelasticProcess ("inelastic");
          G4LEKaonMinusInelastic * the_low_energy_inelastic_model
            = new G4LEKaonMinusInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEKaonMinusInelastic * the_high_energy_inelastic_model
            = new G4HEKaonMinusInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
          pmanager->AddRestProcess (new G4KaonMinusAbsorptionAtRest (), ordDefault);
        } else if (particle_name == "kaon0S") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4KaonZeroSInelasticProcess * the_inelastic_process
            = new G4KaonZeroSInelasticProcess ("inelastic");
          G4LEKaonZeroSInelastic * the_low_energy_inelastic_model
            = new G4LEKaonZeroSInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEKaonZeroInelastic * the_high_energy_inelastic_model
            = new G4HEKaonZeroInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "kaon0L") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4KaonZeroLInelasticProcess * the_inelastic_process
            = new G4KaonZeroLInelasticProcess ("inelastic");
          G4LEKaonZeroLInelastic * the_low_energy_inelastic_model
            = new G4LEKaonZeroLInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEKaonZeroInelastic * the_high_energy_inelastic_model
            = new G4HEKaonZeroInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "proton") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4ProtonInelasticProcess * the_inelastic_process
            = new G4ProtonInelasticProcess ("inelastic");
          G4LEProtonInelastic * the_low_energy_inelastic_model
            = new G4LEProtonInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEProtonInelastic * the_high_energy_inelastic_model
            = new G4HEProtonInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "anti_proton") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4AntiProtonInelasticProcess * the_inelastic_process
            = new G4AntiProtonInelasticProcess ("inelastic");
          G4LEAntiProtonInelastic * the_low_energy_inelastic_model
            = new G4LEAntiProtonInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          G4HEAntiProtonInelastic * the_high_energy_inelastic_model
            = new G4HEAntiProtonInelastic ();
          the_inelastic_process->RegisterMe (the_high_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "deuteron") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4DeuteronInelasticProcess * the_inelastic_process
            = new G4DeuteronInelasticProcess ("inelastic");
          G4LEDeuteronInelastic * the_low_energy_inelastic_model
            = new G4LEDeuteronInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "triton") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4TritonInelasticProcess * the_inelastic_process
            = new G4TritonInelasticProcess ("inelastic");
          G4LETritonInelastic * the_low_energy_inelastic_model
            = new G4LETritonInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else if (particle_name == "alpha") {
          pmanager->AddDiscreteProcess (the_elastic_process);
          G4AlphaInelasticProcess * the_inelastic_process
            = new G4AlphaInelasticProcess ("inelastic");
          G4LEAlphaInelastic * the_low_energy_inelastic_model
            = new G4LEAlphaInelastic ();
          the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
          pmanager->AddDiscreteProcess (the_inelastic_process);
        } else {
          DT_LOG_NOTICE(_logprio(), "Particle '" << particle_name << "' has no hadronic processes");
        }
      }
      return;
    }

    void physics_list::_ConstructGeneral ()
    {
      // Add Decay Process
      G4Decay * theDecayProcess = new G4Decay ();
      theParticleIterator->reset ();
      while ((*theParticleIterator) ()) {
        G4ParticleDefinition * particle = theParticleIterator->value ();
        G4ProcessManager     * pmanager = particle->GetProcessManager ();
        if (theDecayProcess->IsApplicable (*particle)) {
          pmanager->AddProcess (theDecayProcess);
          // set ordering for PostStepDoIt and AtRestDoIt
          pmanager->SetProcessOrdering (theDecayProcess, idxPostStep);
          pmanager->SetProcessOrdering (theDecayProcess, idxAtRest);
        }
      }
      return;
    }

    // static
    const string physics_list::EM_PIXE_MODEL_EMPIRICAL         = "Empirical";
    const string physics_list::EM_PIXE_MODEL_ECPSSR_FORMFACTOR = "ECPSSR_FormFactor";
    const string physics_list::EM_PIXE_MODEL_ECPSSR_ANALYTICAL = "ECPSSR_Analytical";

    physics_list::region_deexcitation_type::region_deexcitation_type()
    {
      fluo = false;
      auger = false;
      pixe = false;
    }

    physics_list::region_deexcitation_type::region_deexcitation_type(bool fluo_,bool auger_, bool pixe_)
    {
      fluo = fluo_;
      auger = auger_;
      pixe = pixe_;
    }

    bool physics_list::region_deexcitation_type::is_activated() const
    {
      return fluo || auger || pixe;
    }

    bool physics_list::region_deexcitation_type::is_fluo() const
    {
      return fluo;
    }

    bool physics_list::region_deexcitation_type::is_auger() const
    {
      return auger;
    }

    bool physics_list::region_deexcitation_type::is_pixe() const
    {
      return pixe;
    }

    void physics_list::_initialize_deexcitation ()
    {
      G4EmProcessOptions emOptions;
      emOptions.SetFluo(_em_fluo_);
      emOptions.SetAuger(_em_auger_);
      emOptions.SetPIXE(_em_pixe_);
      if (! _em_pixe_cross_section_model_.empty()) {
        emOptions.SetPIXECrossSectionModel(_em_pixe_cross_section_model_);
      }

      for (std::map<std::string, region_deexcitation_type>::const_iterator i
             = _em_regions_deexcitation_.begin();
           i != _em_regions_deexcitation_.end();
           i++) {
        const std::string & region_name = i->first;
        const region_deexcitation_type & rd = i->second;
        G4Region * a_region = G4RegionStore::GetInstance ()->GetRegion (region_name);
        DT_THROW_IF(a_region == 0, std::logic_error,
                    "Cannot find region named '" << region_name
                    << "' to apply de-excitation processes ");
        emOptions.SetDeexcitationActiveRegion(region_name, rd.fluo, rd.auger, rd.pixe);
      }

      return;
    }

    void physics_list::SetCuts ()
    {
      DT_LOG_TRACE(_logprio(), "Entering...");
      // G4VUserPhysicsList::SetCutsWithDefault method sets
      // the default cut value for all particle types
      //
      SetCutsWithDefault ();

      // G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250*eV, 1*GeV);

      if (_using_cuts_) {
        DT_LOG_TRACE(_logprio(), "Using cuts...");
        // Defining cuts for different particles:
        // set cut values for gamma at first and for e- second and next for e+,
        // because some processes for e+/e- need cut values for gamma
        SetCutValue(_gamma_cut_,    "gamma");
        SetCutValue(_electron_cut_, "e-");
        SetCutValue(_positron_cut_, "e+");
        SetCutValue(_proton_cut_,   "proton");

        // Defining cuts for the different regions :
        for (map<string, double>::const_iterator i = _region_cuts_.begin ();
             i != _region_cuts_.end ();
             i++) {
          const string & region_name = i->first;
          double region_cut = i->second;
          G4Region * a_region = G4RegionStore::GetInstance ()->GetRegion (region_name);
          if (a_region != 0) {
            G4ProductionCuts * a_region_cuts = new G4ProductionCuts;
            a_region_cuts->SetProductionCut (region_cut);
            a_region->SetProductionCuts (a_region_cuts);
          } else {
            DT_LOG_WARNING(_logprio(), "No region named '"
                           << region_name << "' to be given production cuts !");
          }
        }

        // Verbosing:
        DumpCutValuesTable ();
      }
      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }

    void physics_list::tree_dump (ostream & out_,
                                  const string & title_,
                                  const string & indent_,
                                  bool inherit_) const
    {
      namespace du = datatools;
      string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty ()) {
        out_ << indent << title_ << endl;
      }

      out_ << indent << du::i_tree_dumpable::tag
           << "Initialized                  : " << (_initialized_ ? "Yes": "No") << endl;

      out_ << indent << du::i_tree_dumpable::tag
           << "Logging priority             : "
           << datatools::logger::get_priority_label(_logprio()) << endl;

      out_ << indent << du::i_tree_dumpable::tag
           << "G4 verbosity                 : " << (_g4_verbosity_ ? "Yes" : "No") << endl;

      out_ << indent << du::i_tree_dumpable::tag
           << "EM model                     : " << _em_model_ << endl;

      out_ << indent << du::i_tree_dumpable::tag
           << "Electron energy loss         : "
           << (_electron_energy_loss_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Electron multiple scattering : "
           << (_electron_multiple_scattering_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "MSC use distance to boundary : "
           << (_msc_use_distance_to_boundary_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Set bremsstrahlung           : "
           << (_bremsstrahlung_ ? "Yes" : "No") << endl;

      out_ << indent << du::i_tree_dumpable::tag
           << "Use geantinos                : " << (_use_geantinos_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use optical photons          : " << (_use_optical_photons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use muon leptons             : " << (_use_muon_leptons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use tau leptons              : " << (_use_tau_leptons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use nucleons                 : " << (_use_nucleons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use strange baryons          : " << (_use_strange_baryons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use charm baryons            : " << (_use_charm_baryons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use bottom baryons           : " << (_use_bottom_baryons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use light mesons             : " << (_use_light_mesons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use charm mesons             : " << (_use_charm_mesons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use bottom mesons            : " << (_use_bottom_mesons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use light nuclei             : " << (_use_light_nuclei_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use light anti-nuclei        : " << (_use_light_anti_nuclei_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use generic ion              : " << (_use_generic_ion_ ? "Yes" : "No") << endl;

      {
        // cuts in region
        out_ << indent << du::i_tree_dumpable::tag
             << "Using cuts                 : " << (_using_cuts_ ? "Yes" : "No") << endl;
        out_ << indent << du::i_tree_dumpable::tag
             << "Default cut value          : "
             << _default_cut_value_ / CLHEP::mm << " mm" << endl;
        out_ << indent << du::i_tree_dumpable::tag
             << "Regions cuts               : "
             << (!_region_cuts_.empty () ? "Yes" : "No") << endl;
        for (map<string, double>::const_iterator i = _region_cuts_.begin ();
             i != _region_cuts_.end ();
             i++) {
          out_ << indent << du::i_tree_dumpable::skip_tag;
          map<string, double>::const_iterator j = i;
          j++;
          if (j == _region_cuts_.end ()) {
            out_ << du::i_tree_dumpable::last_tag;
          } else {
            out_ << du::i_tree_dumpable::tag;
          }
          out_ << "Region '" << i->first << "' : Production cuts = "
               << i->second/ CLHEP::mm << " mm" << endl;
        }
      }

      {
        // particles cuts
        out_ << indent << du::i_tree_dumpable::tag
             << "Electron cut               : "
             << _electron_cut_ / CLHEP::mm << " mm" << endl;
        out_ << indent << du::i_tree_dumpable::tag
             << "Positron cut               : "
             << _positron_cut_ / CLHEP::mm << " mm" << endl;
        out_ << indent << du::i_tree_dumpable::last_tag
             << "Gamma cut                  : "
             << _gamma_cut_ / CLHEP::mm << " mm" << endl;
      }

      return;
    }

  } // end of namespace g4

} // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::physics_list,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::physics_list");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation manager embedded physics list instance");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The Geant4 simulation manager class embedded \n"
                                "physics list.                                \n"
                                );

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Not available yet                    \n"
                               );

  ocd_.set_validation_support(false);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::physics_list' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::physics_list,"mctools::g4::physics_list")


// end of physics_list.cc

