// em_model_factory.cc

// Ourselves:
#include <mctools/g4/processes/em_model_factory.h>

// Third party:
// Geant4:
// Base model:
// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4DummyModel.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// EM standard model:
#include <G4BraggIonModel.hh>
#include <G4ICRU73QOModel.hh>
#include <G4BetheBlochIonGasModel.hh>
#include <G4hCoulombScatteringModel.hh>
#include <G4WentzelVIRelModel.hh>
#include <G4BetheBlochModel.hh>
#include <G4UrbanMscModel93.hh>
#include <G4eBremParametrizedModel.hh>
#include <G4PAIModel.hh>
#include <G4WentzelVIModel.hh>
#include <G4UrbanMscModel96.hh>
#include <G4XrayRayleighModel.hh>
#include <G4BetheHeitlerModel.hh>
#include <G4eBremsstrahlungRelModel.hh>
#include <G4UrbanMscModel92.hh>
#include <G4KleinNishinaModel.hh>
#include <G4eBremsstrahlungModel.hh>
#include <G4IonCoulombScatteringModel.hh>
#include <G4PairProductionRelModel.hh>
#include <G4eeToTwoGammaModel.hh>
#include <G4eCoulombScatteringModel.hh>
#include <G4MollerBhabhaModel.hh>
#include <G4BraggIonGasModel.hh>
#include <G4UrbanMscModel95.hh>
#include <G4BraggModel.hh>
#include <G4GoudsmitSaundersonMscModel.hh>
#include <G4PAIPhotonModel.hh>
#include <G4SeltzerBergerModel.hh>
#include <G4PEEffectModel.hh>
#include <G4UrbanMscModel90.hh>
#include <G4ICRU49NuclearStoppingModel.hh>
#include <G4PEEffectFluoModel.hh>
#include <G4eSingleCoulombScatteringModel.hh>

// Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

namespace mctools {

  namespace g4 {

    namespace processes {

      em_model_factory::em_model_factory()
      {
        _initialized_ = false;
        return;
      }

      em_model_factory::~em_model_factory()
      {
        if (is_initialized()) {
          reset();
        }
        return;
      }

      bool em_model_factory::is_initialized() const
      {
        return _initialized_;
      }

      void em_model_factory::initialize()
      {
        DT_THROW_IF(is_initialized(), std::logic_error,
                    "EM model factory is already initialized!");
        _registration();
        _initialized_ = true;
        return;
      }

      void em_model_factory::reset()
      {
        DT_THROW_IF(!is_initialized(), std::logic_error,
                    "EM model factory is not initialized!");
        _initialized_ = false;
        return;
      }

      bool em_model_factory::has_model_type(const std::string & model_type_id_) const
      {
        return _reg_.has(model_type_id_);
      }

      G4VEmModel *
      em_model_factory::create_model(const std::string & model_type_id_,
                                     const std::string & model_name_)
      {
        datatools::properties dummy;
        return create_model(model_type_id_, model_name_, dummy);
      }

      G4VEmModel *
      em_model_factory::create_model(const std::string & model_type_id_,
                                     const std::string & model_name_,
                                     const datatools::properties & model_config_)
      {
        DT_THROW_IF(!is_initialized(), std::logic_error,
                    "EM model factory is not initialized!");
        G4VEmModel * model = 0;

        model = _create_model(model_type_id_, model_name_, model_config_);

        return model;
      }

      G4VEmModel *
      em_model_factory::_create_model(const std::string & model_type_id_,
                                      const std::string & model_name_,
                                      const datatools::properties & /* model_config_ */)
      {
        G4VEmModel * model = 0;

        if (_reg_.has(model_type_id_)) {
          const fact_reg_type::factory_type & f = _reg_.get(model_type_id_);
          model = f();
          _change_model_name_(*model, model_name_);
        }

        return model;
      }

      // static
      void em_model_factory::_change_model_name_(G4VEmModel & model_,
                                                 const std::string & name_)
      {
        const G4String * g4name = &model_.GetName();
        G4String * mutable_g4name = const_cast<G4String *>(g4name);
        *mutable_g4name = name_;
        return;
      }

      void em_model_factory::_registration()
      {
        _registration_base();
        _registration_standard();
        // _registration_lowenergy();
        // _registration_highenergy();
        // _registration_pii();
        // _registration_muons();
        // _registration_polarisation();
        // _registration_xrays();
        // _registration_dna();

        return;
      }

      void em_model_factory::_registration_base()
      {
        _reg_.registration("Dummy", boost::factory<G4DummyModel*>());
        return;
      }

      void em_model_factory::_registration_standard()
      {
        /*

          $ cd {the-Geant4-base-source-directory}

          Find the EM model class name:
          $ find source/processes/electromagnetic/standard/ -name "G4*Model*.hh" | cut -d/ -f6 | cut -d. -f1 > em_model_std.lis

          Automatically build the registration instructions:

          $ cat em_model_std.lis | awk -f process_em_model_list.awk

          with Awk script:
          @code
          BEGIN {
          printf("// List of standard EM models:\n");
          }
          (NF==1){
          class_id=$1;
          gsub("G4","",class_id)
          gsub("Model","",class_id)
          printf("_reg_.registration(\"%s\", boost::factory<%s*>());\n", class_id, $1);
          }
          END{
          printf("// End of list.\n");
          }
          @encode
        */
        // List of standard EM models:
        _reg_.registration("BraggIon", boost::factory<G4BraggIonModel*>());
        _reg_.registration("ICRU73QO", boost::factory<G4ICRU73QOModel*>());
        _reg_.registration("BetheBlochIonGas", boost::factory<G4BetheBlochIonGasModel*>());
        _reg_.registration("hCoulombScattering", boost::factory<G4hCoulombScatteringModel*>());
        _reg_.registration("WentzelVIRel", boost::factory<G4WentzelVIRelModel*>());
        _reg_.registration("BetheBloch", boost::factory<G4BetheBlochModel*>());
        _reg_.registration("UrbanMsc93", boost::factory<G4UrbanMscModel93*>());
        _reg_.registration("eBremParametrized", boost::factory<G4eBremParametrizedModel*>());
        _reg_.registration("PAI", boost::factory<G4PAIModel*>());
        _reg_.registration("WentzelVI", boost::factory<G4WentzelVIModel*>());
        _reg_.registration("UrbanMsc96", boost::factory<G4UrbanMscModel96*>());
        _reg_.registration("XrayRayleigh", boost::factory<G4XrayRayleighModel*>());
        _reg_.registration("BetheHeitler", boost::factory<G4BetheHeitlerModel*>());
        _reg_.registration("eBremsstrahlungRel", boost::factory<G4eBremsstrahlungRelModel*>());
        _reg_.registration("UrbanMsc92", boost::factory<G4UrbanMscModel92*>());
        _reg_.registration("KleinNishina", boost::factory<G4KleinNishinaModel*>());
        _reg_.registration("eBremsstrahlung", boost::factory<G4eBremsstrahlungModel*>());
        _reg_.registration("IonCoulombScattering", boost::factory<G4IonCoulombScatteringModel*>());
        _reg_.registration("PairProductionRel", boost::factory<G4PairProductionRelModel*>());
        _reg_.registration("eeToTwoGamma", boost::factory<G4eeToTwoGammaModel*>());
        _reg_.registration("eCoulombScattering", boost::factory<G4eCoulombScatteringModel*>());
        _reg_.registration("MollerBhabha", boost::factory<G4MollerBhabhaModel*>());
        _reg_.registration("BraggIonGas", boost::factory<G4BraggIonGasModel*>());
        _reg_.registration("UrbanMsc95", boost::factory<G4UrbanMscModel95*>());
        _reg_.registration("Bragg", boost::factory<G4BraggModel*>());
        _reg_.registration("GoudsmitSaundersonMsc", boost::factory<G4GoudsmitSaundersonMscModel*>());
        _reg_.registration("PAIPhoton", boost::factory<G4PAIPhotonModel*>());
        _reg_.registration("SeltzerBerger", boost::factory<G4SeltzerBergerModel*>());
        _reg_.registration("PEEffect", boost::factory<G4PEEffectModel*>());
        _reg_.registration("UrbanMsc90", boost::factory<G4UrbanMscModel90*>());
        _reg_.registration("ICRU49NuclearStopping", boost::factory<G4ICRU49NuclearStoppingModel*>());
        _reg_.registration("PEEffectFluo", boost::factory<G4PEEffectFluoModel*>());
        _reg_.registration("eSingleCoulombScattering", boost::factory<G4eSingleCoulombScatteringModel*>());
        // End of list of standard EM models.

        return;
      }

      // static
      const em_model_factory & em_model_factory::get_instance()
      {
        return const_cast<em_model_factory &>(_instance_());
      }

      // static
      em_model_factory & em_model_factory::grab_instance()
      {
        return _instance_();
      }

      // static
      em_model_factory & em_model_factory::_instance_()
      {
        static boost::scoped_ptr<em_model_factory> _fact;
        if (! _fact) {
          DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                        "Instantation of the static EM model factory...");
          _fact.reset(new em_model_factory);
          _fact->initialize();
        }
        return *_fact;
      }

    } // end of namespace processes

  } // end of namespace g4

} // end of namespace mctools
