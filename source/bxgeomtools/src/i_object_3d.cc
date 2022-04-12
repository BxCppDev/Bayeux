/// \file geomtools/i_object_3d.cc

// Ourselves:
#include <geomtools/i_object_3d.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>

// This project:
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_object_3d, "geomtools::i_object_3d/__system__")

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(i_object_3d,"geomtools::i_object_3d")

  i_object_3d::object_entry::object_entry()
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this << "  name='" << "<none>" << "'");
    return;
  }

  i_object_3d::object_entry::object_entry(const std::string & name_,
                                          const std::string & type_id_,
                                          const datatools::properties & config_,
                                          const handle_type & handle_)
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this << "  name='" << name_ << "'");
    set_name(name_);
    set_type_id(type_id_);
    set_config(config_);
    set_handle(handle_);
    return;
  }

  i_object_3d::object_entry::~object_entry()
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "DTOR=" << this << "  name='" << _name_ << "'");
    reset();
    return;
  }

  void i_object_3d::object_entry::reset()
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Entering...");
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "  Name='" << _name_ << "'");
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "  Handle=" << std::boolalpha << bool(_hobject_));
    // if (_hobject_) {
    //   DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "   Test handle data");
    //   DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "   unique = " << _hobject_.unique());
    //   DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "   count = " << _hobject_.use_count());
    //   const auto & obj = *_hobject_;
    //   DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "   obj type = " << typeid(obj).name());
    // } else {
    //   DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "No handled data");
    // }
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Reset handle...");
    _hobject_.reset();
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "handle is reset.");
    _config_.clear();
    _type_id_.clear();
    _name_.clear();
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "config/name/type_id cleared.");
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Exiting.");
    return;
  }

  void i_object_3d::object_entry::set_name(const std::string & name_)
  {
    _name_ = name_;
    return;
  }

  const std::string & i_object_3d::object_entry::get_name() const
  {
    return _name_;
  }

  void i_object_3d::object_entry::set_type_id(const std::string & type_id_)
  {
    _type_id_ = type_id_;
    return;
  }

  const std::string & i_object_3d::object_entry::get_type_id() const
  {
    return _type_id_;
  }

  const datatools::properties & i_object_3d::object_entry::get_config() const
  {
    return _config_;
  }
  
  void i_object_3d::object_entry::set_config(const datatools::properties & config_)
  {
    _config_ = config_;
    return;
  }

  bool i_object_3d::object_entry::has_object() const
  {
    return bool(_hobject_);
  }

  const i_object_3d & i_object_3d::object_entry::get_object() const
  {
    DT_THROW_IF(!bool(_hobject_), std::logic_error, "Null object handle!");
    return *_hobject_;
  }

  void i_object_3d::object_entry::set_handle(const i_object_3d::handle_type & handle_)
  {
    _hobject_ = handle_;
    return;
  }

  i_object_3d::const_handle_type i_object_3d::object_entry::get_handle() const
  {
    return _hobject_;
  }

  i_object_3d::handle_type i_object_3d::object_entry::grab_handle()
  {
    return _hobject_;
  }

  /* i_object_3d */

  datatools::logger::priority i_object_3d::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void i_object_3d::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
    return;
  }

  void i_object_3d::set_tolerance (double tolerance_)
  {
    DT_THROW_IF (tolerance_ <= 0.0, std::logic_error, "Tolerance value '" << tolerance_ << "' must be positive !");
    _tolerance_ = tolerance_;
    return;
  }

  double i_object_3d::compute_tolerance(double tolerance_) const
  {
    DT_THROW_IF(std::isinf(tolerance_), std::logic_error,
                "Invalid infinite tolerance for shape '"
                << get_shape_name() << "' !");
    if (tolerance_ == constants::get_zero_tolerance()) {
      return constants::get_default_tolerance ();
    } else if (tolerance_ < 0.0 || std::isnan(tolerance_)) {
      return get_tolerance();
    } else {
      return tolerance_;
    }
  }

  double i_object_3d::compute_angular_tolerance(double angular_tolerance_) const
  {
    DT_THROW_IF(std::isinf(angular_tolerance_), std::logic_error,
                "Invalid infinite angular tolerance for shape '"
                << get_shape_name() << "' !");
    if (angular_tolerance_ == constants::get_zero_tolerance()) {
      return constants::get_default_angular_tolerance ();
    } else if (angular_tolerance_ < 0.0 || std::isnan(angular_tolerance_)) {
      return get_angular_tolerance();
    } else {
      return angular_tolerance_;
    }
  }

  double i_object_3d::get_tolerance() const
  {
    return _tolerance_;
  }

  void i_object_3d::set_angular_tolerance(double angular_tolerance_)
  {
    DT_THROW_IF (angular_tolerance_ <= 0.0,
                 std::logic_error,
                 "Angular tolerance value '"
                 << angular_tolerance_ << "' must be positive !");
    _angular_tolerance_ = angular_tolerance_;
    return;
  }

  double i_object_3d::get_angular_tolerance() const
  {
    return _angular_tolerance_;
  }

  bool i_object_3d::is_composite() const
  {
    return false;
  }

  void i_object_3d::_set_defaults()
  {
    _logging_priority_ = datatools::logger::PRIO_FATAL;
    _tolerance_ = constants::get_default_tolerance();
    _angular_tolerance_ = constants::get_default_angular_tolerance();
    _wires_drawer_ = nullptr;
    return;
  }

  bool i_object_3d::has_wires_drawer() const
  {
    return _wires_drawer_ != nullptr;
  }

  void i_object_3d::set_wires_drawer(i_wires_3d_rendering & wires_drawer_)
  {
    _wires_drawer_ = &wires_drawer_;
    return;
  }

  void i_object_3d::reset_wires_drawer()
  {
    if (_wires_drawer_ != nullptr) {
      _wires_drawer_ = nullptr;
    }
    return;
  }

  i_wires_3d_rendering & i_object_3d::grab_wires_drawer()
  {
    DT_THROW_IF(!has_wires_drawer(), std::logic_error,
                "Missing shape wires drawer!");
    return *_wires_drawer_;
  }

  const i_wires_3d_rendering & i_object_3d::get_wires_drawer() const
  {
    DT_THROW_IF(!has_wires_drawer(), std::logic_error,
                "Missing wires 3D drawer!");
    return *_wires_drawer_;
  }

  i_object_3d::i_object_3d()
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this);
    // _set_defaults();
    return;
  }

  i_object_3d::i_object_3d(double tolerance_)
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this);
    // _set_defaults();
    if (tolerance_ <= 0.0) {
      _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
    } else {
      set_tolerance(tolerance_);
    }
    return;
  }

  i_object_3d::i_object_3d(double tolerance_, double angular_tolerance_)
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this);
    // _set_defaults();
    if (tolerance_ <= 0.0 || !datatools::is_valid(tolerance_)) {
      _tolerance_ = constants::get_default_tolerance();
    } else {
      set_tolerance(tolerance_);
    }
    if (angular_tolerance_ <= 0.0 || !datatools::is_valid(angular_tolerance_)) {
      _angular_tolerance_ = constants::get_default_angular_tolerance();
    } else {
      set_angular_tolerance(angular_tolerance_);
    }
    return;
  }

  i_object_3d::i_object_3d(const i_object_3d & src_)
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this);
    // _set_defaults();
    _tolerance_ = src_._tolerance_;
    _angular_tolerance_ = src_._angular_tolerance_;
    _wires_drawer_ = src_._wires_drawer_;
    return;
  }

  i_object_3d & i_object_3d::operator=(const i_object_3d & src_)
  {
    if (this == &src_) {
      return *this;
    }
    _tolerance_ = src_._tolerance_;
    _angular_tolerance_ = src_._angular_tolerance_;
    _wires_drawer_ = src_._wires_drawer_;
    return *this;
  }

  i_object_3d::~i_object_3d()
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "DTOR=" << this);
    this->i_object_3d::_reset();
    return;
  }

  void i_object_3d::initialize_simple()
  {
    datatools::properties dummy_config;
    initialize(dummy_config, nullptr);
    return;
  }

  void i_object_3d::initialize(const datatools::properties & config_,
                               const handle_dict_type * objects_)
  {
    this->i_object_3d::_initialize(config_, objects_);
    return;
  }

  void i_object_3d::_initialize(const datatools::properties & config_,
                                const handle_dict_type * /* objects_ */)
  {
    // Parse logging priority:
    datatools::logger::priority prio = datatools::logger::extract_logging_configuration(config_, _logging_priority_, true);
    if (prio != datatools::logger::PRIO_UNDEFINED) {
      set_logging_priority(prio);
    }

    double tlunit = CLHEP::micrometer;
    if (config_.has_key("tolerance.length_unit")) {
      const std::string tolerance_length_unit_str = config_.fetch_string("tolerance.length_unit");
      tlunit = datatools::units::get_length_unit_from(tolerance_length_unit_str);
    }

    double taunit = CLHEP::degree;
    if (config_.has_key("tolerance.angle_unit")) {
      const std::string tolerance_angle_unit_str = config_.fetch_string("tolerance.angle_unit");
      taunit = datatools::units::get_angle_unit_from(tolerance_angle_unit_str);
    }

    if (config_.has_key("tolerance.length")) {
      double tl = config_.fetch_real("tolerance.length");
      if (! config_.has_explicit_unit("tolerance.length")) {
        tl *= tlunit;
      }
      set_tolerance(tl);
    }

    if (config_.has_key("tolerance.angle")) {
      double ta = config_.fetch_real("tolerance.angle");
      if (! config_.has_explicit_unit("tolerance.angle")) {
        ta *= taunit;
      }
      set_angular_tolerance(ta);
    }

    return;
  }

  void i_object_3d::reset()
  {
    this->i_object_3d::_reset();
    return;
  }

  void i_object_3d::_reset()
  {
    reset_wires_drawer();
    this->i_object_3d::_set_defaults();
    return;
  }

  void i_object_3d::tree_dump(std::ostream & out_,
                              const std::string & title_,
                              const std::string & indent_,
                              bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Logging priority : \"" << datatools::logger::get_priority_label(_logging_priority_) << "\"" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Shape name : \"" << get_shape_name () << "\"" << std::endl;
    {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Dimensionality : " << get_dimensional() <<  std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Tolerance  = " << _tolerance_ / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Angular tolerance  = " << _angular_tolerance_ / CLHEP::radian << " radian" << std::endl;

    {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Wires drawer : " << (has_wires_drawer() ? "<yes>": "<none>") << std::endl;
    }

    return;
  }

  // static
  void i_object_3d::init_ocd(datatools::object_configuration_description & ocd_)
  {

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("tolerance.length_unit")
        .set_from("geomtools::i_object_3d")
        .set_terse_description("The unit symbol used for tolerance length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("um")
        .set_long_description("A character string that represents the default    \n"
                              "length unit for intrinsic tolerance of the solid. \n"
                              )
        .add_example("Set the default tolerance length unit::           \n"
                     "                                                  \n"
                     "  tolerance.length_unit : string = \"um\"         \n"
                     "                                                  \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("tolerance.angle_unit")
        .set_from("geomtools::i_object_3d")
        .set_terse_description("The unit symbol used for tolerance angle unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("degree")
        .set_long_description("A character string that represents the default   \n"
                              "angle unit for intrinsic tolerance of the solid. \n"
                              )
        .add_example("Set the default tolerance length unit::           \n"
                     "                                                  \n"
                     "  tolerance.angle_unit : string = \"degree\"      \n"
                     "                                                  \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("tolerance.length")
        .set_from("geomtools::i_object_3d")
        .set_terse_description("The tolerance length")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the tolerance length::             \n"
                     "                                       \n"
                     "  tolerance.length : real = 1.0e-6 um  \n"
                     "                                       \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("tolerance.angle")
        .set_from("geomtools::i_object_3d")
        .set_terse_description("The tolerance angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the tolerance angle::                \n"
                     "                                         \n"
                     "  tolerance.angle : real = 1.0e-6 radian \n"
                     "                                         \n"
                     )
        ;
    }

    return;
  }

  /* Object 3D getter interface */

  // virtual
  i_object_3d::i_getter::~i_getter()
  {
    return;
  }

  const i_object_3d *
  i_object_3d::i_getter::get(const std::string & name_)
  {
    datatools::properties params;
    return get(name_, params);
  }

} // end of namespace geomtools
