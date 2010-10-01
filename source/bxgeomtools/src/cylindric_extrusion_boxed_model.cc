// -*- mode: c++ ; -*- 
/* cylindric_extrusion_boxed_model.cc
 */

#include <geomtools/cylindric_extrusion_boxed_model.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<cylindric_extrusion_boxed_model> cylindric_extrusion_boxed_model::__CR;
  
  const geomtools::box & cylindric_extrusion_boxed_model::get_box () const
  {
    return __solid;
  }
  
  const geomtools::box & cylindric_extrusion_boxed_model::get_solid () const
  {
    return __solid;
  }
     
  void cylindric_extrusion_boxed_model::set_extrusion_radius (double r_)
  {
    assert_unconstructed("cylindric_extrusion_boxed_model::set_extrusion_radius");

    if (r_ <= 0.0 * CLHEP::mm)
      {
	ostringstream message;
	message << "cylindric_extrusion_boxed_model::set_extrusion_radius: "
		<< "Invalid R " << r_ / CLHEP::mm << " mm !";
	throw runtime_error (message.str ());
      }
    __extrusion_radius = r_;
  }
     
  void cylindric_extrusion_boxed_model::set_mother_x (double x_)
  {
    assert_unconstructed("cylindric_extrusion_boxed_model::set_mother_x");

    if (x_ <= 0.0 * CLHEP::mm)
      {
	ostringstream message;
	message << "cylindric_extrusion_boxed_model::set_mother_x: "
		<< "Invalid X " << x_ / CLHEP::mm << " mm !";
	throw runtime_error (message.str ());
      }
    __mother_x = x_;
  }

  void cylindric_extrusion_boxed_model::set_mother_y (double y_)
  {
    assert_unconstructed("cylindric_extrusion_boxed_model::set_mother_y");

    if (y_ <= 0.0 * CLHEP::mm)
      {
	ostringstream message;
	message << "cylindric_extrusion_boxed_model::set_mother_y: "
		<< "Invalid Y " << y_ / CLHEP::mm << " mm !";
	throw runtime_error (message.str ());
      }
    __mother_y = y_;
  }

  void cylindric_extrusion_boxed_model::set_mother_z (double z_)
  {
    assert_unconstructed("cylindric_extrusion_boxed_model::set_mother_z");

    if (z_ <= 0.0 * CLHEP::mm)
      {
	ostringstream message;
	message << "cylindric_extrusion_boxed_model::set_mother_z: "
		<< "Invalid Z " << z_ / CLHEP::mm << " mm !";
	throw runtime_error (message.str ());
      }
     __mother_z = z_;
  }

  double cylindric_extrusion_boxed_model::get_extrusion_radius () const
  {
    return __extrusion_radius;
  }

  double cylindric_extrusion_boxed_model::get_mother_x () const
  {
    return __mother_x;
  }
  
  double cylindric_extrusion_boxed_model::get_mother_y () const
  {
    return __mother_y;
  }
  
  double cylindric_extrusion_boxed_model::get_mother_z () const
  {
    return __mother_z;
  }

  void cylindric_extrusion_boxed_model::set_material_name (const string & mn_)
  {
    assert_unconstructed("cylindric_extrusion_boxed_model::set_material_name");

     __material_name = mn_;
  }

  void cylindric_extrusion_boxed_model::set_mother_material_name (const string & mn_)
  {
    assert_unconstructed("cylindric_extrusion_boxed_model::set_mother_material_name");

     __mother_material_name = mn_;
  }

  const string & cylindric_extrusion_boxed_model::get_material_name () const
  {
    return __material_name;
  }

  const string & cylindric_extrusion_boxed_model::get_mother_material_name () const
  {
    return __mother_material_name;
  }
  
  string cylindric_extrusion_boxed_model::get_model_id () const
  {
    return "geomtools::cylindric_extrusion_boxed_model";
  }
  
  cylindric_extrusion_boxed_model::cylindric_extrusion_boxed_model () : i_boxed_model ()
  {
    __material_name = "";
    __mother_material_name = "";
    __mother_x = numeric_limits<double>::quiet_NaN ();
    __mother_y = numeric_limits<double>::quiet_NaN ();
    __mother_z = numeric_limits<double>::quiet_NaN ();
    __extrusion_radius = numeric_limits<double>::quiet_NaN ();
  }
  
  cylindric_extrusion_boxed_model::~cylindric_extrusion_boxed_model ()
  {
  }
  
  void cylindric_extrusion_boxed_model::_at_construct (const string & name_,
						       const datatools::utils::properties & config_,
						       models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: cylindric_extrusion_boxed_model::_at_construct: Entering..." << endl;
    set_name (name_);
    double mother_x;
    double mother_y;
    double mother_z;
    double extrusion_radius;
    string material_name = material::MATERIAL_REF_UNKWOWN;
    string mother_material_name = material::MATERIAL_REF_UNKWOWN;
    string lunit_str = "mm"; // default unit
 
    if (config_.has_key ("x"))
      {
	mother_x = config_.fetch_real ("x");
      }  
    else
      {
 	ostringstream message;
	message << "cylindric_extrusion_boxed_model::_at_construct: "
		<< "Missing 'x' property !";
	throw runtime_error (message.str ());
      }

    if (config_.has_key ("y"))
      {
	mother_y = config_.fetch_real ("y");
      }
    else
      {
 	ostringstream message;
	message << "cylindric_extrusion_boxed_model::_at_construct: "
		<< "Missing 'y' property !";
	throw runtime_error (message.str ());
      }

    if (config_.has_key ("z"))
      {
	mother_z = config_.fetch_real ("z");
      }
    else
      {
 	ostringstream message;
	message << "cylindric_extrusion_boxed_model::_at_construct: "
		<< "Missing 'z' property !";
	throw runtime_error (message.str ());
      }

    if (config_.has_key ("radius"))
      {
	extrusion_radius = config_.fetch_real ("radius");
      }
    else
      {
 	ostringstream message;
	message << "cylindric_extrusion_boxed_model::_at_construct: "
		<< "Missing 'radius' property !";
	throw runtime_error (message.str ());
      }

    if (config_.has_key ("length_unit"))
      {
	lunit_str = config_.fetch_string ("length_unit");
      }

    if (config_.has_key ("material.ref"))
      {
	material_name = config_.fetch_string ("material.ref");
      }
    else
      {
	ostringstream message;
	message << "cylindric_extrusion_boxed_model::_at_construct: "
		<< "Missing 'material.ref' property !";
	throw runtime_error (message.str ());
      }

    if (config_.has_key ("material.mother.ref"))
      {
	mother_material_name = config_.fetch_string ("material.mother.ref");
      }
    else
      {
	ostringstream message;
	message << "cylindric_extrusion_boxed_model::_at_construct: "
		<< "Missing 'material.ref' property !";
	throw runtime_error (message.str ());
      }
 
    double lunit = CLHEP::mm;
    lunit = datatools::utils::units::get_length_unit_from (lunit_str);

    mother_x *= lunit;
    mother_y *= lunit;
    mother_z *= lunit;
    extrusion_radius *= lunit;
    
    if (extrusion_radius > 0.5 * mother_x)
      {
	ostringstream message;
	message << "cylindric_extrusion_boxed_model::_at_construct: "
		<< "extrusion radius ("
		<< extrusion_radius / CLHEP::mm << " mm) is too large (X-axis)!";
	throw runtime_error (message.str ());
      }
    if (extrusion_radius > 0.5 * mother_y)
      {
	ostringstream message;
	message << "cylindric_extrusion_boxed_model::_at_construct: "
		<< "extrusion radius ("
		<< extrusion_radius / CLHEP::mm << " mm) is too large (Y-axis) !";
	throw runtime_error (message.str ());
      }

    set_material_name (material_name);
    set_mother_material_name (mother_material_name);
    set_mother_x (mother_x);
    set_mother_y (mother_y);
    set_mother_z (mother_z);
    set_extrusion_radius (extrusion_radius);

    __solid.reset ();
    __solid.set_x (get_mother_x ());
    __solid.set_y (get_mother_y ());
    __solid.set_z (get_mother_z ());
    if (! __solid.is_valid ())
      {
	throw runtime_error ("cylindric_extrusion_boxed_model::_at_construct: Invalid box dimensions !");
      }

    __mother_solid.set_x (get_mother_x ());
    __mother_solid.set_y (get_mother_y ());
    __mother_solid.set_z (get_mother_z ());

    __extrusion_solid.set_diameter (2 * get_extrusion_radius ());
    double eps = 1.0e-5 * CLHEP::mm;
    __extrusion_solid.set_z (get_mother_z () + eps);

    placement p1 (vector_3d (0, 0, 0), 0, 0, 0);
    placement p2 (vector_3d (0, 0, 0), 0, 0, 0);
    __extruded_solid.set_shape1 (__mother_solid, p1);
    __extruded_solid.set_shape2 (__extrusion_solid, p2);
    __extruded_solid.dump (std::clog);  
  
    /*
     */
    __extruded_log.set_name (i_model::make_logical_volume_name ("extruded_box"));
    __extruded_log.set_shape (__extruded_solid);
    __extruded_log.set_material_ref (__mother_material_name);
    __extruded_placement.set (vector_3d (0, 0, 0), 0, 0, 0);

    __extruded_phys.set_name (i_model::make_physical_volume_name ("extruded_box"));
    __extruded_phys.set_placement (__extruded_placement);
    __extruded_phys.set_logical (__extruded_log);
    __extruded_phys.set_mother (get_logical ());

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (material_name);

    if (devel) clog << "DEVEL: cylindric_extrusion_boxed_model::_at_construct: Exiting." << endl;
    return;
  }

  void cylindric_extrusion_boxed_model::tree_dump (ostream & out_, 
				const string & title_ , 
				const string & indent_, 
				bool inherit_) const
  {
     namespace du = datatools::utils;
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

     out_ << indent << i_tree_dumpable::tag 
	  << "Material name : '" << get_material_name () << "'" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Mother X : " << get_mother_x () / CLHEP::mm << " mm" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Mother Y : " << get_mother_y () / CLHEP::mm << " mm" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Mother Z : " << get_mother_z () / CLHEP::mm << " mm" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Extrusion radius : " << get_extrusion_radius () / CLHEP::mm << " mm" << endl;

     {
       out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	    << "Solid : " << endl;
       {
	 ostringstream indent_oss;
	 indent_oss << indent;
	 indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	 __solid.tree_dump (out_, "", indent_oss.str ());
       }   
     }

     return;
  }
    
} // end of namespace geomtools

// end of cylindric_extrusion_boxed_model.cc
