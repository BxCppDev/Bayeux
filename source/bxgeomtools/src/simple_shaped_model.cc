// -*- mode: c++ ; -*- 
/* simple_shaped_model.cc
 */

#include <geomtools/simple_shaped_model.h>
#include <geomtools/color.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<simple_shaped_model> simple_shaped_model::__CR;
  
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
  
  const geomtools::i_shape_3d & simple_shaped_model::get_solid () const
  {
    return *__solid;
  }

  const string & simple_shaped_model::get_shape_name () const
  {
    return __shape_name;
  }

  string simple_shaped_model::get_material_name () const
  {
    return get_logical ().get_material_ref ();
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
    __solid = 0;
    __inner_shape = 0;
    return;
  }

  // dtor:  
  simple_shaped_model::~simple_shaped_model ()
  {
    __solid = 0;
    if (__inner_shape != 0) delete __inner_shape;
    if (__box != 0) delete __box;
    if (__cylinder != 0) delete __cylinder;
    if (__tube != 0) delete __tube;
    if (__sphere != 0) delete __sphere;
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
    string lunit_str = "mm"; // default unit
    string aunit_str = "degree"; // default unit
    string material_name = material::MATERIAL_REF_UNKNOWN;
    string inner_material_name = material::MATERIAL_REF_UNKNOWN;
 
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

    if (config_.has_key ("length_unit"))
      {
	lunit_str = config_.fetch_string ("length_unit");
      }

    if (config_.has_key ("angle_unit"))
      {
	aunit_str = config_.fetch_string ("angle_unit");
      }

    double lunit = CLHEP::mm;
    lunit = datatools::utils::units::get_length_unit_from (lunit_str);
    double aunit = CLHEP::degree;
    aunit = datatools::utils::units::get_angle_unit_from (aunit_str);

    if (config_.has_key ("material.ref"))
      {
	material_name = config_.fetch_string ("material.ref");
      }
    else
      {
	ostringstream message;
	message << "simple_shaped_model::_at_construct: "
		<< "Missing 'material.ref' property !";
	throw runtime_error (message.str ());
      }

    // Box case:
    if (shape_type == "box")
      {
	double x, y, z;
	if (config_.has_key ("x"))
	  {
	    x = config_.fetch_real ("x");
	    x *= lunit;
	  }  
	else
	  {
	    ostringstream message;
	    message << "simple_boxed_model::_at_construct: "
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
	    message << "simple_boxed_model::_at_construct: "
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
	    message << "simple_boxed_model::_at_construct: "
		    << "Missing box 'z' property !";
	    throw runtime_error (message.str ());
	  }

	__box = new box (x, y, z);
	if (! __box->is_valid ())
	  {
	    throw runtime_error ("simple_shaped_model::_at_construct: Invalid box dimensions !");
	  }
	__solid = __box;
	get_logical ().set_material_ref (material_name);
      }
    // Cylinder case:
    else if (shape_type == "cylinder")
      {
	double r, z;
	if (config_.has_key ("r"))
	  {
	    r = config_.fetch_real ("r");
	    r *= lunit;
	  }  
	else
	  {
	    ostringstream message;
	    message << "simple_boxed_model::_at_construct: "
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
	    message << "simple_boxed_model::_at_construct: "
		    << "Missing cylinder 'z' property !";
	    throw runtime_error (message.str ());
	  }

	__cylinder = new cylinder (r, z);
	if (! __cylinder->is_valid ())
	  {
	    throw runtime_error ("simple_shaped_model::_at_construct: Invalid cylinder dimensions !");
	  }
	__solid = __cylinder;
	get_logical ().set_material_ref (material_name);
      }
    // Sphere case:
    else if (shape_type == "sphere")
      {
	double r;
	if (config_.has_key ("r"))
	  {
	    r = config_.fetch_real ("r");
	    r *= lunit;
	  }  
	else
	  {
	    ostringstream message;
	    message << "simple_boxed_model::_at_construct: "
		    << "Missing sphere 'r' property !";
	    throw runtime_error (message.str ());
	  }

	__sphere = new sphere (r);
	if (! __sphere->is_valid ())
	  {
	    throw runtime_error ("simple_shaped_model::_at_construct: Invalid cylinder dimensions !");
	  }
	__solid = __sphere;
	get_logical ().set_material_ref (material_name);
      }
    // Tube case:
    else if (shape_type == "tube")
      {
	//throw runtime_error ("simple_shaped_model::_at_construct: Tube is not supported yet !");
	double inner_r, outer_r, z;
	bool by_extrusion = false; 
	string mode_label = "by_envelope";

	if (config_.has_key ("mode"))
	  {
	    mode_label = config_.fetch_string ("mode");
	    if (mode_label == "by_envelope")
	      {
		by_extrusion = false;
	      }
	    else if (mode_label == "by_extrusion")
	      {
		by_extrusion = true;
	      }
	    else
	      {
		ostringstream message;
		message << "simple_boxed_model::_at_construct: "
			<< "Invalid mode '" << mode_label << "' property !";
		throw runtime_error (message.str ());
	      }
	  }

	if (config_.has_key ("inner_r"))
	  {
	    inner_r = config_.fetch_real ("inner_r");
	    inner_r *= lunit;
	  }  
	else
	  {
	    ostringstream message;
	    message << "simple_boxed_model::_at_construct: "
		    << "Missing tube 'inner_r' property !";
	    throw runtime_error (message.str ());
	  }

	if (config_.has_key ("outer_r"))
	  {
	    outer_r = config_.fetch_real ("outer_r");
	    outer_r *= lunit;
	  }  
	else
	  {
	    ostringstream message;
	    message << "simple_boxed_model::_at_construct: "
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
	    message << "simple_boxed_model::_at_construct: "
		    << "Missing tube 'z' property !";
	    throw runtime_error (message.str ());
	  }

	// build the tube by extrusion of a mother cylinder:
	if (by_extrusion)
	  {
	    /*
	     *  
	     *      _____________________
	     *     |_____________________|
	     *  _ _|_ _ _ _ _ _ _ _ _ _ _|_ _ _
	     * z'  |_____________________|      z
	     *     |_____________________|
	     */
	    // make the envelope a cylinder:
	    __cylinder = new cylinder (outer_r, z);
	    if (! __cylinder->is_valid ())
	      {
		throw runtime_error ("simple_shaped_model::_at_construct: Invalid 'outer' cylinder dimensions !");
	      }
	    __solid = __cylinder;
	    get_logical ().set_material_ref (material_name);
	    // if the tube is extruded, add an extruded 'inner' cylinder within the 'outer' cylinder:
	    if (inner_r > 0.0)
	      {
		__tube = new tube (inner_r, outer_r, z);
		if (! __tube->is_valid ())
		  {
		    throw runtime_error ("simple_shaped_model::_at_construct: Invalid tube dimensions !");
		  }
		cylinder * inner_cyl = new cylinder (inner_r, z);
		if (! inner_cyl->is_valid ())
		  {
		    throw runtime_error ("simple_shaped_model::_at_construct: Invalid 'inner' cylinder dimensions !");
		  }
		if (config_.has_key ("inner_material.ref"))
		  {
		    inner_material_name = config_.fetch_string ("inner_material.ref");
		  }
		else
		  {
		    ostringstream message;
		    message << "simple_shaped_model::_at_construct: "
			    << "Missing tube 'inner_material.ref' property !";
		    throw runtime_error (message.str ());
		  }
		__inner_shape = inner_cyl;
		// inner placement for the extrusion:
		__inner_placement.set (0, 0, 0, 0, 0, 0);
		ostringstream inner_name;
		inner_name << get_logical ().get_name () << ".tube_extrusion";
		__inner_logical.set_name (i_model::make_logical_volume_name (inner_name.str ()));
		__inner_logical.set_material_ref (inner_material_name);
		__inner_logical.set_shape (*__inner_shape); // pass a reference -> logical has not the shape ownership
		__inner_phys.set_name (i_model::make_physical_volume_name (inner_name.str ()));
		__inner_phys.set_placement (__inner_placement);
		__inner_phys.set_logical (__inner_logical);
		__inner_phys.set_mother (this->get_logical ());
	      }
	  }
	// build the tube as a child of a mother cylinder:
	else
	  {
	    // make the envelope a cylinder:
	    __cylinder = new cylinder (outer_r, z);
	    if (! __cylinder->is_valid ())
	      {
		throw runtime_error ("simple_shaped_model::_at_construct: Invalid 'outer' cylinder dimensions !");
	      }
	    __solid = __cylinder;
	    if (config_.has_key ("inner_material.ref"))
	      {
		inner_material_name = config_.fetch_string ("inner_material.ref");
	      }
	    else
	      {
		ostringstream message;
		message << "simple_shaped_model::_at_construct: "
			<< "Missing tube 'inner_material.ref' property !";
		throw runtime_error (message.str ());
	      }
	    get_logical ().set_material_ref (inner_material_name);
	    // if the tube is extruded, add an extruded 'inner' cylinder within the 'outer' cylinder:
	    if (inner_r > 0.0)
	      {
		__tube = new tube (inner_r, outer_r, z);
		if (! __tube->is_valid ())
		  {
		    throw runtime_error ("simple_shaped_model::_at_construct: Invalid tube dimensions !");
		  }
	      }
	    // inner placement
	    {
	      __inner_placement.set (0, 0, 0, 0, 0, 0);
		ostringstream inner_name;
		inner_name << get_logical ().get_name () << ".tube_envelope";
	      __inner_logical.set_name (i_model::make_logical_volume_name (name_));
	      __inner_logical.set_material_ref (material_name);
	      __inner_logical.set_shape (*__tube);
	      if (visibility::has_color (config_))
		{
		  /*
		  cerr << "COLOR = " << visibility::get_color (config_) << endl;
		  string word;
		  getline (cin, word);
		  */
		  visibility::set_color (__inner_logical.parameters (), 
					 visibility::get_color (config_));
		}
	      /*
	      else
		{
		  visibility::set_color (__inner_logical.parameters (), 
					 color::red);
		}
	      */
	      __inner_phys.set_name (i_model::make_physical_volume_name (inner_name.str ()));
	      __inner_phys.set_placement (__inner_placement);
	      __inner_phys.set_logical (__inner_logical);
	      __inner_phys.set_mother (this->get_logical ());
	    }
	  }
     }
    
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (*__solid);

    if (devel) clog << "DEVEL: simple_shaped_model::_at_construct: Exiting." << endl;
    return;
  }

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
    
} // end of namespace geomtools

// end of simple_shaped_model.cc
