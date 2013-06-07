// -*- mode: c++ ; -*-
/* surrounded_boxed_model.cc
 */

#include <geomtools/surrounded_boxed_model.h>

#include <exception>
#include <limits>
#include <list>
#include <map>

#include <mygsl/min_max.h>

#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

//#include <geomtools/regular_linear_placement.h>
#include <geomtools/visibility.h>

namespace geomtools {

  const std::string surrounded_boxed_model::SURROUNDED_LABEL      = "surrounded";
  const std::string surrounded_boxed_model::SURROUNDING_LABEL     = "surrounding";
  const std::string surrounded_boxed_model::LABEL_PROPERTY_PREFIX = "label";
  const std::string surrounded_boxed_model::MODEL_PROPERTY_PREFIX = "model";

  const std::string & surrounded_boxed_model::get_material_name () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    return _material_name_;
  }

  void surrounded_boxed_model::set_material_name (const std::string & mn_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed yet");
    _material_name_ = mn_;
    return;
  }

  const geomtools::box & surrounded_boxed_model::get_box () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    return _solid_;
  }

  const geomtools::box & surrounded_boxed_model::get_solid () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    return _solid_;
  }

  /*** surrounded model ***/
  void surrounded_boxed_model::set_surrounded_label (const std::string & label_)
  {
    _surrounded_label_ = label_;
    return;
  }

  const std::string & surrounded_boxed_model::get_surrounded_label () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    return _surrounded_label_;
  }

  const i_model & surrounded_boxed_model::get_surrounded_model () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    return *_surrounded_model_;
  }

  void surrounded_boxed_model::set_surrounded_model (const i_model & model_)
  {
    _surrounded_model_ = &model_;
    return;
  }

  /*** surrounding models ***/
  size_t surrounded_boxed_model::get_number_of_surrounding_items () const
  {
    return _surrounding_items_.size ();
  }

  bool surrounded_boxed_model::has_surrounding_model (const std::string & label_) const
  {
    labels_dict_type::const_iterator found = _surrounding_labels_.find (label_);
    return found != _surrounding_labels_.end ();
  }

  void surrounded_boxed_model::add_surrounding_model (int i_,
                                                      const i_model & model_,
                                                      const std::string & label_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed yet");

    surrounding_dict_type::const_iterator found = _surrounding_items_.find (i_);
    DT_THROW_IF (found != _surrounding_items_.end (),
                 std::logic_error,
                 "Dictionary already has a model at position " << " '" << i_ << "' !");
    surrounding_item si;
    _surrounding_items_[i_] = si;
    _surrounding_items_[i_].model = &model_;
    _surrounding_items_[i_].placmt.invalidate ();
    std::string label = label_;
    if (label.empty ())
      {
        label = SURROUNDING_LABEL + "_" + _position_labels_[i_];
      }
    DT_THROW_IF (_surrounding_labels_.find (label) != _surrounding_labels_.end (),
                 std::logic_error,
                 "Label '" << label << "' is already used !");
    _surrounding_items_[i_].label = label;
    _surrounding_labels_[label] = i_;
    return;
  }

  const surrounded_boxed_model::surrounding_dict_type & surrounded_boxed_model::get_surrounding_items () const
  {
    return _surrounding_items_;
  }

  const surrounded_boxed_model::labels_dict_type & surrounded_boxed_model::get_surrounding_labels () const
  {
    return _surrounding_labels_;
  }

  const surrounded_boxed_model::surrounding_item &
  surrounded_boxed_model::get_surrounding_item (const std::string & label_) const
  {
    labels_dict_type::const_iterator found = _surrounding_labels_.find (label_);
    DT_THROW_IF (found == _surrounding_labels_.end (),
                 std::logic_error,
                 "Dictionary has no surrounding model with " << "label '" << label_ << "' !");
    return (this->get_surrounding_item (found->second));
  }

  const surrounded_boxed_model::surrounding_item &
  surrounded_boxed_model::get_surrounding_item (int i_) const
  {
    surrounding_dict_type::const_iterator found = _surrounding_items_.find (i_);
    DT_THROW_IF (found == _surrounding_items_.end (),
                 std::logic_error,
                 "Dictionary has no surrounding model item with " << "index '" << i_ << "' !");
    return found->second;
  }

  const i_model & surrounded_boxed_model::get_surrounding_model (const std::string & label_) const
  {
    labels_dict_type::const_iterator found = _surrounding_labels_.find (label_);
    DT_THROW_IF (found == _surrounding_labels_.end (),
                 std::logic_error,
                 "Dictionary has no surrounding model with " << "label '" << label_ << "' !");
    return this->get_surrounding_model (found->second);
  }

  const i_model & surrounded_boxed_model::get_surrounding_model (int i_) const
  {
    surrounding_dict_type::const_iterator found = _surrounding_items_.find (i_);
    DT_THROW_IF (found == _surrounding_items_.end (),
                 std::logic_error,
                 "Dictionary has no surrounding model with " << "index '" << i_ << "' !");
    return *(found->second.model);
  }

  /*** model ID ***/

  std::string surrounded_boxed_model::get_model_id () const
  {
    return "geomtools::surrounded_boxed_model";
  }

  // ctor:
  surrounded_boxed_model::surrounded_boxed_model () : i_boxed_model ()
  {
    if (_position_labels_.size () == 0)
      {
        _position_labels_.push_back ("back");
        _position_labels_.push_back ("front");
        _position_labels_.push_back ("left");
        _position_labels_.push_back ("right");
        _position_labels_.push_back ("bottom");
        _position_labels_.push_back ("top");
      }
    _material_name_ = "";
    _surrounded_model_ = 0;
    _centered_x_ = false;
    _centered_y_ = false;
    _centered_z_ = false;
    return;
  }

  // dtor:
  surrounded_boxed_model::~surrounded_boxed_model ()
  {
    return;
  }

  void surrounded_boxed_model::_at_construct (const std::string & name_,
                                              const datatools::properties & config_,
                                              models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (),  "Entering...");
    set_name (name_);

    /*** material ***/
    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property !");
    const std::string material_name = config_.fetch_string ("material.ref");
    set_material_name (material_name);

    /*** length unit ***/
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string length_unit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (length_unit_str);
    }

    /*** Surrounded model ***/
    DT_THROW_IF (! config_.has_key ("surrounded.model"),
                 std::logic_error,
                 "Missing 'surrounded.model' property !");
    const std::string surrounded_model_name = config_.fetch_string ("surrounded.model");

    /*** Surrounded label ***/
    _surrounded_label_ = SURROUNDED_LABEL;
    if (config_.has_key ("surrounded.label")) {
      set_surrounded_label (config_.fetch_string ("surrounded.label"));
    }

    /*** Centering of the surrounded item ***/
    if (config_.has_flag ("surrounded.centered_x")) {
      DT_LOG_TRACE (get_logging_priority (), "X-centered");
      _centered_x_ = true;
    }

    if (config_.has_flag ("surrounded.centered_y")) {
      DT_LOG_TRACE (get_logging_priority (), "Y-centered");
      _centered_y_ = true;
    }

    if (config_.has_flag ("surrounded.centered_z")) {
      DT_LOG_TRACE (get_logging_priority (), "Z-centered");
      _centered_z_ = true;
    }

    /*** check models ***/
    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary !");

    /*** check if surrounded model exists ***/
    {
      models_col_type::const_iterator found = models_->find (surrounded_model_name);
      DT_THROW_IF (found == models_->end (),
                   std::logic_error,
                   "Cannot find surrounded model with name '" << surrounded_model_name << "' !");
      // check if the model is stackable:
      DT_THROW_IF (! i_shape_3d::is_stackable (found->second->get_logical ().get_shape ()),
                   std::logic_error,
                   "The surrounded model '" << found->second->get_name () << "' is not stackable !");
      set_surrounded_model (*(found->second));
    }

    /*** loop over surrounding models ***/
    {
      int ipos = BACK;
      for (std::vector<std::string>::const_iterator ilabel = _position_labels_.begin ();
           ilabel != _position_labels_.end ();
           ilabel++, ipos++){
        std::string surrounding_model_name;
        std::ostringstream surrounding_item_prop;
        surrounding_item_prop << "surrounded." <<  *ilabel << "_" << MODEL_PROPERTY_PREFIX;
        if (config_.has_key (surrounding_item_prop.str ())) {
          surrounding_model_name = config_.fetch_string (surrounding_item_prop.str ());
        } else {
          // if (devel)
          //   {
          //     ostringstream message;
          //     message << "surrounded_boxed_model::_at_construct: "
          //             << "No '" << surrounding_item_prop.str () << "' property !";
          //     clog << datatools::io::devel << message.str () << endl;
          //   }
          continue;
        }
        // attempt to extract a user defined label:
        std::string label_name;
        std::ostringstream label_item_prop;
        label_item_prop << "surrounded." <<  *ilabel << "_" << LABEL_PROPERTY_PREFIX;
        if (config_.has_key (label_item_prop.str ())) {
          label_name = config_.fetch_string (label_item_prop.str ());
        }

        models_col_type::const_iterator found = models_->find (surrounding_model_name);
        DT_THROW_IF (found == models_->end (),
                     std::logic_error,
                     "Cannot find surrounding model with name '"<< surrounding_model_name << "' !");

        // check if the model is stackable:
        DT_THROW_IF (! i_shape_3d::is_stackable (found->second->get_logical ().get_shape ()),
                     std::logic_error,
                     "The " << *ilabel << " surrounding model '" << found->second->get_name ()
                     << "' is not stackable !");
        add_surrounding_model (ipos, *(found->second), label_name);
      } // end of for
    }
    /*** end of loop over surrounding models ***/

    /*** compute main box dimensions ***/
    mygsl::min_max mmx0;
    mygsl::min_max mmy0;
    mygsl::min_max mmz0;
    mygsl::min_max mmx1;
    mygsl::min_max mmy1;
    mygsl::min_max mmz1;
    const i_shape_3d & the_shape = _surrounded_model_->get_logical ().get_shape ();

    // try to get a stackable data from the shape:
    stackable_data the_SD;
    DT_THROW_IF ((! i_shape_3d::pickup_stackable (the_shape, the_SD)),
                 std::logic_error,
                 "Cannot surround/stack the '" << the_shape.get_shape_name () << "' shape !");
    const double gxmin = the_SD.get_xmin ();
    const double gxmax = the_SD.get_xmax ();
    const double gymin = the_SD.get_ymin ();
    const double gymax = the_SD.get_ymax ();
    const double gzmin = the_SD.get_zmin ();
    const double gzmax = the_SD.get_zmax ();

    // border coordinates:
    double x0 = gxmin;
    double x1 = gxmax;
    double y0 = gymin;
    double y1 = gymax;
    double z0 = gzmin;
    double z1 = gzmax;

    double dx0 = 0.0;
    double dx1 = 0.0;
    double dy0 = 0.0;
    double dy1 = 0.0;
    double dz0 = 0.0;
    double dz1 = 0.0;
    for (surrounding_dict_type::const_iterator i = _surrounding_items_.begin ();
         i != _surrounding_items_.end ();
         i++) {
      const int position = i->first;
      const surrounding_item & si = i->second;
      const i_model * model = si.model;
      const i_shape_3d & the_surrounding_shape = model->get_logical ().get_shape ();

      // try to get a stackable data from the shape:
      stackable_data the_SD2;
      DT_THROW_IF ((! i_shape_3d::pickup_stackable (the_surrounding_shape, the_SD2)),
                   std::logic_error,
                   "Cannot surround/stack the '"
                   << the_surrounding_shape.get_shape_name () << "' surrounding shape !");
      const double g2xmin = the_SD2.get_xmin ();
      const double g2xmax = the_SD2.get_xmax ();
      const double g2ymin = the_SD2.get_ymin ();
      const double g2ymax = the_SD2.get_ymax ();
      const double g2zmin = the_SD2.get_zmin ();
      const double g2zmax = the_SD2.get_zmax ();

      if (position == BACK)
        {
          dx0 = abs (g2xmax - g2xmin);
          x0 -= dx0;
          mmy0.add (g2ymin);
          mmy1.add (g2ymax);
          mmz0.add (g2zmin);
          mmz1.add (g2zmax);
        }
      if (position == FRONT)
        {
          dx1 = abs (g2xmax - g2xmin);
          x1 += dx1;
          mmy0.add (g2ymin);
          mmy1.add (g2ymax);
          mmz0.add (g2zmin);
          mmz1.add (g2zmax);
        }
      if (position == LEFT)
        {
          dy0 = abs (g2ymax - g2ymin);
          y0 -= dy0;
          mmx0.add (g2xmin);
          mmx1.add (g2xmax);
          mmz0.add (g2zmin);
          mmz1.add (g2zmax);
        }
      if (position == RIGHT)
        {
          dy1 = abs (g2ymax - g2ymin);
          y1 += dy1;
          mmx0.add (g2xmin);
          mmx1.add (g2xmax);
          mmz0.add (g2zmin);
          mmz1.add (g2zmax);
        }
      if (position == BOTTOM)
        {
          dz0 = abs (g2zmax - g2zmin);
          z0 -= dz0;
          mmx0.add (g2xmin);
          mmx1.add (g2xmax);
          mmy0.add (g2ymin);
          mmy1.add (g2ymax);
        }
      if (position == TOP)
        {
          dz1 = abs (g2zmax - g2zmin);
          z1 += dz1;
          mmx0.add (g2xmin);
          mmx1.add (g2xmax);
          mmy0.add (g2ymin);
          mmy1.add (g2ymax);
        }
    }

    if (mmx0.get_min () < x0) x0 = mmx0.get_min ();
    if (mmx1.get_max () > x1) x1 = mmx1.get_max ();
    if (mmy0.get_min () < y0) y0 = mmy0.get_min ();
    if (mmy1.get_max () > y1) y1 = mmy1.get_max ();
    if (mmz0.get_min () < z0) z0 = mmz0.get_min ();
    if (mmz1.get_max () > z1) z1 = mmz1.get_max ();
    double surrounded_x = 0;
    double surrounded_y = 0;
    double surrounded_z = 0;
    double dim_x = 2 * std::max (x1, std::abs (x0));
    double dim_y = 2 * std::max (y1, std::abs (y0));
    double dim_z = 2 * std::max (z1, std::abs (z0));
    if (! _centered_x_)
      {
        DT_LOG_TRACE (get_logging_priority (), "X-centered with x0=" << x0);
        dim_x = x1 - x0;
        surrounded_x = -0.5 * dim_x - x0;
      }
    if (! _centered_y_)
      {
        DT_LOG_TRACE (get_logging_priority (), "Y-centered with y0=" << y0);
        dim_y = y1 - y0;
        surrounded_y = -0.5 * dim_y - y0;
      }
    if (! _centered_z_)
      {
        DT_LOG_TRACE (get_logging_priority (), "Z-centered with z0=" << z0);
        dim_z = z1 - z0;
        surrounded_z = -0.5 * dim_z - z0;
      }

    double x = 0.0;
    if (config_.has_key ("x"))
      {
        x = config_.fetch_real ("x");
        if (! config_.has_explicit_unit ("x")) x *= lunit;
        DT_THROW_IF (x < dim_x,
                     std::logic_error,
                     "Enforced X dimension '" << x / CLHEP::mm
                     << "' mm (<" << dim_x / CLHEP::mm << ") is too small for surrounded components to fit !");
        dim_x = x;
      }

    double y = 0.0;
    if (config_.has_key ("y"))
      {
        y = config_.fetch_real ("y");
        if (! config_.has_explicit_unit ("y")) y *= lunit;
        DT_THROW_IF (y < dim_y,
                     std::logic_error,
                     "Enforced Y dimension '" << y / CLHEP::mm
                     << "' mm (<" << dim_y / CLHEP::mm << ") is too small for surrounded components to fit !");
        dim_y = y;
      }

    double z = 0.0;
    if (config_.has_key ("z"))
      {
        z = config_.fetch_real ("z");
        if (! config_.has_explicit_unit ("z")) z *= lunit;
        DT_THROW_IF (z < dim_z,
                     std::logic_error,
                     "Enforced Z dimension '" << z / CLHEP::mm
                     << "' mm (<" << dim_z / CLHEP::mm << ") is too small for surrounded components to fit !");
        dim_z = z;
      }

    // define the enclosing solid box:
    _solid_.reset ();
    _solid_.set_x (dim_x);
    _solid_.set_y (dim_y);
    _solid_.set_z (dim_z);
    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid solid !");

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_solid_);
    get_logical ().set_material_ref (get_material_name ());

    // placement of the surrounded solid:
    _surrounded_placmt_.set (surrounded_x, surrounded_y, surrounded_z, 0.0, 0.0, 0.0);
    _surrounded_phys_.set_name (i_model::make_physical_volume_name (_surrounded_label_));
    _surrounded_phys_.set_placement (_surrounded_placmt_);
    _surrounded_phys_.set_logical (_surrounded_model_->get_logical ());
    _surrounded_phys_.set_mother (get_logical ());

    // placement of the surrounding solids:
    for (surrounding_dict_type::iterator i = _surrounding_items_.begin ();
         i != _surrounding_items_.end ();
         i++) {
      const int position = i->first;
      surrounding_item & si = i->second;
      const i_model * model = si.model;
      const i_shape_3d & the_surrounding_shape = model->get_logical ().get_shape ();
      double xi = surrounded_x;
      double yi = surrounded_y;
      double zi = surrounded_z;

      // try to get a stackable data from the shape:
      stackable_data the_SD2;
      i_shape_3d::pickup_stackable (the_surrounding_shape, the_SD2);
      const double g2xmin = the_SD2.get_xmin ();
      const double g2xmax = the_SD2.get_xmax ();
      const double g2ymin = the_SD2.get_ymin ();
      const double g2ymax = the_SD2.get_ymax ();
      const double g2zmin = the_SD2.get_zmin ();
      const double g2zmax = the_SD2.get_zmax ();
      if (position == BACK) {
        xi += the_SD.get_xmin ();
        xi -= g2xmax;
      }
      if (position == FRONT) {
        xi += the_SD.get_xmax ();
        xi -= g2xmin;
      }
      if (position == LEFT) {
        yi += the_SD.get_ymin ();
        yi -= g2ymax;
      }
      if (position == RIGHT) {
        yi += the_SD.get_ymax ();
        yi -= g2ymin;
      }
      if (position == BOTTOM) {
        zi += the_SD.get_zmin ();
        zi -= g2zmax;
      }
      if (position == TOP) {
        zi += the_SD.get_zmax ();
        zi -= g2zmin;
      }
      si.placmt.set (xi, yi, zi, 0.0, 0.0, 0.0);
      si.phys.set_name (i_model::make_physical_volume_name (si.label));
      si.phys.set_placement (si.placmt);
      si.phys.set_logical (si.model->get_logical ());
      si.phys.set_mother (get_logical ());
    }

    // 2011-12-05 FM : add support for additional internal objects :
    if (_internals_.get_number_of_items () == 0) {
       _internals_.plug_internal_models (config_,
                                         get_logical (),
                                         models_);
    }

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void surrounded_boxed_model::tree_dump (std::ostream & out_,
                                          const std::string & title_ ,
                                          const std::string & indent_,
                                          bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Material : " << get_material_name () << std::endl;
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "X-centered : " << _centered_x_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Y-centered : " << _centered_y_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Z-centered : " << _centered_z_ << std::endl;
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Surrounded model : " << _surrounded_label_ << std::endl;
    }

    {
      for (labels_dict_type::const_iterator i = _surrounding_labels_.begin ();
           i != _surrounding_labels_.end ();
           i++)
        {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Surrounding model : " << "'" << i->first << "'" << " [rank==" << i->second << "]" << std::endl;
        }
    }

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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(surrounded_boxed_model,"geomtools::surrounded_boxed_model");

} // end of namespace geomtools

// end of surrounded_boxed_model.cc
