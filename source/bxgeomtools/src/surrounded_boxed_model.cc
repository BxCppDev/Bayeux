// surrounded_boxed_model.cc

// Ourselves:
#include <geomtools/surrounded_boxed_model.h>

// Standard library:
#include <exception>
#include <limits>
#include <list>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
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
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed yet!");

    surrounding_dict_type::const_iterator found = _surrounding_items_.find (i_);
    DT_THROW_IF (found != _surrounding_items_.end (),
                 std::logic_error,
                 "Dictionary already has a model at position " << " '" << i_ << "' !");
    surrounding_item si;
    _surrounding_items_[i_] = si;
    _surrounding_items_[i_].model = &model_;
    _surrounding_items_[i_].placmt.invalidate ();
    std::string label = label_;
    if (label.empty ()) {
      label = SURROUNDING_LABEL + "_" + position_labels()[i_];
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

  std::string surrounded_boxed_model::get_model_id () const
  {
    return "geomtools::surrounded_boxed_model";
  }

  // static
  const std::vector<std::string> & surrounded_boxed_model::position_labels()
  {
    static std::vector<std::string> _position_labels;
    if (_position_labels.size () == 0) {
      _position_labels.push_back("back");
      _position_labels.push_back("front");
      _position_labels.push_back("left");
      _position_labels.push_back("right");
      _position_labels.push_back("bottom");
      _position_labels.push_back("top");
    }
    return _position_labels;
  }

  surrounded_boxed_model::surrounded_boxed_model () : i_boxed_model ()
  {
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

  void surrounded_boxed_model::_at_construct (const datatools::properties & config_,
                                              models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (),  "Entering...");

    // material:
    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in surrounded boxed model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");
    set_material_name (material_name);

    // length unit:
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string length_unit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (length_unit_str);
    }

    // Surrounded model:
    DT_THROW_IF (! config_.has_key ("surrounded.model"),
                 std::logic_error,
                 "Missing 'surrounded.model' property in surrounded boxed model '" << get_name() << "' !");
    const std::string surrounded_model_name = config_.fetch_string ("surrounded.model");

    // Surrounded label:
    _surrounded_label_ = SURROUNDED_LABEL;
    if (config_.has_key ("surrounded.label")) {
      set_surrounded_label (config_.fetch_string ("surrounded.label"));
    }

    // Centering of the surrounded item:
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

    // check models:
    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary in surrounded boxed model '" << get_name() << "' !");

    // check if surrounded model exists:
    {
      models_col_type::const_iterator found = models_->find(surrounded_model_name);
      DT_THROW_IF (found == models_->end (),
                   std::logic_error,
                   "Cannot find surrounded model with name '" << surrounded_model_name << "' in surrounded boxed model '" << get_name() << "' !");
      // Check if the surrounded model's stackability:
      // 2017-02-23, FM: For now we check full stackability on all directions around the volume.
      // In principle, we should only check directions with a surrounding volume.
      DT_THROW_IF (! i_shape_3d::check_stackability(found->second->get_logical().get_shape(),
                                                    stackable::STACKABILITY_STRONG),
                   std::logic_error,
                   "The surrounded model '" << found->second->get_name() << "' is not stackable in surrounded boxed model '" << get_name() << "' !");
      set_surrounded_model(*(found->second));
    }

    // loop over surrounding models:
    {
      int ipos = DIRECTION_BACK;
      for (std::vector<std::string>::const_iterator ilabel = position_labels().begin ();
           ilabel != position_labels().end ();
           ilabel++, ipos++){
        std::string surrounding_model_name;
        std::ostringstream surrounding_item_prop;
        surrounding_item_prop << "surrounded." <<  *ilabel << "_" << MODEL_PROPERTY_PREFIX;
        if (config_.has_key (surrounding_item_prop.str ())) {
          surrounding_model_name = config_.fetch_string (surrounding_item_prop.str ());
        } else {
          DT_LOG_TRACE (get_logging_priority (), "No '" << surrounding_item_prop.str () << "' property !");
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
                     "Cannot find surrounding model with name '"<< surrounding_model_name << "' in surrounded boxed model '" << get_name() << "' !");

        // check the surrounding model's stackability::
        stackable::stackability_mode sdsm = stackable::STACKABILITY_NONE;
        if (ipos == DIRECTION_BACK) {
          sdsm = stackable::STACKABILITY_XMAX;
        } else if (ipos == DIRECTION_FRONT) {
          sdsm = stackable::STACKABILITY_XMIN;
        } else if (ipos == DIRECTION_LEFT) {
          sdsm = stackable::STACKABILITY_YMAX;
        } else if (ipos == DIRECTION_RIGHT) {
          sdsm = stackable::STACKABILITY_YMIN;
        } else if (ipos == DIRECTION_BOTTOM) {
          sdsm = stackable::STACKABILITY_ZMAX;
        } else if (ipos == DIRECTION_TOP) {
          sdsm = stackable::STACKABILITY_ZMIN;
        }
        // 2017-02-23, FM: For now we check full stackability on all directions around the volume.
        sdsm = stackable::STACKABILITY_STRONG;
        DT_THROW_IF (! i_shape_3d::check_stackability(found->second->get_logical().get_shape(), sdsm),
                     std::logic_error,
                     "The " << *ilabel << " surrounding model '" << found->second->get_name ()
                     << "' has no required stackability for surrounded boxed model '" << get_name() << "' !");
        add_surrounding_model(ipos, *(found->second), label_name);
      } // end of for
    }
    // end of loop over surrounding models.

    // compute main box dimensions:
    mygsl::min_max mmx0;
    mygsl::min_max mmy0;
    mygsl::min_max mmz0;
    mygsl::min_max mmx1;
    mygsl::min_max mmy1;
    mygsl::min_max mmz1;
    const i_shape_3d & the_shape = _surrounded_model_->get_logical ().get_shape ();

    // try to get a stackable data from the shape:
    stackable_data the_SD;
    DT_THROW_IF ((! i_shape_3d::pickup_stackable(the_shape, the_SD)),
                 std::logic_error,
                 "Cannot surround/stack the '" << the_shape.get_shape_name () << "' shape in surrounded boxed model '" << get_name() << "' !");
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
                   << the_surrounding_shape.get_shape_name () << "' surrounding shape in surrounded boxed model '" << get_name() << "' !");
      const double g2xmin = the_SD2.get_xmin ();
      const double g2xmax = the_SD2.get_xmax ();
      const double g2ymin = the_SD2.get_ymin ();
      const double g2ymax = the_SD2.get_ymax ();
      const double g2zmin = the_SD2.get_zmin ();
      const double g2zmax = the_SD2.get_zmax ();

      if (position == DIRECTION_BACK) {
        dx0 = std::abs (g2xmax - g2xmin);
        x0 -= dx0;
        mmy0.add (g2ymin);
        mmy1.add (g2ymax);
        mmz0.add (g2zmin);
        mmz1.add (g2zmax);
      }
      if (position == DIRECTION_FRONT) {
        dx1 = std::abs (g2xmax - g2xmin);
        x1 += dx1;
        mmy0.add (g2ymin);
        mmy1.add (g2ymax);
        mmz0.add (g2zmin);
        mmz1.add (g2zmax);
      }
      if (position == DIRECTION_LEFT) {
        dy0 = std::abs (g2ymax - g2ymin);
        y0 -= dy0;
        mmx0.add (g2xmin);
        mmx1.add (g2xmax);
        mmz0.add (g2zmin);
        mmz1.add (g2zmax);
      }
      if (position == DIRECTION_RIGHT) {
        dy1 = std::abs (g2ymax - g2ymin);
        y1 += dy1;
        mmx0.add (g2xmin);
        mmx1.add (g2xmax);
        mmz0.add (g2zmin);
        mmz1.add (g2zmax);
      }
      if (position == DIRECTION_BOTTOM) {
        dz0 = std::abs (g2zmax - g2zmin);
        z0 -= dz0;
        mmx0.add (g2xmin);
        mmx1.add (g2xmax);
        mmy0.add (g2ymin);
        mmy1.add (g2ymax);
      }
      if (position == DIRECTION_TOP) {
        dz1 = std::abs (g2zmax - g2zmin);
        z1 += dz1;
        mmx0.add (g2xmin);
        mmx1.add (g2xmax);
        mmy0.add (g2ymin);
        mmy1.add (g2ymax);
      }
    }

    if (mmx0.is_valid()) {
      if (mmx0.get_min () < x0) x0 = mmx0.get_min ();
    }
    if (mmx1.is_valid()) {
      if (mmx1.get_max () > x1) x1 = mmx1.get_max ();
    }
    if (mmy0.is_valid()) {
      if (mmy0.get_min () < y0) y0 = mmy0.get_min ();
    }
    if (mmy1.is_valid()) {
      if (mmy1.get_max () > y1) y1 = mmy1.get_max ();
    }
    if (mmz0.is_valid()) {
      if (mmz0.get_min () < z0) z0 = mmz0.get_min ();
    }
    if (mmz1.is_valid()) {
      if (mmz1.get_max () > z1) z1 = mmz1.get_max ();
    }
    double surrounded_x = 0;
    double surrounded_y = 0;
    double surrounded_z = 0;
    double dim_x = 2 * std::max (x1, std::abs (x0));
    double dim_y = 2 * std::max (y1, std::abs (y0));
    double dim_z = 2 * std::max (z1, std::abs (z0));
    if (! _centered_x_) {
      DT_LOG_TRACE (get_logging_priority (), "X-centered with x0=" << x0);
      dim_x = x1 - x0;
      surrounded_x = -0.5 * dim_x - x0;
    }
    if (! _centered_y_) {
      DT_LOG_TRACE (get_logging_priority (), "Y-centered with y0=" << y0);
      dim_y = y1 - y0;
      surrounded_y = -0.5 * dim_y - y0;
    }
    if (! _centered_z_) {
      DT_LOG_TRACE (get_logging_priority (), "Z-centered with z0=" << z0);
      dim_z = z1 - z0;
      surrounded_z = -0.5 * dim_z - z0;
    }

    double x = 0.0;
    if (config_.has_key ("x")) {
      x = config_.fetch_real ("x");
      if (! config_.has_explicit_unit ("x")) x *= lunit;
      DT_THROW_IF (x < dim_x,
                   std::logic_error,
                   "Enforced X dimension '" << x / CLHEP::mm
                   << "' mm (<" << dim_x / CLHEP::mm << ") is too small for surrounded components to fit in surrounded boxed model '" << get_name() << "' !");
      dim_x = x;
    }

    double y = 0.0;
    if (config_.has_key ("y")) {
      y = config_.fetch_real ("y");
      if (! config_.has_explicit_unit ("y")) y *= lunit;
      DT_THROW_IF (y < dim_y,
                   std::logic_error,
                   "Enforced Y dimension '" << y / CLHEP::mm
                   << "' mm (<" << dim_y / CLHEP::mm << ") is too small for surrounded components to fit !");
      dim_y = y;
    }

    double z = 0.0;
    if (config_.has_key ("z")) {
      z = config_.fetch_real ("z");
      if (! config_.has_explicit_unit ("z")) z *= lunit;
      DT_THROW_IF (z < dim_z,
                   std::logic_error,
                   "Enforced Z dimension '" << z / CLHEP::mm
                   << "' mm (<" << dim_z / CLHEP::mm << ") is too small for surrounded components to fit in surrounded boxed model '" << get_name() << "' !");
      dim_z = z;
    }

    // define the enclosing solid box:
    _solid_.reset();
    _solid_.set_x(dim_x);
    _solid_.set_y(dim_y);
    _solid_.set_z(dim_z);
    _solid_.lock();
    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid solid in surrounded boxed model '" << get_name() << "' !");

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (_material_name_);

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
      if (position == DIRECTION_BACK) {
        xi += the_SD.get_xmin ();
        xi -= g2xmax;
      }
      if (position == DIRECTION_FRONT) {
        xi += the_SD.get_xmax ();
        xi -= g2xmin;
      }
      if (position == DIRECTION_LEFT) {
        yi += the_SD.get_ymin ();
        yi -= g2ymax;
      }
      if (position == DIRECTION_RIGHT) {
        yi += the_SD.get_ymax ();
        yi -= g2ymin;
      }
      if (position == DIRECTION_BOTTOM) {
        zi += the_SD.get_zmin ();
        zi -= g2zmax;
      }
      if (position == DIRECTION_TOP) {
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
                                        grab_logical (),
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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(surrounded_boxed_model,"geomtools::surrounded_boxed_model")

} // end of namespace geomtools


// OCD support for class '::geomtools::surrounded_boxed_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::surrounded_boxed_model, ocd_)
{
  ocd_.set_class_name("geomtools::surrounded_boxed_model");
  ocd_.set_class_description("A geometry model implementing a boxed model surrounded with some other volumes on their 6 faces");
  ocd_.set_class_library("geomtools");
  /*
    ocd_.set_class_documentation("  \n"
    );
  */

  // Inherit the OCD support from the parent class:
  geomtools::i_model::init_ocd(ocd_);


  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("material.ref")
      .set_terse_description("The label of the material the mother volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .add_example("Using the 'Air' material::                     \n"
                   "                                               \n"
                   "   material.ref : string = \"Air\"             \n"
                   "                                               \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("length_unit")
      .set_terse_description("The length unit symbol")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("mm")
      .set_long_description("This property set the symbol of the default length\n"
                            "unit.                                             \n"
                            )
      .add_example("Using cm::                                       \n"
                   "                                                 \n"
                   "   length_unit : string = \"cm\"                 \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("surrounded.model")
      .set_terse_description("The name of the geometry model to be surrounded")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("All geometry models are searched for an external     \n"
                            "dictionary of models, typically from a model        \n"
                            "factory. They must provide a *stackable* interface. \n"
                            )
      .add_example("Set the name of the model to be surrounded by other models:: \n"
                   "                                                             \n"
                   "   surrounded.model : string = \"vertex_detector.model\"     \n"
                   "                                                             \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("surrounded.label")
      .set_terse_description("The label associated to the geometry model to be surrounded")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string(geomtools::surrounded_boxed_model::SURROUNDED_LABEL)
      .add_example("Set a label associated to the geometry model to be surrounded:: \n"
                   "                                                                \n"
                   "   surrounded.label : string = \"VertexDetector\"               \n"
                   "                                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("surrounded.centered_x")
      .set_terse_description("A flag to center setup in the mother box the along the X axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Center the setup along the X axis::      \n"
                   "                                         \n"
                   "   surrounded.centered_x : boolean = 1   \n"
                   "                                         \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("surrounded.centered_y")
      .set_terse_description("A flag to center setup in the mother box the along the Y axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Center the setup along the Y axis::      \n"
                   "                                         \n"
                   "   surrounded.centered_y : boolean = 1   \n"
                   "                                         \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("surrounded.centered_z")
      .set_terse_description("A flag to center setup in the mother box the along the Z axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Center the setup along the Z axis::      \n"
                   "                                         \n"
                   "   surrounded.centered_z : boolean = 1   \n"
                   "                                         \n"
                   )
      ;
  }

  int ipos = geomtools::DIRECTION_BACK;
  for (std::vector<std::string>::const_iterator ilabel
         = geomtools::surrounded_boxed_model::position_labels().begin ();
       ilabel != geomtools::surrounded_boxed_model::position_labels().end ();
       ilabel++, ipos++) {
    {
      std::ostringstream surrounding_item_key;
      surrounding_item_key << "surrounded." <<  *ilabel << "_" << geomtools::surrounded_boxed_model::MODEL_PROPERTY_PREFIX;
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern(surrounding_item_key.str())
        .set_terse_description(std::string("The name of the surrounding geometry model at ") + *ilabel + " position")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description("All geometry models are searched for an external     \n"
                              "dictionary of models, typically from a model        \n"
                              "factory. They must provide a *stackable* interface. \n"
                              )
        // .add_example("Set the name of the model to be surrounded by other models:: \n"
        //              "                                                             \n"
        //              "   surrounded.model : string = \"vertex_detector.model\"     \n"
        //              "                                                             \n"
        //              )
        ;
    }

    {
      std::ostringstream surrounding_label_key;
      surrounding_label_key << "surrounded." <<  *ilabel << "_" << geomtools::surrounded_boxed_model::LABEL_PROPERTY_PREFIX;
      std::ostringstream def_surrounding_label_key;
      def_surrounding_label_key << geomtools::surrounded_boxed_model::SURROUNDING_LABEL + "_" + *ilabel;
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern(surrounding_label_key.str())
        .set_terse_description(std::string("The label associated to the surrounding geometry model at ") + *ilabel + " position")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string(def_surrounding_label_key.str())
        // .add_example("Set a label associated to the geometry model to be surrounded:: \n"
        //              "                                                                \n"
        //              "   surrounded.label : string = \"VertexDetector\"               \n"
        //              "                                                                \n"
        //              )
        ;
    }
  }

  ocd_.set_configuration_hints("This model is configured through a configuration file that \n"
                               "uses the format of 'datatools::properties' setup file.     \n"
                               "                                                           \n"
                               "Example: ::                                                \n"
                               "                                                           \n"
                               "   material.ref     : string = \"Vacuum\"                        \n"
                               "   length_unit      : string = \"cm\"                            \n"
                               "   surrounded.model        : string = \"dragon_body.model\"      \n"
                               "   surrounded.label        : string = \"Body\"                   \n"
                               "   surrounded.front_model  : string = \"dragon_head.model\"      \n"
                               "   surrounded.front_label  : string = \"Head\"                   \n"
                               "   surrounded.back_model   : string = \"dragon_tail.model\"      \n"
                               "   surrounded.back_label   : string = \"Tail\"                   \n"
                               "   surrounded.left_model   : string = \"dragon_left_wing.model\" \n"
                               "   surrounded.left_label   : string = \"LeftWing\"               \n"
                               "   surrounded.right_model  : string = \"dragon_right_wing.model\"\n"
                               "   surrounded.right_label  : string = \"RightWing\"              \n"
                               "   surrounded.top_model    : string = \"dragon_spine.model\"     \n"
                               "   surrounded.top_label    : string = \"Spine\"                  \n"
                               "   surrounded.bottom_model : string = \"dragon_legs.model\"      \n"
                               "   surrounded.bottom_label : string = \"Legs\"                   \n"
                               "   surrounded.centered_x   : boolean = 0                         \n"
                               "   surrounded.centered_y   : boolean = 1                         \n"
                               "   surrounded.centered_z   : boolean = 0                         \n"
                               "                                                                 \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::surrounded_boxed_model, "geomtools::surrounded_boxed_model")
