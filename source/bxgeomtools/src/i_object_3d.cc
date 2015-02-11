/** \file geomtools/i_object_3d.cc */

// Ourselves:
#include <geomtools/i_object_3d.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>

// This project:
#include <geomtools/i_wires_drawer.h>

namespace geomtools {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_object_3d, "geomtools::i_object_3d/__system__");

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(i_object_3d,"geomtools::i_object_3d");

  i_object_3d::object_entry::object_entry()
  {
    _status_ = 0;
    return;
  }

  i_object_3d::object_entry::~object_entry()
  {
    reset();
    return;
  }

  void i_object_3d::object_entry::reset()
  {
    _config_.clear();
    _name_.clear();
    _type_id_.clear();
    _hobject_.reset();
    _status_ = 0;
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

  void i_object_3d::object_entry::set_status(uint32_t s_)
  {
    _status_ = s_;
    return;
  }

  uint32_t i_object_3d::object_entry::get_status() const
  {
    return _status_;
  }

  const datatools::properties & i_object_3d::object_entry::get_config() const
  {
    return _config_;
  }

  datatools::properties & i_object_3d::object_entry::grab_config()
  {
    return _config_;
  }

  bool i_object_3d::object_entry::has_object() const
  {
    return _hobject_.has_data();
  }

  const i_object_3d & i_object_3d::object_entry::get_object() const
  {
    return _hobject_.get();
  }

  void i_object_3d::object_entry::set_object(i_object_3d * obj_)
  {
    _hobject_.reset(obj_);
    return;
  }

  /* i_object_3d */

  void i_object_3d::set_tolerance (double tolerance_)
  {
    DT_THROW_IF (tolerance_ <= 0.0, std::logic_error, "Tolerance value '" << tolerance_ << "' must be positive !");
    _tolerance_ = tolerance_;
    return;
  }

  double i_object_3d::get_tolerance() const
  {
    return _tolerance_;
  }

  void i_object_3d::set_angular_tolerance (double angular_tolerance_)
  {
    DT_THROW_IF (angular_tolerance_ <= 0.0,
                 std::logic_error,
                 "Angular tolerance value '" << angular_tolerance_ << "' must be positive !");
    _angular_tolerance_ = angular_tolerance_;
    return;
  }

  double i_object_3d::get_angular_tolerance() const
  {
    return _angular_tolerance_;
  }

  bool i_object_3d::is_composite () const
  {
    return false;
  }

  bool i_object_3d::has_user_draw () const
  {
    return _user_draw_ != 0;
  }

  void * i_object_3d::get_user_draw () const
  {
    return _user_draw_;
  }

  void i_object_3d::set_user_draw (void * user_draw_)
  {
    _user_draw_ = user_draw_;
    return;
  }

  void i_object_3d::set_defaults()
  {
    _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
    _angular_tolerance_ = GEOMTOOLS_DEFAULT_ANGULAR_TOLERANCE;
    _user_draw_ = 0;
    _wires_drawer_ = 0;
    return;
  }

  bool i_object_3d::has_wires_drawer() const
  {
    return _wires_drawer_ != 0;
  }

  void i_object_3d::set_wires_drawer(i_wires_drawer & wires_drawer_)
  {
    _wires_drawer_ = &wires_drawer_;
    return;
  }

  i_wires_drawer & i_object_3d::grab_wires_drawer()
  {
    DT_THROW_IF(!has_wires_drawer(), std::logic_error,
                "Missing shape wires drawer!");
    return *_wires_drawer_;
  }

  const i_wires_drawer & i_object_3d::get_wires_drawer() const
  {
    DT_THROW_IF(!has_wires_drawer(), std::logic_error,
                "Missing shape wires drawer!");
    return *_wires_drawer_;
  }

  i_object_3d::i_object_3d ()
  {
    set_defaults();
    return;
  }

  i_object_3d::i_object_3d (double tolerance_)
  {
    set_defaults();
    if (tolerance_ <= 0.0) {
      _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
    } else {
      set_tolerance(tolerance_);
    }
    return;
  }

  i_object_3d::~i_object_3d ()
  {
    this->i_object_3d::reset();
    return;
  }

  void i_object_3d::initialize(const datatools::properties & config_,
                               const handle_dict_type * /* objects_ */)
  {
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

  void i_object_3d::reset ()
  {
    this->i_object_3d::set_defaults();
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
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Wires drawer : " << (has_wires_drawer() ? "Yes": "No") << std::endl;
    }

    {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "User draw : " << (has_user_draw() ? "Yes": "No") << std::endl;
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

  const i_object_3d *
  i_object_3d::i_getter::get(const std::string & name_)
  {
    datatools::properties params;
    return get(name_, params);
  }

} // end of namespace geomtools
