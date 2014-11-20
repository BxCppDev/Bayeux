/** \file geomtools/i_object_3d.cc */

// Ourselves:
#include <geomtools/box.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>

namespace geomtools {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_object_3d, "geomtools::i_object_3d/__system__");

  i_object_3d::object_entry::object_entry()
  {
    status = 0;
    return;
  }

  i_object_3d::object_entry::~object_entry()
  {
    reset();
    return;
  }

  void i_object_3d::object_entry::reset()
  {
    config.clear();
    hobject.reset();
    status = 0;
    return;
  }

  uint32_t i_object_3d::object_entry::get_status() const
  {
    return status;
  }

  bool i_object_3d::object_entry::has_object() const
  {
    return hobject.has_data();
  }

  const i_object_3d & i_object_3d::object_entry::get_object() const
  {
    return hobject.get();
  }

  datatools::logger::priority i_object_3d::get_logging() const
  {
    return _logging;
  }

  void i_object_3d::set_logging(datatools::logger::priority l_)
  {
    _logging = l_;
    return;
  }

  double i_object_3d::get_tolerance() const
  {
    return _tolerance_;
  }

  void i_object_3d::set_tolerance (double tolerance_)
  {
    DT_THROW_IF (tolerance_ <= 0.0, std::logic_error, "Tolerance value '" << tolerance_ << "' must be positive !");
    _tolerance_ = tolerance_;
    return;
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

  datatools::properties & i_object_3d::grab_properties ()
  {
    return _auxiliaries_;
  }

  const datatools::properties & i_object_3d::get_properties () const
  {
    return _auxiliaries_;
  }

  datatools::properties & i_object_3d::grab_auxiliaries ()
  {
    return _auxiliaries_;
  }

  const datatools::properties & i_object_3d::get_auxiliaries () const
  {
    return _auxiliaries_;
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
    _logging = datatools::logger::PRIO_FATAL;
    _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
    _angular_tolerance_ = GEOMTOOLS_DEFAULT_ANGULAR_TOLERANCE;
    _user_draw_ = 0;
     return;
  }

  i_object_3d::i_object_3d ()
  {
    this->i_object_3d::set_defaults();
    return;
  }

  i_object_3d::i_object_3d (double tolerance_)
  {
    this->i_object_3d::set_defaults();
    if (tolerance_ <= 0.0) {
      _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
    } else {
      set_tolerance (tolerance_);
    }
    return;
  }

  i_object_3d::~i_object_3d ()
  {
    this->i_object_3d::reset();
    return;
  }

  void i_object_3d::initialize(const datatools::properties & config_, const handle_dict_type * /* objects_ */)
  {
    datatools::logger::priority p =
      datatools::logger::extract_logging_configuration(config_,
                                                       datatools::logger::PRIO_FATAL,
                                                       true);
    if (p != datatools::logger::PRIO_UNDEFINED) {
      set_logging(p);
    }

    double tlunit = CLHEP::micrometer;
    if (config_.has_key ("tolerance.length_unit")) {
      const std::string tolerance_length_unit_str = config_.fetch_string ("tolerance.length_unit");
      tlunit = datatools::units::get_length_unit_from (tolerance_length_unit_str);
    }

    double taunit = CLHEP::degree;
    if (config_.has_key ("tolerance.angle_unit")) {
      const std::string tolerance_angle_unit_str = config_.fetch_string ("tolerance.angle_unit");
      taunit = datatools::units::get_angle_unit_from (tolerance_angle_unit_str);
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

    config_.export_starting_with(_auxiliaries_, "aux.");

    return;
  }

  void i_object_3d::reset ()
  {
    _auxiliaries_.clear();
    this->i_object_3d::set_defaults();
    return;
  }

  void i_object_3d::tree_dump(std::ostream & out_,
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
         << "Shape name : \"" << get_shape_name () << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority : \"" << datatools::logger::get_priority_label(get_logging()) << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Tolerance  = " << _tolerance_ / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Angular tolerance  = " << _angular_tolerance_ / CLHEP::radian << " radian" << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Auxiliary properties : ";
      if ( _auxiliaries_.size () == 0) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _auxiliaries_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Dimensionality : " << get_dimensional() <<  std::endl;
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "User draw : " << (has_user_draw() ? "Yes": "No") << std::endl;

    }

    return;
  }

  // static
  void i_object_3d::init_ocd(datatools::object_configuration_description & ocd_)
  {

    datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "");

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

    /// "aux.*"

    return;
  }

  /* Object 3D getter interface */

  const i_object_3d *
  i_object_3d::i_getter::get (const std::string & name_)
  {
    datatools::properties params;
    return get(name_, params);
  }

} // end of namespace geomtools
