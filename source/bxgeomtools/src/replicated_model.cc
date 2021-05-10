// replicated_model.cc

// Ourselves:
#include <geomtools/replicated_model.h>

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

  const geomtools::box & replicated_model::get_box () const
  {
    return _solid_;
  }

  const geomtools::box & replicated_model::get_solid () const
  {
    return _solid_;
  }

  void replicated_model::set_number_of_items (size_t n_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _number_of_items_ = n_;
    return;
  }

  size_t replicated_model::get_number_of_items () const
  {
    return _number_of_items_;
  }

  double replicated_model::get_step () const
  {
    return _step_;
  }

  double replicated_model::get_x () const
  {
    return _x_;
  }

  double replicated_model::get_y () const
  {
    return _y_;
  }

  double replicated_model::get_z () const
  {
    return _z_;
  }

  void replicated_model::set_model (const i_model & model_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _model_ = &model_;
    return;
  }

  const i_model & replicated_model::get_model () const
  {
    return *_model_;
  }

  std::string replicated_model::get_model_id () const
  {
    return "geomtools::replicated_model";
  }

  replicated_model::replicated_model () : i_boxed_model ()
  {
    _model_ = 0;
    _x_ = std::numeric_limits<double>::quiet_NaN ();
    _y_ = std::numeric_limits<double>::quiet_NaN ();
    _z_ = std::numeric_limits<double>::quiet_NaN ();
    _number_of_items_ = 0;
    _step_ = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  replicated_model::~replicated_model ()
  {
    return;
  }

  void replicated_model::_pre_construct (datatools::properties & setup_,
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

  void replicated_model::_post_construct (datatools::properties & setup_,
                                          models_col_type * models_)
  {
    //datatools::logger::priority log_level = get_logging_priority ();
    //log_level = datatools::logger::PRIO_DEBUG;
    DT_LOG_NOTICE(get_logging_priority (),
                  "Post-construction processing for model '" << get_name() << "' ...");
    this->i_model::_post_construct(setup_, models_);

    // Processing possible internal daughter volumes in the mother volume:
    _internals_.plug_internal_models(get_logical().get_parameters(),
                                     grab_logical(),
                                     models_);

    return;
  }

  void replicated_model::_at_construct (const datatools::properties & config_,
                                        models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    /*** material ***/
    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in replicated model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");

    if (config_.has_flag ("replicated.force_stackable"))
      {
        datatools::properties stackable_config;
        config_.export_and_rename_starting_with (stackable_config,
                                                 "replicated.force_stackable.",
                                                 stackable::STACKABLE_PREFIX);
        DT_THROW_IF (! _sd_.initialize (stackable_config),
                     std::logic_error,
                     "Cannot build the stackable data in replicated model '" << get_name() << "' !");
      }

    std::string replicated_label = "replicated";
    if (config_.has_key ("replicated.label"))
      {
        replicated_label = config_.fetch_string ("replicated.label");
      }

    DT_THROW_IF (! config_.has_key ("replicated.axis"), std::logic_error, "Missing 'replicated.axis' property in replicated model '" << get_name() << "' !");
    const std::string replicant_axis_label = config_.fetch_string ("replicated.axis");

    DT_THROW_IF (! config_.has_key ("replicated.number_of_items"), std::logic_error, "Missing 'replicated.number_of_items' property in replicated model '" << get_name() << "' !");
    const size_t number_of_items = config_.fetch_integer ("replicated.number_of_items");

    DT_THROW_IF (! config_.has_key ("replicated.model"), std::logic_error, "Missing 'replicated.model' property in replicated model '" << get_name() << "' !");
    const std::string model_name = config_.fetch_string ("replicated.model");

    double default_length_unit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      default_length_unit = datatools::units::get_length_unit_from (lunit_str);
    }

    double x_box;
    double y_box;
    double z_box;
    datatools::invalidate(x_box);
    datatools::invalidate(y_box);
    datatools::invalidate(z_box);

    if (config_.has_key("x")) {
      x_box = config_.fetch_real("x");
      if (! config_.has_explicit_unit("x")) {
        x_box *= default_length_unit;
      }
    }

    if (config_.has_key("y")) {
      y_box = config_.fetch_real("y");
      if (! config_.has_explicit_unit("y")) {
        y_box *= default_length_unit;
      }
    }

    if (config_.has_key("z")) {
      z_box = config_.fetch_real("z");
      if (! config_.has_explicit_unit("z")) {
        z_box *= default_length_unit;
      }
    }

    if (config_.has_key ("replicated.step")) {
      _step_ = config_.fetch_real ("replicated.step");
      DT_THROW_IF (_step_ <= 0.0, std::logic_error, "Invalid value for 'replicated.step' property in replicated model '" << get_name() << "' !");
      if (! config_.has_explicit_unit ("replicated.step")) {
        _step_ *= default_length_unit;
      }
    }

    DT_THROW_IF (number_of_items == 0, std::logic_error, "Number of items is zero in replicated model '" << get_name() << "' !");
    set_number_of_items (number_of_items);

    bool axis_ok = false;
    if (replicant_axis_label == "x") axis_ok = true;
    else if (replicant_axis_label == "y") axis_ok = true;
    else if (replicant_axis_label == "z") axis_ok = true;
    DT_THROW_IF (! axis_ok, std::logic_error, "Invalid replicant axis in replicated model '" << get_name() << "' !");

    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary in replicated model '" << get_name() << "' !");

    // Stackable model:
    {
      models_col_type::const_iterator found =
        models_->find (model_name);
      DT_THROW_IF (found == models_->end (),
                   std::logic_error,
                   "Cannot find model with name '" << model_name << "' !");
      set_model (dynamic_cast<const i_model &>(*(found->second)));
    }

    const i_shape_3d & shape = _model_->get_logical ().get_shape ();
    stackable_data sd;
    i_shape_3d::pickup_stackable (shape, sd);
    double dx = sd.get_xmax () - sd.get_xmin ();
    double dy = sd.get_ymax () - sd.get_ymin ();
    double dz = sd.get_zmax () - sd.get_zmin ();
    if (_sd_.is_valid_weak()) {
      if (replicant_axis_label == "x" && _sd_.is_valid_x()) {
        dx = _sd_.get_xmax () - _sd_.get_xmin ();
      }
      if (replicant_axis_label == "y" && _sd_.is_valid_y()) {
        dy = _sd_.get_ymax () - _sd_.get_ymin ();
      }
      if (replicant_axis_label == "z" && _sd_.is_valid_z()) {
        dz = _sd_.get_zmax () - _sd_.get_zmin ();
      }
    }
    _x_ = dx;
    _y_ = dy;
    _z_ = dz;
    double x0, y0, z0;
    x0 = y0 = z0 = 0.0;
    if (replicant_axis_label == "x") {
      const double step_x = datatools::is_valid (_step_) ? _step_ : dx;
      _replica_placement_.set_replicant_step_x (step_x);
      _x_ = dx + step_x * (get_number_of_items () - 1);
      x0 = -0.5 * _x_ + 0.5 * dx;
    }
    if (replicant_axis_label == "y") {
      const double step_y = datatools::is_valid (_step_) ? _step_ : dy;
      _replica_placement_.set_replicant_step_y (step_y);
      _y_ =  dy + step_y * (get_number_of_items () - 1);
      y0 = -0.5 * _y_ + 0.5 * dy;
    }
    if (replicant_axis_label == "z") {
      const double step_z = datatools::is_valid (_step_) ? _step_ : dz;
      _replica_placement_.set_replicant_step_z (step_z);
      _z_ =  dz + step_z * (get_number_of_items () - 1);
      z0 = -0.5 * _z_ + 0.5 * dz;
    }
    _replica_placement_.set_number_of_items (get_number_of_items ());

    if (datatools::is_valid(x_box)) {
      DT_THROW_IF(x_box < _x_, std::logic_error, "Invalid request on X dimension of the mother box!");
      _x_ = x_box;
    }
    if (datatools::is_valid(y_box)) {
      DT_THROW_IF(y_box < _y_, std::logic_error, "Invalid request on Y dimension of the mother box!");
      _y_ = y_box;
    }
    if (datatools::is_valid(z_box)) {
      DT_THROW_IF(z_box < _z_, std::logic_error, "Invalid request on Z dimension of the mother box!");
      _z_ = z_box;
    }

    _solid_.reset();
    _solid_.set_x(_x_);
    _solid_.set_y(_y_);
    _solid_.set_z(_z_);
    _solid_.lock();
    DT_THROW_IF (! _solid_.is_valid(), std::logic_error, "Invalid solid in replicated model '" << get_name() << "' !");

    grab_logical().set_name(i_model::make_logical_volume_name(get_name()));
    grab_logical().set_shape(_solid_);
    grab_logical().set_material_ref(material_name);

    placement basic_p;
    basic_p.set(x0, y0, z0, 0, 0, 0);
    _replica_placement_.set_basic_placement(basic_p);
    _phys_.set_name(i_model::make_physical_volume_name(replicated_label,
                                                         get_number_of_items()));
    _phys_.set_placement(_replica_placement_);
    _phys_.set_logical(_model_->get_logical ());
    _phys_.set_mother(_logical);

    DT_LOG_TRACE (get_logging_priority(), "Exiting.");
    return;
  }

  void replicated_model::tree_dump (std::ostream & out_,
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
         << "Number of replicated items : " << _number_of_items_ << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Replicated placement : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _replica_placement_.tree_dump (out_, "", indent_oss.str ());
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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(replicated_model,"geomtools::replicated_model")

} // end of namespace geomtools

// OCD support for class '::geomtools::replicated_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::replicated_model, ocd_)
{
  ocd_.set_class_name("geomtools::replicated_model");
  ocd_.set_class_description("A geometry model implementing a mother tube with some daughter volumes regularily placed on a circle");
  ocd_.set_class_library("geomtools");

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
      .set_terse_description("The label of the material the mother volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      // .set_long_description("This label of the material the mother volume is made of."
      //                       )
      .add_example("Using 'air'::                                  \n"
                   "                                               \n"
                   "   material.ref : string = \"air\"             \n"
                   "                                               \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("replicated.label")
      .set_terse_description("The label used to name the daughter volumes")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("replicated")
      .add_example("Using an alternative label::                      \n"
                   "                                                  \n"
                   "   replicated.label : string = \"go_round_items\" \n"
                   "                                                  \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("replicated.axis")
      .set_terse_description("The label of the replication axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The value is taken from:          \n"
                            "                                  \n"
                            " * ``x`` : the X axis             \n"
                            " * ``y`` : the Y axis             \n"
                            " * ``z`` : the Z axis             \n"
                            "                                  \n"
                            )
      .add_example("Replication along the Y axis::                  \n"
                   "                                                \n"
                   "   replicated.axis : string = \"y\"             \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("replicated.model")
      .set_terse_description("The name of the geometry model to be replicated")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The geometry model is searched for an external  \n"
                            "dictionary of models, typically from a model    \n"
                            "factory.                                        \n"
                            )
      .add_example("Replication of model named ``\"block\"``:: \n"
                   "                                                    \n"
                   "   replicated.model : string = \"block\"            \n"
                   "                                                    \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("replicated.number_of_items")
      .set_terse_description("The number of replicated volumes")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .add_example("Replicate 3 objects::                           \n"
                   "                                                \n"
                   "   replicated.number_of_items : integer = 3     \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("replicated.step")
      .set_terse_description("The step length along the replication axis")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("If not given explicitely, the step length      \n"
                            "is computed from the dimension of the daughter \n"
                            "volumes which must have *stackable* support.   \n"
                            )
      .add_example("Set an arbitrary step: ::                       \n"
                   "                                                \n"
                   "  replicated.step : real as length = 20.0 mm    \n"
                   "                                                \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x")
      .set_terse_description("The X dimension of the mother box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Set an arbitrary length::          \n"
                   "                                   \n"
                   "  x : real as length = 30.0 mm     \n"
                   "                                   \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y")
      .set_terse_description("The Y dimension of the mother box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Set an arbitrary width::           \n"
                   "                                   \n"
                   "  y : real as length = 30.0 mm     \n"
                   "                                   \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("z")
      .set_terse_description("The Z dimension of the mother box")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Set an arbitrary height::          \n"
                   "                                   \n"
                   "  z : real as length = 30.0 mm     \n"
                   "                                   \n"
                   )
      ;
  }

  // Add support for internal/daughter volumes:
  geomtools::MWIM::init_ocd(ocd_);

  ocd_.set_configuration_hints("This model is configured through a configuration file that \n"
                               "uses the format of 'datatools::properties' setup file.     \n"
                               "                                                           \n"
                               "Example: ::                                                \n"
                               "                                                           \n"
                               "  length_unit                : string  = \"mm\"            \n"
                               "  x                          : real as length = 10.0 mm    \n"
                               "  y                          : real as length = 30.0 mm    \n"
                               "  z                          : real as length = 50.0 mm    \n"
                               "  material.ref               : string  = \"air\"           \n"
                               "  replicated.axis            : string  = \"x\"             \n"
                               "  replicated.model           : string  = \"detector_unit\" \n"
                               "  replicated.number_of_items : integer = 4                 \n"
                               "  replicated.step            : real as length = 10.0 cm    \n"
                               "                                                           \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::replicated_model,
                               "geomtools::replicated_model")
