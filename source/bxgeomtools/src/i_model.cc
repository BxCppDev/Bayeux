// i_model.cc

// Ourselves:
#include <geomtools/i_model.h>

// Standard library:
#include <stdexcept>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <geomtools/sensitive.h>
#include <geomtools/material.h>
#include <geomtools/visibility.h>
#include <geomtools/mapping_utils.h>
#include <geomtools/shape_factory.h>

namespace geomtools {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION (i_model, "geomtools::i_model/__system__");

  // static
  const std::string & i_model::solid_suffix()
  {
    static std::string token = ".solid";
    return token;
  }

  const std::string & i_model::logical_suffix()
  {
    static std::string token = ".log";
    return token;
  }

  const std::string & i_model::physical_suffix()
  {
    static std::string token = ".phys";
    return token;
  }

  const std::string & i_model::phantom_solid_flag()
  {
    static std::string token = "phantom_solid";
    return token;
  }

  const std::string & i_model::exported_properties_prefixes_key()
  {
    static std::string token = "__geomtools__i_model__exported_properties_prefixes";
    return token;
  }

  datatools::logger::priority i_model::get_logging_priority () const
  {
    return _logging_priority;
  }

  void i_model::set_logging_priority (datatools::logger::priority p_)
  {
    _logging_priority = p_;
    return;
  }

  std::string i_model::make_solid_name (const std::string & basename_)
  {
    return basename_ + i_model::solid_suffix();
  }

  std::string i_model::make_logical_volume_name (const std::string & basename_)
  {
    return basename_ + i_model::logical_suffix();
  }

  std::string i_model::make_physical_volume_name (const std::string & basename_)
  {
    return basename_ + i_model::physical_suffix();
  }

  std::string i_model::make_physical_volume_name (const std::string & basename_,
                                                  int nitems_)
  {
    std::ostringstream oss;
    oss << basename_ << i_model::physical_suffix()
        << "." << "__" << 0 << ".." << (nitems_ - 1) << "__";
    return oss.str ();
  }

  std::string i_model::make_physical_volume_name_per_item (const std::string & basename_,
                                                           int item_)
  {
    std::ostringstream oss;
    oss << basename_ << i_model::physical_suffix()
        << "." << "__" << item_ << "__";
    return oss.str ();
  }

  std::string i_model::make_physical_volume_name_per_item (const std::string & basename_,
                                                           int i_, int j_)
  {
    std::ostringstream oss;
    oss << basename_ << i_model::physical_suffix()
        << "." << "__" << i_ << "__"
        << "__" << j_ << "__";
    return oss.str ();
  }

  std::string i_model::make_physical_volume_name (const std::string & basename_,
                                                  int ncols_,
                                                  int nrows_)
  {
    std::ostringstream oss;
    oss << basename_ << i_model::physical_suffix()
        << "." << "__" << 0 << ".." << (ncols_ - 1) << "__"
        << "__" << 0 << ".." << (nrows_ - 1) << "__";
    return oss.str ();
  }

  std::string i_model::extract_label_from_physical_volume_name (const std::string & physical_volume_name_)
  {
    const size_t pos = physical_volume_name_.rfind (i_model::physical_suffix());
    DT_THROW_IF(pos == physical_volume_name_.npos,
                std::logic_error,
                "Do not recognize a physical volume name from '"
                << physical_volume_name_ << "' !");
    return physical_volume_name_.substr (0, pos);
  }

  bool i_model::is_constructed () const
  {
    return _constructed_;
  }

  bool i_model::is_debug () const
  {
    return _logging_priority >= datatools::logger::PRIO_DEBUG;
  }

  void i_model::set_debug (bool new_value_)
  {
    if (new_value_) {
      _logging_priority = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority = datatools::logger::PRIO_WARNING;
    }
    return;
  }

  bool i_model::has_name() const
  {
    return ! _name_.empty();
  }

  const std::string & i_model::get_name () const
  {
    return _name_;
  }

  void i_model::set_name (const std::string & name_)
  {
    _name_ = name_;
    return;
  }

  bool i_model::is_phantom_solid () const
  {
    return _phantom_solid;
  }

  void i_model::_set_phantom_solid (bool ps_)
  {
    _phantom_solid = ps_;
    return;
  }

  const datatools::properties & i_model::parameters () const
  {
    return _parameters_;
  }

  datatools::properties & i_model::parameters ()
  {
    return _parameters_;
  }

  i_model::i_model (const std::string & /*dummy_*/)
  {
    _constructed_     = false;
    _logging_priority = datatools::logger::PRIO_WARNING;
    _phantom_solid    = false;
    return;
  }

  i_model::~i_model ()
  {
    _parameters_.clear ();
    return;
  }

  bool i_model::has_shape_factory() const
  {
    return _shape_factory_ != 0;
  }

  void i_model::set_shape_factory(shape_factory & sf_)
  {
    _shape_factory_ = &sf_;
    return;
  }

  shape_factory & i_model::grab_shape_factory()
  {
    DT_THROW_IF(!has_shape_factory(),
                std::logic_error,
                "No referenced shape factory!");
    return *_shape_factory_;
  }

  const shape_factory & i_model::get_shape_factory() const
  {
    DT_THROW_IF(!has_shape_factory(),
                std::logic_error,
                "No referenced shape factory!");
    return *_shape_factory_;
  }

  const geomtools::logical_volume & i_model::get_logical () const
  {
    return _logical;
  }

  geomtools::logical_volume & i_model::grab_logical ()
  {
    return _logical;
  }

  void i_model::_common_construct (datatools::properties & setup_)
  {
    {
      // Logging priority:
      datatools::logger::priority lp
        = datatools::logger::extract_logging_configuration(setup_, _logging_priority);
      DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority level for geometry model '" << _name_ << "' !");
      set_logging_priority(lp);
    }
    return;
  }

  void i_model::_pre_construct (datatools::properties & setup_, models_col_type * /*models_*/)
  {
    if (setup_.has_flag (i_model::phantom_solid_flag())) {
      _set_phantom_solid (true);
    }

    return;
  }

  void i_model::_post_construct (datatools::properties & setup_, models_col_type * /*models_*/)
  {
    std::vector<std::string> exported_properties_prefixes;
    if (setup_.has_key(i_model::exported_properties_prefixes_key())) {
      setup_.fetch(i_model::exported_properties_prefixes_key(),
                   exported_properties_prefixes);
    }
    // Push some of the model's setup properties into the logical for further usage:
    DT_LOG_DEBUG(get_logging_priority(),
                 "Number of exported properties prefixes is "
                 << exported_properties_prefixes.size() << " in geometry model '" << get_name() << "'...");
    for (size_t i = 0; i < exported_properties_prefixes.size(); i++) {
      const std::string & prefix = exported_properties_prefixes[i];
      DT_LOG_DEBUG(get_logging_priority(), "Export properties starting with '"
                   << prefix << "' to the top logical in geometry model '" << get_name() << "'...");
      setup_.export_starting_with (_logical.grab_parameters (), prefix);
    }

    return;
  }

  void i_model::construct (const std::string & name_,
                           const datatools::properties & setup_,
                           models_col_type * models_)
  {
    std::vector<std::string> dummy;
    construct(name_, setup_, dummy, models_);
    return;
  }

  void i_model::construct (const std::string & name_,
                           const datatools::properties & setup_,
                           const std::vector<std::string> & properties_prefixes_,
                           models_col_type * models_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error,
                 "Model '" << name_ << "' has been already constructed !");
    datatools::properties & setup = const_cast<datatools::properties &> (setup_);
    // Set the name of the model as soon as possible:
    if (_name_.empty() && ! name_.empty()) {
      set_name(name_);
    }
    {
      // Set description for logical parameters :
      std::ostringstream log_params_desc;
      log_params_desc << "Auxiliary properties for top logical volume in model '"
                      << name_ << "'";
      _logical.grab_parameters ().set_description(log_params_desc.str());
    }
    setup.store(i_model::exported_properties_prefixes_key(),
                properties_prefixes_);
    _common_construct (setup);
    _pre_construct (setup, models_);
    _at_construct (name_, setup_, models_);
    _post_construct (setup, models_);
    _logical.set_geometry_model(*this);
    if (setup.has_key(i_model::exported_properties_prefixes_key())) {
      setup.erase(i_model::exported_properties_prefixes_key());
    }
    _logical.lock();
    _constructed_ = true;
    return;
  }

  void i_model::tree_dump (std::ostream & out_,
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
         << "Name        : \"" << _name_ << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Model ID    : \"" << get_model_id () << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging     : \""
         << datatools::logger::get_priority_label(_logging_priority) << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Shape factory : ";
    if (has_shape_factory()) {
      out_ << "<yes>";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Constructed : " << _constructed_ << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Parameters  : ";
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _parameters_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Logical     : " << std::endl;
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
      _logical.tree_dump (out_, "", indent_oss.str ());
    }

    return;
  }

  void i_model::init_ocd(datatools::object_configuration_description & ocd_)
  {

    // Logging:
    datatools::logger::declare_ocd_logging_configuration(ocd_);

    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern(i_model::phantom_solid_flag())
        .set_from("geomtools::i_model")
        .set_terse_description("The flag for a phantom solid")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .set_long_description("This property tags the solid as 'phantom'.  \n"
                              "It is not used yet in the Bayeux/geomtools  \n"
                              "library because the 'phantom' solid concept \n"
                              "is not implemented yet.                     \n"
                              )
        .add_example("Make the solid a phantom::         \n"
                     "                                   \n"
                     "  phantom_solid : boolean = false  \n"
                     "                                   \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools
