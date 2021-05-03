// multiple_items_model.cc

// Ourselves:
#include <geomtools/multiple_items_model.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/exception.h>

// This project:
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/utils.h>
#include <geomtools/visibility.h>
#include <geomtools/material.h>

namespace geomtools {

  MWIM & multiple_items_model::grab_internals ()
  {
    return _internals_;
  }

  const MWIM & multiple_items_model::get_internals () const
  {
    return _internals_;
  }

  std::string multiple_items_model::get_model_id () const
  {
    return "geomtools::multiple_items_model";
  }

  const box & multiple_items_model::get_box () const
  {
    return _solid_;
  }

  const box & multiple_items_model::get_solid () const
  {
    return _solid_;
  }

  void multiple_items_model::set_material_name (const std::string & m_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _material_name_ = m_;
    return;
  }

  const std::string & multiple_items_model::get_material_name () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
     return _material_name_;
  }

  multiple_items_model::multiple_items_model ()
    : i_boxed_model ()
  {
    return;
  }

  multiple_items_model::~multiple_items_model ()
  {
    //__items.clear ();
    return;
  }

  void multiple_items_model::_at_construct (const datatools::properties & config_,
                                            models_col_type * models_)
  {

    /*** box dimensions ***/
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string length_unit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (length_unit_str);
    }

    double x;
    datatools::invalidate (x);
    DT_THROW_IF (! config_.has_key ("x"), std::logic_error,
                 "Missing 'x' property  in multiple items model '" << get_name() << "' !");
    x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) {
      x *= lunit;
    }

    double y;
    datatools::invalidate (y);
    DT_THROW_IF (! config_.has_key ("y"), std::logic_error,
                 "Missing 'y' property  in multiple items model '" << get_name() << "' !");
    y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) {
      y *= lunit;
    }

    double z;
    datatools::invalidate (z);
    DT_THROW_IF (! config_.has_key ("z"), std::logic_error,
                 "Missing 'z' property  in multiple items model '" << get_name() << "' !");
    z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) {
      z *= lunit;
    }

    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in multiple items model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");
    set_material_name (material_name);

    _solid_.reset ();
    _solid_.set_x (x);
    _solid_.set_y (y);
    _solid_.set_z (z);
    _solid_.lock ();
    DT_THROW_IF (!_solid_.is_valid (), std::logic_error, "Invalid solid in multiple items model '" << get_name() << "' !");

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (_material_name_);

    _internals_.plug_internal_models (config_, grab_logical (), models_);

    return;
  }


  void multiple_items_model::tree_dump (std::ostream & out_,
                                        const std::string & title_,
                                        const std::string & indent_,
                                        bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    {
      // Material:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Material : " << get_material_name () << std::endl;
    }

    {
      // Items:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Internal items : " << get_internals ().get_items ().size () << std::endl;
    }

    {
      // Solid:
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _solid_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(multiple_items_model,"geomtools::multiple_items_model")

} // end of namespace geomtools
