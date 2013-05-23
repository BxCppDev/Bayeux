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

  using namespace std;

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
    assert_unconstructed ("geomtools::grid_model::set_model");

    // check if model has a logical volume with a stackable shape:
    const i_shape_3d & shape = model_.get_logical ().get_shape ();
    // if (! i_shape_3d::is_stackable (model_.get_logical ().get_shape ()))
    //   {
    //     ostringstream message;
    //     message << "geomtools::grid_model::set_model: "
    //             << "Model '" << model_.get_name () << "' is not stackable ! "
    //     throw logic_error (message.str ());
    //   }
    _model_ = &model_;
    return;
  }

  const i_model & grid_model::get_model () const
  {
    return *_model_;
  }

  string grid_model::get_model_id () const
  {
    return "geomtools::grid_model";
  }

  grid_model::grid_model () : i_boxed_model ()
  {
    _model_ = 0;
    _x_ = numeric_limits<double>::quiet_NaN ();
    _y_ = numeric_limits<double>::quiet_NaN ();
    _z_ = numeric_limits<double>::quiet_NaN ();
    _number_of_items_[0] = 0;
    _number_of_items_[1] = 0;
    _step_[0] = numeric_limits<double>::quiet_NaN ();
    _step_[1] = numeric_limits<double>::quiet_NaN ();
    return;
  }

  grid_model::~grid_model ()
  {
    return;
  }

  void grid_model::_at_construct (const string & name_,
                                              const datatools::properties & config_,
                                              models_col_type * models_)
  {
    bool devel = false;
    if (devel) clog << "DEVEL: grid_model::_at_construct: Entering..." << endl;
    set_name (name_);
    double x;
    double y;
    double z;
    string material_name; // = material::constants::instance ().MATERIAL_REF_DEFAULT;
    string model_name;
    size_t number_of_items[2];
    number_of_items[0] = number_of_items[1] = 0;
    double step[2];
    string grid_daughter_label;
    string grid_plane_label = "";
    double length_unit = CLHEP::mm;

    /*** material ***/
    DT_THROW_IF (! config_.has_key ("material.ref"), logic_error, "Missing 'material.ref' property !");
    material_name = config_.fetch_string ("material.ref");

    if (config_.has_flag ("grid.force_stackable"))
      {
        datatools::properties stackable_config;
        config_.export_and_rename_starting_with(stackable_config,
                                                "grid.force_stackable.",
                                                stackable::STACKABLE_PREFIX);
        DT_THROW_IF (! _sd_.initialize(stackable_config), logic_error,
                     "Cannot build the stackable data !");
      }

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        length_unit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("grid.daughter_label"))
      {
        grid_daughter_label = config_.fetch_string ("grid.daughter_label");
      }

    DT_THROW_IF (! config_.has_key ("grid.plane"),std::logic_error,
                 "Missing 'grid.plane' property !");
    grid_plane_label = config_.fetch_string ("grid.plane");

    // XXX
    if (grid_plane_label == "xy") {

      // Numbers of steps :
      DT_THROW_IF (! config_.has_key ("grid.x.number_of_items"),logic_error,
                   "Missing 'grid.x.number_of_items' property !");
      number_of_items[0] = config_.fetch_integer ("grid.x.number_of_items");

      DT_THROW_IF (! config_.has_key ("grid.y.number_of_items"),logic_error,
                   "Missing 'grid.y.number_of_items' property !");
      number_of_items[1] = config_.fetch_integer ("grid.y.number_of_items");

      // Steps :
      if (config_.has_key ("grid.x.step")) {
        step[0] = config_.fetch_real ("grid.x.step");
        DT_THROW_IF (step[0] <= 0.0 , logic_error,
                     "Invalid value for 'grid.x.step' property !");
        if (! config_.has_explicit_unit ("grid.x.step")) step[0] *= length_unit;
      }
      else {
        DT_THROW_IF(true, logic_error, "Missing 'grid.x.step' property !");
      }
      if (config_.has_key ("grid.y.step")) {
        step[1] = config_.fetch_real ("grid.y.step");
        DT_THROW_IF (step[1] <= 0.0, logic_error,
                     "Invalid value for 'grid.y.step' property !");
        if (! config_.has_explicit_unit ("grid.y.step")) step[1] *= length_unit;
      }
      else {
        DT_THROW_IF(true, logic_error, "Missing 'grid.z.step' property !");
      }

    }
    else if (grid_plane_label == "xz") {

      // Numbers of steps :
      DT_THROW_IF (!config_.has_key ("grid.x.number_of_items"),logic_error,
                   "Missing 'grid.x.number_of_items' property !");
      number_of_items[0] = config_.fetch_integer ("grid.x.number_of_items");

      DT_THROW_IF (!config_.has_key ("grid.z.number_of_items"),logic_error,
                   "Missing 'grid.z.number_of_items' property !");
      number_of_items[1] = config_.fetch_integer ("grid.z.number_of_items");

      // Steps :
      if (config_.has_key ("grid.x.step"))
        {
          step[0] = config_.fetch_real ("grid.x.step");
          DT_THROW_IF (step[0] <= 0.0 , logic_error, "Invalid value for 'grid.x.step' property !");
          if (! config_.has_explicit_unit ("grid.x.step")) step[0] *= length_unit;
        }
      else {
        DT_THROW_IF(true, logic_error, "Missing 'grid.x.step' property !");
      }

      DT_THROW_IF (! config_.has_key ("grid.z.step"), logic_error, "Missing 'grid.z.step' property !");
      step[1] = config_.fetch_real ("grid.z.step");
      DT_THROW_IF (step[1] <= 0.0 ,logic_error,
                   "Invalid value for 'grid.z.step' property !");
      if (! config_.has_explicit_unit ("grid.z.step")) step[1] *= length_unit;
    }
    else if (grid_plane_label == "yz") {
      // Numbers of steps :
      DT_THROW_IF (!config_.has_key ("grid.y.number_of_items"), logic_error,
                   "Missing 'grid.y.number_of_items' property !");
      number_of_items[0] = config_.fetch_integer ("grid.y.number_of_items");
      DT_THROW_IF (!config_.has_key ("grid.z.number_of_items"), logic_error,
                   "Missing 'grid.z.number_of_items' property !");
      number_of_items[1] = config_.fetch_integer ("grid.z.number_of_items");
      // Steps :
      DT_THROW_IF (!config_.has_key ("grid.y.step"), logic_error, "Missing 'grid.y.step' property !");
      step[0] = config_.fetch_real ("grid.y.step");
      DT_THROW_IF (step[0] <= 0.0 , logic_error, "Invalid value for 'grid.y.step' property !");
      if (! config_.has_explicit_unit ("grid.y.step")) step[0] *= length_unit;
      DT_THROW_IF (! config_.has_key ("grid.z.step"), logic_error,"Missing 'grid.z.step' property !");
      step[1] = config_.fetch_real ("grid.z.step");
      DT_THROW_IF (step[1] <= 0.0 , logic_error, "Invalid value for 'grid.z.step' property !");
      if (! config_.has_explicit_unit ("grid.z.step")) step[1] *= length_unit;
    } else {
      DT_THROW_IF(true,logic_error,"Invalid grid plane label '" << grid_plane_label << "' property !");
    }

    if (config_.has_key ("grid.model"))
      {
        model_name = config_.fetch_string ("grid.model");
      }
    else
      {
        ostringstream message;
        message << "geomtools::grid_model::_at_construct: "
                << "Missing 'grid.model' property !";
        throw logic_error (message.str ());
      }

    _grid_plane_label_ = grid_plane_label;

    if (number_of_items[0] == 0)
      {
        ostringstream message;
        message << "geomtools::grid_model::_at_construct: "
                << "Number of items on first axis is zero !";
        throw logic_error (message.str ());
      }

    if (number_of_items[1] == 0)
      {
        ostringstream message;
        message << "geomtools::grid_model::_at_construct: "
                << "Number of items on second axis is zero !";
        throw logic_error (message.str ());
      }

    _number_of_items_[0] = number_of_items[0];
    _number_of_items_[1] = number_of_items[1];
    _step_[0] = step[0];
    _step_[1] = step[1];

    if (! models_)
      {
        ostringstream message;
        message << "geomtools::grid_model::_at_construct: "
                << "Missing logicals dictionary !";
        throw logic_error (message.str ());
      }

    // Stackable model:
    {
      models_col_type::const_iterator found =
        models_->find (model_name);
      if (found != models_->end ())
        {
          set_model (dynamic_cast<const i_model &>(*(found->second)));
        }
      else
        {
          ostringstream message;
          message << "geomtools::grid_model::_at_construct: "
                  << "Cannot find model with name '"
                  << model_name << "' !";
          throw logic_error (message.str ());
        }
    }

    double dx, dy, dz;
    datatools::invalidate(dx);
    datatools::invalidate(dy);
    datatools::invalidate(dz);
    const i_shape_3d & shape = _model_->get_logical ().get_shape ();
    stackable_data sd;
    i_shape_3d::pickup_stackable (shape, sd);
    dx = sd.get_xmax () - sd.get_xmin ();
    dy = sd.get_ymax () - sd.get_ymin ();
    dz = sd.get_zmax () - sd.get_zmin ();
    if (_sd_.is_valid_weak())
      {
        if (_grid_plane_label_ == "xy")
          {
            if (_sd_.is_valid_x())
              {
                dx = _sd_.get_xmax () - _sd_.get_xmin ();
              }
            if (_sd_.is_valid_y())
              {
                dy = _sd_.get_ymax () - _sd_.get_ymin ();
              }
          }
        if (_grid_plane_label_ == "xz")
          {
            if (_sd_.is_valid_x())
              {
                dx = _sd_.get_xmax () - _sd_.get_xmin ();
              }
            if (_sd_.is_valid_z())
              {
                dz = _sd_.get_zmax () - _sd_.get_zmin ();
              }
          }
        if (_grid_plane_label_ == "yz")
          {
            if (_sd_.is_valid_y())
              {
                dy = _sd_.get_ymax () - _sd_.get_ymin ();
              }
            if (_sd_.is_valid_z())
              {
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
    if (_grid_plane_label_ == "xy")
      {
        _grid_placement_.set_mode (regular_grid_placement::MODE_XY);
        double step_x = dx;
        double step_y = dy;
        if (datatools::is_valid(_step_[0]) && datatools::is_valid(_step_[1]))
          {
            step_x = _step_[0];
            step_y = _step_[1];
          }
        _grid_placement_.set_steps (step_x, step_y);
        _x_ = dx + step_x * (get_number_of_items (0) - 1);
        x0 = -0.5 * _x_ + 0.5 * dx;
        _y_ = dy + step_y * (get_number_of_items (1) - 1);
        y0 = -0.5 * _y_ + 0.5 * dy;
      }
    if (_grid_plane_label_ == "xz")
      {
        _grid_placement_.set_mode (regular_grid_placement::MODE_XZ);
        double step_x = dx;
        double step_z = dz;
        if (datatools::is_valid(_step_[0]) && datatools::is_valid(_step_[1]))
          {
            step_x = _step_[0];
            step_z = _step_[1];
          }
        _grid_placement_.set_steps (step_x, step_z);
        _x_ = dx + step_x * (get_number_of_items (0) - 1);
        x0 = -0.5 * _x_ + 0.5 * dx;
        _z_ = dz + step_z * (get_number_of_items (1) - 1);
        z0 = -0.5 * _z_ + 0.5 * dz;
      }
    if (_grid_plane_label_ == "yz")
      {
        _grid_placement_.set_mode (regular_grid_placement::MODE_YZ);
        double step_y = dy;
        double step_z = dz;
        if (datatools::is_valid(_step_[0]) && datatools::is_valid(_step_[1]))
          {
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

    if (config_.has_key ("x"))
      {
        double x = config_.fetch_real ("x");
        if (! config_.has_explicit_unit ("x")) x *= length_unit;
        if (x < _x_ ) {
          std::ostringstream message;
          message << "geomtools::grid_model::_at_construct: "
                  << "Value for 'x' property is too small (<" << _x_ / CLHEP::mm << " mm) !";
          throw logic_error (message.str ());
        }
        _x_ = x;
      }

    if (config_.has_key ("y"))
      {
        double y = config_.fetch_real ("y");
        if (! config_.has_explicit_unit ("y")) y *= length_unit;
        if (y < _y_ ) {
          std::ostringstream message;
          message << "geomtools::grid_model::_at_construct: "
                  << "Value for 'y' property is too small (<" << _y_ / CLHEP::mm << " mm) !";
          throw logic_error (message.str ());
        }
        _y_ = y;
      }

    if (config_.has_key ("z"))
      {
        double z = config_.fetch_real ("z");
        if (! config_.has_explicit_unit ("z")) z *= length_unit;
        if (z < _z_ ) {
          std::ostringstream message;
          message << "geomtools::grid_model::_at_construct: "
                  << "Value for 'z' property is too small (<" << _z_ / CLHEP::mm << " mm) !";
          throw logic_error (message.str ());
        }
        _z_ = z;
      }

    _solid_.reset ();
    _solid_.set_x (_x_);
    _solid_.set_y (_y_);
    _solid_.set_z (_z_);
    if (! _solid_.is_valid ())
      {
        throw logic_error ("geomtools::grid_model::_at_construct: Invalid solid !");
      }
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_solid_);
    if (material_name.empty() && _model_->get_logical ().has_material_ref ())
      {
        material_name = _model_->get_logical ().get_material_ref ();
      }
    get_logical ().set_material_ref (material_name);
    _grid_daughter_label_ = grid_daughter_label;
    if (_grid_daughter_label_.empty())
      {
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

    if (devel) clog << "DEVEL: grid_model::_at_construct: Exiting." << endl;
    return;
  }

  void grid_model::tree_dump (ostream & out_,
                                          const string & title_ ,
                                          const string & indent_,
                                          bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X : " << get_x () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y : " << get_y () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z : " << get_z () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Grid label      : '" << _grid_plane_label_ << "'" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Grid dimensions : " << _number_of_items_[0] << 'x'  << _number_of_items_[1] << endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Grid placement (box) : " << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _grid_placement_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << endl;
      {
        ostringstream indent_oss;
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
