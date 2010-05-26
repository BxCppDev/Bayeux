// -*- mode: c++ ; -*- 
/* simple_boxed_model.cc
 */

#include <geomtools/simple_boxed_model.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<simple_boxed_model> simple_boxed_model::__CR;
  
  const geomtools::box & simple_boxed_model::get_box () const
  {
    return __solid;
  }
  
  const geomtools::box & simple_boxed_model::get_solid () const
  {
    return __solid;
  }
     
  void simple_boxed_model::set_x (double x_)
  {
    assert_unconstructed("simple_boxed_model::set_x");

    if (x_ <= 0.0 * CLHEP::mm)
      {
	ostringstream message;
	message << "simple_boxed_model::set_x: "
		<< "Invalid X " << x_ / CLHEP::mm << " mm !";
	throw runtime_error (message.str ());
      }
    __x = x_;
  }

  void simple_boxed_model::set_y (double y_)
  {
    assert_unconstructed("simple_boxed_model::set_y");

    if (y_ <= 0.0 * CLHEP::mm)
      {
	ostringstream message;
	message << "simple_boxed_model::set_y: "
		<< "Invalid Y " << y_ / CLHEP::mm << " mm !";
	throw runtime_error (message.str ());
      }
    __y = y_;
  }

  void simple_boxed_model::set_z (double z_)
  {
    assert_unconstructed("simple_boxed_model::set_z");

    if (z_ <= 0.0 * CLHEP::mm)
      {
	ostringstream message;
	message << "simple_boxed_model::set_z: "
		<< "Invalid Z " << z_ / CLHEP::mm << " mm !";
	throw runtime_error (message.str ());
      }
     __z = z_;
  }

  double simple_boxed_model::get_x () const
  {
    return __x;
  }
  
  double simple_boxed_model::get_y () const
  {
    return __y;
  }
  
  double simple_boxed_model::get_z () const
  {
    return __z;
  }

  void simple_boxed_model::set_material_name (const string & mn_)
  {
    assert_unconstructed("simple_boxed_model::set_material_name");

     __material_name = mn_;
  }

  const string & simple_boxed_model::get_material_name () const
  {
    return __material_name;
  }
  
  string simple_boxed_model::get_model_id () const
  {
    return "geomtools::simple_boxed_model";
  }
  
  simple_boxed_model::simple_boxed_model () : i_boxed_model ()
  {
    __material_name = "";
    __x = numeric_limits<double>::quiet_NaN ();
    __y = numeric_limits<double>::quiet_NaN ();
    __z = numeric_limits<double>::quiet_NaN ();
  }
  
  simple_boxed_model::~simple_boxed_model ()
  {
  }
  
  void simple_boxed_model::_at_construct (const string & name_,
				    const datatools::utils::properties & config_,
				    models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: simple_boxed_model::_at_construct: Entering..." << endl;
    set_name (name_);
    double x;
    double y;
    double z;
    string material_name = material::MATERIAL_REF_UNKWOWN;
    string lunit_str = "mm"; // default unit
 
    if (config_.has_key ("x"))
      {
	x = config_.fetch_real ("x");
      }  
    else
      {
 	ostringstream message;
	message << "simple_boxed_model::_at_construct: "
		<< "Missing 'x' property !";
	throw runtime_error (message.str ());
      }

    if (config_.has_key ("y"))
      {
	y = config_.fetch_real ("y");
      }
    else
      {
 	ostringstream message;
	message << "simple_boxed_model::_at_construct: "
		<< "Missing 'y' property !";
	throw runtime_error (message.str ());
      }

    if (config_.has_key ("z"))
      {
	z = config_.fetch_real ("z");
      }
    else
      {
 	ostringstream message;
	message << "simple_boxed_model::_at_construct: "
		<< "Missing 'z' property !";
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
	message << "simple_boxed_model::_at_construct: "
		<< "Missing 'material.ref' property !";
	throw runtime_error (message.str ());
      }
 
    double lunit = CLHEP::mm;
    lunit = datatools::utils::units::get_length_unit_from (lunit_str);

    x *= lunit;
    y *= lunit;
    z *= lunit;

    set_material_name (material_name);
    set_x (x);
    set_y (y);
    set_z (z);

    __solid.reset ();
    __solid.set_x (get_x ());
    __solid.set_y (get_y ());
    __solid.set_z (get_z ());
    if (! __solid.is_valid ())
      {
	throw runtime_error ("simple_boxed_model::_at_construct: Invalid box dimensions !");
      }
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (material_name);

    if (devel) clog << "DEVEL: simple_boxed_model::_at_construct: Exiting." << endl;
    return;
  }

  void simple_boxed_model::tree_dump (ostream & out_, 
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
	  << "X : " << get_x () / CLHEP::mm << " mm" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Y : " << get_y () / CLHEP::mm << " mm" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Z : " << get_z () / CLHEP::mm << " mm" << endl;

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

// end of simple_boxed_model.cc
