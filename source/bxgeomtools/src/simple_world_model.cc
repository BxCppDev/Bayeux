// -*- mode: c++ ; -*-
/* simple_world_model.cc
 */

// Ourselves:
#include <geomtools/simple_world_model.h>

// Standard library:
#include <exception>

// This project:
#include <geomtools/visibility.h>
#include <datatools/units.h>

namespace geomtools {

  const std::string simple_world_model::SETUP_LABEL = "setup";

  const geomtools::box & simple_world_model::get_solid () const
  {
    return _solid_;
  }

  std::string simple_world_model::get_model_id () const
  {
    return "geomtools::simple_world_model";
  }

  // ctor:
  simple_world_model::simple_world_model () : geomtools::i_model ()
  {
    _setup_model_ = 0;
    return;
  }

  simple_world_model::~simple_world_model ()
  {
    return;
  }

  void simple_world_model::_at_construct (const datatools::properties & config_,
                                          models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    double length_unit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string length_unit_str = config_.fetch_string ("length_unit");
      length_unit = datatools::units::get_length_unit_from (length_unit_str);
    }

    double angle_unit = CLHEP::degree;
    if (config_.has_key ("angle_unit")) {
      const std::string angle_unit_str = config_.fetch_string ("angle_unit");
      angle_unit = datatools::units::get_angle_unit_from (angle_unit_str);
    }

    double world_x;
    datatools::invalidate (world_x);
    if (config_.has_key ("world.x")) {
      world_x = config_.fetch_real ("world.x");
      if (! config_.has_explicit_unit ("world.x")) world_x *= length_unit;
    }

    double world_y;
    datatools::invalidate (world_y);
    if (config_.has_key ("world.y")) {
      world_y = config_.fetch_real ("world.y");
      if (! config_.has_explicit_unit ("world.y")) world_y *= length_unit;
    }

    double world_z;
    datatools::invalidate (world_z);
    if (config_.has_key ("world.z")) {
      world_z = config_.fetch_real ("world.z");
      if (! config_.has_explicit_unit ("world.z")) world_z *= length_unit;
    }

    double setup_x = 0.;
    if (config_.has_key ("setup.x")) {
      setup_x = config_.fetch_real ("setup.x");
      if (! config_.has_explicit_unit ("setup.x")) setup_x *= length_unit;
    }

    double setup_y = 0.;
    if (config_.has_key ("setup.y")) {
      setup_y = config_.fetch_real ("setup.y");
      if (! config_.has_explicit_unit ("setup.z")) setup_y *= length_unit;
    }

    double setup_z = 0.;
    if (config_.has_key ("setup.z")) {
      setup_z = config_.fetch_real ("setup.z");
      if (! config_.has_explicit_unit ("setup.z")) setup_z *= length_unit;
    }

    double setup_theta = 0.;
    if (config_.has_key ("setup.theta")) {
      setup_theta = config_.fetch_real ("setup.theta");
      if (! config_.has_explicit_unit ("setup.theta")) setup_theta *= angle_unit;
    }

    double setup_phi   = 0.;
    if (config_.has_key ("setup.phi")) {
      setup_phi = config_.fetch_real ("setup.phi");
      if (! config_.has_explicit_unit ("setup.phi")) setup_phi *= angle_unit;
    }

    std::string material = "vacuum";
    if (config_.has_key("material.ref")) {
      material = config_.fetch_string ("material.ref");
    }

    DT_THROW_IF (! config_.has_key ("setup.model"), std::logic_error, "Missing 'setup_model' property in simple world model '" << get_name() << "' !");
    const std::string setup_model_name = config_.fetch_string ("setup.model");

    DT_THROW_IF (! models_, std::logic_error, "Missing logicals dictionary in simple world model '" << get_name() << "' !");

    // Setup model:
    {
      models_col_type::const_iterator found = models_->find (setup_model_name);
      DT_THROW_IF (found == models_->end (), std::logic_error, "Cannot find setup model with name '"<< setup_model_name << "' in simple world model '" << get_name() << "' !");
      //_setup_model_ = (dynamic_cast<const test_model_2 *> (found->second));
      _setup_model_ = found->second;
    }

    const vector_3d setup_pos (setup_x, setup_y, setup_z);
    //create_xyz (setup_pos, 0.0, 0.0, 0.0);
    _setup_placement_.set_translation (setup_pos);
    _setup_placement_.set_orientation (setup_phi, setup_theta, 0.0);

    double setup_sx = 0.;
    double setup_sy = 0.;
    double setup_sz = 0.;
    if (_setup_model_->get_logical ().get_shape ().get_shape_name () == "box") {
      const geomtools::box * b
        = dynamic_cast<const geomtools::box *> (&(_setup_model_->get_logical ()).get_shape ());
      setup_sx = b->get_x ();
      setup_sy = b->get_y ();
      setup_sz = b->get_z ();
    }

    double size = setup_sx;
    if (setup_sy > size) size = setup_sy;
    if (setup_sz > size) size = setup_sz;
    size *= 2;
    _world_x_ = size;
    _world_y_ = size;
    _world_z_ = size;

    if (datatools::is_valid (world_x)) {
      _world_x_ = world_x;
    }
    if (datatools::is_valid (world_y)) {
      _world_y_ = world_y;
    }
    if (datatools::is_valid (world_z)) {
      _world_z_ = world_z;
    }

    _solid_.reset();
    _solid_.set_x(_world_x_);
    _solid_.set_y(_world_y_);
    _solid_.set_z(_world_z_);
    _solid_.lock();

    DT_THROW_IF (!_solid_.is_valid (), std::logic_error, "Invalid solid !");

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref(material);

    DT_LOG_TRACE (get_logging_priority (), "Install physicals...");
    _setup_phys_.set_name (i_model::make_physical_volume_name (SETUP_LABEL));
    _setup_phys_.set_placement (_setup_placement_);
    _setup_phys_.set_logical (_setup_model_->get_logical ());
    _setup_phys_.set_mother (_logical);
    DT_LOG_TRACE (get_logging_priority (), "Physicals are installed.");

    DT_LOG_TRACE (get_logging_priority (), "Exiting.")
    return;
  }

  void simple_world_model::tree_dump (std::ostream & out_,
                                      const std::string & title_ ,
                                      const std::string & indent_,
                                      bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);


    {
      // Setup model:
      if (_setup_model_) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Setup model : " << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _setup_model_->tree_dump (out_, "", indent_oss.str ());
        }
      } else {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Setup model : " << "<missing>" << std::endl;
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Setup placement: " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _setup_placement_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Setup physical : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _setup_phys_.tree_dump (out_, "", indent_oss.str ());
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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(simple_world_model,"geomtools::simple_world_model")

} // end of namespace geomtools

// end of simple_world_model.cc
