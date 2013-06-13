// -*- mode: c++ ; -*-
/* replicated_model.cc
 */

#include <geomtools/replicated_model.h>

#include <exception>
#include <limits>

#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>
#include <datatools/units.h>

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

  size_t replicated_model::get_number_of_items ()
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

  void replicated_model::_at_construct (const std::string & name_,
                                        const datatools::properties & config_,
                                        models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");
    //set_name (name_);

    /*** material ***/
    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in replicated model '" << name_ << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");

    if (config_.has_flag ("replicated.force_stackable"))
      {
        datatools::properties stackable_config;
        config_.export_and_rename_starting_with (stackable_config,
                                                 "replicated.force_stackable.",
                                                 stackable::STACKABLE_PREFIX);
        DT_THROW_IF (! _sd_.initialize (stackable_config),
                     std::logic_error,
                     "Cannot build the stackable data in replicated model '" << name_ << "' !");
      }

    std::string replicated_label = "replicated";
    if (config_.has_key ("replicated.label"))
      {
        replicated_label = config_.fetch_string ("replicated.label");
      }

    DT_THROW_IF (! config_.has_key ("replicated.axis"), std::logic_error, "Missing 'replicated.axis' property in replicated model '" << name_ << "' !");
    const std::string replicant_axis_label = config_.fetch_string ("replicated.axis");

    DT_THROW_IF (! config_.has_key ("replicated.number_of_items"), std::logic_error, "Missing 'replicated.number_of_items' property in replicated model '" << name_ << "' !");
    const size_t number_of_items = config_.fetch_integer ("replicated.number_of_items");

    DT_THROW_IF (! config_.has_key ("replicated.model"), std::logic_error, "Missing 'replicated.model' property in replicated model '" << name_ << "' !");
    const std::string model_name = config_.fetch_string ("replicated.model");

    double length_unit = CLHEP::mm;
    if (config_.has_key ("length_unit"))
      {
        const std::string lunit_str = config_.fetch_string ("length_unit");
        length_unit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("replicated.step"))
      {
        _step_ = config_.fetch_real ("replicated.step");
        DT_THROW_IF (_step_ <= 0.0, std::logic_error, "Invalid value for 'replicated.step' property in replicated model '" << name_ << "' !");
        if (! config_.has_explicit_unit ("replicated.step")) {
          _step_ *= length_unit;
        }
      }

    DT_THROW_IF (number_of_items == 0, std::logic_error, "Number of items is zero in replicated model '" << name_ << "' !");
    set_number_of_items (number_of_items);

    bool axis_ok = false;
    if (replicant_axis_label == "x") axis_ok = true;
    else if (replicant_axis_label == "y") axis_ok = true;
    else if (replicant_axis_label == "z") axis_ok = true;
    DT_THROW_IF (! axis_ok, std::logic_error, "Invalid replicant axis in replicated model '" << name_ << "' !");

    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary in replicated model '" << name_ << "' !");

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
    if (_sd_.is_valid_weak())
      {
        if (replicant_axis_label == "x" && _sd_.is_valid_x())
          {
            dx = _sd_.get_xmax () - _sd_.get_xmin ();
          }
        if (replicant_axis_label == "y" && _sd_.is_valid_y())
          {
            dy = _sd_.get_ymax () - _sd_.get_ymin ();
          }
        if (replicant_axis_label == "z" && _sd_.is_valid_z())
          {
            dz = _sd_.get_zmax () - _sd_.get_zmin ();
          }
      }
    _x_ = dx;
    _y_ = dy;
    _z_ = dz;
    double x0, y0, z0;
    x0 = y0 = z0 = 0.0;
    if (replicant_axis_label == "x")
      {
        const double step_x = datatools::is_valid (_step_) ? _step_ : dx;
        _replica_placement_.set_replicant_step_x (step_x);
        _x_ = dx + step_x * (get_number_of_items () - 1);
        x0 = -0.5 * _x_ + 0.5 * dx;
      }
    if (replicant_axis_label == "y")
      {
        const double step_y = datatools::is_valid (_step_) ? _step_ : dy;
        _replica_placement_.set_replicant_step_y (step_y);
        _y_ =  dy + step_y * (get_number_of_items () - 1);
        y0 = -0.5 * _y_ + 0.5 * dy;
      }
    if (replicant_axis_label == "z")
      {
        const double step_z = datatools::is_valid (_step_) ? _step_ : dz;
        _replica_placement_.set_replicant_step_z (step_z);
        _z_ =  dz + step_z * (get_number_of_items () - 1);
        z0 = -0.5 * _z_ + 0.5 * dz;
      }
    _replica_placement_.set_number_of_items (get_number_of_items ());

    _solid_.reset ();
    _solid_.set_x (_x_);
    _solid_.set_y (_y_);
    _solid_.set_z (_z_);
    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid solid in replicated model '" << name_ << "' !");

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_solid_);
    // 2013-06-13 FM : we cannot use the boxed model material here
    // There is no garantee it is the proper one to be used for the envelope solid.
    // std::string material_name = material::constants::instance ().MATERIAL_REF_DEFAULT;
    // if (_boxed_model_->get_logical ().has_material_ref ())
    //   {
    //     material_name = _boxed_model_->get_logical ().get_material_ref ();
    //   }
    get_logical ().set_material_ref (material_name);
    // DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
    //              "******** Replicated model '" << get_name() << "' has material '"
    //              << material_name << "'.");

    placement basic_p;
    basic_p.set (x0, y0, z0, 0, 0, 0);
    _replica_placement_.set_basic_placement (basic_p);
    _phys_.set_name (i_model::make_physical_volume_name (replicated_label,
                                                         get_number_of_items ()));
    _phys_.set_placement (_replica_placement_);
    _phys_.set_logical (_model_->get_logical ());
    _phys_.set_mother (_logical);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(replicated_model,"geomtools::replicated_model");

} // end of namespace geomtools

// end of replicated_model.cc
