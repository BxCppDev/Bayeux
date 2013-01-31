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

  using namespace std;

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

  MWIM & simple_shaped_model::get_internals ()
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

  const string & simple_shaped_model::get_shape_name () const
  {
    return _shape_name_;
  }

  const string & simple_shaped_model::get_material_name () const
  {
    return _material_name_;
  }

  const string & simple_shaped_model::get_filled_material_name () const
  {
    return _filled_material_name_;
  }

  string simple_shaped_model::get_model_id () const
  {
    return "geomtools::simple_shaped_model";
  }

  // ctor:
  simple_shaped_model::simple_shaped_model () : i_model ()
  {
    _box_ = 0;
    _cylinder_ = 0;
    _tube_ = 0;
    _sphere_ = 0;
    _polycone_ = 0;
    _solid_ = 0;
    _inner_shape_ = 0;
    _outer_shape_ = 0;
    _daughter_owner_logical_ = 0;
    _visibility_logical_ = 0;
    _shape_name_ = ""; // no defined shape
    _material_name_ = ""; //material::constants::instance ().MATERIAL_REF_UNKNOWN;
    _filled_material_name_ = ""; //material::constants::instance ().MATERIAL_REF_UNKNOWN;
    _filled_mode_   = filled_utils::FILLED_NONE;
    return;
  }

  // dtor:
  simple_shaped_model::~simple_shaped_model ()
  {
    _solid_ = 0;
    if (_inner_shape_ != 0) delete _inner_shape_;
    if (_outer_shape_ != 0) delete _outer_shape_;
    if (_box_ != 0) delete _box_;
    if (_cylinder_ != 0) delete _cylinder_;
    if (_tube_ != 0) delete _tube_;
    if (_sphere_ != 0) delete _sphere_;
    if (_polycone_ != 0) delete _polycone_;
    return;
  }

  void simple_shaped_model::_at_construct (const string & name_,
                                           const datatools::properties & config_,
                                           models_col_type * models_)
  {
    bool devel = i_model::g_devel;
    if (config_.has_key ("devel"))
      {
        devel = true;
      }
    if (devel) clog << "DEVEL: simple_shaped_model::_at_construct: Entering..." << endl;
    set_name (name_);
    string shape_type = "";

    // Initialization:
    _material_name_ = material::constants::instance ().MATERIAL_REF_UNKNOWN;
    _filled_material_name_ = material::constants::instance ().MATERIAL_REF_UNKNOWN;

    // parsing shape:
    if (config_.has_key ("shape_type"))
      {
        shape_type = config_.fetch_string ("shape_type");
        _shape_name_ = shape_type;
      }
    else
      {
        ostringstream message;
        message << "simple_shaped_model::_at_construct: "
                << "Missing 'shape_type' property !";
        throw logic_error (message.str ());
      }

    // parsing material:
    if (config_.has_key ("material.ref"))
      {
        _material_name_ = config_.fetch_string ("material.ref");
      }
    else
      {
        ostringstream message;
        message << "simple_shaped_model::_at_construct: "
                << "Missing 'material.ref' property !";
        throw logic_error (message.str ());
      }

    // makes the embeded logical volume the default
    // mother of daughter physical volumes:
    _daughter_owner_logical_ = & get_logical ();

    // set the logical name:
    get_logical ().set_name (i_model::make_logical_volume_name (name_));

    // Box case:
    if (shape_type == "box")
      {
        _construct_box (name_, config_, models_);
      }
    // Cylinder case:
    else if (shape_type == "cylinder")
      {
        _construct_cylinder (name_, config_, models_);
      }
    // Sphere case:
    else if (shape_type == "sphere")
      {
        _construct_sphere (name_, config_, models_);
      }
    // Tube case:
    else if (shape_type == "tube")
      {
        _construct_tube (name_, config_, models_);
      }
    // Polycone case:
    else if (shape_type == "polycone")
      {
        _construct_polycone (name_, config_, models_);
      }
    // Polyhedra case:
    else if (shape_type == "polyhedra")
      {
        _construct_polyhedra (name_, config_, models_);
      }
    else
      {
        ostringstream message;
        message << "simple_shaped_model::_at_construct: "
                << "Shape '" << shape_type << "' is not supported !";
        throw logic_error (message.str ());
      }

    // set the envelope solid shape:
    get_logical ().set_shape (*_solid_);

    // search for internal item to install within the model envelope:
    /**
     // 2011-11-21 FM : was
    if (_daughter_owner_logical_ != 0)
    */
     // 2011-11-21 FM : try this to forbid a second plug attempt...
    if (_daughter_owner_logical_ != 0 && _internals_.get_number_of_items () == 0)
      {
        //clog << "DEVEL: simple_shaped_model::_at_construct: _daughter_owner_logical_ && plug_internal_models..." << endl;
        _internals_.plug_internal_models (config_,
                                          *_daughter_owner_logical_,
                                          models_);
      }

    if (devel) clog << "DEVEL: simple_shaped_model::_at_construct: Exiting." << endl;
    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_box (const string & name_,
                                            const datatools::properties & config_,
                                            models_col_type * models_)
  {
    double lunit = CLHEP::mm;
    double x, y, z;

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("x"))
      {
        x = config_.fetch_real ("x");
        x *= lunit;
      }
    else
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_box: "
                << "Missing box 'x' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("y"))
      {
        y = config_.fetch_real ("y");
        y *= lunit;
      }
    else
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_box: "
                << "Missing box 'y' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("z"))
      {
        z = config_.fetch_real ("z");
        z *= lunit;
      }
    else
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_box: "
                << "Missing box 'z' property !";
        throw logic_error (message.str ());
      }

    // build the box:
    _box_ = new box (x, y, z);
    if (! _box_->is_valid ())
      {
        throw logic_error ("simple_shaped_model::_construct_box: Invalid box dimensions !");
      }
    _solid_ = _box_;
    get_logical ().set_material_ref (_material_name_);

    // search for internal item to install within the model envelope:
    //clog << "DEVEL: simple_shaped_model::_construct_box: plug_internal_models..." << endl;
    _internals_.plug_internal_models (config_, get_logical (), models_);

    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_cylinder (const string & name_,
                                                 const datatools::properties & config_,
                                                 models_col_type * models_)
  {
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    double r, z;
    datatools::invalidate (r);
    datatools::invalidate (z);

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("angle_unit"))
      {
        string aunit_str = config_.fetch_string ("angle_unit");
        aunit = datatools::units::get_angle_unit_from (aunit_str);
      }

    if (! datatools::is_valid (r))
      {
        if (config_.has_key ("r"))
          {
            r = config_.fetch_real ("r");
            r *= lunit;
          }
        else if (config_.has_key ("radius"))
          {
            r = config_.fetch_real ("radius");
            r *= lunit;
          }
        else if (config_.has_key ("diameter"))
          {
            r = 0.5 * config_.fetch_real ("diameter");
            r *= lunit;
          }
      }
    if (! datatools::is_valid (r))
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_cylinder: "
                << "Missing cylinder 'r' or 'diameter' property !";
        throw std::logic_error (message.str ());
      }

    if (config_.has_key ("z"))
      {
        z = config_.fetch_real ("z");
        z *= lunit;
      }
    else
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_cylinder: "
                << "Missing cylinder 'z' property !";
        throw logic_error (message.str ());
      }

    // build the cylinder:
    _cylinder_ = new cylinder (r, z);
    if (! _cylinder_->is_valid ())
      {
        throw logic_error ("simple_shaped_model::_construct_cylinder: Invalid cylinder dimensions !");
      }
    _solid_ = _cylinder_;
    get_logical ().set_material_ref (_material_name_);
    // search for internal item to install within the model envelope:
    //clog << "DEVEL: simple_shaped_model::_construct_cylinder: plug_internal_models..." << endl;
     _internals_.plug_internal_models (config_, get_logical (), models_);

    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_sphere (const string & name_,
                                               const datatools::properties & config_,
                                               models_col_type* models_)
  {
    string material_name = material::constants::instance ().MATERIAL_REF_UNKNOWN;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    double r;

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("angle_unit"))
      {
        string aunit_str = config_.fetch_string ("angle_unit");
        aunit = datatools::units::get_angle_unit_from (aunit_str);
      }

    if (config_.has_key ("r"))
      {
        r = config_.fetch_real ("r");
        r *= lunit;
      }
    else
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_sphere: "
                << "Missing sphere 'r' property !";
        throw logic_error (message.str ());
      }

    // build the sphere:
    _sphere_ = new sphere (r);
    if (! _sphere_->is_valid ())
      {
        throw logic_error ("simple_shaped_model::_construct_sphere: Invalid sphere dimension !");
      }
    _solid_ = _sphere_;
    get_logical ().set_material_ref (_material_name_);
    // search for internal item to install within the model envelope:
    //clog << "DEVEL: simple_shaped_model::_construct_sphere: plug_internal_models..." << endl;
    _internals_.plug_internal_models (config_, get_logical (), models_);

    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_tube (const string & name_,
                                             const datatools::properties & config_,
                                             models_col_type* models_)
  {
    string filled_material_name = material::constants::instance ().MATERIAL_REF_UNKNOWN;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    double inner_r, outer_r, z;
    datatools::invalidate (inner_r);
    datatools::invalidate (outer_r);
    datatools::invalidate (z);

    string filled_mode_label = filled_utils::FILLED_NONE_LABEL;

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("angle_unit"))
      {
        string aunit_str = config_.fetch_string ("angle_unit");
        aunit = datatools::units::get_angle_unit_from (aunit_str);
      }

    if (! datatools::is_valid (inner_r))
      {
        if (config_.has_key ("inner_r"))
          {
            inner_r = config_.fetch_real ("inner_r");
            inner_r *= lunit;
          }
        else if (config_.has_key ("inner_radius"))
          {
            inner_r = config_.fetch_real ("inner_radius");
            inner_r *= lunit;
          }
        else if (config_.has_key ("inner_diameter"))
          {
            inner_r = 0.5 * config_.fetch_real ("inner_diameter");
            inner_r *= lunit;
          }
      }
    if (! datatools::is_valid (inner_r))
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_tube: "
                << "Missing tube 'inner_r' property !";
        cerr << "WARNING: " << message.str () << " Using 0-default inner radius !" << endl;
        inner_r = 0.0;
      }


    if (! datatools::is_valid (outer_r))
      {
        if (config_.has_key ("outer_r"))
          {
            outer_r = config_.fetch_real ("outer_r");
            outer_r *= lunit;
          }
        else if (config_.has_key ("outer_radius"))
          {
            outer_r = config_.fetch_real ("outer_radius");
            outer_r *= lunit;
          }
        else if (config_.has_key ("outer_diameter"))
          {
            outer_r = 0.5 * config_.fetch_real ("outer_diameter");
            outer_r *= lunit;
          }
      }
     if (! datatools::is_valid (outer_r))
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_tube: "
                << "Missing tube 'outer_r' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("z"))
      {
        z = config_.fetch_real ("z");
        z *= lunit;
      }
    else
      {
        ostringstream message;
        message << "simple_boxed_model::_construct_tube: "
                << "Missing tube 'z' property !";
        throw logic_error (message.str ());
      }

    // filled mode:
    if (config_.has_key ("filled_mode"))
      {
        filled_mode_label = config_.fetch_string ("filled_mode");
        if (filled_mode_label == filled_utils::FILLED_NONE_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_NONE;
          }
        else if (filled_mode_label == filled_utils::FILLED_BY_ENVELOPE_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_BY_ENVELOPE;
          }
        else if (filled_mode_label == filled_utils::FILLED_BY_EXTRUSION_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_BY_EXTRUSION;
          }
        else
          {
            ostringstream message;
            message << "simple_boxed_model::_construct_tube: "
                    << "Invalid mode '" << filled_mode_label << "' property !";
            throw logic_error (message.str ());
          }
      }
    else
      {
        _filled_mode_ = filled_utils::FILLED_NONE;
      }

    // filling material:
    if (_filled_mode_ != filled_utils::FILLED_NONE)
      {
        // parsing material:
        if (config_.has_key ("material.filled.ref"))
          {
            _material_name_ = config_.fetch_string ("material.filled.ref");
          }
        else
          {
            ostringstream message;
            message << "simple_shaped_model::_construct_tube: "
                    << "Missing 'material.filled.ref' property !";
            throw logic_error (message.str ());
          }
      }

    // build the tube:
    _tube_ = new tube (inner_r, outer_r, z);
    if (! _tube_->is_valid ())
      {
        throw logic_error ("simple_shaped_model::_construct_tube: Invalid  tube dimensions !");
      }

    // use the plain tube as solid envelope of the model:
    if (_filled_mode_ == filled_utils::FILLED_NONE)
      {
        _solid_ = _tube_;
        get_logical ().set_material_ref (_material_name_);
      }

    // build the tube by cylindric extrusion of a mother cylinder:
    if (_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION)
      {
        /*
         *          _____________________
         *         |_____________________|
         *      _ _|_ _ _ _ _ _ _ _ _ _ _|_ _ _
         *     z'  |_____________________|      z
         *         |_____________________|
         */
        // make the envelope a cylinder:
        _cylinder_ = new cylinder;
        _tube_->compute_outer_cylinder (*_cylinder_);
        if (! _cylinder_->is_valid ())
          {
            throw logic_error ("simple_shaped_model::_construct_tube: Invalid 'outer' cylinder dimensions !");
          }
        _solid_ = _cylinder_;
        get_logical ().set_material_ref (_material_name_);
        // if the tube is extruded, add an extruded 'inner' cylinder
        // within the 'outer' cylinder:
        if (_tube_->is_extruded ())
          {
            cylinder * inner_cyl = new cylinder;
            _tube_->compute_inner_cylinder (*inner_cyl);
            if (! inner_cyl->is_valid ())
              {
                delete inner_cyl;
                throw logic_error ("simple_shaped_model::_at_construct: Invalid 'inner' cylinder dimensions !");
              }
            _inner_shape_ = inner_cyl;
            // inner placement for the extrusion:
            _inner_placement_.set (0, 0, 0, 0, 0, 0);
            ostringstream inner_name;
            inner_name << "__" << get_logical ().get_name () << ".tube_by_extrusion";
            _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name.str ()));
            _inner_logical_.set_material_ref (_filled_material_name_);
            _inner_logical_.set_shape (*_inner_shape_); // pass a reference -> logical has not the shape ownership
            _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name.str ()));
            _inner_phys_.set_placement (_inner_placement_);
            _inner_phys_.set_logical (_inner_logical_);
            _inner_phys_.set_mother (this->get_logical ());

            // makes the child extrusion the mother of daughter physical volumes:
            _daughter_owner_logical_ = &_inner_logical_;
          }
      }

    // build the tube as a child of a mother cylinder:
    if (_filled_mode_ == filled_utils::FILLED_BY_ENVELOPE)
      {
        // make the envelope a cylinder:
        _cylinder_ = new cylinder;
        _tube_->compute_outer_cylinder (*_cylinder_);
        if (! _cylinder_->is_valid ())
          {
            throw logic_error ("simple_shaped_model::_construct_tube: Invalid 'outer' cylinder dimensions !");
          }
        _solid_ = _cylinder_;
        get_logical ().set_material_ref (_filled_material_name_);
        if (! _tube_->is_extruded ())
          {
            // if the tube is not extruded, no daughter physical volumes can be placed
            // within the 'outer' envelope cylinder:
            _daughter_owner_logical_ = 0;
          }
        else
          {
            // if the tube is extruded, add the tube within the 'outer' envelope cylinder:
            _inner_placement_.set (0, 0, 0, 0, 0, 0);
            ostringstream inner_name;
            inner_name << "__" << get_logical ().get_name () << ".tube_by_envelope";
            _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name.str ()));
            _inner_logical_.set_material_ref (_material_name_);
            _inner_logical_.set_shape (*_tube_);
            if (visibility::has_color (config_))
              {
                visibility::set_color (_inner_logical_.parameters (),
                                       visibility::get_color (config_));
              }
            _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name.str ()));
            _inner_phys_.set_placement (_inner_placement_);
            _inner_phys_.set_logical (_inner_logical_);
            _inner_phys_.set_mother (this->get_logical ());
          }
      }

    return;
  }

  /*****************************************************/

  void simple_shaped_model::_construct_polycone (const string & name_,
                                                 const datatools::properties & config_,
                                                 models_col_type* models_)
  {
    bool devel = i_model::g_devel;
    if (config_.has_key ("devel"))
      {
        devel = true;
      }
    if (devel)
      {
        cerr << "DEVEL: simple_shaped_model::_construct_polycone: "
             << "Name = '" << name_ << "'"
             << endl;
      }
    string filled_material_name = material::constants::instance ().MATERIAL_REF_UNKNOWN;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    string filled_mode_label = filled_utils::FILLED_NONE_LABEL;

    double z_min, z_max;
    datatools::invalidate (z_min);
    datatools::invalidate (z_max);
    //string build_mode_label = "points";

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("angle_unit"))
      {
        string aunit_str = config_.fetch_string ("angle_unit");
        aunit = datatools::units::get_angle_unit_from (aunit_str);
      }

    /*
      if (config_.has_key ("build_mode"))
      {
      build_mode_label = config_.fetch_string ("build_mode");
      }
    */

    // filled mode:
    if (config_.has_key ("filled_mode"))
      {
        filled_mode_label = config_.fetch_string ("filled_mode");
        if (filled_mode_label == filled_utils::FILLED_NONE_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_NONE;
          }
        else if (filled_mode_label == filled_utils::FILLED_BY_ENVELOPE_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_BY_ENVELOPE;
          }
        else if (filled_mode_label == filled_utils::FILLED_BY_EXTRUSION_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_BY_EXTRUSION;
          }
        else
          {
            ostringstream message;
            message << "simple_boxed_model::_at_construct: "
                    << "Invalid mode '" << filled_mode_label << "' property !";
            throw logic_error (message.str ());
          }
      }
    else
      {
        _filled_mode_ = filled_utils::FILLED_NONE;
      }

    // filling material:
    if (_filled_mode_ != filled_utils::FILLED_NONE)
      {
        // parsing material:
        if (config_.has_key ("material.filled.ref"))
          {
            _material_name_ = config_.fetch_string ("material.filled.ref");
          }
        else
          {
            ostringstream message;
            message << "simple_shaped_model::_construct_polycone: "
                    << "Missing 'material.filled.ref' property !";
            throw logic_error (message.str ());
          }
      }

    _polycone_ = new polycone ();
    _polycone_->initialize (config_);
    if (! _polycone_->is_valid ())
      {
        _polycone_->tree_dump (cerr, "Invalid polycone: ", "ERROR:" );
        throw logic_error ("simple_shaped_model::_construct_polycone: Invalid polycone build parameters !");
      }
    if (devel)
      {
        _polycone_->tree_dump (cerr, "Polycone: ", "DEVEL:" );
      }

    if (_filled_mode_ == filled_utils::FILLED_NONE)
      {
        _solid_ = _polycone_;
        get_logical ().set_material_ref (_material_name_);
      }

    // build the polycone by extrusion of a mother polycone:
    if (_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION)
      {
        // make the envelope a polycone:
        polycone * envelope_polycone = new polycone;
        _polycone_->compute_outer_polycone (*envelope_polycone);
        if (! envelope_polycone->is_valid ())
          {
            throw logic_error ("simple_shaped_model::_construct_polycone: Invalid envelope polycone !");
          }
        _outer_shape_ = envelope_polycone;
        _solid_ = _outer_shape_;
        get_logical ().set_material_ref (_material_name_);
        // if the polycone is extruded, add an extruded 'inner' polycone
        // within the 'outer' polycone:
        if (_polycone_->is_extruded ())
          {
            polycone * inner_pol = new polycone;
            _polycone_->compute_inner_polycone (*inner_pol);
            if (! inner_pol->is_valid ())
              {
                delete inner_pol;
                throw logic_error ("simple_shaped_model::_construct_polycone: Invalid 'inner' polycone dimensions !");
              }
            _inner_shape_ = inner_pol;
            // inner placement for the extrusion:
            _inner_placement_.set (0, 0, 0, 0, 0, 0);
            ostringstream inner_name;
            inner_name << "__" << get_logical ().get_name () << ".polycone_by_extrusion";
            _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name.str ()));
            _inner_logical_.set_material_ref (_filled_material_name_);
            _inner_logical_.set_shape (*_inner_shape_); // pass a reference -> logical has not the shape ownership
            _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name.str ()));
            _inner_phys_.set_placement (_inner_placement_);
            _inner_phys_.set_logical (_inner_logical_);
            _inner_phys_.set_mother (this->get_logical ());

            // makes the child extrusion the mother of daughter physical volumes:
            _daughter_owner_logical_ = &_inner_logical_;
          }
      }

    // build the polycone as a filled child of a mother filled polycone:
    if (_filled_mode_ == filled_utils::FILLED_BY_ENVELOPE)
      {
        // make the envelope a cylinder:
        polycone * outer_polycone = new polycone;
        _polycone_->compute_outer_polycone (*outer_polycone);
        if (! outer_polycone->is_valid ())
          {
            throw logic_error ("simple_shaped_model::_construct_tube: Invalid 'outer' cylinder dimensions !");
          }
        _outer_shape_ = outer_polycone;
        _solid_ = _outer_shape_;
        get_logical ().set_material_ref (_filled_material_name_);
        if (! _polycone_->is_extruded ())
          {
            // if the polycon is not extruded, no daughter physical volumes can be placed
            // within the 'outer' envelope polycone:
            _daughter_owner_logical_ = 0;
          }
        else
          {
            // if the polycone is extruded, add the polycone
            // within the 'outer' envelope polycone:
            _inner_placement_.set (0, 0, 0, 0, 0, 0);
            ostringstream inner_name;
            inner_name << "__" << get_logical ().get_name () << ".polycone_by_envelope";
            _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name.str ()));
            _inner_logical_.set_material_ref (_material_name_);
            _inner_logical_.set_shape (*_polycone_);
            if (visibility::has_color (config_))
              {
                visibility::set_color (_inner_logical_.parameters (),
                                       visibility::get_color (config_));
              }
            _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name.str ()));
            _inner_phys_.set_placement (_inner_placement_);
            _inner_phys_.set_logical (_inner_logical_);
            _inner_phys_.set_mother (this->get_logical ());
          }
      }

    return;
  }


  /*****************************************************/

  void simple_shaped_model::_construct_polyhedra (const string & name_,
                                                  const datatools::properties & config_,
                                                  models_col_type* models_)
  {
    bool devel = i_model::g_devel;
    if (config_.has_key ("devel"))
      {
        devel = true;
      }
    if (devel)
      {
        cerr << "DEVEL: simple_shaped_model::_construct_polyhedra: "
             << "Name = '" << name_ << "'"
             << endl;
      }
    string filled_material_name = material::constants::instance ().MATERIAL_REF_UNKNOWN;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    string filled_mode_label = filled_utils::FILLED_NONE_LABEL;

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("angle_unit"))
      {
        string aunit_str = config_.fetch_string ("angle_unit");
        aunit = datatools::units::get_angle_unit_from (aunit_str);
      }

    // filled mode:
    if (config_.has_key ("filled_mode"))
      {
        filled_mode_label = config_.fetch_string ("filled_mode");
        if (filled_mode_label == filled_utils::FILLED_NONE_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_NONE;
          }
        else if (filled_mode_label == filled_utils::FILLED_BY_ENVELOPE_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_BY_ENVELOPE;
          }
        else if (filled_mode_label == filled_utils::FILLED_BY_EXTRUSION_LABEL)
          {
            _filled_mode_ = filled_utils::FILLED_BY_EXTRUSION;
          }
        else
          {
            ostringstream message;
            message << "simple_boxed_model::_at_construct: "
                    << "Invalid mode '" << filled_mode_label << "' property !";
            throw logic_error (message.str ());
          }
      }
    else
      {
        _filled_mode_ = filled_utils::FILLED_NONE;
      }

    // filling material:
    if (_filled_mode_ != filled_utils::FILLED_NONE)
      {
        // parsing material:
        if (config_.has_key ("material.filled.ref"))
          {
            _material_name_ = config_.fetch_string ("material.filled.ref");
          }
        else
          {
            ostringstream message;
            message << "simple_shaped_model::_construct_polyhedra: "
                    << "Missing 'material.filled.ref' property !";
            throw logic_error (message.str ());
          }
      }

    _polyhedra_ = new polyhedra ();
    _polyhedra_->initialize (config_);
    if (! _polyhedra_->is_valid ())
      {
        _polyhedra_->tree_dump (cerr, "Invalid polyhedra: ", "ERROR:" );
        throw logic_error ("simple_shaped_model::_construct_polyhedra: Invalid polyhedra build parameters !");
      }
    if (devel)
      {
        _polyhedra_->tree_dump (cerr, "Polyhedra: ", "DEVEL:" );
      }
    if (_filled_mode_ == filled_utils::FILLED_NONE)
      {
        _solid_ = _polyhedra_;
        get_logical ().set_material_ref (_material_name_);
      }

    // build the polyhedra by extrusion of a mother polyhedra:
    if (_filled_mode_ == filled_utils::FILLED_BY_EXTRUSION)
      {
        // make the envelope a polyhedra:
        polyhedra * envelope_polyhedra = new polyhedra;
        _polyhedra_->compute_outer_polyhedra (*envelope_polyhedra);
        if (! envelope_polyhedra->is_valid ())
          {
            throw logic_error ("simple_shaped_model::_construct_polyhedra: Invalid envelope polyhedra !");
          }
        _outer_shape_ = envelope_polyhedra;
        _solid_ = _outer_shape_;
        get_logical ().set_material_ref (_material_name_);
        // if the polyhedra is extruded, add an extruded 'inner' polyhedra
        // within the 'outer' polyhedra:
        if (_polyhedra_->is_extruded ())
          {
            polyhedra * inner_pol = new polyhedra;
            _polyhedra_->compute_inner_polyhedra (*inner_pol);
            if (! inner_pol->is_valid ())
              {
                delete inner_pol;
                throw logic_error ("simple_shaped_model::_construct_polyhedra: Invalid 'inner' polyhedra dimensions !");
              }
            _inner_shape_ = inner_pol;
            // inner placement for the extrusion:
            _inner_placement_.set (0, 0, 0, 0, 0, 0);
            ostringstream inner_name;
            inner_name << "__" << get_logical ().get_name () << ".polyhedra_by_extrusion";
            _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name.str ()));
            _inner_logical_.set_material_ref (_filled_material_name_);
            _inner_logical_.set_shape (*_inner_shape_); // pass a reference -> logical has not the shape ownership
            _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name.str ()));
            _inner_phys_.set_placement (_inner_placement_);
            _inner_phys_.set_logical (_inner_logical_);
            _inner_phys_.set_mother (this->get_logical ());

            // makes the child extrusion the mother of daughter physical volumes:
            _daughter_owner_logical_ = &_inner_logical_;
          }
      }

    // build the polyhedra as a filled child of a mother filled polyhedra:
    if (_filled_mode_ == filled_utils::FILLED_BY_ENVELOPE)
      {
        // make the envelope a cylinder:
        polyhedra * outer_polyhedra = new polyhedra;
        _polyhedra_->compute_outer_polyhedra (*outer_polyhedra);
        if (! outer_polyhedra->is_valid ())
          {
            throw logic_error ("simple_shaped_model::_construct_tube: Invalid 'outer' cylinder dimensions !");
          }
        _outer_shape_ = outer_polyhedra;
        _solid_ = _outer_shape_;
        get_logical ().set_material_ref (_filled_material_name_);
        if (! _polyhedra_->is_extruded ())
          {
            // if the polyhedra is not extruded, no daughter physical volumes can be placed
            // within the 'outer' envelope polyhedra:
            _daughter_owner_logical_ = 0;
          }
        else
          {
            // if the polyhedra is extruded, add the polyhedra
            // within the 'outer' envelope polyhedra:
            _inner_placement_.set (0, 0, 0, 0, 0, 0);
            ostringstream inner_name;
            inner_name << "__" << get_logical ().get_name () << ".polyhedra_by_envelope";
            _inner_logical_.set_name (i_model::make_logical_volume_name (inner_name.str ()));
            _inner_logical_.set_material_ref (_material_name_);
            _inner_logical_.set_shape (*_polyhedra_);
            if (visibility::has_color (config_))
              {
                visibility::set_color (_inner_logical_.parameters (),
                                       visibility::get_color (config_));
              }
            _inner_phys_.set_name (i_model::make_physical_volume_name (inner_name.str ()));
            _inner_phys_.set_placement (_inner_placement_);
            _inner_phys_.set_logical (_inner_logical_);
            _inner_phys_.set_mother (this->get_logical ());
          }
      }

    return;
  }

  /*******************************************/

  void simple_shaped_model::_post_construct (datatools::properties & setup_)
  {
    bool devel = i_model::g_devel;
    if (devel)
      {
        cerr << "DEVEL: " << "simple_shaped_model::_post_construct: "
             << "Entering..." << endl;
      }
    //material::extract (setup_, get_logical ().parameters ());
    sensitive::extract (setup_, get_logical ().parameters ());
    visibility::extract (setup_, get_logical ().parameters ());
    if (_solid_)
      {
        stackable::extract (setup_, _solid_->properties ());
      }
    if (_daughter_owner_logical_ != 0)
      {
        mapping_utils::extract (setup_, _daughter_owner_logical_->parameters ());
      }
    // if (is_filled ())
    //   {
    //  if (is_filled_by_extrusion  ())
    //    {
    //      visibility::extract (setup_, _inner_logical_.parameters ());
    //      /*
    //      if (_visibility_logical_ != 0)
    //        {
    //          visibility::extract (setup_, _inner_logical_.parameters ());
    //        }
    //      */
    //    }

    //  if (is_filled_by_envelope  ())
    //    {
    //      visibility::extract (setup_, _daughter_owner_logical_->parameters ());
    //      /*
    //      if (_visibility_logical_ != 0)
    //        {
    //          visibility::extract (setup_, _daughter_owner_logical_->parameters ());
    //        }
    //      */
    //    }

    //    }
    // else
    //   {
    //  visibility::extract (setup_, get_logical ().parameters ());
    //   }
    return;
  }

  /*******************************************/

  void simple_shaped_model::tree_dump (ostream & out_,
                                       const string & title_ ,
                                       const string & indent_,
                                       bool inherit_) const
  {
    using namespace datatools;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    // Shape name:
    {
      out_ << indent << i_tree_dumpable::tag
           << "Shape name : '" << get_shape_name () << "'" << endl;
    }

    // solid:
    {
      out_ << indent << i_tree_dumpable::tag
           << "Solid : ";
      if (_solid_ != 0)
        {
          out_ << endl;
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << i_tree_dumpable::skip_tag;
          _solid_->tree_dump (out_, "", indent_oss.str ());
        }
      else
        {
          out_ << "<none>" << endl;
        }
    }

    // filled material name:
    if (! _filled_material_name_.empty ())
      {
        out_ << indent << i_tree_dumpable::tag
             << "Filled material name : '" << get_filled_material_name () << "'" << endl;
      }

    {
      out_ << indent << i_tree_dumpable::tag
           << "Filled mode : '" << _filled_mode_ << "'" << endl;

    }

    // material name:
    {
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_)
           << "Material name : '" << get_material_name () << "'" << endl;
    }

    return;
  }

  // registration :   
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(simple_shaped_model,"geomtools::simple_shaped_model");

} // end of namespace geomtools

// end of simple_shaped_model.cc
