// base_physics_constructor.cc

// Ourselves:
#include <mctools/g4/base_physics_constructor.h>

// Third party:
// - Geant4:
#include <G4BuilderType.hh>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace mctools {

  namespace g4 {

    std::string g4_builder_type_to_label(int bt_)
    {
      switch (bt_) {
      case bUnknown         : return std::string("unknown");
      case bTransportation  : return std::string("transportation");
      case bElectromagnetic : return std::string("electromagnetic");
      case bEmExtra         : return std::string("electromagnetic_extra");
      case bDecay           : return std::string("decay");
      case bHadronElastic   : return std::string("hadron_elastic");
      case bHadronInelastic : return std::string("hadron_inelastic");
      case bStopping        : return std::string("stopping");
      case bIons            : return std::string("ions");
      default               : return std::string("invalid");
      }
    }

    int label_to_g4_builder_type(const std::string & label_)
    {
      if (label_ == g4_builder_type_to_label(bUnknown)) return bUnknown;
      if (label_ == g4_builder_type_to_label(bTransportation)) return bTransportation;
      if (label_ == g4_builder_type_to_label(bElectromagnetic)) return bElectromagnetic;
      if (label_ == g4_builder_type_to_label(bEmExtra)) return bEmExtra;
      if (label_ == g4_builder_type_to_label(bDecay)) return bDecay;
      if (label_ == g4_builder_type_to_label(bHadronElastic)) return bHadronElastic;
      if (label_ == g4_builder_type_to_label(bHadronInelastic)) return bHadronInelastic;
      if (label_ == g4_builder_type_to_label(bStopping)) return bStopping;
      if (label_ == g4_builder_type_to_label(bIons)) return bIons;
      return -1; // Special invalid code
    }

    DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_physics_constructor, "mctools::g4::base_physics_constructor/__system__")

    bool base_physics_constructor::is_initialized() const
    {
      return _initialized_;
    }

    void base_physics_constructor::set_name(const std::string & name_)
    {
      _name_ = name_;
      return;
    }

    const std::string & base_physics_constructor::get_name() const
    {
      return _name_;
    }

    void base_physics_constructor::set_class_id(const std::string & class_id_)
    {
      _class_id_ = class_id_;
      return;
    }

    const std::string & base_physics_constructor::get_class_id() const
    {
      return _class_id_;
    }

    bool base_physics_constructor::has_mother_physics_list() const
    {
      return _mother_physics_list_ != 0;
    }

    const physics_list & base_physics_constructor::get_mother_physics_list() const
    {
      DT_THROW_IF(_mother_physics_list_ == 0, std::logic_error, "No mother physics list !");
      return *_mother_physics_list_;
    }

    base_physics_constructor::base_physics_constructor() :
      G4VPhysicsConstructor()
    {
      _initialized_= false;
      _mother_physics_list_ = 0;
      return;
    }

    base_physics_constructor::~base_physics_constructor()
    {
      if (_initialized_) {
        DT_LOG_WARNING(_logprio(), "Hum! Initialization flag is set at destruction stage ! "
                       << "This may be a problem !");
      }
      return;
    }

    void base_physics_constructor::ConstructParticle()
    {
      return;
    }

    void base_physics_constructor::ConstructProcess()
    {
      return;
    }

    void base_physics_constructor::initialize_standalone(const datatools::properties & config_)
    {
      physics_constructor_dict_type dummy_dict;
      initialize(config_, dummy_dict);
      return;
    }

    void base_physics_constructor::_reset()
    {
      _mother_physics_list_ = 0;
      _name_.clear();
      _class_id_.clear();
      return;
    }

    void base_physics_constructor::_set_initialized(bool i)
    {
      _initialized_ = i;
      return;
    }

    void base_physics_constructor::_set_mother_physics_list(physics_list & mother_)
    {
      _mother_physics_list_ = &mother_;
      return;
    }

    void base_physics_constructor::tree_dump(std::ostream & out_,
                                             const std::string & title_,
                                             const std::string & indent_,
                                             bool inherit_) const
    {
      if (! title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Name                  : '" << _name_ << "'" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Class ID              : '" << _class_id_ << "'" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Logging priority      : '"
           << datatools::logger::get_priority_label(_logprio()) << "'" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Mother physics list   : ";
      if (_mother_physics_list_) {
        out_ << "[@" << _mother_physics_list_ << "]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized           : " << (is_initialized() ? "Yes": "No") << "" << std::endl;

      return;
    }

  } // end of namespace g4

} // end of namespace mctools
