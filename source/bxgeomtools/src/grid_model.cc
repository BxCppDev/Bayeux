// grid_model.cc

// Ourselves:
#include <geomtools/grid_model.h>

// Standard library:
#include <exception>
#include <limits>

// Third party library:
// - Bayeux/datatools:
#include <datatools/units.h>

// This project:
#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(grid_model,"geomtools::grid_model")

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

  size_t grid_model::get_number_of_items (int i_) const
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

  void grid_model::_pre_construct (datatools::properties & setup_,
                                   models_col_type * models_)
  {
    datatools::logger::priority log_level = get_logging_priority ();
    //log_level = datatools::logger::PRIO_DEBUG;
    this->i_model::_pre_construct(setup_,models_);

    std::vector<std::string> properties_prefixes;
    if (setup_.has_key(i_model::exported_properties_prefixes_key())) {
      setup_.fetch(i_model::exported_properties_prefixes_key(),
                   properties_prefixes);
    }

    {
      const std::string & ii_prefix = model_with_internal_items_tools::INTERNAL_ITEM_PREFIX;
      if (std::find(properties_prefixes.begin(),
                    properties_prefixes.end(),
                    ii_prefix) == properties_prefixes.end()) {
        // Add "internal_item." as exported prefixed properties:
        properties_prefixes.push_back(ii_prefix);
        // Update the vector of exported prefixed properties:
        setup_.update(i_model::exported_properties_prefixes_key(),
                      properties_prefixes);
        DT_LOG_DEBUG(log_level,
                     "Update the vector of exported prefixed properties.");
      }
    }

    return;
  }

  void grid_model::_post_construct (datatools::properties & setup_,
                                          models_col_type * models_)
  {
    DT_LOG_NOTICE(get_logging_priority (),
                  "Post-construction processing for model '" << get_name() << "' ...");
    this->i_model::_post_construct(setup_, models_);

    // Processing possible internal daughter volumes in the mother volume:
    _internals_.plug_internal_models(get_logical().get_parameters(),
                                     grab_logical(),
                                     models_);

    return;
  }

  void grid_model::_at_construct (const datatools::properties & config_,
                                  models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    /*** material ***/
    DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property in grid model '" << get_name() << "' !");
    std::string material_name = config_.fetch_string ("material.ref");

    if (config_.has_flag ("grid.force_stackable")) {
      datatools::properties stackable_config;
      config_.export_and_rename_starting_with(stackable_config,
                                              "grid.force_stackable.",
                                              stackable::STACKABLE_PREFIX);
      DT_THROW_IF (! _sd_.initialize(stackable_config), std::logic_error,
                   "Cannot build the stackable data in grid model '" << get_name() << "' !");
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

    DT_THROW_IF (! config_.has_key ("grid.plane"), std::logic_error, "Missing 'grid.plane' property in grid model '" << get_name() << "' !");
    const std::string grid_plane_label = config_.fetch_string ("grid.plane");

    size_t number_of_items[2];
    number_of_items[0] = number_of_items[1] = 0;
    double step[2];
    // XXX
    if (grid_plane_label == "xy") {
      // Numbers of steps :
      DT_THROW_IF (! config_.has_key ("grid.x.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.x.number_of_items' property in grid model '" << get_name() << "' !");
      number_of_items[0] = config_.fetch_integer ("grid.x.number_of_items");

      DT_THROW_IF (! config_.has_key ("grid.y.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.y.number_of_items' property in grid model '" << get_name() << "' !");
      number_of_items[1] = config_.fetch_integer ("grid.y.number_of_items");

      // Steps :
      DT_THROW_IF (! config_.has_key ("grid.x.step"), std::logic_error, "Missing 'grid.x.step' property in grid model '" << get_name() << "' !");
      step[0] = config_.fetch_real ("grid.x.step");
      DT_THROW_IF (step[0] <= 0.0, std::logic_error, "Invalid value for 'grid.x.step' property !");
      if (! config_.has_explicit_unit ("grid.x.step")) step[0] *= length_unit;

      DT_THROW_IF (! config_.has_key ("grid.y.step"), std::logic_error, "Missing 'grid.y.step' property in grid model '" << get_name() << "' !");
      step[1] = config_.fetch_real ("grid.y.step");
      DT_THROW_IF (step[1] <= 0.0, std::logic_error, "Invalid value for 'grid.y.step' property in grid model '" << get_name() << "' !");
      if (! config_.has_explicit_unit ("grid.y.step")) step[1] *= length_unit;
    } else if (grid_plane_label == "xz") {
      // Numbers of steps :
      DT_THROW_IF (! config_.has_key ("grid.x.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.x.number_of_items' property in grid model '" << get_name() << "' !");
      number_of_items[0] = config_.fetch_integer ("grid.x.number_of_items");

      DT_THROW_IF (! config_.has_key ("grid.z.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.z.number_of_items' property in grid model '" << get_name() << "' !");
      number_of_items[1] = config_.fetch_integer ("grid.z.number_of_items");

      // Steps :
      DT_THROW_IF (! config_.has_key ("grid.x.step"), std::logic_error, "Missing 'grid.x.step' property in grid model '" << get_name() << "' !");
      step[0] = config_.fetch_real ("grid.x.step");
      DT_THROW_IF (step[0] <= 0.0, std::logic_error, "Invalid value for 'grid.x.step' property in grid model '" << get_name() << "' !");
      if (! config_.has_explicit_unit ("grid.x.step")) step[0] *= length_unit;

      DT_THROW_IF (! config_.has_key ("grid.z.step"), std::logic_error, "Missing 'grid.z.step' property in grid model '" << get_name() << "' !");
      step[1] = config_.fetch_real ("grid.z.step");
      DT_THROW_IF (step[1] <= 0.0, std::logic_error, "Invalid value for 'grid.z.step' property in grid model '" << get_name() << "' !");
      if (! config_.has_explicit_unit ("grid.z.step")) step[1] *= length_unit;
    } else if (grid_plane_label == "yz") {
      // Numbers of steps :
      DT_THROW_IF (! config_.has_key ("grid.y.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.y.number_of_items' property in grid model '" << get_name() << "' !");
      number_of_items[0] = config_.fetch_integer ("grid.y.number_of_items");
      DT_THROW_IF (! config_.has_key ("grid.z.number_of_items"),
                   std::logic_error,
                   "Missing 'grid.z.number_of_items' property in grid model '" << get_name() << "' !");
      number_of_items[1] = config_.fetch_integer ("grid.z.number_of_items");
      // Steps :
      DT_THROW_IF (! config_.has_key ("grid.y.step"), std::logic_error, "Missing 'grid.y.step' property in grid model '" << get_name() << "' !");
      step[0] = config_.fetch_real ("grid.y.step");
      DT_THROW_IF (step[0] <= 0.0, std::logic_error, "Invalid value for 'grid.y.step' property in grid model '" << get_name() << "' !");
      if (! config_.has_explicit_unit ("grid.y.step")) step[0] *= length_unit;
      DT_THROW_IF (! config_.has_key ("grid.z.step"), std::logic_error, "Missing 'grid.z.step' property in grid model '" << get_name() << "' !");
      step[1] = config_.fetch_real ("grid.z.step");
      DT_THROW_IF (step[1] <= 0.0 , std::logic_error, "Invalid value for 'grid.z.step' property in grid model '" << get_name() << "' !");
      if (! config_.has_explicit_unit ("grid.z.step")) step[1] *= length_unit;
    } else {
      DT_THROW_IF (true, std::logic_error, "Invalid grid plane label '" << grid_plane_label << "' property in grid model '" << get_name() << "' !");
    }
    _grid_plane_label_ = grid_plane_label;

    DT_THROW_IF (! config_.has_key ("grid.model"), std::logic_error, "Missing 'grid.model' property in grid model '" << get_name() << "' !");
    const std::string model_name = config_.fetch_string ("grid.model");

    DT_THROW_IF (number_of_items[0] == 0, std::logic_error, "Number of items on first axis is zero in grid model '" << get_name() << "' !");
    DT_THROW_IF (number_of_items[1] == 0, std::logic_error, "Number of items on second axis is zero in grid model '" << get_name() << "' !");

    _number_of_items_[0] = number_of_items[0];
    _number_of_items_[1] = number_of_items[1];
    _step_[0] = step[0];
    _step_[1] = step[1];

    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary in grid model '" << get_name() << "' !");
    // Stackable model:
    {
      models_col_type::const_iterator found = models_->find (model_name);
      DT_THROW_IF (found == models_->end (), std::logic_error, "Cannot find model with name '" << model_name << "' in grid model '" << get_name() << "' !");
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
      DT_THROW_IF (x < _x_, std::logic_error, "Value for 'x' property is too small (<" << _x_ / CLHEP::mm << " mm) in grid model '" << get_name() << "' !");
      _x_ = x;
    }

    if (config_.has_key ("y")) {
      double y = config_.fetch_real ("y");
      if (! config_.has_explicit_unit ("y")) y *= length_unit;
      DT_THROW_IF (y < _y_, std::logic_error, "Value for 'y' property is too small (<" << _y_ / CLHEP::mm << " mm) in grid model '" << get_name() << "' !");
      _y_ = y;
    }

    if (config_.has_key ("z")) {
      double z = config_.fetch_real ("z");
      if (! config_.has_explicit_unit ("z")) z *= length_unit;
      DT_THROW_IF (z < _z_, std::logic_error, "Value for 'z' property is too small (<" << _z_ / CLHEP::mm << " mm) in grid model '" << get_name() << "' !");
      _z_ = z;
    }

    _solid_.reset();
    _solid_.set_x(_x_);
    _solid_.set_y(_y_);
    _solid_.set_z(_z_);
    _solid_.lock();
    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid solid in grid model '" << get_name() << "' !");

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
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

} // end of namespace geomtools

// OCD support for class '::geomtools::grid_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::grid_model, ocd_)
{
  ocd_.set_class_name("geomtools::grid_model");
  ocd_.set_class_description("A geometry model implementing a mother box with a daughter volume replicated on a 2D mesh");
  ocd_.set_class_library("geomtools");
  /*
  ocd_.set_class_documentation("  \n"
                               );
  */

  // Inherit the OCD support from the parent class:
  geomtools::i_model::init_ocd(ocd_);

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
    cpd.set_name_pattern("material.ref")
      .set_terse_description("The label of the material the grid mother volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This label of the material the grid mother volume is made of."
                            )
      .add_example("Using 'air'::                                  \n"
                   "                                               \n"
                   "   material.ref : string = \"air\"             \n"
                   "                                               \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.force_stackable")
      .set_terse_description("Flag to force the use of explicit stacking informations while stacking the geometry model")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This property triggers the use of some stacking  \n"
                            "informations for the geometry model being placed \n"
                            "on the grid layout.                              \n"
                            )
      .add_example("Force some stacking informations::                \n"
                   "                                                 \n"
                   "  grid.force_stackable : boolean = 1             \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.plane")
      .set_terse_description("The label of the grid plane")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property set the label of the grid plane.               \n"
                            "Possible values are : ``\"xy\"``, ``\"xz\"`` and ``\"yz\"``. \n"
                            )
      .add_example("Set the plane of the grid::                      \n"
                   "                                                 \n"
                   "   grid.plane : string = \"xy\"                  \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.x.number_of_items")
      .set_terse_description("The number of columns along the X axis")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xy;xz")
      .set_long_description("The number of columns along the X axis.    \n")
      .add_example("A grid in the XY plane with 4 items placed      \n"
                   "along the X axis::                              \n"
                   "                                                \n"
                   "   grid.plane             : string = \"xy\"     \n"
                   "   grid.x.number_of_items : integer = 4         \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.x.step")
      .set_terse_description("The step dimension along the X axis")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xy;xz")
      .set_long_description("The step dimension along the X axis.             \n"
                            )
      .add_example("A grid in the XY plane with 4 items placed every 23.4 cm  \n"
                   "along the X axis::                                        \n"
                   "                                                \n"
                   "   grid.plane             : string = \"xy\"     \n"
                   "   grid.x.number_of_items : integer = 4         \n"
                   "   grid.x.step            : real    = 23.4 cm   \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.y.number_of_items")
      .set_terse_description("The number of columns along the Y axis")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xy;yz")
      .set_long_description("The number of columns along the Y axis.          \n")
      .add_example("A grid in the YZ plane with 7 items placed      \n"
                   "along the Y axis::                              \n"
                   "                                                \n"
                   "   grid.plane : string = \"yz\"                 \n"
                   "   grid.y.number_of_items : integer = 7         \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.y.step")
      .set_terse_description("The step dimension along the Y axis")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xy;yz")
      .set_long_description("The step dimension along the Y axis.             \n")
      .add_example("A grid in the XY plane with 4 items placed every 7.2 mm   \n"
                   "along the Y axis::                              \n"
                   "                                                \n"
                   "   grid.plane             : string = \"xy\"     \n"
                   "   grid.y.number_of_items : integer = 4         \n"
                   "   grid.y.step            : real    = 7.3 mm    \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.z.number_of_items")
      .set_terse_description("The number of columns along the Z axis")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xz;yz")
      .set_long_description("The number of columns along the Z axis.          \n"
                            "Example::                                        \n"
                            "                                                \n"
                            "   grid.plane : string = \"yz\"                 \n"
                            "   grid.z.number_of_items : integer = 3         \n"
                            "                                                \n"
                            )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.z.step")
      .set_terse_description("The step dimension along the Z axis")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_triggered_by_label("grid.plane", "xz;yz")
      .set_long_description("The step dimension along the Z axis.             \n")
      .add_example("A grid in the XZ plane with 2 items placed every 12.4 mm  \n"
                   "along the Z axis::                                        \n"
                   "                                                          \n"
                   "  grid.plane             : string = \"xz\"     \n"
                   "  grid.z.number_of_items : integer = 2         \n"
                   "  grid.z.step            : real    = 12.4 mm   \n"
                   "                                               \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("grid.model")
      .set_terse_description("The name of the geometry model to be placed on the grid layout")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The geometry model is searched for an external   \n"
                            "dictionary of models, typically from a model    \n"
                            "factory.                                         \n"
                            )
      .add_example("Grid replication of model named ``\"block\"``:: \n"
                   "                                                \n"
                   "   grid.model : string = \"block\"              \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x")
      .set_terse_description("The X dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("This property sets X dimension of the box."
                            )
      .add_example("Set the X dimension ::                           \n"
                   "                                                 \n"
                   "   x : real as length = 24.5 cm                  \n"
                   "                                                 \n"
                   "or ::                                            \n"
                   "                                                 \n"
                   "   length_unit : string = \"cm\"                 \n"
                   "   x : real = 24.5                               \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y")
      .set_terse_description("The Y dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("This property sets Y dimension of the box.       \n")
      .add_example("Set the Y dimension ::                          \n"
                   "                                                \n"
                   "   y : real as length = 12.34 mm                 \n"
                   "                                                \n"
                   "or ::                                           \n"
                   "                                                \n"
                   "   length_unit : string = \"cm\"                \n"
                   "   y : real = 12.34                              \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("z")
      .set_terse_description("The Z dimension of the box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("This property sets Z dimension of the box.")
      .add_example("Set the Z dimension ::                          \n"
                   "                                                \n"
                   "   z : real as length = 170.0 um                \n"
                   "                                                \n"
                   "or ::                                           \n"
                   "                                                \n"
                   "   length_unit : string = \"cm\"                \n"
                   "   z : real = 24.5                              \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("material.ref")
      .set_terse_description("The label of the material the volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This label of the material the volume is made of.")
      .add_example("Use 'inox' ::                                   \n"
                   "                                                \n"
                   "    material.ref : string = \"inox\"            \n"
                   "                                                \n"
                   )
      ;
  }

  // Add support for internal/daughter volumes:
  geomtools::MWIM::init_ocd(ocd_);

  ocd_.set_configuration_hints("This model is configured through a configuration file that \n"
                               "uses the format of 'datatools::properties' setup file.     \n"
                               "                                                           \n"
                               "Example ::                                                 \n"
                               "                                                           \n"
                               "  length_unit      : string = \"cm\"                       \n"
                               "  x                : real   =  30.0 mm                     \n"
                               "  y                : real   =  30.0 mm                     \n"
                               "  z                : real   =  1.3                         \n"
                               "  grid.model       : string =  \"brick\"                   \n"
                               "  grid.plane       : string =  \"xy\"                      \n"
                               "  grid.x.number_of_items : integer = 2                     \n"
                               "  grid.x.step            : real    = 12.4 mm               \n"
                               "  grid.y.number_of_items : integer = 4                     \n"
                               "  grid.y.step            : real    = 5.2 mm                \n"
                               "  material.ref     : string = \"air\"                      \n"
                               "                                                           \n"
                               "It is also possible to add stacking informations to change \n"
                               "the default stacking behaviour of the model ::             \n"
                               "                                                           \n"
                               "  grid.force_stackable       : boolean = 1                 \n"
                               "  grid.force_stackable.length_unit : string = \"mm\"       \n"
                               "  grid.force_stackable.xmin : real =  1.3                  \n"
                               "  grid.force_stackable.xmax : real = -3.4                  \n"
                               "  grid.force_stackable.ymin : real =  4.3                  \n"
                               "  grid.force_stackable.ymax : real =  2.4                  \n"
                               "                                                           \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::grid_model,
                               "geomtools::grid_model")
