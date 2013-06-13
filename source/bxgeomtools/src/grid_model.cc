// -*- mode: c++ ; -*-
/* grid_model.cc
 */

#include <geomtools/grid_model.h>

#include <exception>
#include <limits>

#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>
#include <datatools/units.h>

namespace geomtools {

  const geomtools::box & grid_model::get_box () const
  {
    return _solid_;
  }

  const geomtools::box & grid_model::get_solid () const
  {
    return _solid_;
  }

  const std::string & grid_model::get_grid_daughter_label () const
  {
    return _grid_daughter_label_;
  }

  size_t grid_model::get_number_of_items (int i_)
  {
    return _number_of_items_[i_];
  }

  double grid_model::get_step (int i_) const
  {
    return _step_[i_];
  }

  double grid_model::get_x () const
  {
    return _x_;
  }

  double grid_model::get_y () const
  {
    return _y_;
  }

  double grid_model::get_z () const
  {
    return _z_;
  }

  void grid_model::set_model (const i_model & model_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed yet");
    _model_ = &model_;
    return;
  }

  const i_model & grid_model::get_model () const
  {
    return *_model_;
  }

  std::string grid_model::get_model_id () const
  {
    return "geomtools::grid_model";
  }

  grid_model::grid_model () : i_boxed_model ()
  {
    _model_ = 0;
    _x_ = std::numeric_limits<double>::quiet_NaN ();
    _y_ = std::numeric_limits<double>::quiet_NaN ();
    _z_ = std::numeric_limits<double>::quiet_NaN ();
    _number_of_items_[0] = 0;
    _number_of_items_[1] = 0;
    _step_[0] = std::numeric_limits<double>::quiet_NaN ();
    _step_[1] = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  grid_model::~grid_model ()
  {
    return;
  }

  void grid_model::_at_construct (const std::string & name_,
                                  const datatools::properties & config_,
                                  models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");
    //set_name (name_);

    /*** material ***/
    DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property in grid model '" << name_ << "' !");
    std::string material_name = config_.fetch_string ("material.ref");

    if (config_.has_flag ("grid.force_stackable")) {
      datatools::properties stackable_config;
      config_.export_and_rename_starting_with(stackable_config,
                                              "grid.force_stackable.",
                                              stackable::STACKABLE_PREFIX);
      DT_THROW_IF (! _sd_.initialize(stackable_config), std::logic_error,
                   "Cannot build the stackable data in grid model '" << name_ << "' !");
    }

    double length_unit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      length_unit = datatools::units::get_length_unit_from (lunit_str);
    }

    std::string grid_daughter_label;
    if (config_.has_key ("grid.daughter_label")) {
      grid_daughter_label = config_.fetch_string ("grid.daughter_label");
    }

    DT_THROW_IF (! config_.has_key ("grid.plane"), std::logic_error, "Missing 'grid.plane' property in grid model '" << name_ << "' !");
    const std::string grid_plane_label = config_.fetch_string ("grid.plane");

    size_t number_of_items[2];
    number_of_items[0] = number_of_items[1] = 0;
    double step[2];
    // XXX
    if (grid_plane_label == "xy") {
      // Numbers of steps :
      DT_THROW_IF (! config_.has_key ("grid.x.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.x.number_of_items' property in grid model '" << name_ << "' !");
      number_of_items[0] = config_.fetch_integer ("grid.x.number_of_items");

      DT_THROW_IF (! config_.has_key ("grid.y.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.y.number_of_items' property in grid model '" << name_ << "' !");
      number_of_items[1] = config_.fetch_integer ("grid.y.number_of_items");

      // Steps :
      DT_THROW_IF (! config_.has_key ("grid.x.step"), std::logic_error, "Missing 'grid.x.step' property in grid model '" << name_ << "' !");
      step[0] = config_.fetch_real ("grid.x.step");
      DT_THROW_IF (step[0] <= 0.0, std::logic_error, "Invalid value for 'grid.x.step' property !");
      if (! config_.has_explicit_unit ("grid.x.step")) step[0] *= length_unit;

      DT_THROW_IF (! config_.has_key ("grid.y.step"), std::logic_error, "Missing 'grid.y.step' property in grid model '" << name_ << "' !");
      step[1] = config_.fetch_real ("grid.y.step");
      DT_THROW_IF (step[1] <= 0.0, std::logic_error, "Invalid value for 'grid.y.step' property in grid model '" << name_ << "' !");
      if (! config_.has_explicit_unit ("grid.y.step")) step[1] *= length_unit;
    } else if (grid_plane_label == "xz") {
      // Numbers of steps :
      DT_THROW_IF (! config_.has_key ("grid.x.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.x.number_of_items' property in grid model '" << name_ << "' !");
      number_of_items[0] = config_.fetch_integer ("grid.x.number_of_items");

      DT_THROW_IF (! config_.has_key ("grid.z.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.z.number_of_items' property in grid model '" << name_ << "' !");
      number_of_items[1] = config_.fetch_integer ("grid.z.number_of_items");

      // Steps :
      DT_THROW_IF (! config_.has_key ("grid.x.step"), std::logic_error, "Missing 'grid.x.step' property in grid model '" << name_ << "' !");
      step[0] = config_.fetch_real ("grid.x.step");
      DT_THROW_IF (step[0] <= 0.0, std::logic_error, "Invalid value for 'grid.x.step' property in grid model '" << name_ << "' !");
      if (! config_.has_explicit_unit ("grid.x.step")) step[0] *= length_unit;

      DT_THROW_IF (! config_.has_key ("grid.z.step"), std::logic_error, "Missing 'grid.z.step' property in grid model '" << name_ << "' !");
      step[1] = config_.fetch_real ("grid.z.step");
      DT_THROW_IF (step[1] <= 0.0, std::logic_error, "Invalid value for 'grid.z.step' property in grid model '" << name_ << "' !");
      if (! config_.has_explicit_unit ("grid.z.step")) step[1] *= length_unit;
    } else if (grid_plane_label == "yz") {
      // Numbers of steps :
      DT_THROW_IF (! config_.has_key ("grid.y.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.y.number_of_items' property in grid model '" << name_ << "' !");
      number_of_items[0] = config_.fetch_integer ("grid.y.number_of_items");
      DT_THROW_IF (! config_.has_key ("grid.z.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.z.number_of_items' property in grid model '" << name_ << "' !");
      number_of_items[1] = config_.fetch_integer ("grid.z.number_of_items");
      // Steps :
      DT_THROW_IF (! config_.has_key ("grid.y.step"), std::logic_error, "Missing 'grid.y.step' property in grid model '" << name_ << "' !");
      step[0] = config_.fetch_real ("grid.y.step");
      DT_THROW_IF (step[0] <= 0.0, std::logic_error, "Invalid value for 'grid.y.step' property in grid model '" << name_ << "' !");
      if (! config_.has_explicit_unit ("grid.y.step")) step[0] *= length_unit;
      DT_THROW_IF (! config_.has_key ("grid.z.step"), std::logic_error, "Missing 'grid.z.step' property in grid model '" << name_ << "' !");
      step[1] = config_.fetch_real ("grid.z.step");
      DT_THROW_IF (step[1] <= 0.0 , std::logic_error, "Invalid value for 'grid.z.step' property in grid model '" << name_ << "' !");
      if (! config_.has_explicit_unit ("grid.z.step")) step[1] *= length_unit;
    } else {
      DT_THROW_IF (true, std::logic_error, "Invalid grid plane label '" << grid_plane_label << "' property in grid model '" << name_ << "' !");
    }
    _grid_plane_label_ = grid_plane_label;

    DT_THROW_IF (! config_.has_key ("grid.model"), std::logic_error, "Missing 'grid.model' property in grid model '" << name_ << "' !");
    const std::string model_name = config_.fetch_string ("grid.model");

    DT_THROW_IF (number_of_items[0] == 0, std::logic_error, "Number of items on first axis is zero in grid model '" << name_ << "' !");
    DT_THROW_IF (number_of_items[1] == 0, std::logic_error, "Number of items on second axis is zero in grid model '" << name_ << "' !");

    _number_of_items_[0] = number_of_items[0];
    _number_of_items_[1] = number_of_items[1];
    _step_[0] = step[0];
    _step_[1] = step[1];

    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary in grid model '" << name_ << "' !");
    // Stackable model:
    {
      models_col_type::const_iterator found = models_->find (model_name);
      DT_THROW_IF (found == models_->end (), std::logic_error, "Cannot find model with name '" << model_name << "' in grid model '" << name_ << "' !");
      set_model (dynamic_cast<const i_model &>(*(found->second)));
    }

    const i_shape_3d & shape = _model_->get_logical ().get_shape ();
    stackable_data sd;
    i_shape_3d::pickup_stackable (shape, sd);
    double dx = sd.get_xmax () - sd.get_xmin ();
    double dy = sd.get_ymax () - sd.get_ymin ();
    double dz = sd.get_zmax () - sd.get_zmin ();
    if (_sd_.is_valid_weak()) {
      if (_grid_plane_label_ == "xy") {
        if (_sd_.is_valid_x()) {
          dx = _sd_.get_xmax () - _sd_.get_xmin ();
        }
        if (_sd_.is_valid_y()) {
          dy = _sd_.get_ymax () - _sd_.get_ymin ();
        }
      }
      if (_grid_plane_label_ == "xz") {
        if (_sd_.is_valid_x()) {
          dx = _sd_.get_xmax () - _sd_.get_xmin ();
        }
        if (_sd_.is_valid_z()) {
          dz = _sd_.get_zmax () - _sd_.get_zmin ();
        }
      }
      if (_grid_plane_label_ == "yz") {
        if (_sd_.is_valid_y()) {
          dy = _sd_.get_ymax () - _sd_.get_ymin ();
        }
        if (_sd_.is_valid_z()) {
          dz = _sd_.get_zmax () - _sd_.get_zmin ();
        }
      }
    }
    _x_ = dx;
    _y_ = dy;
    _z_ = dz;
    double x0, y0, z0;
    x0 = y0 = z0 = 0.0;
    _grid_placement_.set_centered (false);
    if (_grid_plane_label_ == "xy") {
      _grid_placement_.set_mode (regular_grid_placement::MODE_XY);
      double step_x = dx;
      double step_y = dy;
      if (datatools::is_valid(_step_[0]) && datatools::is_valid(_step_[1])) {
        step_x = _step_[0];
        step_y = _step_[1];
      }
      _grid_placement_.set_steps (step_x, step_y);
      _x_ = dx + step_x * (get_number_of_items (0) - 1);
      x0 = -0.5 * _x_ + 0.5 * dx;
      _y_ = dy + step_y * (get_number_of_items (1) - 1);
      y0 = -0.5 * _y_ + 0.5 * dy;
    }
    if (_grid_plane_label_ == "xz") {
      _grid_placement_.set_mode (regular_grid_placement::MODE_XZ);
      double step_x = dx;
      double step_z = dz;
      if (datatools::is_valid(_step_[0]) && datatools::is_valid(_step_[1])) {
        step_x = _step_[0];
        step_z = _step_[1];
      }
      _grid_placement_.set_steps (step_x, step_z);
      _x_ = dx + step_x * (get_number_of_items (0) - 1);
      x0 = -0.5 * _x_ + 0.5 * dx;
      _z_ = dz + step_z * (get_number_of_items (1) - 1);
      z0 = -0.5 * _z_ + 0.5 * dz;
    }
    if (_grid_plane_label_ == "yz") {
      _grid_placement_.set_mode (regular_grid_placement::MODE_YZ);
      double step_y = dy;
      double step_z = dz;
      if (datatools::is_valid(_step_[0]) && datatools::is_valid(_step_[1])) {
        step_y = _step_[0];
        step_z = _step_[1];
      }
      _grid_placement_.set_steps (step_y, step_z);
      _y_ = dy + step_y * (get_number_of_items (0) - 1);
      y0 = -0.5 * _y_ + 0.5 * dy;
      _z_ = dz + step_z * (get_number_of_items (1) - 1);
      z0 = -0.5 * _z_ + 0.5 * dz;
    }
    _grid_placement_.set_number_of_columns (get_number_of_items (0));
    _grid_placement_.set_number_of_rows (get_number_of_items (1));

    if (config_.has_key ("x")) {
      double x = config_.fetch_real ("x");
      if (! config_.has_explicit_unit ("x")) x *= length_unit;
      DT_THROW_IF (x < _x_, std::logic_error, "Value for 'x' property is too small (<" << _x_ / CLHEP::mm << " mm) in grid model '" << name_ << "' !");
      _x_ = x;
    }

    if (config_.has_key ("y")) {
      double y = config_.fetch_real ("y");
      if (! config_.has_explicit_unit ("y")) y *= length_unit;
      DT_THROW_IF (y < _y_, std::logic_error, "Value for 'y' property is too small (<" << _y_ / CLHEP::mm << " mm) in grid model '" << name_ << "' !");
      _y_ = y;
    }

    if (config_.has_key ("z")) {
      double z = config_.fetch_real ("z");
      if (! config_.has_explicit_unit ("z")) z *= length_unit;
      DT_THROW_IF (z < _z_, std::logic_error, "Value for 'z' property is too small (<" << _z_ / CLHEP::mm << " mm) in grid model '" << name_ << "' !");
      _z_ = z;
    }

    _solid_.reset ();
    _solid_.set_x (_x_);
    _solid_.set_y (_y_);
    _solid_.set_z (_z_);
    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid solid in grid model '" << name_ << "' !");

    grab_logical ().set_name (i_model::make_logical_volume_name (name_));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (material_name);
    _grid_daughter_label_ = grid_daughter_label;
    if (_grid_daughter_label_.empty()) {
      _grid_daughter_label_ = "grid";
    }

    placement basic_p;
    basic_p.set (x0, y0, z0, 0, 0, 0);
    _grid_placement_.set_basic_placement (basic_p);
    _phys_.set_name (i_model::make_physical_volume_name (_grid_daughter_label_,
                                                         get_number_of_items (0),
                                                         get_number_of_items (1)));
    _phys_.set_placement (_grid_placement_);
    _phys_.set_logical (_model_->get_logical ());
    _phys_.set_mother (_logical);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void grid_model::tree_dump (std::ostream & out_,
                              const std::string & title_ ,
                              const std::string & indent_,
                              bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X : " << get_x () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y : " << get_y () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z : " << get_z () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Grid label      : '" << _grid_plane_label_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Grid dimensions : " << _number_of_items_[0] << 'x'  << _number_of_items_[1] << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Grid placement (box) : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _grid_placement_.tree_dump (out_, "", indent_oss.str ());
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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(grid_model,"geomtools::grid_model");

} // end of namespace geomtools

// end of grid_model.cc
