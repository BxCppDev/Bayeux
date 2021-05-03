/// replicated_boxed_model.cc

// Ourselves:
#include <geomtools/replicated_boxed_model.h>

// Standard library:
#include <exception>
#include <limits>

// This project:
#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>

namespace geomtools {

  const geomtools::box & replicated_boxed_model::get_box () const
  {
    return _solid_;
  }

  const geomtools::box & replicated_boxed_model::get_solid () const
  {
    return _solid_;
  }

  void replicated_boxed_model::set_number_of_items (size_t n_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");
    _number_of_items_ = n_;
    return;
  }

  size_t replicated_boxed_model::get_number_of_items ()
  {
    return _number_of_items_;
  }

  double replicated_boxed_model::get_x () const
  {
    return _x_;
  }

  double replicated_boxed_model::get_y () const
  {
    return _y_;
  }

  double replicated_boxed_model::get_z () const
  {
    return _z_;
  }

  void replicated_boxed_model::set_boxed_model (const i_model & model_)
  {
    DT_THROW_IF (is_constructed (), std::logic_error, "Operation not allowed ! Model has already been constructed");

    // check if model has a logical volume with a box shape:
    const i_shape_3d & shape = model_.get_logical ().get_shape ();
    DT_THROW_IF (shape.get_shape_name () != box::box_label(), std::logic_error,
                 "Model has no 'box' shape ! Found '" << shape.get_shape_name () << "' !");
    _boxed_model_ = &model_;
    return;
  }

  const i_model & replicated_boxed_model::get_boxed_model () const
  {
    return *_boxed_model_;
  }

  std::string replicated_boxed_model::get_model_id () const
  {
    return "geomtools::replicated_boxed_model";
  }

  replicated_boxed_model::replicated_boxed_model () : i_boxed_model ()
  {
    _boxed_model_ = 0;
    _x_ = std::numeric_limits<double>::quiet_NaN ();
    _y_ = std::numeric_limits<double>::quiet_NaN ();
    _z_ = std::numeric_limits<double>::quiet_NaN ();
    _number_of_items_ = 0;
    return;
  }

  replicated_boxed_model::~replicated_boxed_model ()
  {
    return;
  }

  void replicated_boxed_model::_at_construct (const datatools::properties & config_,
                                              models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    DT_THROW_IF (! config_.has_key ("material.ref"),
                 std::logic_error,
                 "Missing 'material.ref' property in replicated boxed model '" << get_name() << "' !");
    const std::string material_name = config_.fetch_string ("material.ref");

    std::string replicated_label = "replicated";
    if (config_.has_key ("replicated.label"))
      {
        replicated_label = config_.fetch_string ("replicated.label");
      }

    DT_THROW_IF (! config_.has_key ("replicated.axis"),
                 std::logic_error,
                 "Missing 'replicated.axis' property in replicated boxed model '" << get_name() << "' !");
    const std::string replicant_axis_label = config_.fetch_string ("replicated.axis");

    DT_THROW_IF (! config_.has_key ("replicated.model"),
                 std::logic_error,
                 "Missing 'replicated.model' property in replicated boxed model '" << get_name() << "' !");
    const std::string boxed_model_name = config_.fetch_string ("replicated.model");

    DT_THROW_IF (! config_.has_key ("replicated.number_of_items"),
                 std::logic_error,
                 "Missing 'replicated.number_of_items' property in replicated boxed model '" << get_name() << "' !");
    const size_t number_of_items = config_.fetch_integer ("replicated.number_of_items");
    DT_THROW_IF (number_of_items == 0, std::logic_error, "Number of items is zero in replicated boxed model '" << get_name() << "' !");
    set_number_of_items (number_of_items);

    bool axis_ok = false;
    if (replicant_axis_label == "x") axis_ok = true;
    else if (replicant_axis_label == "y") axis_ok = true;
    else if (replicant_axis_label == "z") axis_ok = true;
    DT_THROW_IF (! axis_ok, std::logic_error, "Invalid replicant axis in replicated boxed model '" << get_name() << "' !");

    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary in replicated boxed model '" << get_name() << "' !");
    // Boxed model:
    {
      models_col_type::const_iterator found = models_->find (boxed_model_name);
      DT_THROW_IF (found == models_->end (),
                   std::logic_error,
                   "Cannot find model with name '" << boxed_model_name << "' in replicated boxed model '" << get_name() << "' !");
      set_boxed_model (dynamic_cast<const i_model &>(*(found->second)));
    }

    const box & b =
      dynamic_cast<const box &>(_boxed_model_->get_logical ().get_shape ());
    _x_ = b.get_x ();
    _y_ = b.get_y ();
    _z_ = b.get_z ();
    double x0, y0, z0;
    x0 = y0 = z0 = 0.0;
    if (replicant_axis_label == "x")
      {
        _boxed_replica_placement_.set_replicant_step_x (b.get_x ());
        _x_ = b.get_x ()* get_number_of_items ();
        x0 = -0.5 * _x_ + 0.5 * b.get_x ();
      }
    if (replicant_axis_label == "y")
      {
        _boxed_replica_placement_.set_replicant_step_y (b.get_y ());
        _y_ = b.get_y ()* get_number_of_items ();
        y0 = -0.5 * _y_ + 0.5 * b.get_y ();
      }
    if (replicant_axis_label == "z")
      {
        _boxed_replica_placement_.set_replicant_step_z (b.get_z ());
        _z_ = b.get_z () * get_number_of_items ();
        z0 = -0.5 * _z_ + 0.5 * b.get_z ();
      }
    _boxed_replica_placement_.set_number_of_items (get_number_of_items ());

    _solid_.reset();
    _solid_.set_x(_x_);
    _solid_.set_y(_y_);
    _solid_.set_z(_z_);
    _solid_.lock();
    DT_THROW_IF (! _solid_.is_valid (), std::logic_error, "Invalid solid in replicated boxed model '" << get_name() << "' !");

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_solid_);
    // 2013-06-13 FM : we cannot use the boxed model material here
    // There is no garantee it is the proper one to be used for the envelope solid.
    // std::string material_name = material::constants::instance ().MATERIAL_REF_DEFAULT;
    // if (_boxed_model_->get_logical ().has_material_ref ())
    //   {
    //     material_name = _boxed_model_->get_logical ().get_material_ref ();
    //   }
    grab_logical ().set_material_ref (material_name);

    placement basic_p;
    basic_p.set (x0, y0, z0, 0, 0, 0);
    _boxed_replica_placement_.set_basic_placement (basic_p);
    _boxed_phys_.set_name (i_model::make_physical_volume_name (replicated_label, get_number_of_items ()));
    _boxed_phys_.set_placement (_boxed_replica_placement_);
    _boxed_phys_.set_logical (_boxed_model_->get_logical ());
    _boxed_phys_.set_mother (_logical);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void replicated_boxed_model::tree_dump (std::ostream & out_,
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
           << "Replicated placement (box) : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _boxed_replica_placement_.tree_dump (out_, "", indent_oss.str ());
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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(replicated_boxed_model,"geomtools::replicated_boxed_model")

} // end of namespace geomtools

// end of replicated_boxed_model.cc
