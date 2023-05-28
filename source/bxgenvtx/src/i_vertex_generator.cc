// i_vertex_generator.cc

// Ourselves:
#include <genvtx/i_vertex_generator.h>

// Standard library:
#include <string>
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools
#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/version_id.h>
#include <datatools/version_check.h>
#include <datatools/utils.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
// - Bayeux/mygsl
#include <mygsl/rng.h>
// - Bayeux/geomtools
#include <geomtools/manager.h>
#include <geomtools/geometry_service.h>

// This project:
#include <genvtx/utils.h>
#include <genvtx/vertex_validation.h>

namespace genvtx {

  // Factory system register :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_vertex_generator,
                                                   "genvtx::i_vertex_generator/_system_")

  const std::string & i_vertex_generator::get_name() const
  {
    return _name_;
  }

  bool i_vertex_generator::has_name() const
  {
    return ! _name_.empty();
  }

  void i_vertex_generator::set_vertex_validation_support(bool val_)
  {
    DT_THROW_IF (is_initialized(),
                std::logic_error,
                "Vertex generator '" << get_name() << "' is initialized/locked !");
    DT_THROW_IF (_vertex_validation_,
                std::logic_error,
                "Vertex generator '" << get_name() << "' already has embedded vertex validation !");
     _vertex_validation_support_ = val_;
    return;
  }

  bool i_vertex_generator::is_vertex_validation_supported() const
  {
    return _vertex_validation_support_;
  }

  void i_vertex_generator::set_name(const std::string & name_)
  {
    DT_THROW_IF (is_initialized(),
                std::logic_error,
                "Vertex generator '" << get_name() << "' is initialized/locked !");
    DT_THROW_IF(! ::datatools::name_validation(name_,
                                               ::datatools::NV_NO_HYPHEN
                                               | ::datatools::NV_NO_COLON),
                std::logic_error,
                "Invalid name '" << name_ << "' for vertex generator!");
    _name_ = name_;
  }

  datatools::logger::priority
  i_vertex_generator::get_logging_priority() const
  {
    return _logging_priority;
  }

  void i_vertex_generator::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority = p_;
  }


  bool i_vertex_generator::is_debug() const
  {
    return _logging_priority >= datatools::logger::PRIO_DEBUG;
  }

  void i_vertex_generator::set_debug(bool debug_)
  {
    if (debug_) {
      _logging_priority = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority = datatools::logger::PRIO_FATAL;
    }
    return;
  }

  bool i_vertex_generator::has_geom_setup_requirement() const
  {
    return ! _geom_setup_requirement_.empty();
  }

  void i_vertex_generator::set_geom_setup_requirement(const std::string & geom_setup_requirement_)
  {
    DT_THROW_IF (is_initialized(),
                std::logic_error,
                "Vertex generator '" << get_name() << "' is initialized/locked !");
    _geom_setup_requirement_ = geom_setup_requirement_;
    return;
  }

  std::string i_vertex_generator::get_geom_setup_requirement() const
  {
    return _geom_setup_requirement_;
  }

  bool i_vertex_generator::has_geo_label() const
  {
    return ! _geo_label_.empty();
  }

  void i_vertex_generator::set_geo_label(const std::string & geo_label_)
  {
    _geo_label_ = geo_label_;
    return;
  }

  const std::string & i_vertex_generator::get_geo_label() const
  {
    return _geo_label_;
  }

  void i_vertex_generator::_set_total_weight(const weight_info & a_info)
  {
    _total_weight_ = a_info;
    return;
  }

  bool i_vertex_generator::has_total_weight() const
  {
    return _total_weight_.is_valid();
  }

  const weight_info & i_vertex_generator::get_total_weight() const
  {
    return _total_weight_;
  }

  weight_info & i_vertex_generator::_grab_total_weight()
  {
    return _total_weight_;
  }

  bool i_vertex_generator::has_geom_manager() const
  {
    return _geom_manager_ != 0;
  }

  void i_vertex_generator::check_geom_setup_requirement(const geomtools::manager * gmgr_) const
  {
    // Default check the embedded geometry manager if any :
    const geomtools::manager * gmgr = _geom_manager_;
    if (gmgr_ != 0) {
      // Or check the argument geometry manager:
      gmgr = gmgr_;
    }
    if (gmgr != 0 && has_geom_setup_requirement()) {
        // Example : "snemo"
        const std::string & geom_mgr_setup_label = gmgr->get_setup_label();
        // Example : "2.0"
        const std::string & geom_mgr_setup_version = gmgr->get_setup_version();
        datatools::version_id geom_mgr_setup_vid;
        {
          std::istringstream iss(geom_mgr_setup_version);
          iss >> geom_mgr_setup_vid;
        }
        DT_THROW_IF ((! datatools::validate_version(geom_mgr_setup_label,
                                                     geom_mgr_setup_vid,
                                                     get_geom_setup_requirement())),
                     std::logic_error,
                     "Geometry manager setup label '" << gmgr->get_setup_label()
                     << "' with version '" << geom_mgr_setup_vid
                     << "' does not match the requested setup requirement '"
                     << get_geom_setup_requirement() << "' !");
        DT_LOG_NOTICE(get_logging_priority(),
                      "Geometry manager setup label '" << gmgr->get_setup_label()
                      << "' with version '" << geom_mgr_setup_vid
                      << "' matches the requested setup requirement '"
                      << get_geom_setup_requirement() << "'.");
      }
    return;
  }

  void i_vertex_generator::set_geom_manager(const geomtools::manager & gmgr_)
  {
    DT_THROW_IF (is_initialized(),
                std::logic_error,
                "Vertex generator '" << get_name() << "' is initialized/locked !");
    check_geom_setup_requirement(&gmgr_);
    _geom_manager_ = &gmgr_;
    return;
  }

  const geomtools::manager & i_vertex_generator::get_geom_manager() const
  {
    return *_geom_manager_;
  }

  bool i_vertex_generator::has_vertex_validation() const
  {
    return is_vertex_validation_supported()
      && _vertex_validation_
      && _vertex_validation_->is_initialized();
  }

  bool i_vertex_generator::has_external_prng() const
  {
    return _external_prng_ != 0;
  }

  void i_vertex_generator::set_external_prng(mygsl::rng & prng_)
  {
    _external_prng_ = &prng_;
    return;
  }

  bool i_vertex_generator::has_prng() const
  {
    return has_external_prng();
  }

  mygsl::rng & i_vertex_generator::grab_prng()
  {
    DT_THROW_IF (! has_external_prng(), std::logic_error,
                 "No available PRNG in vertex generator '" << get_name() << "' !");
    return *_external_prng_;
  }

  bool i_vertex_generator::is_time_generator() const
  {
    return _time_generator_;
  }

  i_vertex_generator::i_vertex_generator()
  {
    _logging_priority = datatools::logger::PRIO_FATAL;
    _geom_manager_ = 0;
    _total_weight_.invalidate();
    _external_prng_ = 0;
    _vertex_validation_support_ = true;
    _time_generator_ = false;
    return;
  }

  vertex_validation & i_vertex_generator::_grab_vertex_validation()
  {
     DT_THROW_IF (! _vertex_validation_, std::logic_error,
                 "No vertex validation !");
     return *_vertex_validation_;
  }

  i_vertex_generator::~i_vertex_generator()
  {
    return;
  }

  bool i_vertex_generator::has_next_vertex() const
  {
    return true;
  }
  
  std::size_t i_vertex_generator::get_counter() const
  {
    return _counter_;
  }

  void i_vertex_generator::reset_counter()
  {
    _counter_ = 0;
    return;
  }

  void i_vertex_generator::_shoot_vertex_(mygsl::rng & random_,
                                          geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF(is_time_generator(), std::logic_error, "This is a vertex/time generator!");
    if (is_time_generator()) {
      double time = datatools::invalid_real();
      _shoot_vertex_and_time(random_, vertex_, time);
    } else {
      _shoot_vertex(random_, vertex_);
    }
    return;
  }

  void i_vertex_generator::_shoot_vertex_and_time_(mygsl::rng & random_,
                                                   geomtools::vector_3d & vertex_,
                                                   double & time_)
  {
    DT_THROW_IF(!is_time_generator(), std::logic_error, "This generator is not able to generate a decay time associated to random vertex!");
    _shoot_vertex_and_time(random_, vertex_, time_);
    return;
  }

  // virtual
  void i_vertex_generator::_shoot_vertex(mygsl::rng & /*random_*/,
                                         geomtools::vector_3d & /*vertex_*/)
  {
    DT_THROW(std::logic_error, "This method is not implemented ! "
             << "It should be overloaded for the class of the '" << get_name() << "' vertex generator!");
    return;
  }

  // virtual
  void i_vertex_generator::_shoot_vertex_and_time(mygsl::rng & /*random_*/,
                                                  geomtools::vector_3d & /*vertex_*/,
                                                  double & /*time_*/)
  {
    DT_THROW(std::logic_error, "This method is not implemented ! "
             << "It should be overloaded for the class of the '" << get_name() << "' vertex generator!");
    return;
  }

  void i_vertex_generator::shoot_vertex_and_time(geomtools::vector_3d & vertex_,
                                                 double & time_)
  {
    DT_THROW_IF (_external_prng_ == 0, std::logic_error,
                 "Missing external PRNG handle in vertex/time generator '" << get_name() << "' !");
    shoot_vertex_and_time(*_external_prng_, vertex_, time_);
    return;
  }

  void i_vertex_generator::shoot_vertex_and_time(mygsl::rng & random_,
                                                 geomtools::vector_3d & vertex_,
                                                 double & time_)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    do {
      _shoot_vertex_and_time_(random_, vertex_, time_);
      if (is_vertex_validation_supported() && has_vertex_validation()) {
        // If a vertex validation manager is installed, we use it...
        // Check the validity of the geometry context associated to the candidate vertex:
        DT_THROW_IF (! _grab_vertex_validation().grab_geometry_context().is_valid(),
                     std::logic_error,
                     "Vertex generator '" << get_name()  << "' : "
                     << "Invalid geometry context for vertex validation !");
        // Invoke the validator:
        DT_LOG_TRACE(get_logging_priority(), "Invoking the validator...");
        vertex_validation::validate_status_type status = _grab_vertex_validation().validate();
        if (status == vertex_validation::VS_ACCEPTED) {
          _grab_vertex_validation().grab_geometry_context().reset();
          break;
        } else if (status == vertex_validation::VS_REJECTED) {
          _grab_vertex_validation().grab_geometry_context().reset();
        } else if (status == vertex_validation::VS_MAXTRIES) {
          DT_THROW_IF (true, std::logic_error,
                       "Vertex generator '" << get_name()  << "' : "
                       << "Number of vertex shots exceeds the maximum number of allowed tries !");
          break;
        } else {
          DT_THROW_IF (true, std::logic_error,
                       "Vertex generator '" << get_name()  << "' : "
                       << "It was not possible to apply vertex validation !");
        }
      } else {
        // Otherwise, return the randomize vertex/time
        break;
      }
    } while(true);
    _counter_++;
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

  void i_vertex_generator::shoot_vertex(geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (_external_prng_ == 0, std::logic_error,
                 "Missing external PRNG handle in vertex generator '" << get_name() << "' !");
    shoot_vertex(*_external_prng_, vertex_);
    return;
  }

  void i_vertex_generator::shoot_vertex(mygsl::rng & random_,
                                        geomtools::vector_3d & vertex_)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    do {
      _shoot_vertex(random_, vertex_);
      if (is_vertex_validation_supported() && has_vertex_validation()) {
        // If a vertex validation manager is installed, we use it...
        // Check the validity of the geometry context associated to the candidate vertex:
        DT_THROW_IF (! _grab_vertex_validation().grab_geometry_context().is_valid(),
                     std::logic_error,
                     "Vertex generator '" << get_name()  << "' : "
                     << "Invalid geometry context for vertex validation !");
        // Invoke the validator:
        DT_LOG_TRACE(get_logging_priority(), "Invoking the validator...");
        vertex_validation::validate_status_type status = _grab_vertex_validation().validate();
        if (status == vertex_validation::VS_ACCEPTED) {
          _grab_vertex_validation().grab_geometry_context().reset();
          break;
        } else if (status == vertex_validation::VS_REJECTED) {
          _grab_vertex_validation().grab_geometry_context().reset();
        } else if (status == vertex_validation::VS_MAXTRIES) {
          DT_THROW_IF (true, std::logic_error,
                       "Vertex generator '" << get_name()  << "' : "
                       << "Number of vertex shots exceeds the maximum number of allowed tries !");
          break;
        } else {
          DT_THROW_IF (true, std::logic_error,
                       "Vertex generator '" << get_name()  << "' : "
                       << "It was not possible to apply vertex validation !");
        }
      } else {
        // Otherwise, return the randomize vertex
        break;
      }
    } while(true);
    _counter_++;
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

  geomtools::vector_3d i_vertex_generator::shoot_vertex(mygsl::rng & random_)
  {
    geomtools::vector_3d vertex;
    shoot_vertex(random_, vertex);
    return vertex;
  }

  void i_vertex_generator::_initialize(const datatools::properties & setup_,
                                       datatools::service_manager & service_manager_)
  {
    i_vertex_generator::_initialize_basics(setup_, service_manager_);
    i_vertex_generator::_initialize_geo_manager(setup_, service_manager_);
    i_vertex_generator::_initialize_vertex_validation(setup_, service_manager_);
    return;
  }

  void i_vertex_generator::_initialize_basics(const datatools::properties & setup_,
                                              datatools::service_manager & /*service_manager_*/)
  {
    // Fetch configuration parameters :

    // Logging priority:
    datatools::logger::priority lp
      = datatools::logger::extract_logging_configuration(setup_,
                                                         datatools::logger::PRIO_FATAL);
    DT_THROW_IF (lp ==  datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority !");
    set_logging_priority(lp);

    // Name of the generator(only if not already set) :
    if (_name_.empty()) {
      if (setup_.has_key("name")) {
        set_name(setup_.fetch_string("name"));
      }
    }

    // Set vertex validation support :
    if (setup_.has_key("vertex_validation_support")) {
      set_vertex_validation_support(setup_.fetch_boolean("vertex_validation_support"));
    }

    // Required geometry setup label :
    if (setup_.has_key("geometry.setup_requirement")) {
      DT_LOG_NOTICE(get_logging_priority(),
                    "Loading 'geometry.setup_requirement' rules...");
      std::string geom_setup_requirement = setup_.fetch_string("geometry.setup_requirement");
      set_geom_setup_requirement(geom_setup_requirement);
    }

    // Do we need support for embeded PRNG ???
    check_geom_setup_requirement(0);

    return;
  }

  void i_vertex_generator::_initialize_vertex_validation(const datatools::properties & setup_,
                                                         datatools::service_manager & /* service_manager_ */)
  {
    if (! is_vertex_validation_supported()) {
      // Protect against vertex validation initialization if this feature is not supported:
      return;
    }



    bool vtx_validation = false;
    if (setup_.has_key("validation.activation")) {
      vtx_validation = setup_.fetch_boolean("validation.activation");
    }

    if (!vtx_validation) {
      // Do not activate vertex validation:
      return;
    }

    _vertex_validation_.reset(new vertex_validation);
    datatools::properties vtx_validation_setup;
    setup_.export_and_rename_starting_with(vtx_validation_setup, "validation.", "");
    _vertex_validation_->initialize(vtx_validation_setup);

    return;
  }

  void i_vertex_generator::_initialize_geo_manager(const datatools::properties & setup_,
                                                   datatools::service_manager & service_manager_)
  {
    // Geometry manager :
    if (_geom_manager_ == 0) {
        // Only if geometry manager is not already set :
        if (_geo_label_.empty()) {
            // Service labels :
            DT_THROW_IF (!setup_.has_key("Geo_label"),
                         std::logic_error,
                         "Missing  '" << "Geo_label" << "' property in vertex generator '" << get_name() << "' !");
            _geo_label_ = setup_.fetch_string("Geo_label");
          }

        DT_THROW_IF (_geo_label_.empty(),
                     std::logic_error,
                     "Invalid '" << "Geo_label" << "' property in vertex generator '" << get_name() << "' !");
        DT_THROW_IF (! service_manager_.has(_geo_label_)
                     && service_manager_.is_a<geomtools::geometry_service>(_geo_label_),
                     std::logic_error,
                     "Cannot find '" << _geo_label_ << "' service for vertex generator '" << get_name() << "' !");
        geomtools::geometry_service & Geo
          = service_manager_.grab<geomtools::geometry_service>(_geo_label_);
        set_geom_manager(Geo.get_geom_manager());
    }

    return;
  }

  void i_vertex_generator::initialize_simple()
  {
    datatools::properties dummy_setup;
    initialize_standalone(dummy_setup);
    return;
  }

  void i_vertex_generator::initialize_standalone(const datatools::properties & setup_)
  {
    datatools::service_manager dummy_srvcmgr;
    vg_dict_type dummy_dict;
    initialize(setup_, dummy_srvcmgr, dummy_dict);
    return;
  }

  void i_vertex_generator::initialize_with_dictionary_only(const datatools::properties & setup_,
                                                            vg_dict_type & dictionary_)
  {
    datatools::service_manager dummy_srvcmgr;
    initialize(setup_, dummy_srvcmgr, dictionary_);
    return;
  }

  void i_vertex_generator::initialize_with_service_only(const datatools::properties & setup_,
                                                         datatools::service_manager & service_manager_)
  {
    vg_dict_type dummy_dict;
    initialize(setup_, service_manager_, dummy_dict);
    return;
  }


  void i_vertex_generator::_reset()
  {
    _name_.clear();
    _geo_label_.clear();
    _geom_setup_requirement_.clear();
    _geom_manager_ = 0;
    _time_generator_ = false;
    _total_weight_.invalidate();
    _logging_priority = datatools::logger::PRIO_FATAL;
    if (_vertex_validation_) {
      _vertex_validation_.reset();
    }
    return;
  }

  void i_vertex_generator::_set_time_generator(bool tg_)
  {
    _time_generator_ = tg_;
    return;
  }

  void i_vertex_generator::tree_dump(std::ostream & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Name        : '" << _name_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized : " <<(is_initialized() ? "Yes": "No") << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Logging priority : " << datatools::logger::get_priority_label(_logging_priority) << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Geo label        : '" << _geo_label_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Geometry setup requirement : '" << _geom_setup_requirement_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Geometry manager : " << _geom_manager_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Time generator : " << _time_generator_ << std::endl;

    if (has_total_weight()) {
      // out_ << indent << datatools::i_tree_dumpable::tag
      //      << "Weight info " << _total_weight_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "External PRNG : " << _external_prng_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Vertex validation support : " << _vertex_validation_support_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Vertex validation : " << _vertex_validation_.get() << std::endl;
    if (_vertex_validation_) {
      std::ostringstream indent2_ss;
      indent2_ss << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      _vertex_validation_->tree_dump(out_, "", indent2_ss.str());
    }
    return;
  }


  // static
  void i_vertex_generator::ocd_support(datatools::object_configuration_description & ocd_,
                                       const std::string & label_)
  {
    datatools::logger::declare_ocd_logging_configuration(ocd_,
                                                         "fatal",
                                                         label_,
                                                         "genvtx::i_vertex_generator");

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(label_+"name")
        .set_from("genvtx::i_vertex_generator")
        .set_terse_description("Set the name of the vertex generator")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .add_example("Set an arbitrary name: ::                 \n"
                     "                                          \n"
                     "  name : string = \"random_roof_surface\" \n"
                     "                                          \n"
                     );

      // .set_default_value_string("")
      ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(label_+"vertex_validation_support")
        .set_from("genvtx::i_vertex_generator")
        .set_terse_description("Activate/deactivate the support of vertex validation")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example("Deactivate vertex validation: ::          \n"
                     "                                          \n"
                     "  "+label_+"vertex_validation_support : boolean = 0 \n"
                     "                                          \n"
                     );
      ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(label_+"Geo_label")
        .set_from("genvtx::i_vertex_generator")
        .set_terse_description("Specify requirement on the supported geometry setup")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .add_example("Set the name of the geometry service to be used: :: \n"
                     "                                                    \n"
                     "  "+label_+"Geo_label : string = \"Geo\"            \n"
                     "                                                    \n"
                     );
      ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(label_+"geometry.setup_requirement")
        .set_from("genvtx::i_vertex_generator")
        .set_terse_description("Specify requirement on the supported geometry setup")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        //.set_default_value_string("")
        .add_example("Only support a given version of a given setup: ::                    \n"
                     "                                                                     \n"
                     "  "+label_+"geometry.setup_requirement : string = \"my_setup(=2.0)\" \n"
                     "                                                                     \n"
                     );
      ;
    }

    return;
  }

} // end of namespace genvtx
