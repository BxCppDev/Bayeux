/// replicated_circular_model.cc

// Ourselves:
#include <geomtools/replicated_circular_model.h>

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

  void replicated_circular_model::set_model(const i_model & model_)
  {
    DT_THROW_IF(is_constructed(), std::logic_error, "Operation not allowed ! Model has already been constructed!");
    // // check if model has a logical volume with a box shape:
    // const i_shape_3d & shape = model_.get_logical().get_shape();
    // DT_THROW_IF(shape.get_shape_name() != box::box_label(), std::logic_error,
    //              "Model has no 'box' shape ! Found '" << shape.get_shape_name() << "' !");
    _model_ = &model_;
    return;
  }

  const i_model & replicated_circular_model::get_model() const
  {
    DT_THROW_IF(!is_constructed(), std::logic_error,
                "Operation not allowed ! Model is not constructed yet!");
    return *_model_;
  }

  const geomtools::tube & replicated_circular_model::get_solid() const
  {
    return _solid_;
  }

  std::string replicated_circular_model::get_model_id() const
  {
    return "geomtools::replicated_circular_model";
  }

  const MWIM & replicated_circular_model::get_internals() const
  {
    return _internals_;
  }

  replicated_circular_model::replicated_circular_model()
  {
    _model_ = 0;
    return;
  }

  replicated_circular_model::~replicated_circular_model()
  {
    return;
  }

  void replicated_circular_model::_pre_construct (datatools::properties & setup_,
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

  void replicated_circular_model::_post_construct (datatools::properties & setup_,
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

  void replicated_circular_model::_at_construct(const datatools::properties & config_,
                                                models_col_type * models_)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    double default_length_unit = 1.0 * CLHEP::mm;
    double default_angle_unit  = 1.0 * CLHEP::degree;

    if (config_.has_key("length_unit")) {
      const std::string dlu_label = config_.fetch_string("length_unit");
      default_length_unit = datatools::units::get_unit_from("length", dlu_label);
    }

    if (config_.has_key("angle_unit")) {
      const std::string dau_label = config_.fetch_string("angle_unit");
      default_angle_unit = datatools::units::get_unit_from("angle", dau_label);
    }

    DT_THROW_IF(! config_.has_key("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in replicated circular model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string("material.ref");

    std::string replicated_label = "replicated";
    if( config_.has_key("replicated.label")) {
      replicated_label = config_.fetch_string("replicated.label");
    }

    // DT_THROW_IF(! config_.has_key("replicated.rotation_axis"),
    //              std::logic_error,
    //              "Missing 'rotation.axis' property in replicated circular model '" << get_name() << "' !");
    // const std::string rotation_axis_label = config_.fetch_string("replicated.rotation_axis");
    const std::string rotation_axis_label = "z";
    // DT_THROW_IF( rotation_axis_label != "z",
    //              std::logic_error,
    //              "Unsupported '" << rotation_axis_label << "' "
    //           << "rotation axis in replicated circular model '" << get_name() << "' !");

    DT_THROW_IF(! config_.has_key("replicated.model"),
                 std::logic_error,
                 "Missing 'replicated.model' property in replicated circular model '" << get_name() << "' !");
    const std::string model_name = config_.fetch_string("replicated.model");

    DT_THROW_IF(! config_.has_key("replicated.number_of_items"),
                 std::logic_error,
                 "Missing 'replicated.number_of_items' property in replicated boxed model '" << get_name() << "' !");
    const size_t number_of_items = config_.fetch_integer("replicated.number_of_items");
    DT_THROW_IF(number_of_items <= 1, std::logic_error, "Number of items is < 1 in replicated boxed model '" << get_name() << "' !");

    bool axis_ok = false;
    rotation_axis_type replicated_rotation_axis = ROTATION_AXIS_Z;
    if (rotation_axis_label == "x") {
      axis_ok = true;
      replicated_rotation_axis = ROTATION_AXIS_X;
    } else if (rotation_axis_label == "y") {
      axis_ok = true;
      replicated_rotation_axis = ROTATION_AXIS_Y;
    } else if (rotation_axis_label == "z") {
      axis_ok = true;
      replicated_rotation_axis = ROTATION_AXIS_Z;
    }
    DT_THROW_IF(! axis_ok, std::logic_error, "Invalid rotation replicant axis '" << rotation_axis_label<< "' "
                << "in replicated circular model '" << get_name() << "' !");
    DT_THROW_IF( replicated_rotation_axis != ROTATION_AXIS_Z,
                 std::logic_error,
                 "Unsupported '" << rotation_axis_label << "' "
              << "rotation axis in replicated circular model '" << get_name() << "' !");

    DT_THROW_IF(! models_, std::logic_error, "Missing logicals dictionary in replicated circular model '" << get_name() << "' !");
    // Model:
    {
      models_col_type::const_iterator found = models_->find(model_name);
      DT_THROW_IF(found == models_->end(),
                   std::logic_error,
                   "Cannot find model with name '" << model_name << "' in replicated circular model '" << get_name() << "' !");
      set_model(dynamic_cast<const i_model &>(*(found->second)));
    }

    size_t replicated_number_of_items = number_of_items;
    double replicated_radius;
    DT_THROW_IF(! config_.has_key("replicated.radius"),
                std::logic_error,
                "Missing 'replicated.radius' property in replicated circular model '" << get_name() << "' !");
    replicated_radius = config_.fetch_real("replicated.radius");
    if (! config_.has_explicit_unit("replicated.radius")) {
      replicated_radius *= default_length_unit;
    }
    double replicated_step_angle;
    DT_THROW_IF(! config_.has_key("replicated.step_angle"),
                std::logic_error,
                "Missing 'replicated.step_angle' property in replicated circular model '" << get_name() << "' !");
    replicated_step_angle = config_.fetch_real("replicated.step_angle");
    if (! config_.has_explicit_unit("replicated.step_angle")) {
      replicated_step_angle *= default_angle_unit;
    }
    double replicated_start_angle = 0.0;
    if (config_.has_key("replicated.start_angle")) {
      replicated_start_angle = config_.fetch_real("replicated.step_angle");
      if (! config_.has_explicit_unit("replicated.start_angle")) {
        replicated_start_angle *= default_angle_unit;
      }
    }
    bool replicated_follow_rotation = true;
    if (config_.has_key("replicated.follow_rotation")) {
      replicated_follow_rotation = config_.fetch_boolean("replicated.follow_rotation");
    }

    /// The geometry of the mother tube:
    double mother_r_max;
    double mother_r_min = 0.0 * default_length_unit;
    double mother_z;
    DT_THROW_IF(! config_.has_key("r_max"),
                std::logic_error,
                "Missing 'r_max' property in replicated circular model '" << get_name() << "' !");
    mother_r_max = config_.fetch_real("r_max");
    if (! config_.has_explicit_unit("r_max")) {
      mother_r_max *= default_length_unit;
    }
    if (config_.has_key("r_min")) {
      mother_r_min = config_.fetch_real("r_min");
      if (! config_.has_explicit_unit("r_min")) {
        mother_r_min *= default_length_unit;
      }
    }
    DT_THROW_IF(! config_.has_key("z"),
                std::logic_error,
                "Missing 'z' property in replicated circular model '" << get_name() << "' !");
    mother_z = config_.fetch_real("z");
    if (! config_.has_explicit_unit("z")) {
      mother_z *= default_length_unit;
    }

    // Placement of the replica:
    vector_3d replicated_center(0, 0, 0);
    _replica_placement_.set_follow_rotation(replicated_follow_rotation);
    _replica_placement_.initialize(replicated_center,
                                   replicated_radius,
                                   replicated_step_angle,
                                   replicated_start_angle,
                                   replicated_number_of_items,
                                   replicated_rotation_axis);
    // Mother tube:
    _solid_.reset();
    _solid_.set(mother_r_min, mother_r_max, mother_z);
    _solid_.lock();
    DT_THROW_IF(! _solid_.is_valid(), std::logic_error,
                "Invalid mother tube solid in replicated circular model '" << get_name() << "' !");

    grab_logical().set_name(i_model::make_logical_volume_name(get_name()));
    grab_logical().set_shape(_solid_);
    grab_logical().set_material_ref(material_name);

    _phys_.set_name(i_model::make_physical_volume_name(replicated_label, replicated_number_of_items));
    _phys_.set_placement(_replica_placement_);
    _phys_.set_logical(_model_->get_logical());
    _phys_.set_mother(_logical);

    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

  void replicated_circular_model::tree_dump(std::ostream & out_,
                                          const std::string & title_ ,
                                          const std::string & indent_,
                                          bool inherit_) const
  {
    std::string indent;
    if(! indent_.empty()) indent = indent_;
    i_model::tree_dump(out_, title_, indent, true);

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Model : ";
      if (_model_) {
        out_ << "'" << _model_->get_name() << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Mother tube : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _solid_.tree_dump(out_, "", indent_oss.str());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Replicated placement : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _replica_placement_.tree_dump(out_, "", indent_oss.str());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        _solid_.tree_dump(out_, "", indent_oss.str());
      }
    }

    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(replicated_circular_model,"geomtools::replicated_circular_model")

} // end of namespace geomtools

// OCD support for class '::geomtools::replicated_circular_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::replicated_circular_model, ocd_)
{
  ocd_.set_class_name("geomtools::replicated_circular_model");
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
    cpd.set_name_pattern("angle_unit")
      .set_terse_description("The angle_unit unit symbol")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("degree")
      .set_long_description("This property set the symbol of the default angle\n"
                            "unit.                                             \n"
                            )
      .add_example("Using radian::                                   \n"
                   "                                                 \n"
                   "   length_unit : string = \"radian\"             \n"
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
    cpd.set_name_pattern("replicated.model")
      .set_terse_description("The name of the geometry model to be replicated")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The geometry model is searched for an external   \n"
                            "dictionary of models, typically from a model    \n"
                            "factory.                                         \n"
                            )
      .add_example("Circular replication of model named ``\"block\"``:: \n"
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
    cpd.set_name_pattern("replicated.radius")
      .set_terse_description("The radius of the circle for replication")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Set an arbitrary radius::                      \n"
                   "                                               \n"
                   "  replicated.radius : real as length = 20.0 mm \n"
                   "                                               \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("replicated.start_angle")
      .set_terse_description("The start angle of the circular replication")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_default_value_real(0.0)
      .add_example("Start at Pi/4::                                        \n"
                   "                                                       \n"
                   "  replicated.start_angle : real as angle = 45.0 degree \n"
                   "                                                       \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("replicated.step_angle")
      .set_terse_description("The step angle of the circular replication")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Step at Pi/10::                                        \n"
                   "                                                       \n"
                   "  replicated.step_angle : real as angle = 18.0 degree  \n"
                   "                                                       \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("replicated.follow_rotation")
      .set_terse_description("The flag to make replicated volumes following the circular rotation")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(true)
      .add_example("Do not rotate the volumes with the circular rotation:: \n"
                   "                                           \n"
                   "  replicated.follow_rotation : boolean = 0 \n"
                   "                                           \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("r_max")
      .set_terse_description("The outer radius of the mother tube")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .add_example("Set an arbitrary max radius::      \n"
                   "                                   \n"
                   "  r_max : real as length = 30.0 mm \n"
                   "                                   \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("r_min")
      .set_terse_description("The inner radius of the mother tube")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_default_value_real(0.0)
      .add_example("Set an arbitrary min radius::      \n"
                   "                                   \n"
                   "  r_min : real as length = 10.0 mm \n"
                   "                                   \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("z")
      .set_terse_description("The height of the mother tube")
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
                               "  length_unit                : string = \"mm\"             \n"
                               "  angle_unit                 : string = \"degree\"         \n"
                               "  r_min                      : real as length = 10.0 mm    \n"
                               "  r_max                      : real as length = 30.0 mm    \n"
                               "  z                          : real as length = 50.0 mm    \n"
                               "  material.ref               : string = \"air\"            \n"
                               "  replicated.model           : string =  \"detector_section\" \n"
                               "  replicated.number_of_items : integer = 4                 \n"
                               "  replicated.start_angle     : real as angle =  0.0 degree \n"
                               "  replicated.step_angle      : real as angle = 90.0 degree \n"
                               "  replicated.follow_rotation : boolean = 1                 \n"
                               "                                                           \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::replicated_circular_model,
                               "geomtools::replicated_circular_model")
