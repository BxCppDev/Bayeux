// i_from_model_vg.cc

// Ourselves:
#include <genvtx/i_from_model_vg.h>

#include <geomtools/manager.h>
#include <geomtools/mapping.h>
#include <geomtools/mapping_plugin.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/id_selector.h>

namespace genvtx {

  // Pimpl-ized working data
  struct i_from_model_vg::_work_type
  {
    geomtools::id_selector source_selector; //!< A selector of GIDs
    _work_type();
    ~_work_type();
    void reset();
  };

  i_from_model_vg::_work_type::_work_type()
  {
    return;
  }

  i_from_model_vg::_work_type::~_work_type()
  {
    reset();
    return;
  }

  void i_from_model_vg::_set_defaults()
  {
    return;
  }

  void i_from_model_vg::_work_type::reset()
  {
    if (source_selector.is_initialized()) {
      source_selector.reset();
    }
    return;
  }

  const geomtools::id_selector & i_from_model_vg::_get_source_selector() const
  {
    return _work_->source_selector;
  }

  i_from_model_vg::i_from_model_vg() : genvtx::i_vertex_generator()
  {
    _set_defaults();
    return;
  }

  i_from_model_vg::~i_from_model_vg()
  {
    return;
  }

  bool i_from_model_vg::has_origin_def() const
  {
    return _origin_def_.is_defined();
  }

  const origin_definition & i_from_model_vg::get_origin_def() const
  {
    return _origin_def_;
  }

  void i_from_model_vg::set_origin_def(const origin_definition & origin_def_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _origin_def_ = origin_def_;
    return;
  }

  bool i_from_model_vg::has_mapping_plugin_name() const
  {
    return ! _mapping_plugin_name_.empty();
  }

  const std::string & i_from_model_vg::get_mapping_plugin_name() const
  {
    return _mapping_plugin_name_;
  }

  void i_from_model_vg::set_mapping_plugin_name(const std::string & mpn_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _mapping_plugin_name_ = mpn_;
    return;
  }

  bool i_from_model_vg::has_materials_plugin_name() const
  {
    return ! _materials_plugin_name_.empty();
  }

  const std::string & i_from_model_vg::get_materials_plugin_name() const
  {
    return _materials_plugin_name_;
  }

  void i_from_model_vg::set_materials_plugin_name(const std::string & mpn_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _materials_plugin_name_ = mpn_;
    return;
  }

  void i_from_model_vg::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    this->i_vertex_generator::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Origin definition  : ";
    if (!has_origin_def()) {
      out_ << "<none>";
    }
    out_ << std::endl;
    if (has_origin_def()) {
      _origin_def_.config.tree_dump(out_, "", indent_ + "|   ");
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Materials plugin : ";
    if (!has_materials_plugin_name()) {
      out_ << "<none>";
    } else {
      out_ << "'" << get_materials_plugin_name() << "'";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Mapping plugin : ";
    if (!has_mapping_plugin_name()) {
      out_ << "<none>";
    } else {
      out_ << "'" << get_mapping_plugin_name() << "'";
    }
    out_ << std::endl;

    return;
  }

  void i_from_model_vg::_initialize(const ::datatools::properties & setup_,
                                    ::datatools::service_manager & service_manager_,
                                    ::genvtx::vg_dict_type & /* generators_ */)
  {
    this->::genvtx::i_vertex_generator::_initialize(setup_, service_manager_);

    DT_THROW_IF(! has_geom_manager(), std::logic_error,
                "Missing geometry manager in vertex generator '" << get_name() << "' !");

    // Fetch the geometry origin definition:
    if (!_origin_def_.is_defined()) {
      _origin_def_.load(setup_);
    }
    DT_THROW_IF(!_origin_def_.is_defined(),
                std::logic_error,
                "Missing origin definition for vertex generator '" << get_name() << "' !");

    // Fetch the geometry mapping plugin name:
    if (! has_mapping_plugin_name()) {
      if (setup_.has_key("mapping.plugin_name")) {
        std::string mpn;
        mpn = setup_.fetch_string("mapping.plugin_name");
        set_mapping_plugin_name(mpn);
      }
    }
    // DT_THROW_IF(!has_mapping_plugin_name(),
    //             std::logic_error,
    //             "Missing origin definition for vertex generator '" << get_name() << "' !");

    // Fetch the geometry materials plugin name:
    if (! has_materials_plugin_name()) {
      if (setup_.has_key("materials.plugin_name")) {
        std::string mpn;
        mpn = setup_.fetch_string("materials.plugin_name");
        set_materials_plugin_name(mpn);
      }
    }

    // Initialize working data:
    _work_.reset(new _work_type);

    // Special operations:
    _initialize_source_selector();

    return;
  }

  void i_from_model_vg::_initialize_source_selector()
  {
    _work_->source_selector.set_id_mgr(get_geom_manager().get_id_mgr());
    _work_->source_selector.initialize(_origin_def_.config);
    if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
      _work_->source_selector.tree_dump(std::cerr,
                                        "genvtx::i_from_model_vg::initialize: ID selector: ",
                                        "debug: ");
    }
    return;
  }

  void i_from_model_vg::_reset()
  {
    // Clear working data:
    if (_work_.get() != 0) {
      _work_->reset();
      _work_.reset();
    }
    _origin_def_.reset();
    _mapping_plugin_name_.clear();
    _materials_plugin_name_.clear();
    this->i_vertex_generator::_reset();
    return;
  }

  // static
  void i_from_model_vg::ocd_support(datatools::object_configuration_description & ocd_,
                                    const std::string & label_)
  {
    ::genvtx::i_vertex_generator::ocd_support(ocd_, label_);

    // datatools::logger::declare_ocd_logging_configuration(ocd_,
    //                                                      "fatal",
    //                                                      label_,
    //                                                      "genvtx::i_from_model_vg");

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(label_+"origin")
        .set_from("genvtx::i_from_model_vg")
        .set_terse_description("Set the rule that defined the physical volumes from which the vertexes are generated")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .add_example("Set an origin rule: ::  \n"
                     "                                          \n"
                     "  origin : string = \" category='detector' module={0;3;4} row=[0;3] column={*} \" \n"
                     "                                          \n"
                     );
      ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(label_+"mapping.plugin_name")
        .set_from("genvtx::i_from_model_vg")
        .set_terse_description("Set the name of the geometry mapping plugin")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .add_example("Set an arbitrary mapping plugin: ::       \n"
                     "                                          \n"
                     "  "+label_+"mapping.plugin_name : string = \"contamination_mapping\" \n"
                     "                                          \n"
                     );
      ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(label_+"materials.plugin_name")
        .set_from("genvtx::i_from_model_vg")
        .set_terse_description("Set the name of the materials plugin")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .add_example("Set an arbitrary materials plugin: ::     \n"
                     "                                          \n"
                     "  "+label_+"materials.plugin_name : string = \"materials_driver\" \n"
                     "                                          \n"
                     );
      ;
    }

    return;
  }

} // end of namespace genvtx
