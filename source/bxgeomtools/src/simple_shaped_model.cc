// \file geomtools/simple_shaped_model.cc

// Ourselves:
#include <geomtools/simple_shaped_model.h>

// Standard library:
#include <iostream>
#include <exception>
#include <sstream>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
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
#include <geomtools/shape_factory.h>

namespace geomtools {

  simple_shaped_model::shape_build_mode_type simple_shaped_model::get_shape_build_mode() const
  {
    return _sbm_;
  }

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

  const simple_shaped_model::envelope_tolerance_type &
  simple_shaped_model::get_envelope_tolerance() const
  {
    return _envelope_tolerance_;
  }

  simple_shaped_model::envelope_tolerance_type &
  simple_shaped_model::grab_envelope_tolerance()
  {
    return _envelope_tolerance_;
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

  const std::string & simple_shaped_model::get_shape_type_id () const
  {
    return _shape_type_id_;
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

  simple_shaped_model::simple_shaped_model () : i_model ()
  {
    _sbm_                    = SBM_INVALID;
    _shape_type_id_          = ""; // no defined shape
    _material_name_          = ""; // material::material_ref_unknown();
    _filled_mode_            = filled_utils::FILLED_NONE;
    _filled_material_name_   = ""; // material::material_ref_unknown();
    return;
  }

  simple_shaped_model::~simple_shaped_model ()
  {
    if (_sbm_ == SBM_LEGACY) {
      if (_inner_shape_) _inner_shape_.reset();
      if (_outer_shape_) _outer_shape_.reset();
      if (_box_) _box_.reset();
      if (_cylinder_) _cylinder_.reset();
      if (_tube_) _tube_.reset();
      if (_sphere_) _sphere_.reset();
      if (_polycone_) _polycone_.reset();
      if (_polyhedra_) _polyhedra_.reset();
    }
    if (_solid_) _solid_.reset();
    return;
  }

  void simple_shaped_model::_at_construct (const datatools::properties & config_,
                                           models_col_type * models_)
  {
    datatools::logger::priority logging = get_logging_priority();
    logging = datatools::logger::PRIO_DEBUG;
    DT_LOG_TRACE(logging, "Entering...");

    // Initialization:
    _filled_material_name_ = material::material_ref_unknown();

    // Main shape material:
    DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error,
                 "Missing 'material.ref' property in simple shaped model '" << get_name() << "' !");
    _material_name_ = config_.fetch_string ("material.ref");

    if (config_.has_key("shape_build_mode")) {
      const std::string & sbm_label = config_.fetch_string("shape_build_mode");
      if (sbm_label == "legacy") {
        _sbm_ = SBM_LEGACY;
      } else if (sbm_label == "factory") {
        _sbm_ = SBM_FACTORY;
      } else {
        DT_THROW(std::logic_error,
                 "Invalid label for shape build mode ('" << sbm_label << "') property in simple shaped model '" << get_name() << "' !");
      }
    }
    if (_sbm_ == SBM_INVALID) {
      // Default shape build mode:
      _sbm_ = SBM_DEFAULT;
    }

    // Set the logical name:
    grab_logical().set_name(i_model::make_logical_volume_name(get_name()));

    // Parsing shape:

    if (_sbm_ == SBM_FACTORY) {
      // Solid built from the referenced shape factory:
      DT_THROW_IF(!has_shape_factory(), std::logic_error, "Missing shape factory!");
      /*
       *  shape_build_mode : string = "factory"
       *
       *  shapes.names     : string[3] = "A" "B" "C"
       *
       *  shapes.shape_type.A : string = "geomtools::box"
       *  shapes.params.A.x       : real as length = 3.2 mm
       *  shapes.params.A.y       : real as length = 1.6 mm
       *  shapes.params.A.z       : real as length = 0.8 mm
       *
       *  shapes.shape_type.B : string = "geomtools::cylinder"
       *  shapes.params.B.r       : real as length = 0.75 mm
       *  shapes.params.B.z       : real as length = 0.5 mm
       *
       *  shapes.shape_type.C : string = "geomtools::union_3d"
       *  shapes.params.C.first_shape.name       : string = "A"
       *  shapes.params.C.second_shape.name      : string = "B"
       *  shapes.params.C.second_shape.placement : string = "0.0 0.0 0.5 (mm)"
       *
       *  shape_ref : string = "C"
       *
       */
      {
        datatools::properties shapes_defs;
        config_.export_and_rename_starting_with(shapes_defs, "shapes.", "");
        grab_shape_factory().parse_shapes(shapes_defs);
      }

      if (config_.has_key("shape_ref")) {
        // Reference
        std::string shape_ref = config_.fetch_string("shape_ref");
        DT_THROW_IF(!get_shape_factory().has(shape_ref), std::logic_error,
                    "Shape factory does not contains a solid named '" << shape_ref
                    << "' for model '" << get_name() << "'!");
        _solid_.reset(dynamic_cast<i_shape_3d *>(const_cast<i_object_3d *>(&get_shape_factory().get(shape_ref))));
      } else {
        DT_THROW_IF (! config_.has_key ("shape_type"), std::logic_error,
                     "Missing 'shape_type' property in simple shaped model '" << get_name() << "' !");
        _shape_type_id_ = config_.fetch_string("shape_type");
        datatools::properties shape_config;
        config_.export_and_rename_starting_with(shape_config, "shape.", "");
        std::ostringstream solid_name_oss;
        solid_name_oss << get_name() << i_model::solid_suffix();
        i_object_3d & obj3d = grab_shape_factory().create(solid_name_oss.str(), _shape_type_id_, shape_config);
        // obj3d.set_name(solid_name_oss.str());
        DT_THROW_IF(obj3d.get_dimensional() != i_object_3d::DIMENSIONAL_3,
                    std::logic_error,
                    "Object provided by the shape factory (type='" << _shape_type_id_ << "') is not a 3D-shape!");
        _solid_.reset(dynamic_cast<i_shape_3d *>(&obj3d));
      }

      // 2015-06-12, FM: Add missing material name in the logical
      grab_logical().set_material_ref(_material_name_);

    } else {
      // Legacy solid:
      DT_THROW_IF(! config_.has_key("shape_type"), std::logic_error,
                  "Missing 'shape_type' property in simple shaped model '" << get_name() << "' !");
      _shape_type_id_ = config_.fetch_string("shape_type");

      // Shape fill mode:
      _filled_mode_ = filled_utils::FILLED_NONE;
      if (config_.has_key("filled_mode")) {
        const std::string filled_mode_label = config_.fetch_string("filled_mode");
        _filled_mode_ = filled_utils::get_filled_mode(filled_mode_label);
        DT_THROW_IF(_filled_mode_ == filled_utils::FILLED_UNDEFINED,
                    std::logic_error,
                    "Invalid filled mode '" << filled_mode_label << "' property in simple shaped (tube) model '" << get_name() << "' !");
        // if (is_filled()) {
        //   DT_LOG_WARNING(get_logging_priority (),
        //                  "Filled mode '" << filled_utils::get_filled_mode_label(_filled_mode_)
        //                  << "' is not recommended unless you know what you do !");
        // }
      }

      // Label of the shape in 'filled envelope' mode:
      if (is_filled_by_envelope()) {
        DT_LOG_DEBUG(logging, "-> filled_by_envelope");
       
        if (config_.has_flag("envelope.deflated")) {
          _envelope_tolerance_.deflated_shape = true;
          DT_LOG_DEBUG(logging, "Envelope -> deflated");
        }
 
        if (config_.has_key("envelope.tolerance.r")) {
          double r_eps = config_.fetch_real_with_explicit_dimension("envelope.tolerance.r",
                                                                   "length");
          DT_THROW_IF(r_eps <= 0.0, std::domain_error,
                      "Invalid envelope radial tolerance!")
          _envelope_tolerance_.r_tolerance = r_eps;
          DT_LOG_DEBUG(logging, "Envelope -> tolerance.r=" <<  _envelope_tolerance_.r_tolerance);
        }

        if (config_.has_key("envelope.tolerance.x")) {
          double x_eps = config_.fetch_real_with_explicit_dimension("envelope.tolerance.x",
                                                                   "length");
          DT_THROW_IF(x_eps <= 0.0, std::domain_error,
                      "Invalid envelope tolerance on X-axis!")
          _envelope_tolerance_.x_tolerance = x_eps;
        }

        if (config_.has_key("envelope.tolerance.y")) {
          double y_eps = config_.fetch_real_with_explicit_dimension("envelope.tolerance.y",
                                                                   "length");
          DT_THROW_IF(y_eps <= 0.0, std::domain_error,
                      "Invalid envelope tolerance on Y-axis!")
          _envelope_tolerance_.y_tolerance = y_eps;
        }

        if (config_.has_key("envelope.tolerance.theta")) {
          double theta_eps = config_.fetch_real_with_explicit_dimension("envelope.tolerance.theta",
                                                                   "angle");
          DT_THROW_IF(theta_eps <= 0.0, std::domain_error,
                      "Invalid envelope tolerance on theta angle (colatitude)!")
          _envelope_tolerance_.theta_tolerance = theta_eps;
        }

        if (config_.has_key("envelope.tolerance.phi")) {
          double phi_eps = config_.fetch_real_with_explicit_dimension("envelope.tolerance.phi",
                                                                   "angle");
          DT_THROW_IF(phi_eps <= 0.0, std::domain_error,
                      "Invalid envelope tolerance on phi angle (longitude)!")
          _envelope_tolerance_.phi_tolerance = phi_eps;
        }

        if (config_.has_key("envelope.tolerance.z")) {
          double z_eps = config_.fetch_real_with_explicit_dimension("envelope.tolerance.z",
                                                                   "length");
          DT_THROW_IF(z_eps <= 0.0, std::domain_error,
                      "Invalid envelope tolerance on Z-axis!")
          _envelope_tolerance_.z_tolerance = z_eps;
          DT_LOG_DEBUG(logging, "Envelope -> tolerance.z=" <<  _envelope_tolerance_.z_tolerance);
        }
        
        _filled_label_ = _shape_type_id_ + "_by_envelope";
        if (config_.has_key("filled_label")) {
          _filled_label_ = config_.fetch_string("filled_label");
        }
        DT_THROW_IF(_filled_label_.empty(),
                     std::logic_error,
                     "Invalid filled label '" << _filled_label_ << "' property in simple shaped (tube) model '" << get_name() << "' !");
        DT_LOG_DEBUG(get_logging_priority(), "Filled label is '" << _filled_label_ << "'!");
      }

      // Filling material:
      if (is_filled()){
        // Parsing material:
        DT_THROW_IF (! config_.has_key ("material.filled.ref"),
                     std::logic_error,
                     "Missing 'material.filled.ref' property in simple shaped (tube) model '" << get_name() << "' !");
        _filled_material_name_ = config_.fetch_string ("material.filled.ref");
      }

      if (get_shape_type_id () == "box") {
        // Box case:
        _construct_box (config_, models_);
      } else if (get_shape_type_id () == "cylinder") {
        // Cylinder case:
        _construct_cylinder (config_, models_);
      } else if (get_shape_type_id () == "sphere") {
        // Sphere case:
        _construct_sphere (config_, models_);
      } else if (get_shape_type_id () == "tube") {
        // Tube case:
        _construct_tube (config_, models_);
      } else if (get_shape_type_id () == "polycone") {
        // Polycone case:
        _construct_polycone (config_, models_);
      } else if (get_shape_type_id () == "polyhedra") {
        // Polyhedra case:
        _construct_polyhedra (config_, models_);
      } else {
        DT_THROW_IF (true, std::logic_error, "Shape '" << get_shape_type_id () << "' is not supported in simple shaped model '" << get_name() << "' !");
      }
    }

    if (!_solid_->is_locked()) {
      _solid_->lock();
    }

    // Set the envelope solid shape:
    grab_logical().set_shape(*_solid_);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void simple_shaped_model::_construct_box (const datatools::properties & config_,
                                            models_col_type * /*models_*/)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Construct box named '" << get_name() << "'");
    // Build the box:
    _box_.reset(new box);
    try {
      DT_LOG_DEBUG(get_logging_priority(), "Initialize box from config: ");
      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        config_.tree_dump(std::cerr, "", "[debug] ");
      }
      _box_->initialize(config_);
      DT_LOG_DEBUG(get_logging_priority(), "Box is initialized.");
      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        _box_->tree_dump(std::cerr, "Box: ", "[debug] ");
      }
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
      _box_.reset();
      throw error;
    }
    DT_THROW_IF (! _box_->is_valid(), std::logic_error,
                 "Invalid box parameters in simple shaped (box) model '" << get_name() << "' !");
    DT_LOG_DEBUG(get_logging_priority(), "Box validity: " << _box_->is_valid());
    _solid_ = _box_;
    grab_logical().set_material_ref(_material_name_);
    return;
  }

  void simple_shaped_model::_construct_cylinder (const datatools::properties & config_,
                                                 models_col_type * /*models_*/)
  {
    // Build the cylinder:
    _cylinder_.reset(new cylinder);
    try {
      _cylinder_->initialize(config_);
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
      _cylinder_.reset();
      throw error;
    }
    DT_THROW_IF (! _cylinder_->is_valid(), std::logic_error,
                 "Invalid cylinder dimensions in simple shaped (cylinder) model '" << get_name() << "'");
    _solid_ = _cylinder_;
    grab_logical().set_material_ref(_material_name_);

    return;
  }

  void simple_shaped_model::_construct_sphere (const datatools::properties & config_,
                                               models_col_type* /*models_*/)
  {
    // Build the sphere:
    _sphere_.reset(new sphere);
    try {
      _sphere_->initialize(config_);
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
      _sphere_.reset();
      throw error;
    }
    DT_THROW_IF (! _sphere_->is_valid(), std::logic_error,
                 "Invalid sphere parameters in simple shaped (sphere) model '" << get_name() << "' !");
    _solid_ = _sphere_;
    grab_logical().set_material_ref(_material_name_);

    return;
  }

  void simple_shaped_model::_construct_tube (const datatools::properties & config_,
                                             models_col_type* models_)
  {
    // Build the tube:
    _tube_.reset(new tube);
    try {
      _tube_->initialize(config_);
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
      _tube_.reset();
      throw error;
    }

    DT_THROW_IF (! _tube_->is_valid(), std::logic_error,
                 "Invalid tube dimensions in simple shaped (tube) model '" << get_name() << "' !");

    // Use the plain tube as solid envelope of the model:
    if (_filled_mode_ == filled_utils::FILLED_NONE) {
      _solid_ = _tube_;
      grab_logical().set_material_ref(_material_name_);
    }

    DT_THROW_IF(_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION,
                std::logic_error,
                "No support for tube construction 'by extrusion' in simple shaped (tube) model '" << get_name() << "' !");

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
    DT_THROW_IF (! _cylinder_->is_valid (), std::logic_error, "Invalid 'outer' cylinder dimensions in simple shaped (tube) model '" << get_name() << "' !");
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
    "Invalid 'inner' cylinder dimensions in simple shaped (tube) model '" << get_name() << "' !");
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
      // std::cerr << "DEVEL: geomtools::simple_shaped_model::_construct_tube: FILLED_BY_ENVELOPE" << std::endl;
      double r_eps = 0.0;
      if (_envelope_tolerance_.has_r_tolerance()) {
        r_eps = _envelope_tolerance_.r_tolerance;
      }
      double z_eps = 0.0;
      if (_envelope_tolerance_.has_z_tolerance()) {
        z_eps = _envelope_tolerance_.z_tolerance;
      }
      double in_r  = datatools::invalid_real();
      double out_r = _tube_->get_outer_r();
      double z     = _tube_->get_z();
      // Make the envelope a cylinder:
      _cylinder_.reset(new cylinder);
      if (_envelope_tolerance_.deflated_shape) {
        std::shared_ptr<tube> deflated_tube;
        if (_tube_->has_inner_r()) {
          in_r = _tube_->get_inner_r();
          deflated_tube.reset(new tube(in_r, out_r - r_eps, z - 2 * z_eps));
        } else {
          deflated_tube.reset(new tube(out_r - r_eps, z - 2 * z_eps));
        }
        _tube_->compute_outer_cylinder(*_cylinder_);
        // Replace the original tube by the deflated one:
        _tube_ = deflated_tube;
      } else {
        // Build the inflated cylinder envelope:
        _tube_->compute_envelope(*_cylinder_, r_eps, z_eps);
        DT_THROW_IF (! _cylinder_->is_valid(), std::logic_error,
                     "Invalid 'outer' cylinder dimensions in simple shaped (tube) model '" << get_name() << "' !");
      }
      _solid_ = _cylinder_;
      grab_logical().set_material_ref(_filled_material_name_);
      // If the tube is extruded, add the tube within the 'outer' envelope cylinder:
      _inner_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      std::string model_basename = extract_basename_from_model_name(get_name());
      const std::string inner_name = "__" + model_basename + "." + _filled_label_;
      _inner_model_.set_solid(*_tube_);
      _inner_model_.set_material_name(_material_name_);
      _inner_model_.construct(inner_name, models_);
      _inner_phys_.set_name(i_model::make_physical_volume_name(_filled_label_));
      _inner_phys_.set_placement(_inner_placement_);
      _inner_phys_.set_logical(_inner_model_.get_logical());
      _inner_phys_.set_mother(this->get_logical());
    }
    return;
  }

  void simple_shaped_model::_construct_polycone (const datatools::properties & config_,
                                                 models_col_type * models_)
  {
    // Build the polycone:
    _polycone_.reset(new polycone);
    try {
      _polycone_->initialize(config_);
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
      _polycone_.reset();
      throw error;
    }
    DT_THROW_IF (! _polycone_->is_valid (), std::logic_error,
                 "Invalid polycone build parameters in simple shaped (polycone) model '" << get_name() << "' !");

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
      "No support for polycone construction 'by extrusion' in simple shaped (polycone) model '" << get_name() << "' !");

      // Build the polycone model by extrusion of a mother polycone:
      if (_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION) {
      // Make the envelope a polycone:
      polycone * envelope_polycone = new polycone;
      _polycone_->compute_outer_polycone (*envelope_polycone);
      DT_THROW_IF (! envelope_polycone->is_valid (), std::logic_error,
      "Invalid envelope polycone in simple shaped (polycone) model '" << get_name() << "' !");
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
      "Invalid 'inner' polycone dimensions in simple shaped (polycone) model '" << get_name() << "' !");
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
      double r_eps = 0.0;
      if (_envelope_tolerance_.has_r_tolerance()) {
        r_eps = _envelope_tolerance_.r_tolerance;
      }
      double z_eps = 0.0;
      if (_envelope_tolerance_.has_z_tolerance()) {
        z_eps = _envelope_tolerance_.z_tolerance;
      }
      double theta_eps = 0.0;
      if (_envelope_tolerance_.has_theta_tolerance()) {
        theta_eps = _envelope_tolerance_.theta_tolerance;
      }

      // Make the envelope a polycone:
      polycone outer_polycone;
      _polycone_->compute_outer_polycone(outer_polycone);
      std::shared_ptr<polycone> envelope_polycone(new polycone);
      if ((r_eps or z_eps) and _envelope_tolerance_.deflated_shape) {
        outer_polycone.compute_envelope(*envelope_polycone, 0.0, 0.0, 0.0);
        std::shared_ptr<polycone> deflated_polycone(new polycone);
        _polycone_->compute_deflated(*deflated_polycone, r_eps, z_eps, theta_eps);
        // Replace the original polycone by the deflated one:
        _polycone_ = deflated_polycone;
      } else {
        outer_polycone.compute_envelope(*envelope_polycone, r_eps, z_eps, theta_eps);
      }
      _outer_shape_ = envelope_polycone;
      _solid_ = _outer_shape_;
      grab_logical ().set_material_ref (_filled_material_name_);
      // If the polycone is extruded, add the 'inner' polycone
      // within the 'outer' envelope polycone:
      _inner_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      std::string model_basename = extract_basename_from_model_name(get_name());
      const std::string inner_name = "__" + model_basename + "." + _filled_label_;
      _inner_model_.set_solid(*_polycone_);
      _inner_model_.set_material_name(_material_name_);
      _inner_model_.construct(inner_name, models_);
      
      _inner_phys_.set_name (i_model::make_physical_volume_name (_filled_label_));
      _inner_phys_.set_placement (_inner_placement_);
      _inner_phys_.set_logical (_inner_model_.get_logical());
      _inner_phys_.set_mother (this->get_logical ());
    }

    return;
  }


  void simple_shaped_model::_construct_polyhedra (const datatools::properties & config_,
                                                  models_col_type* models_)
  {
    _polyhedra_.reset(new polyhedra);
    try {
      _polyhedra_->initialize(config_);
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
      _polyhedra_.reset();
      throw error;
    }
    _polyhedra_->initialize (config_);
    DT_THROW_IF (! _polyhedra_->is_valid (), std::logic_error,
                 "Invalid polyhedra build parameters in simple shaped (polyhedra) model '" << get_name() << "' !");

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
                "No support for polyhedra construction 'by extrusion' in simple shaped (polyhedra) model '" << get_name() << "' !");

    // build the polyhedra by extrusion of a mother polyhedra:
    /*
      if (_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION)  {
      // make the envelope a polyhedra:
      polyhedra * envelope_polyhedra = new polyhedra;
      _polyhedra_->compute_outer_polyhedra (*envelope_polyhedra);
      DT_THROW_IF (! envelope_polyhedra->is_valid (), std::logic_error, "Invalid envelope polyhedra in simple shaped (polyhedra) model '" << get_name() << "' !");
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
      DT_THROW_IF (true, std::logic_error, "Invalid 'inner' polyhedra dimensions in simple shaped (polyhedra) model '" << get_name() << "' !");
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
      double r_eps = 0.0;
      if (_envelope_tolerance_.has_r_tolerance()) {
        r_eps = _envelope_tolerance_.r_tolerance;
      }
      double z_eps = 0.0;
      if (_envelope_tolerance_.has_z_tolerance()) {
        z_eps = _envelope_tolerance_.z_tolerance;
      }

      // Make the envelope a polyhedra:
      polyhedra outer_polyhedra;
      _polyhedra_->compute_outer_polyhedra(outer_polyhedra);
      std::shared_ptr<polyhedra> envelope_polyhedra(new polyhedra);
      if ((r_eps or z_eps) and _envelope_tolerance_.deflated_shape) {
        outer_polyhedra.compute_envelope(*envelope_polyhedra, 0.0, 0.0);
        std::shared_ptr<polyhedra> deflated_polyhedra(new polyhedra);
        _polyhedra_->compute_deflated(*deflated_polyhedra, r_eps, z_eps);
        // Replace the original polyhedra by the deflated one:
        _polyhedra_ = deflated_polyhedra;
      } else {
        outer_polyhedra.compute_envelope(*envelope_polyhedra, r_eps, z_eps);
      }
      _outer_shape_ = envelope_polyhedra;
      _solid_ = _outer_shape_;
      grab_logical ().set_material_ref (_filled_material_name_);
      // If the polycone is extruded, add the 'inner' polycone
      // within the 'outer' envelope polycone:
      _inner_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      std::string model_basename = extract_basename_from_model_name(get_name());
      const std::string inner_name = "__" + model_basename + "." + _filled_label_;
      _inner_model_.set_solid(*_polyhedra_);
      _inner_model_.set_material_name(_material_name_);
      _inner_model_.construct(inner_name, models_);
      
      _inner_phys_.set_name (i_model::make_physical_volume_name (_filled_label_));
      _inner_phys_.set_placement (_inner_placement_);
      _inner_phys_.set_logical (_inner_model_.get_logical());
      _inner_phys_.set_mother (this->get_logical ());
    }

    return;
  }

  void simple_shaped_model::_pre_construct (datatools::properties & setup_,
                                            models_col_type * /*models_*/)
  {
    datatools::logger::priority log_level = get_logging_priority ();
    //log_level = datatools::logger::PRIO_DEBUG;
    // this->i_model::_pre_construct(setup_, models_);

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
        // Add "internal_item." as exported property prefix:
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
    // std::cerr << "DEVEL: simple_shaped_model::_post_construct: "
    //           << "Entering model '" << get_name() << "'"
    //           << std::endl;
    datatools::logger::priority log_level = get_logging_priority();
    //log_level = datatools::logger::PRIO_DEBUG;
    DT_LOG_NOTICE(get_logging_priority (),
                  "Post-construction processing for model '" << get_name() << "' ...");
    if (! is_filled()) {
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
          _inner_model_.grab_logical().grab_parameters().set_description(log_params_desc.str());
        }
        std::vector<std::string> exported_prefixes;
        if (setup_.has_key(i_model::exported_properties_prefixes_key())) {
          setup_.fetch(i_model::exported_properties_prefixes_key(),
                       exported_prefixes);
        }
        for (size_t i = 0; i < exported_prefixes.size(); i++) {
          const std::string & topic_prefix = exported_prefixes[i];
          const std::string filled_topic_prefix = topic_prefix + "filled.";
          DT_LOG_NOTICE(get_logging_priority(),
                        "Reprocessing '" << topic_prefix << "' properties for model '" << get_name() << "' ...");
          // All (top+filled) properties :
          datatools::properties tmp_params;
          setup_.export_starting_with(tmp_params, topic_prefix);
          // Only properties for the 'filled' volume :
          datatools::properties filled_params;
          tmp_params.export_starting_with(filled_params, filled_topic_prefix);
          // Only 'top' properties :
          tmp_params.erase_all_starting_with(filled_topic_prefix);
          // Inner logical inherits the 'top' properties :
          tmp_params.export_starting_with(_inner_model_.grab_logical().grab_parameters(), topic_prefix);
          // Top logical inherits the 'filled' properties :
          grab_logical().grab_parameters().erase_all_starting_with(filled_topic_prefix);
          grab_logical().grab_parameters().erase_all_starting_with(topic_prefix);
          filled_params.export_and_rename_starting_with(grab_logical().grab_parameters(), filled_topic_prefix, topic_prefix);
        }
      } // is_filled_by_envelope()
    } // Filled mode
    grab_logical().set_geometry_model(*this);

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
        _internals_.plug_internal_models(_inner_model_.get_logical().get_parameters(),
                                         _inner_model_.grab_logical(),
                                         models_);
        DT_LOG_DEBUG (log_level, "Number of items in shape : "
                      << _internals_.get_number_of_items());
      }
      // _inner_logical_.lock();
    } else {
      DT_LOG_DEBUG (log_level, "Processing internal items (none)...");
      // Top logical is the shape :
      DT_LOG_DEBUG (log_level, "Processing internal items in shape...");
      DT_LOG_DEBUG (log_level, "Logical parameters for model '"
                    << get_name() << "' : ");
      if (log_level >= datatools::logger::PRIO_DEBUG) {
        get_logical().get_parameters().tree_dump(std::cerr, "");
      }
      // std::cerr <<
      //   "*********** Model '" <<  get_name() << "' : plug internal items... *************"
      //                         << std::endl;
      // get_logical().get_parameters().tree_dump(std::cerr, "logical parameters:", "****** ");
      _internals_.plug_internal_models(get_logical().get_parameters(),
                                       grab_logical(),
                                       models_);
      // std::cerr <<
      //   "*********** Model '" <<  get_name() << "' : plug internal items done *************"
      //                         << std::endl;
    }

    return;
  }

  void simple_shaped_model::tree_dump (std::ostream & out_,
                                       const std::string & title_ ,
                                       const std::string & indent_,
                                       bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    // Shape build mode:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Shape build mode : '";
      if (get_shape_build_mode() == SBM_LEGACY) {
        out_ << "legacy";
      } else if (get_shape_build_mode() == SBM_FACTORY) {
        out_ << "factory";
      } else {
        out_ << "<none>";
      }
      out_ << "'" << std::endl;
    }

    // Shape type ID:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Shape type ID : '" << get_shape_type_id () << "'" << std::endl;
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
             << "Filled material name : '" << get_filled_material_name() << "'" << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Inner logical : " << std::endl;
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::skip_tag;
      _inner_model_.get_logical().tree_dump(out_, "",indent_oss.str ());

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Number of internal items within cavity: '" << _filled_internals_.get_number_of_items() << "'" << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Number of internal items : '" << _internals_.get_number_of_items() << "'" << std::endl;

    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(simple_shaped_model,"geomtools::simple_shaped_model")

} // end of namespace geomtools

// OCD support for class '::geomtools::simple_shaped_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::simple_shaped_model, ocd_)
{
  ocd_.set_class_name("geomtools::simple_shaped_model");
  ocd_.set_class_description("A geometry model implementing a simple 3D shape");
  ocd_.set_class_library("geomtools");
  ocd_.set_class_description("This very useful geometry model is able to implement \n"
                             "volumes with various simple 3D shapes. The following \n"
                             "shapes are supported in *legacy* mode:                       \n"
                             "                                                             \n"
                             " * Box : see OCD support for class ``geomtool::box``.        \n"
                             " * Cylinder: see OCD support for class ``geomtool::cylinder``\n"
                             " * Tube : see OCD support for class ``geomtool::tube``       \n"
                             " * Sphere : see OCD support for class ``geomtool::sphere``.  \n"
                             " * Polycone                                          \n"
                             " * Polyhedra                                         \n"
                             "                                                     \n"
                             "In *factory* mode, any solid 3D shape registered in  \n"
                             "shape system factory can be instantiated.            \n"
                             "                                                     \n"
                             "The generated volume can also host daughter volumes, \n"
                             "thanks to the 'internal items' interface.            \n"
                             );

  // Inherit parent class/interface OCD support:
  geomtools::i_model::init_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("shape_build_mode")
      .set_terse_description("The build mode for the 3D shape implemented within the geometry model")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property specifies the solid shape build mode.          \n"
                            "Allowed values are.                                          \n"
                            "                                                             \n"
                            " * ``\"legacy\"`` : use hardcoded method for building solids \n"
                            " * ``\"factory\"`` : use a factory of registered solids      \n"
                            "   This mode needs a shape factory to be referenced by the   \n"
                            "   geometry model.                                           \n"
                            "                                                             \n"
                            )
      .add_example("Use the legacy mode: ::                           \n"
                   "                                                  \n"
                   " #@description The shape build mode               \n"
                   " shape_build_mode : string = \"legacy\"           \n"
                   "                                                  \n"
                   )
      .add_example("Use the factory mode: ::                          \n"
                   "                                                  \n"
                   " #@description The shape build mode               \n"
                   " shape_build_mode : string = \"factory\"          \n"
                   "                                                  \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("shape_type")
      .set_terse_description("The name of the 3D shape implemented within the geometry model")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property specifies the type of solid shape.  \n"
                            "For the *legacy* mode, allowed values are:        \n"
                            "                                                  \n"
                            " * ``\"box\"`` : a 3D box                         \n"
                            " * ``\"cylinder\"`` : a cylinder                  \n"
                            " * ``\"tube\"`` : a tube                          \n"
                            " * ``\"sphere\"`` : a spherical section           \n"
                            " * ``\"polycone\"`` : a polycone                  \n"
                            " * ``\"polyhedra\"`` : a polyhedra                \n"
                            "                                                  \n"
                            "Depending on the requested shape, additional      \n"
                            "configuration properties can/must be used to      \n"
                            "setup the logical volume(s) associated to this    \n"
                            "model. You may read the specific OCD support      \n"
                            "for each shape (if available).                    \n"
                            "                                                  \n"
                            "For the *legacy* mode, allowed values are:        \n"
                            "                                                  \n"
                            " * ``\"geomtools::box\"`` : 3D box                \n"
                            " * ``\"geomtools::cylinder\"`` : cylinder         \n"
                            " * any shape type identifier registered in the    \n"
                            "   shape system factory.                          \n"
                            "                                                  \n"
                           )
      .add_example("Use a box: ::                                     \n"
                   "                                                  \n"
                   " #@description The shape build mode               \n"
                   " shape_build_mode : string = \"legacy\"           \n"
                   " #@description The box type                       \n"
                   " shape_type  : string = \"box\"                   \n"
                   " #@description The default length unit            \n"
                   " length_unit : string = \"cm\"                    \n"
                   " #@description The X dimension of the box         \n"
                   " x           : real as length = 100.0 mm          \n"
                   " #@description The Y dimension of the box         \n"
                   " y           : real as length = 5.1 # default unit\n"
                   " #@description The Z dimension of the box         \n"
                   " z           : real as length = 2.4 # default unit\n"
                   "                                                  \n"
                   )
      .add_example("Use a cylinder ::                                 \n"
                   "                                                  \n"
                   " #@description The shape build mode               \n"
                   " shape_build_mode : string = \"legacy\"           \n"
                   " #@description The box type                       \n"
                   " shape_type  : string = \"cylinder\"              \n"
                   " #@description The default length unit            \n"
                   " length_unit : string = \"cm\"                    \n"
                   " #@description The radius of the cylinder         \n"
                   " r           : real as length = 53.3 mm           \n"
                   " #@description The height of the cylinder         \n"
                   " z           : real as length = 2.4 # default unit\n"
                   "                                                  \n"
                   )
      .add_example("Use a spherical section ::                        \n"
                   "                                                  \n"
                   " #@description The shape build mode               \n"
                   " shape_build_mode : string = \"legacy\"           \n"
                   " #@description The box type                       \n"
                   " shape_type  : string = \"sphere\"                \n"
                   " #@description The default length unit            \n"
                   " length_unit : string = \"mm\"                    \n"
                   " #@description The default angle unit             \n"
                   " angle_unit  : string = \"degree\"                \n"
                   " #@description The outer radius of the spherical section\n"
                   " r_max       : real as length = 200.0 mm                \n"
                   " #@description The inner radius of the spherical section\n"
                   " r_min       : real as length = 125.0 mm                \n"
                   " #@description The starting phi angle                   \n"
                   " start_phi   : real as angle= 30.0 degree               \n"
                   " #@description The delta phi angle                      \n"
                   " delta_phi   : real as angle= 60.0 degree               \n"
                   " #@description The starting theta angle                 \n"
                   " start_theta   : real as angle= 20.0 degree             \n"
                   " #@description The delta theta angle                    \n"
                   " delta_theta   : real as angle= 70.0 degree             \n"
                   "                                                        \n"
                   )
      .add_example("Use a tube with (no filled mode)::                \n"
                   "                                                  \n"
                   " #@description The shape build mode               \n"
                   " shape_build_mode : string = \"legacy\"           \n"
                   " #@description The tube type                      \n"
                   " shape_type  : string = \"tube\"                  \n"
                   " #@description The default length unit            \n"
                   " length_unit : string = \"cm\"                    \n"
                   " #@description The inner radius of the tube       \n"
                   " inner_r     : real as length = 24.3 mm           \n"
                   " #@description The outer radius of the tube       \n"
                   " outer_r     : real as length = 53.3 mm           \n"
                   " #@description The height of the tube             \n"
                   " z           : real as length = 5.3 # default unit\n"
                   "                                                  \n"
                   )
      .add_example("Use a polycone (no filled mode)::                        \n"
                   "                                                         \n"
                   " #@description The shape build mode                      \n"
                   " shape_build_mode : string = \"legacy\"                  \n"
                   " #@description The tube type                             \n"
                   " shape_type  : string = \"polycone\"                     \n"
                   " #@description The default length unit                   \n"
                   " length_unit : string = \"cm\"                           \n"
                   " #@description The build mode                            \n"
                   " build_mode  : string = \"points\"                       \n"
                   " #@description The Z positions of three polycone sections\n"
                   " list_of_z   : real[4] in mm = -30.0 -15.0 5.0 23.0      \n"
                   " #@description The outer radius of the polycone sections \n"
                   " list_of_rmax : real[4] in mm = 10.0 15.0 20.0 20.0      \n"
                   " #@description The inner radius of the polycone sections \n"
                   " list_of_rmin : real[4] in mm =  5.0  5.0 10.0 10.0      \n"
                   "                                                         \n"
                   )
      .add_example("Use a polyhedra (no filled mode)::                        \n"
                   "                                                          \n"
                   " #@description The shape build mode                       \n"
                   " shape_build_mode : string = \"legacy\"                   \n"
                   " #@description The tube type                              \n"
                   " shape_type  : string = \"polyhedra\"                     \n"
                   " #@description The default length unit                    \n"
                   " length_unit : string = \"cm\"                            \n"
                   " #@description The build mode                             \n"
                   " build_mode  : string = \"points\"                        \n"
                   " #@description The number of sides of the polyhedra       \n"
                   " sides       : integer        = 4                         \n"
                   " #@description The Z positions of three polyhedra sections\n"
                   " list_of_z   : real[4] in mm  = -30.0 -15.0 5.0 23.0      \n"
                   " #@description The outer radius of the polyhedra sections \n"
                   " list_of_rmax : real[4] in mm =  10.0 15.0 20.0 20.0      \n"
                   " #@description The inner radius of the polyhedra sections \n"
                   " list_of_rmin : real[4] in mm =   5.0  5.0 10.0 10.0      \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("filled_mode")
      .set_terse_description("The mode for filling shapes with an excavation")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This property specifies the way one must build the    \n"
                            "requested shape when it has a natural excavation/hole \n"
                            "in it (tube, polycone, polyhedra, in the *legacy* mode). \n"
                            "Allowed values are.                                   \n"
                            "                                                                  \n"
                            " * ``\"none\"`` : Nothing special to do (default value).          \n"
                            " * ``\"by_envelope\"`` : A two step build mode impling a          \n"
                            "   mother volume used as the envelope of the requested shape:     \n"
                            "                                                                  \n"
                            "    * for a tube: the model is implemented                        \n"
                            "      with a cylinder mother/envelope volume filled with its own material  \n"
                            "      and the requested tube shape itself is placed within        \n"
                            "      this mother shape with its own material.                    \n"
                            "    * for a polycone: the model is implemented                    \n"
                            "      with a polycone mother volume (without a hole) filled       \n"
                            "      with its own material. The requested polycone shape itself  \n"
                            "      is placed within this mother shape with its own material.   \n"
                            "    * for a polyhedra: the model is implemented                   \n"
                            "      with a polyhedra mother volume (without a hole) filled      \n"
                            "      with its own material. The requested polyhedra shape itself \n"
                            "      is placed within this mother shape with its own material.   \n"
                             "                                                                 \n"
                            "This property must be used by experts only because                \n"
                            "it may be rather difficult to understand and manipulate.          \n"
                            "                                                                  \n"
                            )
      .add_example("This example shows how to construct a tube using the *by envelope* mode.          \n"
                   "Additional properties must/may be used, like the mandatory ``material.filled.ref``\n"
                   "property that specify the material used within the central hole: ::               \n"
                   "                                                  \n"
                   " #@description The tube type                      \n"
                   " shape_type  : string = \"tube\"                  \n"
                   " #@description The default length unit            \n"
                   " length_unit : string = \"cm\"                    \n"
                   " #@description The inner radius of the tube       \n"
                   " inner_r     : real as length = 24.3 mm           \n"
                   " #@description The outer radius of the tube       \n"
                   " outer_r     : real as length = 53.3 mm           \n"
                   " #@description The height of the tube             \n"
                   " z           : real as length = 5.3 # default unit\n"
                   " #@description The filled mode                       \n"
                   " filled_mode         : string = \"by_envelope\"      \n"
                   " filled_label        : string = \"hole\"             \n"
                   " material.ref        : string = \"Iron\"             \n"
                   " material.filled.ref : string = \"Air\"              \n"
                   "                                                     \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("filled_label")
      .set_terse_description("The label used for the embedded volume in *by envelope* mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("filled_mode", "by_envelope")
      .set_default_value_string("by_envelope")
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("material.ref")
      .set_terse_description("The label of the material the mother volume is made of")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .add_example("Using 'Mylar': ::                              \n"
                   "                                               \n"
                   "   material.ref : string = \"Iron\"            \n"
                   "                                               \n"
                   )
      ;
  }

  {
    datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("material.filled.ref")
      .set_terse_description("The label of the material the excavated volume is made of in *by envelope* mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_triggered_by_label("filled_mode", "by_envelope")
      .add_example("Using 'Air': ::                                \n"
                   "                                               \n"
                   "   material.filled.ref : string = \"Air\"      \n"
                   "                                               \n"
                   )
      ;
  }

  // Add support for internal/daughter volumes:
  geomtools::MWIM::init_ocd(ocd_);

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::simple_shaped_model,
                               "geomtools::simple_shaped_model")
