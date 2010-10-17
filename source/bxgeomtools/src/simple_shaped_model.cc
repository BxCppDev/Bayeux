// -*- mode: c++ ; -*- 
/* simple_shaped_model.cc
 */

#include <geomtools/simple_shaped_model.h>

namespace geomtools {

  using namespace std;
 
  bool simple_shaped_model::is_filled () const
  {
    return __filled_mode != filled_utils::FILLED_NONE;
  }
 
  bool simple_shaped_model::is_filled_by_envelope () const
  {
    return __filled_mode == filled_utils::FILLED_BY_ENVELOPE;
  }
 
  bool simple_shaped_model::is_filled_by_extrusion () const
  {
    return __filled_mode == filled_utils::FILLED_BY_EXTRUSION;
  }

  MWIM & simple_shaped_model::get_internals ()
  {
    return __internals;
  }

  const MWIM & simple_shaped_model::get_internals () const
  {
    return __internals;
  }
 
  const geomtools::box & simple_shaped_model::get_box () const
  {
    return *__box;
  }
  
  const geomtools::cylinder & simple_shaped_model::get_cylinder () const
  {
    return *__cylinder;
  }
  
  const geomtools::tube & simple_shaped_model::get_tube () const
  {
    return *__tube;
  }
  
  const geomtools::sphere & simple_shaped_model::get_sphere () const
  {
    return *__sphere;
  }
  
  const geomtools::polycone & simple_shaped_model::get_polycone () const
  {
    return *__polycone;
  }
  
  const geomtools::i_shape_3d & simple_shaped_model::get_solid () const
  {
    return *__solid;
  }

  const string & simple_shaped_model::get_shape_name () const
  {
    return __shape_name;
  }

  const string & simple_shaped_model::get_material_name () const
  {
    return __material_name;
  }

  const string & simple_shaped_model::get_filled_material_name () const
  {
    return __filled_material_name;
  }
  
  string simple_shaped_model::get_model_id () const
  {
    return "geomtools::simple_shaped_model";
  }
  
  // ctor:
  simple_shaped_model::simple_shaped_model () : i_model ()
  {
    __box = 0;
    __cylinder = 0;
    __tube = 0;
    __sphere = 0;
    __polycone = 0;
    __solid = 0;
    __inner_shape = 0;
    __outer_shape = 0;
    __daughter_owner_logical = 0;
    __visibility_logical = 0;
    //cerr << "DEVEL: 1 *****" << "  "  << "*****" << endl;
    __shape_name = ""; // no defined shape
    //cerr << "DEVEL: 2 *****" << material::MATERIAL_REF_UNKNOWN << "*****" << endl;
    __material_name = ""; //material::MATERIAL_REF_UNKNOWN;
    __filled_material_name = ""; //material::MATERIAL_REF_UNKNOWN;
    __filled_mode   = filled_utils::FILLED_NONE;
    return;
  }
 
  // dtor:  
  simple_shaped_model::~simple_shaped_model ()
  {
    __solid = 0;
    if (__inner_shape != 0) delete __inner_shape;
    if (__outer_shape != 0) delete __outer_shape;
    if (__box != 0) delete __box;
    if (__cylinder != 0) delete __cylinder;
    if (__tube != 0) delete __tube;
    if (__sphere != 0) delete __sphere;
    if (__polycone != 0) delete __polycone;
    return;
  }
  
  void simple_shaped_model::_at_construct (const string & name_,
					   const datatools::utils::properties & config_,
					   models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: simple_shaped_model::_at_construct: Entering..." << endl;
    set_name (name_);
    string shape_type = "";

    // Initialization:
    __material_name = material::MATERIAL_REF_UNKNOWN;
    __filled_material_name = material::MATERIAL_REF_UNKNOWN;
 
    // parsing shape:
    if (config_.has_key ("shape_type"))
      {
	shape_type = config_.fetch_string ("shape_type");
	__shape_name = shape_type;
      }  
    else
      {
 	ostringstream message;
	message << "simple_shaped_model::_at_construct: "
		<< "Missing 'shape_type' property !";
	throw runtime_error (message.str ());
      }

    // parsing material:
    if (config_.has_key ("material.ref"))
      {
	__material_name = config_.fetch_string ("material.ref");
      }
    else
      {
	ostringstream message;
	message << "simple_shaped_model::_at_construct: "
		<< "Missing 'material.ref' property !";
	throw runtime_error (message.str ());
      }

    // makes the embeded logical volume the default 
    // mother of daughter physical volumes:
    __daughter_owner_logical = & get_logical ();

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
    
    // set the envelope solid shape:
    get_logical ().set_shape (*__solid);

    // search for internal item to install within the model envelope:
    if (__daughter_owner_logical != 0)
      {
	__internals.plug_internal_models (config_, 
					  *__daughter_owner_logical, 
					  models_);
      }

    if (devel) clog << "DEVEL: simple_shaped_model::_at_construct: Exiting." << endl;
    return;
  }

  /***********************************************************/
  
  void simple_shaped_model::_construct_box (const string & name_,
					    const datatools::utils::properties & config_,
					    models_col_t * models_)
  {
    double lunit = CLHEP::mm;
    double x, y, z;

    if (config_.has_key ("length_unit"))
      {
	string lunit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
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
	throw runtime_error (message.str ());
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
	throw runtime_error (message.str ());
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
	throw runtime_error (message.str ());
      }
    
    // build the box:
    __box = new box (x, y, z);
    if (! __box->is_valid ())
      {
	throw runtime_error ("simple_shaped_model::_construct_box: Invalid box dimensions !");
      }
    __solid = __box;
    get_logical ().set_material_ref (__material_name);
    
    // search for internal item to install within the model envelope:
    __internals.plug_internal_models (config_, get_logical (), models_);
    
    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_cylinder (const string & name_,
						 const datatools::utils::properties & config_,
						 models_col_t * models_)
  {
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    double r, z;

    if (config_.has_key ("length_unit"))
      {
	string lunit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }
    
    if (config_.has_key ("angle_unit"))
      {
	string aunit_str = config_.fetch_string ("angle_unit");
	aunit = datatools::utils::units::get_angle_unit_from (aunit_str);
      }
    
    if (config_.has_key ("r"))
      {
	r = config_.fetch_real ("r");
	r *= lunit;
      }  
    else
      {
	ostringstream message;
	message << "simple_boxed_model::_construct_cylinder: "
		<< "Missing cylinder 'r' property !";
	throw runtime_error (message.str ());
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
	throw runtime_error (message.str ());
      }
    
    // build the cylinder:
    __cylinder = new cylinder (r, z);
    if (! __cylinder->is_valid ())
      {
	throw runtime_error ("simple_shaped_model::_construct_cylinder: Invalid cylinder dimensions !");
      }
    __solid = __cylinder;
    get_logical ().set_material_ref (__material_name);
    // search for internal item to install within the model envelope:
    __internals.plug_internal_models (config_, get_logical (), models_);
    
    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_sphere (const string & name_,
					       const datatools::utils::properties & config_,
					       models_col_t * models_)
  {
    string material_name = material::MATERIAL_REF_UNKNOWN;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    double r;

    if (config_.has_key ("length_unit"))
      {
	string lunit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }
    
    if (config_.has_key ("angle_unit"))
      {
	string aunit_str = config_.fetch_string ("angle_unit");
	aunit = datatools::utils::units::get_angle_unit_from (aunit_str);
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
	throw runtime_error (message.str ());
      }
    
    // build the sphere:
    __sphere = new sphere (r);
    if (! __sphere->is_valid ())
      {
	throw runtime_error ("simple_shaped_model::_construct_sphere: Invalid sphere dimension !");
	  }
    __solid = __sphere;
    get_logical ().set_material_ref (__material_name);
    // search for internal item to install within the model envelope:
    __internals.plug_internal_models (config_, get_logical (), models_);
    
    return;
  }

  /***********************************************************/

  void simple_shaped_model::_construct_tube (const string & name_,
					     const datatools::utils::properties & config_,
					     models_col_t * models_)
  {
    string filled_material_name = material::MATERIAL_REF_UNKNOWN;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    double inner_r, outer_r, z;
    string filled_mode_label = filled_utils::FILLED_NONE_LABEL;

    if (config_.has_key ("length_unit"))
      {
	string lunit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }
    
    if (config_.has_key ("angle_unit"))
      {
	string aunit_str = config_.fetch_string ("angle_unit");
	aunit = datatools::utils::units::get_angle_unit_from (aunit_str);
      }

    if (config_.has_key ("inner_r"))
      {
	inner_r = config_.fetch_real ("inner_r");
	inner_r *= lunit;
      }  
    else
      {
	ostringstream message;
	message << "simple_boxed_model::_construct_tube: "
		<< "Missing tube 'inner_r' property !";
	//throw runtime_error (message.str ());
	cerr << "WARNING: " << message.str () << " Using 0-default inner radius !" << endl;
	inner_r = 0.0;
      }
    
    if (config_.has_key ("outer_r"))
      {
	outer_r = config_.fetch_real ("outer_r");
	outer_r *= lunit;
      }  
    else
      {
	ostringstream message;
	message << "simple_boxed_model::_construct_tube: "
		<< "Missing tube 'outer_r' property !";
	throw runtime_error (message.str ());
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
	throw runtime_error (message.str ());
      }

    // filled mode:
    if (config_.has_key ("filled_mode"))
      {
	filled_mode_label = config_.fetch_string ("filled_mode");
	if (filled_mode_label == filled_utils::FILLED_NONE_LABEL)
	  {
	    __filled_mode = filled_utils::FILLED_NONE;
	  }
	else if (filled_mode_label == filled_utils::FILLED_BY_ENVELOPE_LABEL)
	  {
	    __filled_mode = filled_utils::FILLED_BY_ENVELOPE;
	  }
	else if (filled_mode_label == filled_utils::FILLED_BY_EXTRUSION_LABEL)
	  {
	    __filled_mode = filled_utils::FILLED_BY_EXTRUSION;
	  }
	else
	  {
	    ostringstream message;
	    message << "simple_boxed_model::_construct_tube: "
		    << "Invalid mode '" << filled_mode_label << "' property !";
	    throw runtime_error (message.str ());
	  }
      }
    else
      {
	__filled_mode = filled_utils::FILLED_NONE;
      }
    
    // filling material:
    if (__filled_mode != filled_utils::FILLED_NONE)
      {
	// parsing material:
	if (config_.has_key ("material.filled.ref"))
	  {
	    __material_name = config_.fetch_string ("material.filled.ref");
	  }
	else
	  {
	    ostringstream message;
	    message << "simple_shaped_model::_construct_tube: "
		    << "Missing 'material.filled.ref' property !";
	    throw runtime_error (message.str ());
	  }
      }

    // build the tube:
    __tube = new tube (inner_r, outer_r, z);
    if (! __tube->is_valid ())
      {
	throw runtime_error ("simple_shaped_model::_construct_tube: Invalid  tube dimensions !");
      }

    // use the plain tube as solid envelope of the model:
    if (__filled_mode == filled_utils::FILLED_NONE)
      {
	__solid = __tube;
	get_logical ().set_material_ref (__material_name);
      }
    
    // build the tube by cylindric extrusion of a mother cylinder:
    if (__filled_mode == filled_utils::FILLED_BY_EXTRUSION)
      {
	/*
	 *          _____________________  
	 *         |_____________________|
	 *      _ _|_ _ _ _ _ _ _ _ _ _ _|_ _ _
	 *     z'  |_____________________|      z
	 *         |_____________________|
	 */
	// make the envelope a cylinder:
	__cylinder = new cylinder;
	__tube->compute_outer_cylinder (*__cylinder);
	if (! __cylinder->is_valid ())
	  {
	    throw runtime_error ("simple_shaped_model::_construct_tube: Invalid 'outer' cylinder dimensions !");
	  }
	__solid = __cylinder;
	get_logical ().set_material_ref (__material_name);
 	// if the tube is extruded, add an extruded 'inner' cylinder 
	// within the 'outer' cylinder:
	if (__tube->is_extruded ())
	  {
	    cylinder * inner_cyl = new cylinder;
	    __tube->compute_inner_cylinder (*inner_cyl);
	    if (! inner_cyl->is_valid ())
	      {
		delete inner_cyl;
		throw runtime_error ("simple_shaped_model::_at_construct: Invalid 'inner' cylinder dimensions !");
	      }
	    __inner_shape = inner_cyl;
	    // inner placement for the extrusion:
	    __inner_placement.set (0, 0, 0, 0, 0, 0);
	    ostringstream inner_name;
	    inner_name << "__" << get_logical ().get_name () << ".tube_extrusion";
	    __inner_logical.set_name (i_model::make_logical_volume_name (inner_name.str ()));
	    __inner_logical.set_material_ref (__filled_material_name);
	    __inner_logical.set_shape (*__inner_shape); // pass a reference -> logical has not the shape ownership
	    __inner_phys.set_name (i_model::make_physical_volume_name (inner_name.str ()));
	    __inner_phys.set_placement (__inner_placement);
	    __inner_phys.set_logical (__inner_logical);
	    __inner_phys.set_mother (this->get_logical ());

	    // makes the child extrusion the mother of daughter physical volumes:
	    __daughter_owner_logical = &__inner_logical;
	  }
      }
    
    // build the tube as a child of a mother cylinder:
    if (__filled_mode == filled_utils::FILLED_BY_ENVELOPE)
      {
	// make the envelope a cylinder:
	__cylinder = new cylinder;
	__tube->compute_outer_cylinder (*__cylinder);
	if (! __cylinder->is_valid ())
	  {
	    throw runtime_error ("simple_shaped_model::_construct_tube: Invalid 'outer' cylinder dimensions !");
	  }
	__solid = __cylinder;
	get_logical ().set_material_ref (__filled_material_name);
	if (! __tube->is_extruded ())
	  {
	    // if the tube is not extruded, no daughter physical volumes can be placed 
	    // within the 'outer' envelope cylinder:
	    __daughter_owner_logical = 0;
	  }
	else
	  {
	    // if the tube is extruded, add the tube within the 'outer' envelope cylinder:
	    __inner_placement.set (0, 0, 0, 0, 0, 0);
	    ostringstream inner_name;
	    inner_name << "__" << get_logical ().get_name () << ".tube_envelope";
	    __inner_logical.set_name (i_model::make_logical_volume_name (name_));
	    __inner_logical.set_material_ref (__material_name);
	    __inner_logical.set_shape (*__tube);
	    if (visibility::has_color (config_))
	      {
		visibility::set_color (__inner_logical.parameters (), 
				       visibility::get_color (config_));
	      }
	    __inner_phys.set_name (i_model::make_physical_volume_name (inner_name.str ()));
	    __inner_phys.set_placement (__inner_placement);
	    __inner_phys.set_logical (__inner_logical);
	    __inner_phys.set_mother (this->get_logical ());
	  }
      }

    return;
  }

  /*****************************************************/
      
  void simple_shaped_model::_construct_polycone (const string & name_,
						 const datatools::utils::properties & config_,
						 models_col_t * models_)
  {
    string filled_material_name = material::MATERIAL_REF_UNKNOWN;
    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    string filled_mode_label = filled_utils::FILLED_NONE_LABEL;

    double z_min, z_max;
    datatools::utils::invalidate (z_min);
    datatools::utils::invalidate (z_max);
    //string build_mode_label = "points";

    if (config_.has_key ("length_unit"))
      {
	string lunit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }
    
    if (config_.has_key ("angle_unit"))
      {
	string aunit_str = config_.fetch_string ("angle_unit");
	aunit = datatools::utils::units::get_angle_unit_from (aunit_str);
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
	    __filled_mode = filled_utils::FILLED_NONE;
	  }
	else if (filled_mode_label == filled_utils::FILLED_BY_ENVELOPE_LABEL)
	  {
	    __filled_mode = filled_utils::FILLED_BY_ENVELOPE;
	  }
	else if (filled_mode_label == filled_utils::FILLED_BY_EXTRUSION_LABEL)
	  {
	    __filled_mode = filled_utils::FILLED_BY_EXTRUSION;
	  }
	else
	  {
	    ostringstream message;
	    message << "simple_boxed_model::_at_construct: "
		    << "Invalid mode '" << filled_mode_label << "' property !";
	    throw runtime_error (message.str ());
	  }
      }
    else
      {
	__filled_mode = filled_utils::FILLED_NONE;
      }
    
    // filling material:
    if (__filled_mode != filled_utils::FILLED_NONE)
      {
	// parsing material:
	if (config_.has_key ("material.filled.ref"))
	  {
	    __material_name = config_.fetch_string ("material.filled.ref");
	  }
	else
	  {
	    ostringstream message;
	    message << "simple_shaped_model::_construct_polycone: "
		    << "Missing 'material.filled.ref' property !";
	    throw runtime_error (message.str ());
	  }
      }
    
      /*
      if (config_.has_key ("z_min"))
      {
      z_min = config_.fetch_real ("z_min");
      z_min *= lunit;
      }  
      
      if (config_.has_key ("z_max"))
      {
      z_max = config_.fetch_real ("z_max");
      z_max *= lunit;
      }  
      */
    
    __polycone = new polycone ();
    __polycone->initialize (config_);
    if (! __polycone->is_valid ())
      {
	throw runtime_error ("simple_shaped_model::_construct_polycone: Invalid polycone build parameters !");
      }
    
    if (__filled_mode == filled_utils::FILLED_NONE)
      {
	__solid = __polycone;
	get_logical ().set_material_ref (__material_name);
      }
     
     // build the polycone by extrusion of a mother polycone:
     if (__filled_mode == filled_utils::FILLED_BY_EXTRUSION)
      {
	// make the envelope a polycone:
	polycone * envelope_polycone = new polycone;
	__polycone->compute_outer_polycone (*envelope_polycone);
	if (! envelope_polycone->is_valid ())
	  {
	    throw runtime_error ("simple_shaped_model::_construct_polycone: Invalid envelope polycone !");
	  }
	__outer_shape = envelope_polycone;
	__solid = __outer_shape;
	get_logical ().set_material_ref (__material_name);
	// if the polycone is extruded, add an extruded 'inner' polycone 
	// within the 'outer' polycone:
	if (__polycone->is_extruded ())
	  {
	    polycone * inner_pol = new polycone;
	    __polycone->compute_inner_polycone (*inner_pol);
	    if (! inner_pol->is_valid ())
	      {
		delete inner_pol;
		throw runtime_error ("simple_shaped_model::_construct_polycone: Invalid 'inner' polycone dimensions !");
	      }
	    __inner_shape = inner_pol;
	    // inner placement for the extrusion:
	    __inner_placement.set (0, 0, 0, 0, 0, 0);
	    ostringstream inner_name;
	    inner_name << "__" << get_logical ().get_name () << ".polycone_extrusion";
	    __inner_logical.set_name (i_model::make_logical_volume_name (inner_name.str ()));
	    __inner_logical.set_material_ref (__filled_material_name);
	    __inner_logical.set_shape (*__inner_shape); // pass a reference -> logical has not the shape ownership
	    __inner_phys.set_name (i_model::make_physical_volume_name (inner_name.str ()));
	    __inner_phys.set_placement (__inner_placement);
	    __inner_phys.set_logical (__inner_logical);
	    __inner_phys.set_mother (this->get_logical ());

	    // makes the child extrusion the mother of daughter physical volumes:
	    __daughter_owner_logical = &__inner_logical;
	  }
      }

     // build the polycone as a filled child of a mother filled polycone:
     if (__filled_mode == filled_utils::FILLED_BY_ENVELOPE)
      {
	// make the envelope a cylinder:
	polycone * outer_polycone = new polycone;
	__polycone->compute_outer_polycone (*outer_polycone);
	if (! outer_polycone->is_valid ())
	  {
	    throw runtime_error ("simple_shaped_model::_construct_tube: Invalid 'outer' cylinder dimensions !");
	  }
	__outer_shape = outer_polycone;
	__solid = __outer_shape;
	get_logical ().set_material_ref (__filled_material_name);
	if (! __polycone->is_extruded ())
	  {
	    // if the polycon is not extruded, no daughter physical volumes can be placed 
	    // within the 'outer' envelope polycon:
	    __daughter_owner_logical = 0;
	  }
	else
	  {
	    // if the polycon is extruded, add the polycon 
	    // within the 'outer' envelope polycon:
	    __inner_placement.set (0, 0, 0, 0, 0, 0);
	    ostringstream inner_name;
	    inner_name << "__" << get_logical ().get_name () << ".polycon_envelope";
	    __inner_logical.set_name (i_model::make_logical_volume_name (name_));
	    __inner_logical.set_material_ref (__material_name);
	    __inner_logical.set_shape (*__polycone);
	    if (visibility::has_color (config_))
	      {
		visibility::set_color (__inner_logical.parameters (), 
				       visibility::get_color (config_));
	      }
	    __inner_phys.set_name (i_model::make_physical_volume_name (inner_name.str ()));
	    __inner_phys.set_placement (__inner_placement);
	    __inner_phys.set_logical (__inner_logical);
	    __inner_phys.set_mother (this->get_logical ());
	  }
      }
 
    return;
  }

  /*******************************************/

  void simple_shaped_model::_post_construct (datatools::utils::properties & setup_)
  {
    cerr << "DEVEL: " << "simple_shaped_model::_post_construct: "
	 << "Entering..." << endl;
    //material::extract (setup_, get_logical ().parameters ());
    sensitive::extract (setup_, get_logical ().parameters ());
    visibility::extract (setup_, get_logical ().parameters ());
    if (__daughter_owner_logical != 0)
      {
	mapping_utils::extract (setup_, __daughter_owner_logical->parameters ());
      }
    // if (is_filled ())
    //   {
    // 	if (is_filled_by_extrusion  ())
    // 	  {
    // 	    visibility::extract (setup_, __inner_logical.parameters ());
    // 	    /*
    // 	    if (__visibility_logical != 0)
    // 	      {
    // 		visibility::extract (setup_, __inner_logical.parameters ());
    // 	      }
    // 	    */
    // 	  }
 
    // 	if (is_filled_by_envelope  ())
    // 	  {
    // 	    visibility::extract (setup_, __daughter_owner_logical->parameters ());
    // 	    /*
    // 	    if (__visibility_logical != 0)
    // 	      {
    // 		visibility::extract (setup_, __daughter_owner_logical->parameters ());
    // 	      }
    // 	    */
    // 	  }
	
    //    }
    // else
    //   {
    // 	visibility::extract (setup_, get_logical ().parameters ());
    //   }
    return;
  }

  /*******************************************/

  void simple_shaped_model::tree_dump (ostream & out_, 
				       const string & title_ , 
				       const string & indent_, 
				       bool inherit_) const
  {
     namespace du = datatools::utils;
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

     // solid:
     {
       out_ << indent << i_tree_dumpable::tag
	    << "Solid : ";
       if (__solid != 0)
	 {
	   out_ << endl;
	   ostringstream indent_oss;
	   indent_oss << indent;
	   indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	   __solid->tree_dump (out_, "", indent_oss.str ());
	 }   
       else
	 {
	   out_ << "<none>" << endl;
	 }
     }

     // material name:
     {
       out_ << indent << i_tree_dumpable::inherit_tag (inherit_)  
	    << "Material name : '" << get_material_name () << "'" << endl;
     }

     return;
  }
  
  // register this creator:   
  geomtools::i_model::creator_registration<simple_shaped_model> simple_shaped_model::__CR;
    
} // end of namespace geomtools

// end of simple_shaped_model.cc
