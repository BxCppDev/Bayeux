// -*- mode: c++ ; -*-
/* simple_boxed_model.cc
 */

#include <geomtools/simple_boxed_model.h>

#include <iostream>
#include <string>
#include <exception>
#include <limits>

#include <datatools/units.h>

namespace geomtools {

  const geomtools::box & simple_boxed_model::get_box () const
  {
    return _solid_;
  }

  const geomtools::box & simple_boxed_model::get_solid () const
  {
    return _solid_;
  }

  void simple_boxed_model::set_x (double x_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    DT_THROW_IF (x_ <= 0.0 * CLHEP::mm, std::domain_error, "Invalid X " << x_ / CLHEP::mm << " mm !");
    _x_ = x_;
    return;
  }

  void simple_boxed_model::set_y (double y_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    DT_THROW_IF (y_ <= 0.0 * CLHEP::mm, std::domain_error, "Invalid Y " << y_ / CLHEP::mm << " mm !");
    _y_ = y_;
    return;
  }

  void simple_boxed_model::set_z (double z_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    DT_THROW_IF (z_ <= 0.0 * CLHEP::mm, std::domain_error, "Invalid Z " << z_ / CLHEP::mm << " mm !");
    _z_ = z_;
    return;
  }

  double simple_boxed_model::get_x () const
  {
    return _x_;
  }

  double simple_boxed_model::get_y () const
  {
    return _y_;
  }

  double simple_boxed_model::get_z () const
  {
    return _z_;
  }

  void simple_boxed_model::set_material_name (const std::string & mn_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _material_name_ = mn_;
    return;
  }

  const std::string & simple_boxed_model::get_material_name () const
  {
    return _material_name_;
  }

  std::string simple_boxed_model::get_model_id () const
  {
    return "geomtools::simple_boxed_model";
  }

  simple_boxed_model::simple_boxed_model () : i_boxed_model ()
  {
    _material_name_ = "";
    _x_ = std::numeric_limits<double>::quiet_NaN ();
    _y_ = std::numeric_limits<double>::quiet_NaN ();
    _z_ = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  simple_boxed_model::~simple_boxed_model ()
  {
    return;
  }

  void simple_boxed_model::_at_construct (const datatools::properties & config_,
                                          models_col_type * /*models_*/)
  {
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit"))
      {
        const std::string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    DT_THROW_IF (! config_.has_key ("x"), std::logic_error, "Missing 'x' property in simple boxed model '" << get_name() << "' !");
    double x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) {
      x *= lunit;
    }
    DT_THROW_IF (! config_.has_key ("y"), std::logic_error, "Missing 'y' property in simple boxed model '" << get_name() << "' !");
    double y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) {
      y *= lunit;
    }
    DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing 'z' property in simple boxed model '" << get_name() << "' !");
    double z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) {
      z *= lunit;
    }

    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in simple boxed model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");
    set_material_name (material_name);

    set_x (x);
    set_y (y);
    set_z (z);

    _solid_.reset();
    _solid_.set_x(get_x ());
    _solid_.set_y(get_y ());
    _solid_.set_z(get_z ());
    _solid_.lock();
    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid box dimensions in simple boxed model '" << get_name() << "' !");

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (material_name);

    return;
  }

  void simple_boxed_model::tree_dump (std::ostream & out_,
                                      const std::string & title_ ,
                                      const std::string & indent_,
                                      bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Material name : '" << get_material_name () << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X : " << get_x () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y : " << get_y () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z : " << get_z () / CLHEP::mm << " mm" << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _solid_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(simple_boxed_model,"geomtools::simple_boxed_model")

} // end of namespace geomtools
