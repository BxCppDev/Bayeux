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

// G4 stuff :
#include <globals.hh>
#include <G4ProcessManager.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTypes.hh>
#include <G4UnitsTable.hh>
#include <G4MesonConstructor.hh>
#include <G4BaryonConstructor.hh>
#include <G4IonConstructor.hh>

// EM processes
#include <G4ComptonScattering.hh>
#include <G4GammaConversion.hh>
#include <G4PhotoElectricEffect.hh>
#include <G4eMultipleScattering.hh>

#include <G4hMultipleScattering.hh>
#include <G4MuMultipleScattering.hh>
#include <G4eIonisation.hh>
#include <G4eBremsstrahlung.hh>
#include <G4eplusAnnihilation.hh>
#include <G4MuIonisation.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>
#include <G4hIonisation.hh>
#include <G4ionIonisation.hh>

#include <G4LivermoreBremsstrahlungModel.hh>
#include <G4LivermoreComptonModel.hh>
#include <G4LivermoreGammaConversionModel.hh>
#include <G4LivermoreIonisationModel.hh>
#include <G4LivermorePhotoElectricModel.hh>

#include <G4PenelopeAnnihilationModel.hh>
#include <G4PenelopeBremsstrahlungModel.hh>
#include <G4PenelopeComptonModel.hh>
#include <G4PenelopeGammaConversionModel.hh>
#include <G4PenelopeIonisationModel.hh>
#include <G4PenelopePhotoElectricModel.hh>

// Hadronic processes
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

    const string physics_list::EM_MODEL_STANDARD   = "standard";
    const string physics_list::EM_MODEL_LOW_ENERGY = "low_energy";
    const string physics_list::EM_MODEL_PENELOPE   = "penelope";

    const string & physics_list::get_em_model () const
    {
      return _em_model_;
    }

    bool physics_list::is_em_standard () const
    {
      return _em_model_ == EM_MODEL_STANDARD;
    }

    bool physics_list::is_em_low_energy () const
    {
      return _em_model_ == EM_MODEL_LOW_ENERGY;
    }

    bool physics_list::is_em_penelope () const
    {
      return _em_model_ == EM_MODEL_PENELOPE;
    }

    bool physics_list::is_debug () const
    {
      return _debug_;
    }

    // ctor:
    physics_list::physics_list (bool logging_) : G4VUserPhysicsList ()
    {
      // clog << datatools::io::devel
      //      << "physics_list::physics_list: Entering...." << endl;
      _debug_       = false;
      _initialized_ = false;

      _logging_ = logging_;

      this->_set_defaults ();
      return;
    }

    // dtor:
    physics_list::~physics_list ()
    {
      // clog << datatools::io::devel
      //      << "physics_list::~physics_list: ******************* " << endl;
      return;
    }

    void physics_list::initialize (const datatools::properties & config_)
    {
      // clog << datatools::io::devel
      //      << "mctools::g4::physics_list::initialize: Entering..." << endl;

      if (_initialized_)
        {
          throw logic_error ("mctools::g4::physics_list::initialize: Already initialized !");
        }

      // Parse debug:
      if (config_.has_flag ("debug"))
        {
          _debug_ = true;
        }

      // Processes activation:
      if (config_.has_key ("em.model"))
        {
          _em_model_
            = config_.fetch_string ("em.model");
        }
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Electro-Magnetic model set to: '" << _em_model_ << "'" << endl;
        }

      if (config_.has_key ("electron.energy_loss"))
        {
          _electron_energy_loss_
            = config_.fetch_boolean ("electron.energy_loss");
        }
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Electrons Energy Loss set to: "
               <<  _electron_energy_loss_ << endl;
        }

      if (config_.has_key ("electron.multiple_scattering"))
        {
          _electron_multiple_scattering_
            = config_.fetch_boolean ("electron.multiple_scattering");
        }
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Electrons Multiple Scatering set to: "
               << _electron_multiple_scattering_ << endl;
        }

      if (config_.has_key ("bremsstrahlung"))
        {
          _bremsstrahlung_
            = config_.fetch_boolean ("bremsstrahlung");
        }
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Bremsstrahlung set to: " <<  _bremsstrahlung_ << endl;
        }

      if (config_.has_key ("use_he_leptons"))
        {
          _use_he_leptons_ = config_.fetch_boolean ("use_he_leptons");
        }
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Use HE leptons set to: " << _use_he_leptons_ << endl;
        }

      if (config_.has_key ("use_mesons"))
        {
          _use_mesons_ = config_.fetch_boolean ("use_mesons");
        }
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Use mesons set to: " << _use_mesons_ << endl;
        }

      if (config_.has_key ("use_neutrons"))
        {
          _use_neutrons_ = config_.fetch_boolean ("use_neutrons");
        }
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Use neutrons set to: " << _use_neutrons_ << endl;
        }

      // Cuts:
      if (config_.has_flag ("using_cuts"))
        {
          _using_cuts_ = true;
        }
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Use cuts set to: " << _using_cuts_ << endl;
        }

      if (_using_cuts_)
        {
          // Production cuts:
          vector<string> regions;
          double lunit = CLHEP::mm;
          if (config_.has_key ("production_cuts.regions"))
            {
              config_.fetch ("production_cuts.regions", regions);

              if (config_.has_key ("production_cuts.length_unit"))
                {
                  string lunit_str = config_.fetch_string ("production_cuts.length_unit");
                  lunit = datatools::units::get_length_unit_from (lunit_str);
                }

              for (int i = 0; i < (int) regions.size (); i++)
                {
                  const string & region_label = regions[i];
                  ostringstream key;
                  key << "production_cuts." << region_label;
                  if (! config_.has_key (key.str ()))
                    {
                      ostringstream message;
                      message << "mctools::g4:physics_list::initialize: "
                              << "Missing production cuts for '"
                              << key.str () << "' !";
                      throw logic_error (message.str ());
                    }
                  double production_cut = config_.fetch_real (key.str ());
                  production_cut *= lunit;
                  _region_cuts_[region_label] = production_cut;
                }
            }

          clog << datatools::io::notice
               << "mctools::g4:physics_list::initialize: "
               << "Production cuts for regions : " << endl;

          for (map<string, double>::const_iterator i = _region_cuts_.begin ();
               i != _region_cuts_.end ();
               i++)
            {
              map<string, double>::const_iterator j = i;
              j++;
              if (j == _region_cuts_.end ())
                {
                  clog << "`-- ";
                }
              else
                {
                  clog << "|-- ";
                }
              clog << "Region '" << i->first << "'  :  Production cuts = "
                   << i->second/ CLHEP::mm << " mm" << endl;
            }

          // Electron cut :
          if (config_.has_key ("electron_cut"))
            {
              _electron_cut_ = config_.fetch_real ("electron_cut") * CLHEP::mm;
            }
          if (is_debug ())
            {
              clog << datatools::io::debug
                   << "mctools::g4::physics_list::initialize: "
                   << "Electron cut set to: " << _electron_cut_ / CLHEP::mm << " mm"
                   << endl;
            }

          // Positron cut :
          if (config_.has_key ("positron_cut"))
            {
              _positron_cut_ = config_.fetch_real ("positron_cut") * CLHEP::mm;
            }
          if (is_debug ())
            {
              clog << datatools::io::debug
                   << "mctools::g4::physics_list::initialize: "
                   << "Positron cut set to: " << _positron_cut_ / CLHEP::mm << " mm"
                   << endl;
            }

          // Gamma cut :
          if (config_.has_key ("gamma_cut"))
            {
              _gamma_cut_ = config_.fetch_real ("gamma_cut") * CLHEP::mm;
            }
          if (is_debug ())
            {
              clog << datatools::io::debug
                   << "mctools::g4::physics_list::initialize: "
                   << "Gamma cut set to: " << _gamma_cut_ / CLHEP::mm << " mm"
                   << endl;
            }
        }

      SetVerboseLevel (_g4_verbosity_);
      clog << datatools::io::notice
           << "mctools::g4::physics_list::initialize: "
           << "Original default Cut Length : " << G4BestUnit (defaultCutValue, "Length") << endl;
      SetDefaultCutValue (_default_cut_value_);
      if (is_debug ())
        {
          clog << datatools::io::debug
               << "mctools::g4::physics_list::initialize: "
               << "Default Cut Length : " << G4BestUnit (defaultCutValue, "Length") << endl;
        }
      clog << datatools::io::notice
           << "mctools::g4::physics_list::initialize: "
           << "Default Cut Length : " << G4BestUnit (defaultCutValue, "Length") << endl;

      _initialized_ = true;

      // clog << datatools::io::devel
      //      << "mctools::g4::physics_list::initialize: Exiting."
      //      << endl;
      return;
    }

    void physics_list::_set_defaults ()
    {
      _g4_verbosity_ = VERBOSITY_SILENT;

      _em_model_ = EM_MODEL_STANDARD;
      _electron_energy_loss_         = true;
      _electron_multiple_scattering_ = true;
      _bremsstrahlung_               = true;
      _use_he_leptons_               = false;
      _use_mesons_                   = false;
      _use_neutrons_                 = false;

      _using_cuts_        = false;
      _default_cut_value_ = 1.0e-3 * CLHEP::mm;

      _electron_cut_ = 1.0 * CLHEP::mm;
      _positron_cut_ = 1.0 * CLHEP::mm;
      _gamma_cut_    = 1.0 * CLHEP::mm;

      return;
    }


    void physics_list::ConstructParticle ()
    {
      if (! _initialized_)
        {
          throw std::logic_error ("physics_list::ConstructParticle: Not initialized !");
        }

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
      G4IonConstructor iCtor;
      iCtor.ConstructParticle ();

      return;
    }


    void physics_list::_ConstructBosons ()
    {
      // gammas
      G4Gamma::GammaDefinition ();
      return;
    }

    void physics_list::_ConstructLeptons ()
    {
      // Light leptons e+/e- :
      G4Electron::ElectronDefinition ();
      G4Positron::PositronDefinition ();

      // Heavy/high energy leptons :
      if (_use_he_leptons_)
        {
          // mu+/-
          G4MuonPlus::MuonPlusDefinition ();
          G4MuonMinus::MuonMinusDefinition ();

          // nu_e
          G4NeutrinoE::NeutrinoEDefinition ();
          G4AntiNeutrinoE::AntiNeutrinoEDefinition ();

          // nu_mu
          G4NeutrinoMu::NeutrinoMuDefinition ();
          G4AntiNeutrinoMu::AntiNeutrinoMuDefinition ();
        }
      return;
    }


    void physics_list::_ConstructMesons ()
    {
      if (_use_mesons_)
        {
          // light mesons
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
      return;
    }


    void physics_list::_ConstructBaryons ()
    {
      // baryons
      if (_use_neutrons_)
        {
          // hadronic processes for neutron studies need all baryons
          G4BaryonConstructor bCtor;
          bCtor.ConstructParticle ();
        }
      else
        {
          G4Proton::ProtonDefinition ();
          G4AntiProton::AntiProtonDefinition ();
          G4Neutron::NeutronDefinition ();
          G4AntiNeutron::AntiNeutronDefinition ();
        }

      return;
    }


    void physics_list::ConstructProcess ()
    {
      if (! _initialized_)
        {
          throw std::logic_error ("physics_list::ConstructProcess: Not initialized !");
        }

      AddTransportation ();
      _ConstructEMProcess ();
      _ConstructGeneral ();

      if (_use_neutrons_)
        {
          _ConstructHadronicProcess ();
        }

      return;
    }

    void physics_list::_ConstructEMProcess ()
    {
      double ms_electron_range_factor = 0.005;
      theParticleIterator->reset ();
      while ((*theParticleIterator) ())
        {
          G4ParticleDefinition * particle = theParticleIterator->value ();
          G4ProcessManager     * pmanager = particle->GetProcessManager ();

          G4String particle_name = particle->GetParticleName ();

          if (is_debug ())
            {
              clog << datatools::io::debug
                   << "mctools::g4::physics_list::_ConstructEMProcess: "
                   << "Set electromagnetic processes for '"
                   << particle_name << "'" << endl;
            }

          /***********
           *  gamma  *
           ***********/

          if (particle_name == "gamma")
            {
              G4PhotoElectricEffect * the_photoelectric_effect = new G4PhotoElectricEffect ();
              G4ComptonScattering   * the_compton_scattering   = new G4ComptonScattering   ();
              G4GammaConversion     * the_gamma_conversion     = new G4GammaConversion     ();

              if (is_em_low_energy ())
                {
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
                }
              else if (is_em_penelope ())
                {
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

            }

          /**************
           *  electron  *
           **************/

          else if (particle_name == "e-")
            {
              if (_electron_multiple_scattering_)
                {
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
              if (_electron_energy_loss_)
                {
                  // Ionisation:
                  G4eIonisation * the_ionisation = new G4eIonisation ();

                  if (is_em_low_energy ())
                    {
                      // Livermore:
                      G4LivermoreIonisationModel* the_livermore_ionisation_model
                        = new G4LivermoreIonisationModel ();
                      the_ionisation->SetEmModel(the_livermore_ionisation_model);
                    }
                  else if (is_em_penelope ())
                    {
                      // Penelope:
                      G4PenelopeIonisationModel* the_penelope_ionisation_model
                        = new G4PenelopeIonisationModel ();
                      the_ionisation->SetEmModel(the_penelope_ionisation_model);
                    }

                  // Bremsstrahlung:
                  G4eBremsstrahlung * the_bremsstrahlung = new G4eBremsstrahlung ();
                  if (is_em_low_energy ())
                    {
                      // Livermore:
                      G4LivermoreBremsstrahlungModel * the_livermore_bremsstrahlung_model
                        = new G4LivermoreBremsstrahlungModel ();
                      the_bremsstrahlung->SetEmModel(the_livermore_bremsstrahlung_model);
                    }
                  if (is_em_penelope ())
                    {
                      // Penelope:
                      G4PenelopeBremsstrahlungModel* the_penelope_bremsstrahlung_model
                        = new G4PenelopeBremsstrahlungModel ();
                      the_bremsstrahlung->SetEmModel(the_penelope_bremsstrahlung_model);
                    }
                  pmanager->AddProcess (the_ionisation,       -1, 2, 2);
                  pmanager->AddProcess (the_bremsstrahlung,   -1, 3, 3);
                  pmanager->AddProcess (new G4StepLimiter,    -1,-1, 4);
                }
            }

          /**************
           *  positron  *
           **************/

          else if (particle_name == "e+")
            {
              if (_electron_multiple_scattering_)
                {
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
              if (_electron_energy_loss_)
                {
                  // Ionisation:
                  G4eIonisation * the_ionisation = new G4eIonisation ();
                  if (is_em_low_energy ())
                    {
                      // Livermore:
                      G4LivermoreIonisationModel * the_livermore_ionisation_model
                        = new G4LivermoreIonisationModel ();
                      the_ionisation->SetEmModel(the_livermore_ionisation_model);
                    }
                  else if (is_em_penelope ())
                    {
                      // Penelope:
                      G4PenelopeIonisationModel * the_penelope_ionisation_model
                        = new G4PenelopeIonisationModel ();
                      the_ionisation->SetEmModel(the_penelope_ionisation_model);
                    }

                  // Bremsstrahlung:
                  G4eBremsstrahlung * the_bremsstrahlung = new G4eBremsstrahlung ();
                  if (is_em_low_energy ())
                    {
                      // Livermore:
                      G4LivermoreBremsstrahlungModel * the_livermore_bremsstrahlung_model
                        = new G4LivermoreBremsstrahlungModel ();
                      the_bremsstrahlung->SetEmModel(the_livermore_bremsstrahlung_model);
                    }
                  if (is_em_penelope ())
                    {
                      // Penelope:
                      G4PenelopeBremsstrahlungModel * the_penelope_bremsstrahlung_model
                        = new G4PenelopeBremsstrahlungModel ();
                      the_bremsstrahlung->SetEmModel(the_penelope_bremsstrahlung_model);
                    }

                  // e+ annihilation:
                  G4eplusAnnihilation * the_eplus_annihilation = new G4eplusAnnihilation ();
                  if (is_em_penelope ())
                    {
                      G4PenelopeAnnihilationModel * the_penelope_annihilation_model
                        = new G4PenelopeAnnihilationModel ();
                      the_eplus_annihilation->SetModel(the_penelope_annihilation_model);
                    }

                  pmanager->AddProcess (the_ionisation,         -1, 2, 2);
                  pmanager->AddProcess (the_bremsstrahlung,     -1, 3, 3);
                  pmanager->AddProcess (the_eplus_annihilation,  0,-1, 4);
                  pmanager->AddProcess (new G4StepLimiter,      -1,-1, 5);
                }
            }


          /************
           *   ions   *
           ************/

          else if (particle_name == "alpha" ||
                   particle_name == "GenericIon")
            {
              G4hMultipleScattering * the_ion_multiple_scattering = new G4hMultipleScattering ();
              G4ionIonisation       * the_ion_ionisation          = new G4ionIonisation ();

              pmanager->AddProcess (the_ion_multiple_scattering , -1, 1, 1);
              pmanager->AddProcess (the_ion_ionisation,           -1, 2, 2);
              pmanager->AddProcess (new G4StepLimiter,            -1,-1, 3);
            }

          /*************
           *   muons   *
           *************/

          else if (particle_name == "mu+" ||
                   particle_name == "mu-")
            {
              if (_use_he_leptons_)
                {
                  pmanager->AddProcess (new G4MuMultipleScattering, -1,  1, 1);
                  pmanager->AddProcess (new G4MuIonisation,         -1,  2, 2);
                  pmanager->AddProcess (new G4MuBremsstrahlung,     -1,  3, 3);
                  pmanager->AddProcess (new G4MuPairProduction,     -1,  4, 4);
                  // pmanager->AddProcess (new G4StepLimiter,          -1, -1, 5);
                  // pmanager->AddProcess (new G4UserSpecialCuts,      -1, -1, 5);
                }
            }
          /***********************
           *   other particles   *
           ***********************/

          else
            {
              // all others charged particles
              if ((!particle->IsShortLived ()) &&
                  (particle->GetPDGCharge () != 0.0))
                {
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
      while ((*theParticleIterator)())
        {
          G4ParticleDefinition * particle = theParticleIterator->value ();
          G4ProcessManager     * pmanager = particle->GetProcessManager ();

          G4String particle_name = particle->GetParticleName ();

          if (is_debug ())
            {
              clog << datatools::io::debug
                   << "mctools::g4::physics_list::_ConstructHadronicProcess: "
                   << "Set hadronic processes for '"
                   << particle_name << "'" << endl;
            }

          if (particle_name == "neutron")
            {
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
            }

          else if (particle_name == "anti_neutron")
            {
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
            }
          else
            {
              if (particle_name == "pi+")
                {
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
                }
              else if (particle_name == "pi-")
                {
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
                }
              else if (particle_name == "kaon+")
                {
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
                }
              else if (particle_name == "kaon-")
                {
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
                }
              else if (particle_name == "kaon0S")
                {
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
                }
              else if (particle_name == "kaon0L")
                {
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
                }
              else if (particle_name == "proton")
                {
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
                }
              else if (particle_name == "anti_proton")
                {
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
                }
              else if (particle_name == "deuteron")
                {
                  pmanager->AddDiscreteProcess (the_elastic_process);
                  G4DeuteronInelasticProcess * the_inelastic_process
                    = new G4DeuteronInelasticProcess ("inelastic");
                  G4LEDeuteronInelastic * the_low_energy_inelastic_model
                    = new G4LEDeuteronInelastic ();
                  the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
                  pmanager->AddDiscreteProcess (the_inelastic_process);
                }
              else if (particle_name == "triton")
                {
                  pmanager->AddDiscreteProcess (the_elastic_process);
                  G4TritonInelasticProcess * the_inelastic_process
                    = new G4TritonInelasticProcess ("inelastic");
                  G4LETritonInelastic * the_low_energy_inelastic_model
                    = new G4LETritonInelastic ();
                  the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
                  pmanager->AddDiscreteProcess (the_inelastic_process);
                }
              else if (particle_name == "alpha")
                {
                  pmanager->AddDiscreteProcess (the_elastic_process);
                  G4AlphaInelasticProcess * the_inelastic_process
                    = new G4AlphaInelasticProcess ("inelastic");
                  G4LEAlphaInelastic * the_low_energy_inelastic_model
                    = new G4LEAlphaInelastic ();
                  the_inelastic_process->RegisterMe (the_low_energy_inelastic_model);
                  pmanager->AddDiscreteProcess (the_inelastic_process);
                }
              else
                {
                  if (is_debug ())
                    {
                      clog << datatools::io::debug
                           << "mctools::g4::physics_list::_ConstructHadronicProcess: "
                           << "Particle '" << particle_name
                           << "' has no hadronic processes"
                           << endl;
                    }
                }
            }
          }
      return;
    }

    void physics_list::_ConstructGeneral ()
    {
      // Add Decay Process
      G4Decay * theDecayProcess = new G4Decay ();
      theParticleIterator->reset ();
      while ((*theParticleIterator) ())
        {
          G4ParticleDefinition * particle = theParticleIterator->value ();
          G4ProcessManager     * pmanager = particle->GetProcessManager ();
          if (theDecayProcess->IsApplicable (*particle))
            {
              pmanager->AddProcess (theDecayProcess);
              // set ordering for PostStepDoIt and AtRestDoIt
              pmanager->SetProcessOrdering (theDecayProcess, idxPostStep);
              pmanager->SetProcessOrdering (theDecayProcess, idxAtRest);
            }
        }
      return;
    }

    /*

SNOVA sample:

# Cut (mm) for Calorimeter region
PHYSICS calo_cut D 5.

# Cut (mm) for Tracking region
PHYSICS tracking_cut D 5.

# Cut (mm) for Source region
PHYSICS source_cut D .05

    */

    void physics_list::SetCuts ()
    {
      bool devel = false;

      if (devel)
        {
          clog << datatools::io::devel
               << "mctools::g4::physics_list::SetCuts: Entering..."
               << endl;
        }

      // G4VUserPhysicsList::SetCutsWithDefault method sets
      // the default cut value for all particle types
      //
      SetCutsWithDefault ();

      if (_using_cuts_)
        {
          if (devel)
            {
              clog << datatools::io::devel
                   << "mctools::g4::physics_list::SetCuts: Using cuts..."
                   << endl;
            }
          // Defining cuts for different particles:
          //SetCutValue (_gamma_cut_,    "gamma");
          //SetCutValue (_electron_cut_, "e-");
          //SetCutValue (_positron_cut_, "e+");

          // Defining cuts for the different regions :
          for (map<string, double>::const_iterator i = _region_cuts_.begin ();
               i != _region_cuts_.end ();
               i++)
            {
              const string & region_name = i->first;
              double region_cut = i->second;
              G4Region * a_region = G4RegionStore::GetInstance ()->GetRegion (region_name);
              if (a_region != 0)
                {
                  G4ProductionCuts * a_region_cuts = new G4ProductionCuts;
                  a_region_cuts->SetProductionCut (region_cut);
                  a_region->SetProductionCuts (a_region_cuts);
                }
              else
                {
                  ostringstream message;
                  message << "mctools::g4::physics_list::SetCuts: "
                          << "No region named '"
                          << region_name << "' to be given production cuts !";
                  clog << datatools::io::warning << message.str () << endl;
                }
            }
        }

      // Verbosing:
      DumpCutValuesTable ();

      if (devel)
        {
          clog << datatools::io::devel
               << "mctools::g4::physics_list::SetCuts: Exiting"
               << endl;
        }
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
      if (! title_.empty ())
        {
          out_ << indent << title_ << endl;
        }

      out_ << indent << du::i_tree_dumpable::tag
           << "Initialized                  : " <<  (_initialized_ ? "Yes": "No") << endl;

      out_ << indent << du::i_tree_dumpable::tag
           << "Debug                        : " <<  (_debug_ ? "Yes": "No") << endl;

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
           << "Use leptons                  : " << (_use_he_leptons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use mesons                   : " << (_use_mesons_ ? "Yes" : "No") << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Use neutrons                 : " << (_use_neutrons_ ? "Yes" : "No") << endl;

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
             i++)
          {
            out_ << indent << du::i_tree_dumpable::skip_tag;
            map<string, double>::const_iterator j = i;
            j++;
            if (j == _region_cuts_.end ())
              {
                out_ << du::i_tree_dumpable::last_tag;
              }
            else
              {
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

// end of physics_list.cc

