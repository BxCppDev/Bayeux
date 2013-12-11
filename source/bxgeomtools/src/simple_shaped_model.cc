// -*- mode: c++ ; -*-
/* simple_shaped_model.cc
 */

#include <geomtools/simple_shaped_model.h>

#include <iostream>
#include <exception>
#include <sstream>
#include <limits>

#include <datatools/utils.h>
#include <datatools/units.h>

#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/sphere.h>
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>

#include <geomtools/visibility.h>
#include <geomtools/material.h>
#include <geomtools/sensitive.h>
#include <geomtools/color.h>
#include <geomtools/i_stackable.h>
#include <geomtools/utils.h>
#include <geomtools/mapping_utils.h>

namespace geomtools {

  bool simple_shaped_model::is_filled () const
  {
    return _filled_mode_ != filled_utils::FILLED_NONE;
  }

  bool simple_shaped_model::is_filled_by_envelope () const
  {
    return _filled_mode_ == filled_utils::FILLED_BY_ENVELOPE;
  }

  bool simple_shaped_model::is_filled_by_extrusion () const
  {
    return _filled_mode_ == filled_utils::FILLED_BY_EXTRUSION;
  }

  MWIM & simple_shaped_model::grab_internals ()
  {
    return _internals_;
  }

  const MWIM & simple_shaped_model::get_internals () const
  {
    return _internals_;
  }

  const geomtools::box & simple_shaped_model::get_box () const
  {
    return *_box_;
  }

  const geomtools::cylinder & simple_shaped_model::get_cylinder () const
  {
    return *_cylinder_;
  }

  const geomtools::tube & simple_shaped_model::get_tube () const
  {
    return *_tube_;
  }

  const geomtools::sphere & simple_shaped_model::get_sphere () const
  {
    return *_sphere_;
  }

  const geomtools::polycone & simple_shaped_model::get_polycone () const
  {
    return *_polycone_;
  }

  const geomtools::polyhedra & simple_shaped_model::get_polyhedra () const
  {
    return *_polyhedra_;
  }

  const geomtools::i_shape_3d & simple_shaped_model::get_solid () const
  {
    return *_solid_;
  }

  const std::string & simple_shaped_model::get_shape_name () const
  {
    return _shape_name_;
  }

  const std::string & simple_shaped_model::get_material_name () const
  {
    return _material_name_;
  }

  const std::string & simple_shaped_model::get_filled_material_name () const
  {
    return _filled_material_name_;
  }

  std::string simple_shaped_model::get_model_id () const
  {
    return "geomtools::simple_shaped_model";
  }

  // ctor:
  simple_shaped_model::simple_shaped_model () : i_model ()
  {
    _box_                    = 0;
    _cylinder_               = 0;
    _tube_                   = 0;
    _sphere_                 = 0;
    _polycone_               = 0;
    _solid_                  = 0;
    _inner_shape_            = 0;
    _outer_shape_            = 0;
    _shape_name_             = ""; // no defined shape
    _material_name_          = ""; //material::material_ref_unknown();
    _filled_mode_            = filled_utils::FILLED_NONE;
    _filled_material_name_   = ""; //material::material_ref_unknown();
    return;
  }

  // dtor:
  simple_shaped_model::~simple_shaped_model ()
  {
    _solid_ = 0;
    if (_inner_shape_ != 0) delete _inner_shape_;
    if (_outer_shape_ != 0) delete _outer_shape_;
    if (_box_ != 0)         delete _box_;
    if (_cylinder_ != 0)    delete _cylinder_;
    if (_tube_ != 0)        delete _tube_;
    if (_sphere_ != 0)      delete _sphere_;
    if (_polycone_ != 0)    delete _polycone_;
    return;
  }

  void simple_shaped_model::_at_construct (const std::string & name_,
                                           const datatools::properties & config_,
                                           models_col_type * models_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    // Initialization:
    _filled_material_name_ = material::material_ref_unknown();

    // Parsing shape:
    DT_THROW_IF (! config_.has_key ("shape_type"), std::logic_error, "Missing 'shape_type' property in simple shaped model '" << name_ << "' !");
    _shape_name_ = config_.fetch_string ("shape_type");

    // Shape fill mode:
    _filled_mode_ = filled_utils::FILLED_NONE;
    if (config_.has_key ("filled_mode")) {
      const std::string filled_mode_label = config_.fetch_string ("filled_mode");
      _filled_mode_ = filled_utils::get_filled_mode(filled_mode_label);
      DT_THROW_IF (_filled_mode_ == filled_utils::FILLED_UNDEFINED,
                   std::logic_error,
                   "Invalid filled mode '" << filled_mode_label << "' property in simple shaped (tube) model '" << name_ << "' !");
      // if (is_filled()) {
      //   DT_LOG_WARNING(get_logging_priority (),
      //                  "Filled mode '" << filled_utils::get_filled_mode_label(_filled_mode_)
      //                  << "' is not recommended unless you know what you do !");
      // }
    }

    // Label of the shape in 'filled envelope' mode:
    if (is_filled_by_envelope()){
      _filled_label_ = _shape_name_ + "_by_envelope";
      if (config_.has_key("filled_label")) {
        _filled_label_ = config_.fetch_string("filled_label");
      }
      DT_THROW_IF (_filled_label_.empty(),
                   std::logic_error,
                   "Invalid filled label '" << _filled_label_ << "' property in simple shaped (tube) model '" << name_ << "' !");
    }

    // Main shape material:
    DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property in simple shaped model '" << name_ << "' !");
    _material_name_ = config_.fetch_string ("material.ref");

    // Filling material:
    if (is_filled()){
      // Parsing material:
      DT_THROW_IF (! config_.has_key ("material.filled.ref"),
                   std::logic_error,
                   "Missing 'material.filled.ref' property in simple shaped (tube) model '" << name_ << "' !");
      _filled_material_name_ = config_.fetch_string ("material.filled.ref");
    }

    // Set the logical name:
    grab_logical ().set_name (i_model::make_logical_volume_name (name_));

    if (get_shape_name () == "box") {
      // Box case:
      _construct_box (name_, config_, models_);
    } else if (get_shape_name () == "cylinder") {
      // Cylinder case:
      _construct_cylinder (name_, config_, models_);
    } else if (get_shape_name () == "sphere") {
      // Sphere case:
      _construct_sphere (name_, config_, models_);
    } else if (get_shape_name () == "tube") {
      // Tube case:
      _construct_tube (name_, config_, models_);
    } else if (get_shape_name () == "polycone") {
      // Polycone case:
      _construct_polycone (name_, config_, models_);
    } else if (get_shape_name () == "polyhedra") {
      // Polyhedra case:
      _construct_polyhedra (name_, config_, models_);
    } else {
      DT_THROW_IF (true, std::logic_error, "Shape '" << get_shape_name () << "' is not supported in simple shaped model '" << name_ << "' !");
    }

    // Set the envelope solid shape:
    grab_logical ().set_shape (*_solid_);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_box (const std::string & name_,
                                            const datatools::properties & config_,
                                            models_col_type * /*models_*/)
  {
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    DT_THROW_IF (! config_.has_key ("x"), std::logic_error, "Missing box 'x' property in simple shaped (box) model '" << name_ << "' !");
    double x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) {
      x *= lunit;
    }
    DT_THROW_IF (! config_.has_key ("y"), std::logic_error, "Missing box 'y' property in simple shaped (box) model '" << name_ << "' !");
    double y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) {
      y *= lunit;
    }
    DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing box 'z' property in simple shaped (box) model '" << name_ << "' !");
    double z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) {
      z *= lunit;
    }

    // build the box:
    _box_ = new box (x, y, z);
    DT_THROW_IF (! _box_->is_valid (), std::logic_error, "Invalid box dimensions in simple shaped (box) model '" << name_ << "' !");
    _solid_ = _box_;
    grab_logical ().set_material_ref (_material_name_);

    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_cylinder (const std::string & name_,
                                                 const datatools::properties & config_,
                                                 models_col_type * /*models_*/)
  {
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    double aunit = CLHEP::degree;
    if (config_.has_key ("angle_unit")) {
      const std::string aunit_str = config_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    double r;
    datatools::invalidate (r);
    if (config_.has_key ("r")) {
      r = config_.fetch_real ("r");
      if (! config_.has_explicit_unit ("r")) {
        r *= lunit;
      }
    } else if (config_.has_key ("radius")) {
      r = config_.fetch_real ("radius");
      if (! config_.has_explicit_unit ("radius")) {
        r *= lunit;
      }
    } else if (config_.has_key ("diameter")) {
      r = 0.5 * config_.fetch_real ("diameter");
      if (! config_.has_explicit_unit ("diameter")) {
        r *= lunit;
      }
    }
    DT_THROW_IF (! datatools::is_valid (r), std::logic_error, "Missing cylinder 'r' or 'diameter' property in simple shaped (cylinder) model '" << name_ << "' !");

    DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing cylinder 'z' property in simple shaped (cylinder) model '" << name_ << "' !");
    double z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) {
      z *= lunit;
    }

    // build the cylinder:
    _cylinder_ = new cylinder (r, z);
    DT_THROW_IF (! _cylinder_->is_valid (), std::logic_error, "Invalid cylinder dimensions in simple shaped (cylinder) model '" << name_ << "'");
    _solid_ = _cylinder_;
    grab_logical ().set_material_ref (_material_name_);

    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_sphere (const std::string & name_,
                                               const datatools::properties & config_,
                                               models_col_type* /*models_*/)
  {
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    double aunit = CLHEP::degree;
    if (config_.has_key ("angle_unit")) {
      const std::string aunit_str = config_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    DT_THROW_IF (! config_.has_key ("r"), std::logic_error,
                 "Missing sphere 'r' property in simple shaped (sphere) model '" << name_ << "' !");
    double r = config_.fetch_real ("r");
    if (! config_.has_explicit_unit ("r")) {
      r *= lunit;
    }

    // build the sphere:
    _sphere_ = new sphere (r);
    DT_THROW_IF (! _sphere_->is_valid (), std::logic_error,
                 "Invalid sphere dimension in simple shaped (sphere) model '" << name_ << "' !");
    _solid_ = _sphere_;
    grab_logical ().set_material_ref (_material_name_);

    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_tube (const std::string & name_,
                                             const datatools::properties & config_,
                                             models_col_type* /*models_*/)
  {
    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    double aunit = CLHEP::degree;
    if (config_.has_key ("angle_unit")) {
      const std::string aunit_str = config_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    double inner_r;
    datatools::invalidate (inner_r);
    if (config_.has_key ("inner_r")) {
      inner_r = config_.fetch_real ("inner_r");
      if (! config_.has_explicit_unit ("inner_r")) {
        inner_r *= lunit;
      }
    } else if (config_.has_key ("inner_radius")) {
      inner_r = config_.fetch_real ("inner_radius");
      if (! config_.has_explicit_unit ("inner_radius")) {
        inner_r *= lunit;
      }
    } else if (config_.has_key ("inner_diameter")) {
      inner_r = 0.5 * config_.fetch_real ("inner_diameter");
      if (! config_.has_explicit_unit ("inner_diameter")) {
        inner_r *= lunit;
      }
    }
    if (! datatools::is_valid (inner_r)) {
      DT_LOG_WARNING (get_logging_priority (),
                      "Missing tube 'inner_r' property ! Using 0-default inner radius in simple shaped (tube) model '" << name_ << "' !");
      inner_r = 0.0;
    }

    double outer_r;
    datatools::invalidate (outer_r);
    if (config_.has_key ("outer_r")) {
      outer_r = config_.fetch_real ("outer_r");
      if (! config_.has_explicit_unit ("outer_r")) {
        outer_r *= lunit;
      }
    } else if (config_.has_key ("outer_radius")) {
      outer_r = config_.fetch_real ("outer_radius");
      if (! config_.has_explicit_unit ("outer_radius")) {
        outer_r *= lunit;
      }
    } else if (config_.has_key ("outer_diameter")) {
      outer_r = 0.5 * config_.fetch_real ("outer_diameter");
      if (! config_.has_explicit_unit ("outer_diameter")) {
        outer_r *= lunit;
      }
    }
    DT_THROW_IF (! datatools::is_valid (outer_r), std::logic_error,
                 "Missing tube 'outer_r' property in simple shaped (tube) model '" << name_ << "' !");

    DT_THROW_IF (! config_.has_key ("z"), std::logic_error,
                 "Missing tube 'z' property in simple shaped (tube) model '" << name_ << "' !");
    double z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) {
      z *= lunit;
    }

    // Build the tube:
    _tube_ = new tube (inner_r, outer_r, z);
    DT_THROW_IF (! _tube_->is_valid (), std::logic_error,
                 "Invalid tube dimensions in simple shaped (tube) model '" << name_ << "' !");

    // Use the plain tube as solid envelope of the model:
    if (_filled_mode_ == filled_utils::FILLED_NONE) {
      _solid_ = _tube_;
      grab_logical ().set_material_ref (_material_name_);
    }


    DT_THROW_IF(_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION,
                std::runtime_error,
                "No support for tube construction 'by extrusion' in simple shaped (tube) model '" << name_ << "' !");

    /*
     *          _____________________
     *         |_____________________|
     *      _ _|_ _ _ _ _ _ _ _ _ _ _|_ _ _
     *     z'  |_____________________|      z
     *         |_____________________|
     */
    /*
    // Build the tube model by cylindric extrusion of a mother cylinder:
    if (_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION) {
    // Make the envelope a cylinder:
    _cylinder_ = new cylinder;
    _tube_->compute_outer_cylinder (*_cylinder_);
    DT_THROW_IF (! _cylinder_->is_valid (), std::logic_error, "Invalid 'outer' cylinder dimensions in simple shaped (tube) model '" << name_ << "' !");
    _solid_ = _cylinder_;
    grab_logical ().set_material_ref (_material_name_);
    grab_logical ().set_effective_shape (*_tube_);
    if (! _tube_->is_extruded ()) {
    // If the tube is not extruded, no daughter physical volumes can be placed
    // within the 'outer' envelope cylinder:
    } else {
    // If the tube is extruded, add an extrusion 'inner' cylinder within the 'outer' cylinder:
    cylinder * inner_cyl = new cylinder;
    _tube_->compute_inner_cylinder (*inner_cyl);
    if (! inner_cyl->is_valid ()) {
    delete inner_cyl;
    DT_THROW_IF (true, std::logic_error,
    "Invalid 'inner' cylinder dimensions in simple shaped (tube) model '" << name_ << "' !");
    }
    _inner_shape_ = inner_cyl;
    // Inner placement for the extrusion:
    _inner_placement_.set (0, 0, 0, 0, 0, 0);
    const std::string inner_name = "__" + get_logical ().get_name () + ".tube_by_extrusion";
    _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name));
    _inner_logical_.set_material_ref (_filled_material_name_);
    _inner_logical_.set_shape (*_inner_shape_); // pass a reference -> logical has not the shape ownership
    _inner_logical_.set_geometry_model(*this);
    _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name));
    _inner_phys_.set_placement (_inner_placement_);
    _inner_phys_.set_logical (_inner_logical_);
    _inner_phys_.set_mother (this->get_logical ());

    }
    }
    */

    // Build the tube as a child of a mother cylinder:
    if (_filled_mode_ == filled_utils::FILLED_BY_ENVELOPE) {
      // Make the envelope a cylinder:
      _cylinder_ = new cylinder;
      _tube_->compute_outer_cylinder (*_cylinder_);
      DT_THROW_IF (! _cylinder_->is_valid (), std::logic_error,
                   "Invalid 'outer' cylinder dimensions in simple shaped (tube) model '" << name_ << "' !");
      _solid_ = _cylinder_;
      grab_logical ().set_material_ref (_filled_material_name_);
      //grab_logical ().set_effective_shape (*_tube_);
      //grab_logical ().set_effective_material_ref (_material_name_);
      // If the tube is extruded, add the tube within the 'outer' envelope cylinder:
      _inner_placement_.set (0, 0, 0, 0, 0, 0);
      //const std::string inner_name = "__" + get_logical ().get_name () + "." + _filled_label_;
      const std::string inner_name = "__" + get_name () + "." + _filled_label_;
      _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name));
      _inner_logical_.set_material_ref (_material_name_);
      _inner_logical_.set_shape (*_tube_);
      _inner_logical_.set_geometry_model(*this);
      _inner_phys_.set_name (i_model::make_physical_volume_name (_filled_label_));
      _inner_phys_.set_placement (_inner_placement_);
      _inner_phys_.set_logical (_inner_logical_);
      _inner_phys_.set_mother (this->get_logical ());
    }

    return;
  }

  /*****************************************************/

  void simple_shaped_model::_construct_polycone (const std::string & name_,
                                                 const datatools::properties & config_,
                                                 models_col_type* /*models_*/)
  {
    // Build the polycone:
    _polycone_ = new polycone ();
    _polycone_->initialize (config_);
    DT_THROW_IF (! _polycone_->is_valid (), std::logic_error,
                 "Invalid polycone build parameters in simple shaped (polycone) model '" << name_ << "' !");

    DT_LOG_TRACE (get_logging_priority (), "Polycone:");
    if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
      _polycone_->tree_dump (std::cerr);
    }

    if (_filled_mode_ == filled_utils::FILLED_NONE) {
      _solid_ = _polycone_;
      grab_logical ().set_material_ref (_material_name_);
    }

    /*
      DT_THROW_IF(_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION,
      std::runtime_error,
      "No support for polycone construction 'by extrusion' in simple shaped (polycone) model '" << name_ << "' !");

      // Build the polycone model by extrusion of a mother polycone:
      if (_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION) {
      // Make the envelope a polycone:
      polycone * envelope_polycone = new polycone;
      _polycone_->compute_outer_polycone (*envelope_polycone);
      DT_THROW_IF (! envelope_polycone->is_valid (), std::logic_error,
      "Invalid envelope polycone in simple shaped (polycone) model '" << name_ << "' !");
      _outer_shape_ = envelope_polycone;
      _solid_ = _outer_shape_;
      grab_logical ().set_material_ref (_material_name_);
      grab_logical ().set_effective_shape (*_polycone_);
      // If the polycone is extruded, add an extruded 'inner' polycone within the 'outer' polycone:
      if (! _polycone_->is_extruded ()) {
      // If the polycone is not extruded, no daughter physical volumes can be placed
      // within the 'outer' envelope cylinder:
      } else {
      polycone * inner_pol = new polycone;
      _polycone_->compute_inner_polycone (*inner_pol);
      if (! inner_pol->is_valid ()) {
      delete inner_pol;
      DT_THROW_IF (true, std::logic_error,
      "Invalid 'inner' polycone dimensions in simple shaped (polycone) model '" << name_ << "' !");
      }
      _inner_shape_ = inner_pol;
      // Inner placement for the extrusion:
      _inner_placement_.set (0, 0, 0, 0, 0, 0);
      const std::string inner_name = "__" + get_logical ().get_name () + ".polycone_by_extrusion";
      _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name));
      _inner_logical_.set_material_ref (_filled_material_name_);
      _inner_logical_.set_shape (*_inner_shape_); // pass a reference -> logical has not the shape ownership
      _inner_logical_.set_geometry_model(*this);
      _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name));
      _inner_phys_.set_placement (_inner_placement_);
      _inner_phys_.set_logical (_inner_logical_);
      _inner_phys_.set_mother (this->get_logical ());

      }
      }
    */

    // Build the polycone as a filled child of a mother filled polycone:
    if (_filled_mode_ == filled_utils::FILLED_BY_ENVELOPE) {
      // Make the envelope a cylinder:
      polycone * outer_polycone = new polycone;
      _polycone_->compute_outer_polycone (*outer_polycone);
      DT_THROW_IF (! outer_polycone->is_valid (), std::logic_error, "Invalid 'outer' cylinder dimensions in simple shaped (polycone) model '" << name_ << "' !");
      _outer_shape_ = outer_polycone;
      _solid_ = _outer_shape_;
      grab_logical ().set_material_ref (_filled_material_name_);
      //grab_logical ().set_effective_shape (*_polycone_);
      //grab_logical ().set_effective_material_ref (_material_name_);
      // If the polycone is extruded, add the 'inner' polycone
      // within the 'outer' envelope polycone:
      _inner_placement_.set (0, 0, 0, 0, 0, 0);
      const std::string inner_name = "__" + get_name () + "." + _filled_label_;
      _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name));
      _inner_logical_.set_material_ref (_material_name_);
      _inner_logical_.set_shape (*_polycone_);
      _inner_logical_.set_geometry_model(*this);
      _inner_phys_.set_name (i_model::make_physical_volume_name (_filled_label_));
      _inner_phys_.set_placement (_inner_placement_);
      _inner_phys_.set_logical (_inner_logical_);
      _inner_phys_.set_mother (this->get_logical ());
    }

    return;
  }


  /*****************************************************/

  void simple_shaped_model::_construct_polyhedra (const std::string & name_,
                                                  const datatools::properties & config_,
                                                  models_col_type* /*models_*/)
  {
    _polyhedra_ = new polyhedra ();
    _polyhedra_->initialize (config_);
    DT_THROW_IF (! _polyhedra_->is_valid (), std::logic_error, "Invalid polyhedra build parameters in simple shaped (polyhedra) model '" << name_ << "' !");

    DT_LOG_TRACE (get_logging_priority (), "Polyhedra:");
    if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
      _polyhedra_->tree_dump (std::cerr);
    }

    if (_filled_mode_ == filled_utils::FILLED_NONE) {
      _solid_ = _polyhedra_;
      grab_logical ().set_material_ref (_material_name_);
    }

    DT_THROW_IF(_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION,
                std::runtime_error,
                "No support for polyhedra construction 'by extrusion' in simple shaped (polyhedra) model '" << name_ << "' !");

    // build the polyhedra by extrusion of a mother polyhedra:
    /*
      if (_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION)  {
      // make the envelope a polyhedra:
      polyhedra * envelope_polyhedra = new polyhedra;
      _polyhedra_->compute_outer_polyhedra (*envelope_polyhedra);
      DT_THROW_IF (! envelope_polyhedra->is_valid (), std::logic_error, "Invalid envelope polyhedra in simple shaped (polyhedra) model '" << name_ << "' !");
      _outer_shape_ = envelope_polyhedra;
      _solid_ = _outer_shape_;
      grab_logical ().set_material_ref (_material_name_);
      grab_logical ().set_effective_shape (*_polyhedra_);
      // if the polyhedra is extruded, add an extruded 'inner' polyhedra
      // within the 'outer' polyhedra:
      if (! _polyhedra_->is_extruded ()) {
      // if the polyhedra is not extruded, no daughter physical volumes can be placed
      // within the 'outer' envelope polyhedra:
      } else {
      polyhedra * inner_pol = new polyhedra;
      _polyhedra_->compute_inner_polyhedra (*inner_pol);
      if (! inner_pol->is_valid ()) {
      delete inner_pol;
      DT_THROW_IF (true, std::logic_error, "Invalid 'inner' polyhedra dimensions in simple shaped (polyhedra) model '" << name_ << "' !");
      }
      _inner_shape_ = inner_pol;
      // inner placement for the extrusion:
      _inner_placement_.set (0, 0, 0, 0, 0, 0);
      const std::string inner_name = "__" + get_logical ().get_name () + ".polyhedra_by_extrusion";
      _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name));
      _inner_logical_.set_material_ref (_filled_material_name_);
      _inner_logical_.set_shape (*_inner_shape_); // pass a reference -> logical has not the shape ownership
      _inner_logical_.set_geometry_model(*this);
      _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name));
      _inner_phys_.set_placement (_inner_placement_);
      _inner_phys_.set_logical (_inner_logical_);
      _inner_phys_.set_mother (this->get_logical ());

      }
      }
    */

    // build the polyhedra as a filled child of a mother filled polyhedra:
    if (_filled_mode_ == filled_utils::FILLED_BY_ENVELOPE) {
      // make the envelope a cylinder:
      polyhedra * outer_polyhedra = new polyhedra;
      _polyhedra_->compute_outer_polyhedra (*outer_polyhedra);
      DT_THROW_IF (! outer_polyhedra->is_valid (), std::logic_error, "Invalid 'outer' cylinder dimensions in simple shaped (polyhedra) model '" << name_ << "' !");
      _outer_shape_ = outer_polyhedra;
      _solid_ = _outer_shape_;
      grab_logical ().set_material_ref (_filled_material_name_);
      //grab_logical ().set_effective_shape (*_polyhedra_);
      //grab_logical ().set_effective_material_ref (_material_name_);
      // if the polyhedra is extruded, add the polyhedra
      // within the 'outer' envelope polyhedra:
      _inner_placement_.set (0, 0, 0, 0, 0, 0);
      const std::string inner_name = "__" + get_name () + _filled_label_;
      _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name));
      _inner_logical_.set_material_ref (_material_name_);
      _inner_logical_.set_shape (*_polyhedra_);
      _inner_logical_.set_geometry_model(*this);
      _inner_phys_.set_name (i_model::make_physical_volume_name (_filled_label_));
      _inner_phys_.set_placement (_inner_placement_);
      _inner_phys_.set_logical (_inner_logical_);
      _inner_phys_.set_mother (this->get_logical ());
    }

    return;
  }

  /*******************************************/

  void simple_shaped_model::_pre_construct (datatools::properties & setup_,
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

  void simple_shaped_model::_post_construct (datatools::properties & setup_,
                                             models_col_type * models_)
  {
    datatools::logger::priority log_level = get_logging_priority ();
    //log_level = datatools::logger::PRIO_DEBUG;
    DT_LOG_NOTICE(get_logging_priority (),
                  "Post-construction processing for model '" << get_name() << "' ...");

    if (! is_filled ()) {
      // None mode:
      this->i_model::_post_construct(setup_, models_);
    } else {
      // Filled mode:
      if (is_filled_by_envelope()) {
        {
          // Set description for logical parameters :
          std::ostringstream log_params_desc;
          log_params_desc << "Auxiliary properties for shape logical volume in model '"
                          << get_name() << "'";
          _inner_logical_.grab_parameters ().set_description(log_params_desc.str());
        }
        std::vector<std::string> exported_prefixes;
        if (setup_.has_key(i_model::exported_properties_prefixes_key())) {
          setup_.fetch(i_model::exported_properties_prefixes_key(),
                       exported_prefixes);
        }
        for (size_t i = 0; i < exported_prefixes.size(); i++) {
          const std::string & topic_prefix = exported_prefixes[i];
          const std::string filled_topic_prefix = topic_prefix + "filled.";
          DT_LOG_NOTICE(get_logging_priority (),
                        "Reprocessing '" << topic_prefix << "' properties for model '" << get_name() << "' ...");
          // All (top+filled) properties :
          datatools::properties tmp_params;
          setup_.export_starting_with (tmp_params, topic_prefix);
          // Only properties for the 'filled' volume :
          datatools::properties filled_params;
          tmp_params.export_starting_with (filled_params, filled_topic_prefix);
          // Only 'top' properties :
          tmp_params.erase_all_starting_with (filled_topic_prefix);
          // Inner logical inherits the 'top' properties :
          tmp_params.export_starting_with (_inner_logical_.grab_parameters (), topic_prefix);
          // Top logical inherits the 'filled' properties :
          grab_logical ().grab_parameters ().erase_all_starting_with (filled_topic_prefix);
          grab_logical ().grab_parameters ().erase_all_starting_with (topic_prefix);
          filled_params.export_and_rename_starting_with(grab_logical ().grab_parameters (), filled_topic_prefix, topic_prefix);
        }
      } // is_filled_by_envelope()

    }
    if (_solid_) {
      stackable::extract (setup_, _solid_->grab_properties ());
    }
    grab_logical ().set_geometry_model(*this);

    // Search for internal items to be installed within the logicals :
    if (is_filled()) {
      DT_LOG_DEBUG (log_level, "Processing internal items (filled)...");
      if (is_filled_by_envelope()) {
        // Top logical is the cavity envelope :
        DT_LOG_DEBUG (log_level, "Processing internal items in cavity...");
        _filled_internals_.plug_internal_models (get_logical().get_parameters(),
                                                 grab_logical(),
                                                 models_);
        DT_LOG_DEBUG (log_level, "Number of items in cavity : "
                      << _filled_internals_.get_number_of_items());

        DT_LOG_DEBUG (log_level, "Processing internal items in shape...");
        _internals_.plug_internal_models (_inner_logical_.get_parameters(),
                                          _inner_logical_,
                                          models_);
        DT_LOG_DEBUG (log_level, "Number of items in shape : "
                      << _internals_.get_number_of_items());
      }
      _inner_logical_.lock();
    } else {
      DT_LOG_DEBUG (log_level, "Processing internal items (none)...");
      // Top logical is the shape :
      DT_LOG_DEBUG (log_level, "Processing internal items in shape...");
      DT_LOG_DEBUG (log_level, "Logical parameters for model '"
                    << get_name() << "' : ");
      if (log_level >= datatools::logger::PRIO_DEBUG) {
        get_logical().get_parameters().tree_dump(std::cerr, "");
      }
      _internals_.plug_internal_models (get_logical().get_parameters(),
                                        grab_logical(),
                                        models_);
    }

    return;
  }

  /*******************************************/

  void simple_shaped_model::tree_dump (std::ostream & out_,
                                       const std::string & title_ ,
                                       const std::string & indent_,
                                       bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    // Shape name:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Shape name : '" << get_shape_name () << "'" << std::endl;
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Material name : '" << get_material_name () << "'" << std::endl;
    }

    // Solid:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Solid : ";
      if (_solid_ != 0) {
        out_ << std::endl;
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _solid_->tree_dump (out_, "", indent_oss.str ());
      } else {
        out_ << "<none>" << std::endl;
      }
    }

    // Filled material name:
    if ( _filled_mode_ != filled_utils::FILLED_NONE ) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Filled mode : '" << filled_utils::get_filled_mode_label(_filled_mode_) << "'" << std::endl;
      if (! _filled_material_name_.empty ()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Filled material name : '" << get_filled_material_name () << "'" << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Inner logical : " << std::endl;
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::skip_tag;
      _inner_logical_.tree_dump(out_, "",indent_oss.str ());

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Number of internal items within cavity: '" << _filled_internals_.get_number_of_items() << "'" << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Number of internal items : '" << _internals_.get_number_of_items() << "'" << std::endl;

    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(simple_shaped_model,"geomtools::simple_shaped_model");

} // end of namespace geomtools

// end of simple_shaped_model.cc
