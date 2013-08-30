// -*- mode: c++ ; -*-
/* stacked_model.cc
 */

#include <geomtools/stacked_model.h>
//
#include <exception>
#include <limits>

#include <mygsl/min_max.h>

#include <datatools/units.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

#include <geomtools/i_stackable.h>
#include <geomtools/visibility.h>

namespace geomtools {

  const std::string stacked_model::STACKED_PROPERTY_PREFIX       = "stacked.";
  const std::string stacked_model::STACKED_MODEL_PROPERTY_PREFIX = "stacked.model_";
  const std::string stacked_model::STACKED_LABEL_PROPERTY_PREFIX = "stacked.label_";
  const std::string stacked_model::DEFAULT_STACKED_LABEL_PREFIX  = "stacked";
  const double stacked_model::DEFAULT_MECHANICS_PLAY = 0.0 * CLHEP::mm;
  const double stacked_model::DEFAULT_NUMERICS_PLAY  = 0.0 * CLHEP::mm;


  /****************************************************************/

  bool stacked_model::is_box_solid() const
  {
    return true;
  }

  bool stacked_model::is_cylinder_solid() const
  {
    return false;
  }

  stacked_model::stacked_item::stacked_item ()
  {
    model = 0;
    datatools::invalidate (limit_min);
    datatools::invalidate (limit_max);
    return;
  }

  double stacked_model::stacked_item::get_limit_min () const
  {
    return limit_min;
  }

  double stacked_model::stacked_item::get_limit_max () const
  {
    return limit_max;
  }

  bool stacked_model::stacked_item::has_limit_min () const
  {
    return datatools::is_valid (limit_min);
  }

  bool stacked_model::stacked_item::has_limit_max () const
  {
    return datatools::is_valid (limit_max);
  }

  const std::string & stacked_model::stacked_item::get_label () const
  {
    return label;
  }
  const i_model & stacked_model::stacked_item::get_model () const
  {
    return *model;
  }
  const placement & stacked_model::stacked_item::get_placement () const
  {
    return placmt;
  }
  const physical_volume & stacked_model::stacked_item::get_physical_volume () const
  {
    return phys;
  }

  /****************************************************************/

  // registration ID:
  std::string stacked_model::get_model_id () const
  {
    return "geomtools::stacked_model";
  }

  const std::string & stacked_model::get_material_name () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    return _material_name_;
  }

  void stacked_model::set_material_name (const std::string & mn_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed yet");
    _material_name_ = mn_;
    return;
  }

  void stacked_model::set_stacking_axis (int a_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed yet");
    DT_THROW_IF ((a_ < STACKING_ALONG_X) || (a_ > STACKING_ALONG_Z),
                 std::range_error, "Invalid stacking axis !");
    _stacking_axis_ = a_;
    return;
  }

  int stacked_model::get_stacking_axis () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    return _stacking_axis_;
  }

  bool stacked_model::is_stacking_along_x () const
  {
    return _stacking_axis_ == STACKING_ALONG_X;
  }

  bool stacked_model::is_stacking_along_y () const
  {
    return _stacking_axis_ == STACKING_ALONG_Y;
  }

  bool stacked_model::is_stacking_along_z () const
  {
    return _stacking_axis_ == STACKING_ALONG_Z;
  }

  const geomtools::box & stacked_model::get_box () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    return _solid_;
  }

  const geomtools::box & stacked_model::get_solid () const
  {
    DT_THROW_IF (! is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");
    // if (is_box_solid()) {
    //   return _solid_;
    // }
    // if (is_cylinder_solid()) {
    //   return _cyl_solid_;
    // }
    return _solid_;
  }

  void stacked_model::add_stacked_model (int i_, const i_model & model_, const std::string & label_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has not been constructed yet");

    stacked_dict_type::const_iterator found = _stacked_models_.find (i_);
    DT_THROW_IF (found != _stacked_models_.end (),
                 std::logic_error,
                 "Dictionary already has a model with " << "index '" << i_ << "' !");
    stacked_item bi;
    _stacked_models_[i_] = bi;
    _stacked_models_[i_].model = &model_;
    _stacked_models_[i_].placmt.invalidate ();
    std::string label = label_;
    if (label.empty ())
      {
        std::ostringstream label_oss;
        label_oss << DEFAULT_STACKED_LABEL_PREFIX << "__" << i_ << "__";
        label = label_oss.str ();
      }
    else
      {
        DT_THROW_IF (_labels_.find (label) != _labels_.end (),
                     std::logic_error,
                     "Label '" << label << "' is already used !");
      }
    _stacked_models_[i_].label = label;
    _labels_[label] = i_;
    return;
  }

  const stacked_model::stacked_dict_type & stacked_model::get_models () const
  {
    return _stacked_models_;
  }

  const stacked_model::labels_dict_type & stacked_model::get_labels () const
  {
    return _labels_;
  }

  size_t stacked_model::get_number_of_stacked_models () const
  {
    return _stacked_models_.size ();
  }

  bool stacked_model::has_stacked_model (const std::string & label_) const
  {
    labels_dict_type::const_iterator found = _labels_.find (label_);
    return found != _labels_.end ();
  }

  const stacked_model::stacked_item & stacked_model::get_stacked_item (const std::string & label_) const
  {
    labels_dict_type::const_iterator found = _labels_.find (label_);
    DT_THROW_IF (found == _labels_.end (),
                 std::logic_error,
                 "Dictionary has no model with label '" << label_ << "' !");
    return (this->get_stacked_item (found->second));
  }

  const stacked_model::stacked_item & stacked_model::get_stacked_item (int i_) const
  {
    stacked_dict_type::const_iterator found = _stacked_models_.find (i_);
    DT_THROW_IF (found == _stacked_models_.end (),
                 std::logic_error,
                 "Dictionary has no model item with index '" << i_ << "' !");
    return found->second;
  }

  const i_model & stacked_model::get_stacked_model (const std::string & label_) const
  {
    labels_dict_type::const_iterator found = _labels_.find (label_);
    DT_THROW_IF (found == _labels_.end (),
                 std::logic_error,
                 "Dictionary has no model with label '" << label_ << "' !");
    return this->get_stacked_model (found->second);
  }

  const i_model & stacked_model::get_stacked_model (int i_) const
  {
    stacked_dict_type::const_iterator found = _stacked_models_.find (i_);
    DT_THROW_IF (found == _stacked_models_.end (),
                 std::logic_error,
                 "Dictionary has no model with index '" << i_ << "' !");
    return *(found->second.model);
  }

  // ctor:
  stacked_model::stacked_model () : i_boxed_model ()
  {
    _material_name_  = "";
    _stacking_axis_  = STACKING_ALONG_INVALID;
    _mechanics_play_ = DEFAULT_MECHANICS_PLAY;
    _numerics_play_  = DEFAULT_NUMERICS_PLAY;
    return;
  }

  // dtor:
  stacked_model::~stacked_model ()
  {
    return;
  }

  void stacked_model::_at_construct (const std::string & name_,
                                     const datatools::properties & config_,
                                     models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (),  "Entering...");
    // set_name (name_);

    /*** material ***/
    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in stacked model '" << name_ << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");
    set_material_name (material_name);

    /*** Length unit ***/
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit"))
      {
        const std::string length_unit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (length_unit_str);
      }

    /*** Position play ***/
    if (config_.has_key ("mechanics_play"))
      {
        double mechanics_play = config_.fetch_real ("mechanics_play");
        if (! config_.has_explicit_unit ("mechanics_play")) mechanics_play *= lunit;
        DT_THROW_IF (mechanics_play < 0.0,
                     std::logic_error,
                     "Mechanics play '" <<  mechanics_play / CLHEP::mm << "' mm cannot be negative !");
        _mechanics_play_ = mechanics_play;
      }

    if (config_.has_key ("numerics_play"))
      {
        double numerics_play = config_.fetch_real ("numerics_play");
        if (! config_.has_explicit_unit ("numerics_play")) numerics_play *= lunit;
        DT_THROW_IF (numerics_play < 0.0,
                     std::logic_error,
                     "Numerics play '" <<  numerics_play / CLHEP::mm << "' mm cannot be negative in stacked model '" << name_ << "' !");
        _numerics_play_ = numerics_play;
      }

    /*** Stacking axis ***/
    DT_THROW_IF (!config_.has_key ("stacked.axis"), std::logic_error, "Missing 'stacked.axis' property in stacked model '" << name_ << "' !");
    const std::string stacking_axis_label = config_.fetch_string ("stacked.axis");
    int stacking_axis;
    if (stacking_axis_label == "x")
      {
        stacking_axis = STACKING_ALONG_X;
      }
    else if (stacking_axis_label == "y")
      {
        stacking_axis = STACKING_ALONG_Y;
      }
    else if (stacking_axis_label == "z")
      {
        stacking_axis = STACKING_ALONG_Z;
      }
    else
      {
        DT_THROW_IF (true, std::logic_error, "Unknown axis label '" << stacking_axis_label << "' in stacked model '" << name_ << "' !");
      }
    set_stacking_axis (stacking_axis);

    /*** number of stacked stacked models ***/
    DT_THROW_IF (! config_.has_key ("stacked.number_of_items"),
                 std::logic_error,
                 "Missing 'stacked.number_of_items' property in stacked model '" << name_ << "' !");
    const size_t number_of_items = config_.fetch_integer ("stacked.number_of_items");

    /*** check models ***/
    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary in stacked model '" << name_ << "' !");

    /*** loop over models to be stacked ***/
    for (size_t i = 0; i < number_of_items; i++)
      {
        std::ostringstream stacked_item_prop;
        stacked_item_prop << STACKED_MODEL_PROPERTY_PREFIX << i;
        DT_THROW_IF (! config_.has_key (stacked_item_prop.str ()),
                     std::logic_error,
                     "Missing '" << stacked_item_prop.str () << "' property in stacked model '" << name_ << "' !");
        const std::string stacked_model_name = config_.fetch_string (stacked_item_prop.str ());

        // attempt to extract a user defined label:
        std::ostringstream label_item_prop;
        label_item_prop << STACKED_LABEL_PROPERTY_PREFIX  << i;
        std::string label_name;
        if (config_.has_key (label_item_prop.str ()))
          {
            label_name = config_.fetch_string (label_item_prop.str ());
          }

        models_col_type::const_iterator found = models_->find (stacked_model_name);
        DT_THROW_IF (found == models_->end (),
                     std::logic_error,
                     "Cannot find model with name '" << stacked_model_name << "' in stacked model '" << name_ << "' !");
        // check if the model is stackable:
        DT_THROW_IF (! i_shape_3d::is_stackable (found->second->get_logical ().get_shape ()),
                     std::logic_error,
                     "The embedded model '" << found->second->get_name () << "' is not stackable in stacked model '" << name_ << "' !");
        add_stacked_model (i, *(found->second), label_name);
      }

    /*** compute main box dimensions ***/
    mygsl::min_max mmx;
    mygsl::min_max mmy;
    mygsl::min_max mmz;
    double stacked_x = 0.0;
    double stacked_y = 0.0;
    double stacked_z = 0.0;
    for (stacked_dict_type::const_iterator i = _stacked_models_.begin ();
         i != _stacked_models_.end ();
         i++)
      {
        const int index = i->first;
        const stacked_item & bi = i->second;
        const i_model * stacked_model = bi.model;

        const i_shape_3d & the_shape = stacked_model->get_logical ().get_shape ();

        // try to get a stackable data from the shape:
        stackable_data the_SD;
        DT_THROW_IF (! i_shape_3d::pickup_stackable (the_shape, the_SD),
                     std::logic_error,
                     "Cannot stack non-stackable model of shape '" << the_shape.get_shape_name () << "' in stacked model '" << name_ << "' !");

        DT_LOG_TRACE (get_logging_priority (),
                      "Dump stackable data for '" << stacked_model->get_name () <<
                      "' from '" << name_ << "'...");
        if (get_logging_priority () >= datatools::logger::PRIO_TRACE)
          {
            the_SD.tree_dump (std::cerr);
          }

        // Parse special stacking position directives:
        double gmin, gmax;
        datatools::invalidate (gmin);
        datatools::invalidate (gmax);
        {
          std::string stacked_model_name;
          std::string label_name;
          std::ostringstream stacked_min_prop;
          stacked_min_prop << "stacked.limit_min_" << index;
          if (config_.has_key (stacked_min_prop.str ()))
            {
              gmin = config_.fetch_real (stacked_min_prop.str ());
              if (! config_.has_explicit_unit (stacked_min_prop.str ())) gmin *= lunit;
              stacked_item & mod_bi = const_cast <stacked_item &> (bi);
              mod_bi.limit_min = gmin;
            }
        }
        {
          std::string stacked_model_name;
          std::string label_name;
          std::ostringstream stacked_max_prop;
          stacked_max_prop << "stacked.limit_max_" << index;
          if (config_.has_key (stacked_max_prop.str ()))
            {
              gmax = config_.fetch_real (stacked_max_prop.str ());
              if (! config_.has_explicit_unit (stacked_max_prop.str ())) gmax *= lunit;
              stacked_item & mod_bi = const_cast <stacked_item &> (bi);
              mod_bi.limit_max = gmax;
            }
        }

        double gxmin = the_SD.get_xmin ();
        double gymin = the_SD.get_ymin ();
        double gzmin = the_SD.get_zmin ();
        double gxmax = the_SD.get_xmax ();
        double gymax = the_SD.get_ymax ();
        double gzmax = the_SD.get_zmax ();
        if (bi.has_limit_min ())
          {
            if (is_stacking_along_x ())
              {
                gxmin = bi.get_limit_min ();
              }
            else if (is_stacking_along_y ())
              {
                gymin = bi.get_limit_min ();
              }
            else if (is_stacking_along_z ())
              {
                gzmin = bi.get_limit_min ();
              }
          }
        if (bi.has_limit_max ())
          {
            if (is_stacking_along_x ())
              {
                gxmax = bi.get_limit_max ();
              }
            else if (is_stacking_along_y ())
              {
                gymax = bi.get_limit_max ();
              }
            else if (is_stacking_along_z ())
              {
                gzmax = bi.get_limit_max ();
              }
          }

        // Compute the effective dimensions of the stacked item within the stack:
        const double full_x = gxmax - gxmin;
        const double full_y = gymax - gymin;
        const double full_z = gzmax - gzmin;

        // Not sure of that:
        mmx.add (full_x);
        mmy.add (full_y);
        mmz.add (full_z);

        if (is_stacking_along_x ())
          {
            stacked_x += full_x;
          }
        else if (is_stacking_along_y ())
          {
            stacked_y += full_y;
          }
        else if (is_stacking_along_z ())
          {
            stacked_z += full_z;
          }
      }

    if (is_stacking_along_x ())
      {
        stacked_y = mmy.get_max ();
        stacked_z = mmz.get_max ();
      }
    else if (is_stacking_along_y ())
      {
        stacked_x = mmx.get_max ();
        stacked_z = mmz.get_max ();
      }
    else if (is_stacking_along_z ())
      {
        stacked_x = mmx.get_max ();
        stacked_y = mmy.get_max ();
      }

    double dim_x = stacked_x;
    double dim_y = stacked_y;
    double dim_z = stacked_z;

    if (config_.has_key ("x"))
      {
        double x = config_.fetch_real ("x");
        if (! config_.has_explicit_unit ("x")) x *= lunit;
        DT_THROW_IF (x < stacked_x,
                     std::logic_error,
                     "Enforced X dimension '" << x / CLHEP::mm << "' mm (<" << stacked_x / CLHEP::mm << ") " <<
                     "is too small for stacked components to fit in stacked model '" << name_ << "' !");
        dim_x = x;
      }

    if (config_.has_key ("y"))
      {
        double y = config_.fetch_real ("y");
        if (! config_.has_explicit_unit ("y")) y *= lunit;
        DT_THROW_IF (y < stacked_y,
                     std::logic_error,
                     "Enforced Y dimension '" << y / CLHEP::mm << "' mm (<" << stacked_y / CLHEP::mm << ") " <<
                     "is too small for stacked components to fit in stacked model '" << name_ << "' !");
        dim_y = y;
      }

    if (config_.has_key ("z"))
      {
        double z = config_.fetch_real ("z");
        if (! config_.has_explicit_unit ("z")) z *= lunit;
        DT_THROW_IF (z < stacked_z,
                     std::logic_error,
                     "Enforced Z dimension '" << z / CLHEP::mm << "' mm (<" << stacked_z / CLHEP::mm << ") " <<
                     "is too small for stacked components to fit in stacked model '" << name_ << "' !");
        dim_z = z;
      }

    if (_mechanics_play_ > 0.0)
      {
        dim_x += _mechanics_play_;
        dim_y += _mechanics_play_;
        dim_z += _mechanics_play_;
      }
    if (_numerics_play_ > 0.0)
      {
        dim_x += _numerics_play_;
        dim_y += _numerics_play_;
        dim_z += _numerics_play_;
      }
    _solid_.reset ();
    _solid_.set_x (dim_x);
    _solid_.set_y (dim_y);
    _solid_.set_z (dim_z);
    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid solid in stacked model '" << name_ << "' !");

    grab_logical ().set_name (i_model::make_logical_volume_name (name_));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (material_name);

    // starting position:
    double pos = 0.0;
    if (is_stacking_along_x ())
      {
        pos = -0.5 * stacked_x;
      }
    else if (is_stacking_along_y ())
      {
        pos = -0.5 * stacked_y;
      }
    else if (is_stacking_along_z ())
      {
        pos = -0.5 * stacked_z;
      }

    for (stacked_dict_type::iterator i = _stacked_models_.begin ();
         i != _stacked_models_.end ();
         i++)
      {
        stacked_item & bi = i->second;
        const i_model * stacked_model = bi.model;
        double xi, yi, zi;
        xi = yi = zi = 0.0;

        const i_shape_3d & the_shape = stacked_model->get_logical ().get_shape ();
        stackable_data the_SD;
        DT_THROW_IF (! i_shape_3d::pickup_stackable (the_shape, the_SD),
                     std::logic_error,
                     "Cannot stack '"
                     << the_shape.get_shape_name () << "' shape in stacked model '" << name_ << "' !");

        double gxmin = the_SD.get_xmin ();
        double gymin = the_SD.get_ymin ();
        double gzmin = the_SD.get_zmin ();
        double gxmax = the_SD.get_xmax ();
        double gymax = the_SD.get_ymax ();
        double gzmax = the_SD.get_zmax ();

        // Eventually extract specific stacking limits:
        if (bi.has_limit_min ())
          {
            if (is_stacking_along_x ())
              {
                gxmin = bi.get_limit_min ();
              }
            else if (is_stacking_along_y ())
              {
                gymin = bi.get_limit_min ();
              }
            else if (is_stacking_along_z ())
              {
                gzmin = bi.get_limit_min ();
              }
          }
        if (bi.has_limit_max ())
          {
            if (is_stacking_along_x ())
              {
                gxmax = bi.get_limit_max ();
              }
            else if (is_stacking_along_y ())
              {
                gymax = bi.get_limit_max ();
              }
            else if (is_stacking_along_z ())
              {
                gzmax = bi.get_limit_max ();
              }
          }

        if (is_stacking_along_x ())
          {
            xi = pos - gxmin;
            pos += gxmax - gxmin;
          }
        else if (is_stacking_along_y ())
          {
            yi = pos - gymin;
            pos += gymax - gymin;
          }
        else if (is_stacking_along_z ())
          {
            zi = pos - gzmin;
            pos += gzmax - gzmin;
          }
        //double stacked_rotation_angle;
        bi.placmt.set (xi, yi, zi, 0.0, 0.0, 0.0);
        bi.phys.set_name (i_model::make_physical_volume_name (bi.label));
        bi.phys.set_placement (bi.placmt);
        bi.phys.set_logical (bi.model->get_logical ());
        bi.phys.set_mother (_logical);
      } // for

    // 2011-12-05 FM : add support for additional internal objects :
    if (_internals_.get_number_of_items () == 0)
      {
        DT_LOG_TRACE (get_logging_priority (), "Process MWIM");
        _internals_.plug_internal_models (config_,
                                          grab_logical (),
                                          models_);
      }

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void stacked_model::tree_dump (std::ostream & out_,
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
           << "Stacking axis : " << get_stacking_axis () << std::endl;
    }

    {
      for (labels_dict_type::const_iterator i = _labels_.begin ();
           i != _labels_.end ();
           i++)
        {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Stacked model : " << "'" << i->first << "'" << " [rank==" << i->second << "]" << std::endl;
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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(stacked_model, "geomtools::stacked_model");

} // end of namespace geomtools

// end of stacked_model.cc
