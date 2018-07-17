// vertex_validation.cc

// Ourselves:
#include <genvtx/vertex_validation.h>

// Third party:
// - Boost:
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
// - Bayeux/datatools:
#include <datatools/factory.h>
// - Bayeux/geomtools:
#include <geomtools/logical_volume.h>
#include <geomtools/geom_info.h>
#include <geomtools/geom_id.h>
// - Bayeux/cuts:
#include <cuts/i_cut.h>

// This project:
#include <genvtx/not_in_daughters_vertex_validator.h>
#include <genvtx/in_materials_vertex_validator.h>

namespace genvtx {

  // static
  datatools::factory_register<cuts::i_cut> & validator_factory_system_register()
  {
    static boost::scoped_ptr<datatools::factory_register<cuts::i_cut> > _vv_factory;
    if (!_vv_factory) {
      _vv_factory.reset(new datatools::factory_register<cuts::i_cut>("vertex_validator/_system_"));
      // No automatic import, but manual registration of cut classes of interest:
      _vv_factory->registration<genvtx::not_in_daughters_vertex_validator>(not_in_daughters_vertex_validator::system_factory_auto_registration_id());
      _vv_factory->registration<genvtx::in_materials_vertex_validator>(in_materials_vertex_validator::system_factory_auto_registration_id());
      /*
      _vv_factory->registration(not_in_daughters_vertex_validator::system_factory_auto_registration_id(),
                                boost::factory<genvtx::not_in_daughters_vertex_validator*>(),
                                typeid(genvtx::not_in_daughters_vertex_validator)
                                );
      _vv_factory->registration(in_materials_vertex_validator::system_factory_auto_registration_id(),
                                boost::factory<genvtx::in_materials_vertex_validator*>()
                                );
      */
    }
    return *_vv_factory;
  }

  // static
  const size_t vertex_validation::DEFAULT_MAX_NUMBER_OF_TRIES;

  vertex_validation::geometry_context::geometry_context()
  {
    _ginfo_ = 0;
    _gid_ = 0;
    _log_volume_ = 0;
    _global_candidate_vertex_ = 0;
    geomtools::invalidate(_local_candidate_vertex_);
    return;
  }

  void vertex_validation::geometry_context::reset()
  {
    _ginfo_ = 0;
    _gid_ = 0;
    _log_volume_ = 0;
    _global_candidate_vertex_ = 0;
    geomtools::invalidate(_local_candidate_vertex_);
    return;
  }

  bool vertex_validation::geometry_context::has_ginfo() const
  {
    return _ginfo_ != 0;
  }

  bool vertex_validation::geometry_context::has_gid() const
  {
    return _gid_ != 0;
  }

  bool vertex_validation::geometry_context::has_logical_volume() const
  {
    return _log_volume_ != 0;
  }

  bool vertex_validation::geometry_context::has_local_candidate_vertex() const
  {
    return geomtools::is_valid(_local_candidate_vertex_);
  }

  bool vertex_validation::geometry_context::has_global_candidate_vertex() const
  {
    return _global_candidate_vertex_ != 0;
  }

  void vertex_validation::geometry_context::set_ginfo(const geomtools::geom_info & gi_)
  {
    _ginfo_ = &gi_;
    if (_ginfo_->has_logical()) {
      set_logical_volume(_ginfo_->get_logical());
    }
    if (_ginfo_->has_logical()) {
      set_gid(_ginfo_->get_geom_id());
    }
    return;
  }

  void vertex_validation::geometry_context::set_gid(const geomtools::geom_id & gid_)
  {
    _gid_ = &gid_;
    return;
  }

  void vertex_validation::geometry_context::set_logical_volume(const geomtools::logical_volume & lv_)
  {
    _log_volume_ = &lv_;
    return;
  }

  const geomtools::geom_info &
  vertex_validation::geometry_context::get_ginfo() const
  {
    DT_THROW_IF(!has_ginfo(), std::logic_error,
                "Missing geometry info!");
    return *_ginfo_;
  }

  const geomtools::logical_volume &
  vertex_validation::geometry_context::get_logical_volume() const
  {
    DT_THROW_IF(!has_logical_volume(), std::logic_error,
                "Missing logical volume!");
    return *_log_volume_;
  }

  const geomtools::geom_id &
  vertex_validation::geometry_context::get_gid() const
  {
    DT_THROW_IF(!has_gid(), std::logic_error,
                "Missing geometry Id!");
    return *_gid_;
  }

  void vertex_validation::geometry_context::set_local_candidate_vertex(const geomtools::vector_3d & vtx_)
  {
    _local_candidate_vertex_ = vtx_;
    return;
  }

  const geomtools::vector_3d &
  vertex_validation::geometry_context::get_local_candidate_vertex() const
  {
    DT_THROW_IF(!has_local_candidate_vertex(), std::runtime_error,
                "Missing local candidate vertex!");
    return _local_candidate_vertex_;
  }

  void vertex_validation::geometry_context::set_global_candidate_vertex(const geomtools::vector_3d & vtx_)
  {
    _global_candidate_vertex_ = &vtx_;
    return;
  }

  const geomtools::vector_3d &
  vertex_validation::geometry_context::get_global_candidate_vertex() const
  {
    DT_THROW_IF(!has_global_candidate_vertex(), std::runtime_error,
                "Missing global candidate vertex!");
    return *_global_candidate_vertex_;
  }

  bool vertex_validation::geometry_context::is_valid() const
  {
    return (has_local_candidate_vertex() || has_global_candidate_vertex());
  }

  vertex_validation::geometry_context & vertex_validation::grab_geometry_context()
  {
    return *_geo_context_;
  }

  const vertex_validation::geometry_context & vertex_validation::get_geometry_context() const
  {
    return *_geo_context_;
  }

  vertex_validation::vertex_validation()
  {
    _initialized_ = false;
    _logging_ = datatools::logger::PRIO_FATAL;
    _number_of_tries_ = 0;
    _maximum_number_of_tries_ = DEFAULT_MAX_NUMBER_OF_TRIES;
    _validator_ = 0;
    _owned_validator_ = false;
    return;
  }

  vertex_validation::~vertex_validation()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  bool vertex_validation::is_initialized() const
  {
    return _initialized_;
  }

  void vertex_validation::set_maximum_number_of_tries(const size_t tries_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Vertex validation is already initialized !");
    _maximum_number_of_tries_ = tries_;
    return;
  }

  size_t vertex_validation::get_maximum_number_of_tries() const
  {
    return _maximum_number_of_tries_;
  }

  void vertex_validation::set_logging_priority(datatools::logger::priority priority_)
  {
    _logging_ = priority_;
    return;
  }

  datatools::logger::priority vertex_validation::get_logging_priority() const
  {
    return _logging_;
  }

  void vertex_validation::reset_geometry_context()
  {
    if (_geo_context_) {
      _geo_context_.reset();
    }
    return;
  }

  bool vertex_validation::is_owned_validator() const
  {
    return _owned_validator_;
  }

  void vertex_validation::set_validator(cuts::i_cut & validator_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Vertex validation is already initialized !");
    _validator_ = &validator_;
    _owned_validator_ = false;
    return;
  }

  void vertex_validation::set_validator(cuts::i_cut * validator_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Vertex validation is already initialized !");
    _validator_ = validator_;
    _owned_validator_ = true;
    return;
  }

  bool vertex_validation::has_validator() const
  {
    return _validator_ != 0;
  }

  void vertex_validation::reset_validator()
  {
    if (_validator_) {
      if (is_owned_validator()) {
        delete _validator_;
        _owned_validator_ = false;
      }
      _validator_ = 0;
    }
    return;
  }

  void vertex_validation::initialize(const datatools::properties & config_)
  {
    // Logging priority:
    datatools::logger::priority lp
      = datatools::logger::extract_logging_configuration(config_,
                                                         datatools::logger::PRIO_FATAL);
    DT_THROW_IF(lp ==  datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority !");
    set_logging_priority(lp);

    if (config_.has_key("maximum_number_of_tries")) {
      const size_t max_nbr_tries = config_.fetch_integer("maximum_number_of_tries");
      set_maximum_number_of_tries(max_nbr_tries);
    }

    if (!has_validator()) {
      DT_THROW_IF(! config_.has_key("validator_id"), std::logic_error,
                  "Missing 'validator_id' property!");
      std::string validator_id = config_.fetch_string("validator_id");
      datatools::factory_register<cuts::i_cut> & facreg = validator_factory_system_register();
      DT_THROW_IF(! facreg.has(validator_id), std::logic_error,
                  "Unknown 'validator_id' property : '" << validator_id << "' is not registered "
                  << "in the validator factory system register!");
      // Acess the factory for this vertex validator:
      const cuts::i_cut::factory_register_type::factory_type & the_factory
        = facreg.get(validator_id);
      cuts::i_cut * validator_ptr = the_factory();

      datatools::properties validator_config;
      config_.export_and_rename_starting_with(validator_config, "validator_params.", "");
      validator_ptr->initialize_standalone(validator_config);
      set_validator(validator_ptr);
    }

    DT_THROW_IF(_validator_ == 0,
                std::logic_error,
                "Missing vertex validator cut !");

    // Instantiate the geometry context:
    _geo_context_.reset(new geometry_context);

    _initialized_ = true;
    return;
  }

  void vertex_validation::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Vertex validation is not initialized !");
    _initialized_ = false;
    reset_validator();
    reset_geometry_context();
    reset_number_of_tries();
    _maximum_number_of_tries_ = DEFAULT_MAX_NUMBER_OF_TRIES;
    return;
  }

  size_t vertex_validation::get_number_of_tries() const
  {
    return _number_of_tries_;
  }

  void vertex_validation::reset_number_of_tries()
  {
    _number_of_tries_ = 0;
    return;
  }

  bool vertex_validation::is_maximum_number_of_tries_reached() const
  {
    return _maximum_number_of_tries_ > 0 && _number_of_tries_ > _maximum_number_of_tries_;
  }

  vertex_validation::validate_status_type vertex_validation::validate()
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Vertex validation is not initialized !");
    DT_THROW_IF(! _validator_, std::logic_error,
                "Missing validator cut !");
    _validator_->set_user_data<geometry_context>(get_geometry_context());
    _number_of_tries_++;
    const int selection = _validator_->process();
    validate_status_type status = VS_UNDEFINED;
    if (is_maximum_number_of_tries_reached()) {
      status = VS_MAXTRIES;
    } else if (selection == cuts::SELECTION_INAPPLICABLE) {
      DT_LOG_ERROR(get_logging_priority(), "Vertex validation can not be applied !");
    } else if (selection == cuts::SELECTION_REJECTED) {
      DT_LOG_TRACE(get_logging_priority(), "Vertex " << _geo_context_->get_local_candidate_vertex() << " using '"
                   << _validator_->get_name() << "' cut has been rejected");
      status = VS_REJECTED;
    } else if (selection == cuts::SELECTION_ACCEPTED) {
      DT_LOG_TRACE(get_logging_priority(), "Vertex " << _geo_context_->get_local_candidate_vertex() << " using '"
                   << _validator_->get_name() << "' cut has been accepted");
      status = VS_ACCEPTED;
      reset_number_of_tries();
    }
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return status;
  }

  void vertex_validation::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized : " << (is_initialized() ? "Yes": "No") <<std:: endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority : '"
         << datatools::logger::get_priority_label(_logging_) << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Validator : " << _validator_;
    if (_owned_validator_) {
      out_ << " (owned)";
    }
    out_ << std::endl;
    if (_validator_) {
      std::ostringstream indent2;
      indent2 << indent << datatools::i_tree_dumpable::skip_tag;
      _validator_->tree_dump(out_, "", indent2.str());
    }
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Geometry context : " << _geo_context_.get() << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Maximum number of tries : " << _maximum_number_of_tries_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Current number of tries : " << _number_of_tries_ << std::endl;

    return;
  }


  // static
  void vertex_validation::ocd_support(datatools::object_configuration_description & ocd_,
                                      const std::string & label_)
  {
    datatools::logger::declare_ocd_logging_configuration(ocd_,
                                                         "fatal",
                                                         label_,
                                                         "genvtx::vertex_validation");


   {
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern(label_+"maximum_number_of_tries")
      .set_from("genvtx::vertex_validation")
      .set_terse_description("Set the maximum number of tries before failure")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_default_value_integer(DEFAULT_MAX_NUMBER_OF_TRIES)
      .set_long_description("The vertex validation mechanism is based on a \n"
                            "Von Neumann rejection technique. This implies \n"
                            "that many candidate vertexes can be shot before\n"
                            "one is finally accepted, depending on the criterion\n"
                            "used by the vertex generator as its selector/cut. \n"
                            "The vertex validation implements a check of the \n"
                            "number of tries performed during the validation \n"
                            "process. If this property is set to some strictly \n"
                            "positive integer value, the `validate` method will check at\n"
                            "each loop if the current number of tries reaches this\n"
                            "limit and will return the `VS_MAXTRIES` status. \n"
                            "The (not recommended) value of `0` means that no \n"
                            "limit check is performed.\n"
                            )
      .add_example("Use a very high value for the number of tries.          \n"
                   "This should be used only if we know that the rejection  \n"
                   "probability using the Von Neumann method is very large::\n"
                   "                                                        \n"
                   "  "+label_+"maximum_number_of_tries : integer = 10000             \n"
                   "                                                        \n"
                   )
      .add_example("Set no limit for the number of tries.                 \n"
                   "This should be used with extreme care because         \n"
                   "it could lead to some inifinite loop if the `validate`\n"
                   "method always return `VS_REJECTED`::                  \n"
                   "                                                      \n"
                   "  maximum_number_of_tries : integer = 0               \n"
                   "                                                      \n"
                   )
      ;
   }

   return;
  }

} // end of namespace genvtx
