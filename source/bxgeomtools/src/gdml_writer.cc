// -*- mode: c++ ; -*- 
/* gdml_writer.cc
 */

#include <geomtools/gdml_writer.h>

namespace geomtools {

  using namespace std;

  const string gdml_writer::DEFAULT_XML_VERSION = "1.0";
  const string gdml_writer::DEFAULT_XML_ENCODING = "UTF-8";
  const string gdml_writer::DEFAULT_GDML_SCHEMA   = "gdml.xsd";

  const string gdml_writer::DEFINE_SECTION    = "define";
  const string gdml_writer::MATERIALS_SECTION = "materials";
  const string gdml_writer::SOLIDS_SECTION    = "solids";
  const string gdml_writer::STRUCTURE_SECTION = "structure";
  const string gdml_writer::SETUP_SECTION     = "setup";

  bool gdml_writer::__g_using_html_symbols = false;

  void gdml_writer::set_using_html_symbols (bool u_)
  {
    __g_using_html_symbols = u_;
  }

  bool gdml_writer::is_using_html_symbols ()
  {
    return __g_using_html_symbols;
  }

  string gdml_writer::to_html (const string & name_)
  {
    string str1 = name_;
    if (gdml_writer::is_using_html_symbols ())
      {
	boost::replace_all (str1, "." , "&#46;");
	boost::replace_all (str1, "[" , "&#91;");
	boost::replace_all (str1, "]" , "&#93;");
	boost::replace_all (str1, "{" , "&#123;");
	boost::replace_all (str1, "}" , "&#124;");
      }
    return str1;
  }

  string gdml_writer::to_ascii (const string & name_)
  {
    string str1 = name_;
    //if (is_using_html_symbols ())
      {
	boost::replace_all (str1 , "&#46;"  , ".");
	boost::replace_all (str1 , "&#91;"  , "[");
	boost::replace_all (str1 , "&#93;"  , "]");
	boost::replace_all (str1 , "&#123;" , "{");
	boost::replace_all (str1 , "&#124;" , "}");
      }
    return str1;
  }

  const ostringstream & gdml_writer::get_stream (const string & section_) const
  {
    return _get_stream (section_);
  }

  const ostringstream & gdml_writer::_get_stream (const string & section_) const
  {
    __streams_col_t::const_iterator i = __streams.find (section_);
    if (i == __streams.end ())
      {
	ostringstream message;
	message << "gdml_writer::_get_stream: " 
		<< "Invalid section '" << section_ 
		<< "' !";
	throw runtime_error (message.str ());		
      }
    return *(i->second);
  }

  ostringstream & gdml_writer::_get_stream (const string & section_)
  {
    __streams_col_t::iterator i = __streams.find (section_);
    if (i == __streams.end ())
      {
	ostringstream message;
	message << "gdml_writer::_get_stream: " 
		<< "Invalid section '" << section_ 
		<< "' !";
	throw runtime_error (message.str ());		
      }
    return *(i->second);
  }

  bool gdml_writer::has_external_materials_stream () const
  {
    return __external_materials_stream != 0;
  }

  void gdml_writer::reset_external_materials_stream () 
  {
    __external_materials_stream = 0;
  }

  bool gdml_writer::is_initialized () const
  {
    return __initialized;
  }

  // ctor:
  gdml_writer::gdml_writer ()
  {
    __initialized = false;
    __external_materials_stream = 0;
    init ();
  }
  
  gdml_writer::~gdml_writer ()
  {
    if (__initialized) 
      {
	reset ();
      }
  }

  void gdml_writer::init ()
  {
    if (__initialized) 
      {
	clog << "WARNING: gdml_writer::init: Already initialized !" << endl;
      }
    __streams[DEFINE_SECTION]    = new ostringstream;
    __streams[MATERIALS_SECTION] = new ostringstream;
    __streams[SOLIDS_SECTION]    = new ostringstream;
    __streams[STRUCTURE_SECTION] = new ostringstream;
    __streams[SETUP_SECTION]     = new ostringstream;
    __initialized = true;
   }
  
  void gdml_writer::reset ()
  {
    if (! __initialized) 
      {
	return;
      }
    if (__streams.size())
      {
	for (__streams_col_t::iterator i = __streams.begin ();
	     i != __streams.end ();
	     i++)
	  {
	    delete i->second;
	  }
	__streams.clear ();
      }
    reset_external_materials_stream ();
    __initialized = false;
  }

  /*** Defines ***/
  
  void gdml_writer::add_constant (const string & name_, 
				  double value_)
  {
    _get_stream (DEFINE_SECTION) << "<constant" 
				 << " name=" << '"' << to_html (name_) << '"' 
				 << " value="  << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << value_ << '"';
    _get_stream (DEFINE_SECTION) << " />" << endl;
    _get_stream (DEFINE_SECTION) << endl;
  }
  
  void gdml_writer::add_quantity (const string & name_, 
				  const string & quantity_type_, 
				  const string & unit_str_, 
				  double value_)
  {
    double unit = units::get_unit_from (quantity_type_, unit_str_);
    _get_stream (DEFINE_SECTION) << "<quantity" 
		    << " name=" << '"' << to_html (name_) << '"' 
		    << " type=" << '"' << quantity_type_ << '"' 
		    << " value="  << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << (value_ / unit) << '"'
		    << " unit=" << '"' << unit_str_ << '"' 
		    << " />" << endl;
    _get_stream (DEFINE_SECTION) << endl;
  }

  void gdml_writer::add_variable (const string & name_, 
				  double value_)
  {
    _get_stream (DEFINE_SECTION) << "<variable" 
		    << " name=" << '"' << to_html (name_) << '"' 
		    << " value="  << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << value_ << '"'
		    << " />" << endl;
    _get_stream (DEFINE_SECTION) << endl;
  }

  void gdml_writer::add_variable (const string & name_, 
				  const string & expr_value_)
  {
    _get_stream (DEFINE_SECTION) << "<variable" 
		    << " name=" << '"' << to_html (name_) << '"' 
		    << " value="  << '"' << expr_value_ << '"' 
		    << " />" << endl;
    _get_stream (DEFINE_SECTION) << endl;
  }

  void gdml_writer::add_position (const string & name_, 
				  double x_, double y_, double z_, 
				  const string & unit_str_)
  {
    //clog << "DEVEL: gdml_writer::add_position: Entering..." << endl;
    double unit = units::get_length_unit_from (unit_str_);
    _get_stream (DEFINE_SECTION) << "<position" 
		    << " name=" << '"' << to_html (name_) << '"' 
		    << " x=" << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << x_ / unit << '"'
		    << " y=" << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << y_ / unit << '"'
		    << " z=" << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << z_ / unit << '"'
		    << " unit=" << '"' << unit_str_ << '"' 
		    << " />" << endl;
    _get_stream (DEFINE_SECTION) << endl;
  }
  
  void gdml_writer::add_position (const string & name_, 
				  const vector_3d & v_, 
				  const string & unit_str_)
  {
    add_position (name_, v_.x (), v_.y (), v_.z (), unit_str_);
  }

  void gdml_writer::add_rotation (const string & name_, 
				  const string & axis_, 
				  double angle_,
				  const string & unit_str_)
  {
    double angle_unit = units::get_angle_unit_from (unit_str_);
    if ((axis_ != "x") && (axis_ != "y") && (axis_ != "z"))
      {
	ostringstream message;
	message << "gdml_writer::add_rotation: " 
		<< "Invalid rotation axis '" << axis_ 
		<< "' for rotation '" << name_ << "' !";
	throw runtime_error (message.str ());		
      }
    _get_stream (DEFINE_SECTION) << "<rotation" 
		    << " name=" << '"' << to_html (name_) << '"' 
		    << " " << axis_ << "=" << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << (angle_ / angle_unit) << '"' 
		    << " unit=" << '"' << unit_str_ << '"' 
		    << " />" << endl;
    _get_stream (DEFINE_SECTION) << endl;
  }

  void gdml_writer::add_rotation (const string & name_, 
				  const rotation_3d & rot_, 
				  const string & unit_str_)
  {
    double angle_unit = units::get_angle_unit_from (unit_str_);
    _get_stream (DEFINE_SECTION) << "<rotation" 
				 << " name=" << '"' << to_html (name_) << '"'; 

    double xx = rot_.xx ();
    double yx = rot_.yx ();
    double yy = rot_.yy ();
    double yz = rot_.yz ();
    double zx = rot_.zx ();
    double zy = rot_.zy ();
    double zz = rot_.zz ();
    double a, b, c;
    {
      // from a sample by Evgueni Tcherniaev:
      //  see also: http://en.wikipedia.org/wiki/Euler_angles -> Table of matrices
      double cosb = sqrt (xx * xx + yx * yx); 
      if (cosb > 16 * numeric_limits<double>::epsilon()) 
	{
	  a = atan2 ( zy, zz);
	  b = atan2 (-zx, cosb);
	  c = atan2 ( yx, xx);
	}
      else
	{
	  a = atan2 (-yz, yy);
	  b = atan2 (-zx, cosb);
	  c = 0.;
	}
    }
    
    _get_stream (DEFINE_SECTION) << " " << "x" << "=" << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << (a / angle_unit) << '"'; 
 
    _get_stream (DEFINE_SECTION) << " " << "y" << "=" << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << (b / angle_unit) << '"'; 
 
    _get_stream (DEFINE_SECTION) << " " << "z" << "=" << '"';
    _get_stream (DEFINE_SECTION).precision (15);
    _get_stream (DEFINE_SECTION) << (c / angle_unit) << '"'; 

    _get_stream (DEFINE_SECTION) << " unit=" << '"' << unit_str_ << '"' 
				 << " />" << endl;
    _get_stream (DEFINE_SECTION) << endl;
  }

  /*** Materials ***/

  void gdml_writer::add_isotope (const string & name_, 
				 size_t atomic_number_,
				 size_t number_of_nucleons_,
				 double a_)
  {
    _get_stream (MATERIALS_SECTION) << "<isotope" 
		       << " name=" << '"' << to_html (name_) << '"' 
		       << " Z=" << '"' << atomic_number_ << '"' 
		       << " N=" << '"' << number_of_nucleons_ << '"' 
		       << " >" << endl;
    _get_stream (MATERIALS_SECTION) << "  <atom"
		       << " type=" << '"' << "A" << '"'
		       << " value=" << '"';
    _get_stream (MATERIALS_SECTION).precision (15);
    _get_stream (MATERIALS_SECTION) << a_ << '"' << " >" << endl;
    _get_stream (MATERIALS_SECTION) << "</isotope>" << endl; 
    _get_stream (MATERIALS_SECTION) << endl; 
  }

  void gdml_writer::add_element (const string & name_, 
				 size_t atomic_number_,
				 const string & formula_,
				 double a_)
  {
    _get_stream (MATERIALS_SECTION) << "<element" 
		       << " name=" << '"' << to_html (name_) << '"' 
		       << " Z=" << '"' << atomic_number_ << '"' 
		       << " formula=" << '"' << formula_ << '"' 
		       << " >" << endl;
    _get_stream (MATERIALS_SECTION) << "  <atom"
		       << " value=" << '"';
    _get_stream (MATERIALS_SECTION).precision (15);
    _get_stream (MATERIALS_SECTION) << a_ << '"' << " />" << endl;
    _get_stream (MATERIALS_SECTION) << "</element>" << endl; 
    _get_stream (MATERIALS_SECTION) << endl; 
   }

  void gdml_writer::add_element (const string & name_, 
				 const map<string, double> & fractions_)
  {
    ostringstream materials_stream;
    materials_stream << "<element" 
		       << " name=" << '"' << to_html (name_) << '"' 
		       << " >" << endl;
    double s = 0.0;
    for (map<string, double>::const_iterator i = fractions_.begin ();
	 i != fractions_.end ();
	 i++)
      {
	string ref = i->first;
	double n = i->second;
	if (n < 0.0 || n > 1.0)
	  {    
	    ostringstream message;
	    message << "gdml_writer::add_element: " 
		    << "Invalid fraction value '" << n 
		    << "' for referenced '" << ref << "' in element '" 
		    << name_ << "' !";
	    throw runtime_error (message.str ());	
	  }
	s += n;
	
	materials_stream << "  <fraction" 
			   << " ref=" << '"' << ref << '"'
			   << " n=" << '"';
	materials_stream.precision (15);
	materials_stream << n << '"' << " />" << endl;
      }
    if (s > 1.0)
      {    ostringstream message;
	message << "gdml_writer::add_element: " 
		<< "Invalid fraction sum in element '" 
		<< name_ << "' !";
	throw runtime_error (message.str ());	
      }
  
    materials_stream << "</element>" << endl; 

    _get_stream (MATERIALS_SECTION) << materials_stream.str ();
    _get_stream (MATERIALS_SECTION) << endl; 
  }

  void gdml_writer::add_element (const string & name_, 
				 const string & ref_)
  {
    map<string, double> m;
    m[ref_] = 1.0;
    add_element (name_, m);
  }
  
  void gdml_writer::add_element (const string & name_, 
				 const string & ref1_, double fraction1_, 
				 const string & ref2_, double fraction2_)
  {
    map<string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    add_element (name_, m);
  }

  void gdml_writer::add_element (const string & name_, 
				 const string & ref1_, double fraction1_, 
				 const string & ref2_, double fraction2_, 
				 const string & ref3_, double fraction3_)
  {
    map<string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    add_element (name_, m);
  }

  void gdml_writer::add_element (const string & name_, 
				 const string & ref1_, double fraction1_, 
				 const string & ref2_, double fraction2_, 
				 const string & ref3_, double fraction3_, 
				 const string & ref4_, double fraction4_)
  {
    map<string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    m[ref4_] = fraction4_;
    add_element (name_, m);
  }
  
  void gdml_writer::add_element (const string & name_, 
				 const string & ref1_, double fraction1_, 
				 const string & ref2_, double fraction2_, 
				 const string & ref3_, double fraction3_, 
				 const string & ref4_, double fraction4_, 
				 const string & ref5_, double fraction5_)
  {
    map<string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    m[ref4_] = fraction4_;
    m[ref5_] = fraction5_;
    add_element (name_, m);
  }
 
  void gdml_writer::add_material (const string & name_, 
				  double atomic_number_,
				  double density_,
				  double a_)
  {
     _get_stream (MATERIALS_SECTION) << "<material" 
			<< " name=" << '"' << to_html (name_) << '"' 
			<< " Z=" << '"';
     _get_stream (MATERIALS_SECTION).precision (15);
     _get_stream (MATERIALS_SECTION) << atomic_number_ << '"' << " >" << endl;
     
     _get_stream (MATERIALS_SECTION) << "  <D"
			<< " value=" << '"';
     _get_stream (MATERIALS_SECTION).precision (15);
     _get_stream (MATERIALS_SECTION) << (density_ / (CLHEP::g / CLHEP::cm3)) 
			<< '"' 
			<< " unit=" << '"' << "g/cm3" << '"' 
			<< " />" << endl;

     _get_stream (MATERIALS_SECTION) << "  <atom"
			<< " value=" << '"';
     _get_stream (MATERIALS_SECTION).precision (15);
     _get_stream (MATERIALS_SECTION) << a_ << '"' << " />" << endl;
     
     _get_stream (MATERIALS_SECTION) << "</material >" << endl; 
     _get_stream (MATERIALS_SECTION) << endl; 
  }

  void gdml_writer::add_material (const string & name_,
				  const string & formula_,
				  double density_,  
				  const map<string, double> & fractions_)
  {
    ostringstream materials_stream;
    materials_stream << "<material" 
		       << " name=" << '"' << to_html (name_) << '"' 
		       << " formula=" << '"' << formula_ << '"' 
		       << " >" << endl;
     
    materials_stream << "  <D"
		       << " value=" << '"';
    materials_stream.precision (15);
    materials_stream << (density_ / (CLHEP::g / CLHEP::cm3)) 
		     << '"' 
		     << " unit=" << '"' << "g/cm3" << '"' 
		     << " />" << endl;

    double s = 0.0;
    for (map<string, double>::const_iterator i = fractions_.begin ();
	 i != fractions_.end ();
	 i++)
      {
	string ref = i->first;
	double n = i->second;
	if (n < 0.0 || n > 1.0)
	  {    
	    ostringstream message;
	    message << "gdml_writer::add_material: " 
		    << "Invalid fraction value '" << n 
		    << "' for referenced '" << ref << "' in element '" 
		    << name_ << "' !";
	    throw runtime_error (message.str ());	
	  }
	s += n;
	
	materials_stream << "  <fraction" 
			   << " ref=" << '"' << ref << '"'
			   << " n=" << '"';
	materials_stream.precision (15);
	materials_stream << n << '"' << " />" << endl;
      }
    if (s > 1.0)
      {    ostringstream message;
	message << "gdml_writer::add_material: " 
		<< "Invalid fraction sum in material '" 
		<< name_ << "' !";
	throw runtime_error (message.str ());	
      }
  
    materials_stream << "</material>" << endl; 

    _get_stream (MATERIALS_SECTION) << materials_stream.str ();
    _get_stream (MATERIALS_SECTION) << endl; 
  }
  

  void gdml_writer::add_material (const string & name_,
				  const string & formula_,
				  double density_,  
				  const map<string, size_t> & composites_)
  {
    ostringstream materials_stream;
    materials_stream << "<material" 
		       << " name=" << '"' << to_html (name_) << '"' 
		       << " formula=" << '"' << formula_ << '"' 
		       << " >" << endl;
     
    materials_stream << "  <D"
		       << " value=" << '"';
    materials_stream.precision (15);
    materials_stream << (density_ / (CLHEP::g / CLHEP::cm3)) 
		     << '"' 
		     << " unit=" << '"' << "g/cm3" << '"' 
		     << " />" << endl;

    for (map<string, size_t>::const_iterator i = composites_.begin ();
	 i != composites_.end ();
	 i++)
      {
	string ref = i->first;
	size_t n = i->second;
	if (n == 0)
	  {    
	    ostringstream message;
	    message << "gdml_writer::add_material: " 
		    << "Invalid composite value '" << n 
		    << "' for referenced '" << ref << "' in element '" 
		    << name_ << "' !";
	    throw runtime_error (message.str ());	
	  }
	materials_stream << "  <composite" 
			   << " ref=" << '"' << ref << '"'
			   << " n=" << '"' << n << '"' << " />" << endl;;
      }
    materials_stream << "</material>" << endl; 
    _get_stream (MATERIALS_SECTION) << materials_stream.str ();
    _get_stream (MATERIALS_SECTION) << endl; 
  }
 
  /*** solid ***/
 
  bool gdml_writer::solid_type_is_valid (const string & solid_type_)
  {
    if (solid_type_ == "box") 
      {
	return true;
      }
    if (solid_type_ == "cone")
      {
	return true;
      }
    if (solid_type_ == "ellipsoid")
      {
	return true;
      }
    if (solid_type_ == "eltube")
      {
	return true;
      }
    if (solid_type_ == "elcone")
      {
	return true;
      }
    if (solid_type_ == "orb")
      {
	return true;
      }
    if (solid_type_ == "para")
      {
	return true;
      }
    if (solid_type_ == "polycone")
      {
	return true;
      }
    if (solid_type_ == "polyhedra")
      {
	return true;
      }
    if (solid_type_ == "sphere")
      {
	return true;
      }
    if (solid_type_ == "torus")
      {
	return true;
      }
    if (solid_type_ == "trd")
      {
	return true;
      }
    if (solid_type_ == "trap")
      {
	return true;
      }
    if (solid_type_ == "hype")
      {
	return true;
      }
    if (solid_type_ == "tube")
      {
	return true;
      }
    if (solid_type_ == "twistedbox")
      {
	return true;
      }
    if (solid_type_ == "twistedtrd")
      {
	return true;
      }
    if (solid_type_ == "twistedtrap")
      {
	return true;
      }
    if (solid_type_ == "twistedtube")
      {
	return true;
      }
    if (solid_type_ == "xtru")
      {
	return true;
      }
    if (solid_type_ == "arb8")
      {
	return true;
      }
    if (solid_type_ == "tessellated")
      {
	return true;
      }
    if (solid_type_ == "tet")
      {
	return true;
      }
    if (solid_type_ == "union")
      {
	return true;
      }
    if (solid_type_ == "subtraction")
      {
	return true;
      }
    if (solid_type_ == "intersection")
      {
	return true;
      }
    return false;
  }

  void gdml_writer::add_gdml_box (const string & name_, 
				  double x_, double y_, double z_, 
				  const string & lunit_str_)
  {
    double lunit = units::get_length_unit_from (lunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "box"
		  << " name=" << '"' << to_html (name_) << '"';

    solids_stream << " x=" << '"';
    solids_stream.precision (15);
    solids_stream << x_ / lunit << '"';

    solids_stream << " y=" << '"';
    solids_stream.precision (15);
    solids_stream << y_ / lunit << '"';

    solids_stream << " z=" << '"';
    solids_stream.precision (15);
    solids_stream << z_ / lunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " />" << endl << endl; 

    _get_stream (SOLIDS_SECTION) << solids_stream.str ();
  }
  

  void gdml_writer::add_box (const string & name_, 
			     const box & b_,
			     const string & lunit_str_)
  {
    add_gdml_box (name_, b_.get_x (), b_.get_y (), b_.get_z (), lunit_str_);
  }
    

  void gdml_writer::add_gdml_orb (const string & name_, 
				  double r_,
				  const string & lunit_str_)
  {
    double lunit = units::get_length_unit_from (lunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "sphere"
		  << " name=" << '"' << to_html (name_) << '"';

    solids_stream << " r=" << '"';
    solids_stream.precision (15);
    solids_stream << r_ / lunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';

    solids_stream << " />" << endl << endl; 
    _get_stream (SOLIDS_SECTION) << solids_stream.str ();    
  }
    

  void gdml_writer::add_gdml_sphere (const string & name_, 
				     double rmin_, double rmax_, 
				     double start_phi_, double delta_phi_,
				     double start_theta_, double delta_theta_,
				     const string & lunit_str_,
				     const string & aunit_str_)
  {
    double lunit = units::get_length_unit_from (lunit_str_);
    double aunit = units::get_angle_unit_from (aunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "sphere"
		  << " name=" << '"' << to_html (name_) << '"';

    if (rmin_ > 0.0)
      {
	solids_stream << " rmin=" << '"';
	solids_stream.precision (15);
	solids_stream << rmin_ / lunit << '"';
      }

    solids_stream << " rmax=" << '"';
    solids_stream.precision (15);
    solids_stream << rmax_ / lunit << '"';

    if (start_phi_ > 0.0)
      {
	solids_stream << " startphi=" << '"';
	solids_stream.precision (15);
	solids_stream << start_phi_ / aunit << '"';
      }

    solids_stream << " deltaphi=" << '"';
    solids_stream.precision (15);
    solids_stream << delta_phi_ / aunit << '"';

    if (start_theta_ > 0.0)
      {
	solids_stream << " starttheta=" << '"';
	solids_stream.precision (15);
	solids_stream << start_theta_ / aunit << '"';
      }

    solids_stream << " deltatheta=" << '"';
    solids_stream.precision (15);
    solids_stream << delta_theta_ / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " />" << endl << endl; 
    _get_stream (SOLIDS_SECTION) << solids_stream.str ();    
  }


  void gdml_writer::add_gdml_tube (const string & name_, 
				   double rmin_, double rmax_, double z_, 
				   double start_phi_, double delta_phi_,
				   const string & lunit_str_,
				   const string & aunit_str_)
  {
    double lunit = units::get_length_unit_from (lunit_str_);
    double aunit = units::get_angle_unit_from (aunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "tube"
		  << " name=" << '"' << to_html (name_) << '"';

    if (rmin_ > 0.0)
      {
	solids_stream << " rmin=" << '"';
	solids_stream.precision (15);
	solids_stream << rmin_ / lunit << '"';
      }

    solids_stream << " rmax=" << '"';
    solids_stream.precision (15);
    solids_stream << rmax_ / lunit << '"';

    solids_stream << " z=" << '"';
    solids_stream.precision (15);
    solids_stream << z_ / lunit << '"';

    if (start_phi_ != 0.0)
      {
	solids_stream << " startphi=" << '"';
	solids_stream.precision (15);
	solids_stream << start_phi_ / aunit << '"';
      }

    solids_stream << " deltaphi=" << '"';
    solids_stream.precision (15);
    solids_stream << delta_phi_ / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " />" << endl << endl; 
    _get_stream (SOLIDS_SECTION) << solids_stream.str ();
  }
  
  void gdml_writer::add_gdml_union (const string & name_, 
				    const string & first_ref_, 
				    const string & second_ref_, 
				    const string & position_ref_, 
				    const string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean (name_, "union", first_ref_, second_ref_, 
				   position_ref_, rotation_ref_);
  }
  
  void gdml_writer::add_gdml_subtraction (const string & name_, 
				    const string & first_ref_, 
				    const string & second_ref_, 
				    const string & position_ref_, 
				    const string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean (name_, "subtraction", first_ref_, second_ref_, 
				   position_ref_, rotation_ref_);
  }
  
  void gdml_writer::add_gdml_intersection (const string & name_, 
					   const string & first_ref_, 
					   const string & second_ref_, 
					   const string & position_ref_, 
					   const string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean (name_, "intersection", first_ref_, second_ref_, 
				   position_ref_, rotation_ref_);
  }
  
  void gdml_writer::add_gdml_boolean (const string & name_, 
				      const string & boolean_type_, 
				      const string & first_ref_, 
				      const string & second_ref_, 
				      const string & position_ref_, 
				      const string & rotation_ref_)
  {
    if ((boolean_type_ != "union") 
	&& (boolean_type_ != "subtraction") && (boolean_type_ != "intersection"))
      {
	ostringstream message;
	message << "gdml_writer::add_gdml_boolean: " 
		<< "Invalid boolean solid type '" << boolean_type_ 
		<< "' for solid '" << name_ << "' !";
	throw runtime_error (message.str ());		
      }
    ostringstream solids_stream;
    solids_stream << "<" <<  boolean_type_
		  << " name=" << '"' << to_html (name_) << '"'
		  << " >" << endl;

    solids_stream << "  <first ref=" << '"' <<  to_html (first_ref_) << '"' << " />" << endl;
    solids_stream << "  <second ref=" << '"' <<  to_html (second_ref_) << '"' << " />" << endl;
    solids_stream << "  <positionref ref=" << '"' <<  to_html (position_ref_) << '"' << " />" << endl;
    solids_stream << "  <rotationref ref=" << '"' <<  to_html (rotation_ref_) << '"' << " />" << endl;

    solids_stream << "</" <<  boolean_type_ << ">" << endl << endl; 
    _get_stream (SOLIDS_SECTION) << solids_stream.str (); 
  }

  void gdml_writer::add_cylinder (const string & name_, 
				  const cylinder & c_,
				  const string & lunit_str_,
				  const string & aunit_str_)
  {
    add_gdml_tube (name_, 
		   0.0, 
		   c_.get_radius (), 
		   c_.get_z (), 
		   0.0, 
		   2. * M_PI * CLHEP::radian, 
		   lunit_str_, 
		   aunit_str_);
  }


  void gdml_writer::add_tube (const string & name_, 
			      const tube & t_,
			      const string & lunit_str_,
			      const string & aunit_str_)
  {
    add_gdml_tube (name_, 
		   t_.get_inner_r (), 
		   t_.get_outer_r (), 
		   t_.get_z (), 
		   0.0, 
		   2. * M_PI * CLHEP::radian, 
		   lunit_str_, 
		   aunit_str_);
  }


  void gdml_writer::add_sphere (const string & name_, 
				const sphere & s_,
				const string & lunit_str_,
				const string & aunit_str_)
  {
    add_gdml_orb (name_, 
		  s_.get_r (), 
		  lunit_str_);
  }

  /*
  void gdml_writer::add_solid (const string & name_, 
			       const string & solid_type_,
			       const datatools::utils::properties & params_)
  {
    if (! gdml_writer::solid_type_is_valid (solid_type_))
      {
	    ostringstream message;
	    message << "gdml_writer::add_solid: " 
		    << "Invalid solit type '" << solid_type_ 
		    << "' for solid '"
		    << name_ << "' !";
	    throw runtime_error (message.str ());		
      }
    bool has_length = true;
    bool has_angle = false;
    size_t count_length = 0;
    size_t count_angle = 0;

    ostringstream solids_stream;
    solids_stream << "<" <<  solid_type_
		  << " name=" << '"' << to_html (name_) << '"';
    datatools::utils::properties::keys_col_t keys;
    params_.keys (keys);
    for (datatools::utils::properties::keys_col_t::const_iterator i = keys.begin ();
	 i != keys.end ();
	 i++)
      {
	if (params_.is_integer (*i))
	  {
	    solids_stream << " " << *i << "=" << '"';
	    int ivalue = params_.fetch_integer (*i);
	    solids_stream << ivalue;
	  }
	else if (params_.is_real (*i))
	  {
	    bool is_length = false;
	    bool is_angle = false;
	    string propname;
	    if (*i.substr (0,7) == "length.")
	      {
		is_length = true;
		propname = *i.substr (7);
	      }
	    if (*i.substr (0,6) == "angle.")
	      {
		is_angle = true;
		propname = *i.substr (6);
	      }
	    if (! propname.empty ())
	      {
		double dvalue = params_.fetch_real (*i);
		solids_stream.precision (15);
		if (is_length)
		  {
		    count_length++;
		    solids_stream << dvalue / CLHEP::mm;
		  }
		if (is_angle)
		  {
		    count_angle++;
		    solids_stream << dvalue / CLHEP::degree;
		  }
	      }
	  }
	solids_stream << '"';
      } 
    if (count_length)
      {
	solids_stream << " lunit=" << '"' << "mm" << '"';
      }
    if (count_angle)
      {
	solids_stream << " aunit=" << '"' << "degree" << '"';
      }
    solids_stream << " />" << endl; 
    _get_stream (SOLIDS_SECTION) << solids_stream.str ();
  }
  */
 
  /*** Structure ***/

  gdml_writer::physvol::physvol (const string & volumeref_,
				 const string & positionref_,
				 const string & rotationref_,
				 const string & scaleref_)
  {
    volumeref = volumeref_;
    positionref = positionref_;
    rotationref = rotationref_;
    scaleref = scaleref_;
  } 

  const string gdml_writer::replicavol::REPLICATED_ALONG_AXIS = "replicated_along_axis";

  void gdml_writer::add_replica_volume (const string & name_,
					const string & material_ref_,
					const string & solid_ref_,
					const replicavol & replicavol_,
					const string & lunit_str_,
					const string & aunit_str_,
					const map<string, string> & aux_)
  {
    double lunit = units::get_length_unit_from (lunit_str_);
    _get_stream (STRUCTURE_SECTION) << "<volume" << " name=" << '"' << to_html (name_) << '"' << " >" << endl;
    _get_stream (STRUCTURE_SECTION) << "  <materialref" << " ref=" << '"' << to_html (material_ref_) << '"' << " />" << endl;
    _get_stream (STRUCTURE_SECTION) << "  <solidref   " << " ref=" << '"' << to_html (solid_ref_) << '"' << " />" << endl;

    _get_stream (STRUCTURE_SECTION) << "  <replicavol number=" << '"' << replicavol_.number << '"' << " >" << endl;
    _get_stream (STRUCTURE_SECTION) << "    <volumeref  " << " ref=" << '"' << to_html (replicavol_.volumeref) << '"' << " />" << endl;

    _get_stream (STRUCTURE_SECTION) << "    <" << replicavol_.mode << ">" << endl;

    _get_stream (STRUCTURE_SECTION) << "      <direction  " 
				    << replicavol_.direction<< "=" << '"' << "1" << '"' << " />" << endl;

    _get_stream (STRUCTURE_SECTION) << "      <width" 
				    << " value=" 
				    << '"' << replicavol_.width / lunit << '"' 
				    << " unit=" 
				    << '"' << lunit_str_ << '"' 
				    << " />" << endl;
 
    _get_stream (STRUCTURE_SECTION) << "      <offset" 
				    << " value=" 
				    << '"' << replicavol_.offset / lunit << '"' 
				    << " unit=" 
				    << '"' << lunit_str_ << '"' 
				    << " />" << endl;

    _get_stream (STRUCTURE_SECTION) << "    </" << replicavol_.mode << ">" << endl;
    _get_stream (STRUCTURE_SECTION) << "  </replicavol >" << endl << endl; 

    add_volume_auxiliaries (aux_);

    _get_stream (STRUCTURE_SECTION) << "</volume>" << endl << endl; 
  }
  
  void gdml_writer::add_volume_auxiliaries (const map<string,string> & aux_)
  {
    _get_stream (STRUCTURE_SECTION) << endl;  
    for (map<string,string>::const_iterator i = aux_.begin ();
	 i != aux_.end ();
	 i++)
      {
	_get_stream (STRUCTURE_SECTION) << "  <auxiliary "
					<< "auxtype=\""
					<< i->first
					<< "\" "
					<< " auxvalue=\""
					<< i->second
					<< "\" />" << endl;
      }
    _get_stream (STRUCTURE_SECTION) << endl;
    return;
  }

  void gdml_writer::add_volume (const string & name_,
				const string & material_ref_,
				const string & solid_ref_,
				const list<physvol> & phys_vols_,
				const map<string, string> & aux_)
  {
    _get_stream (STRUCTURE_SECTION) << "<volume" << " name=" << '"' << to_html (name_) << '"' << " >" << endl;
    _get_stream (STRUCTURE_SECTION) << "  <materialref" << " ref=" << '"' << to_html (material_ref_) << '"' << " />" << endl;
    _get_stream (STRUCTURE_SECTION) << "  <solidref   " << " ref=" << '"' << to_html (solid_ref_) << '"' << " />" << endl;

    if ( phys_vols_.size ())
      {
	_get_stream (STRUCTURE_SECTION) << endl;
      }
    for (list<physvol>::const_iterator i = phys_vols_.begin ();
	 i != phys_vols_.end ();
	 i++)
      {
	_get_stream (STRUCTURE_SECTION) << "  <physvol>" << endl;
	_get_stream (STRUCTURE_SECTION) << "    <volumeref  " << " ref=" << '"' << i->volumeref << '"' << " />" << endl;
	_get_stream (STRUCTURE_SECTION) << "    <positionref" << " ref=" << '"' << i->positionref << '"' << " />" << endl;
	if (! i->rotationref.empty ())
	  {
	    _get_stream (STRUCTURE_SECTION) << "    <rotationref" << " ref=" << '"' << i->rotationref << '"' << " />" << endl;
	  }
	if (! i->scaleref.empty ())
	  {
	    _get_stream (STRUCTURE_SECTION) << "    <scaleref  " << " ref=" << '"' << i->scaleref << '"' << " />" << endl;
	  }
	_get_stream (STRUCTURE_SECTION) << "  </physvol>" << endl << endl;
      }

    add_volume_auxiliaries (aux_);

    _get_stream (STRUCTURE_SECTION) << "</volume>" << endl << endl; 
  }

  void gdml_writer::add_volume (const string & name_,
				const string & material_ref_,
				const string & solid_ref_)
  {
    map<string, string> aux;
    add_volume (name_, material_ref_, solid_ref_, aux);
  }

  void gdml_writer::add_volume (const string & name_,
				const string & material_ref_,
				const string & solid_ref_,
				const map<string, string> & aux_)
  {
    //cerr << "DEVEL: gdml_writer::add_volume: " << "Entering..." << endl;
    list<physvol> empty_phys_vols;
    add_volume (name_, material_ref_, solid_ref_, empty_phys_vols, aux_);
    //cerr << "DEVEL: gdml_writer::add_volume: " << "Exiting." << endl;
  }
  
  void gdml_writer::add_volume (const string & name_,
				const string & material_ref_,
				const string & solid_ref_,
				const list<physvol> & phys_vols_)
  {
    map<string, string> aux;
    add_volume (name_, 
		material_ref_, 
		solid_ref_, 
		phys_vols_, 
		aux);
  }
    
  void gdml_writer::add_replica_volume (const string & name_,
					const string & material_ref_,
					const string & solid_ref_,
					const replicavol & replicavol_,
					const string & lunit_str_,
					const string & aunit_str_)
  {
    map<string, string> aux;
    add_replica_volume (name_, 
			material_ref_, 
			solid_ref_, 
			replicavol_, 
			lunit_str_, 
			aunit_str_,
			aux);
  }
 
  /*** Setup ***/

  void gdml_writer::add_setup (const string & name_, 
			       const string & world_ref_,
			       const string & version_)
  {
    _get_stream (SETUP_SECTION) << "<setup" 
		    << " name=" << '"' << to_html (name_) << '"' 
		    << " version=" << '"' << version_ << '"'
		    << " >" << endl;
    _get_stream (SETUP_SECTION) << "  <world ref=" << '"' << to_html (world_ref_) << '"'
		    << " />" << endl; 
    _get_stream (SETUP_SECTION) << "</setup>" << endl << endl; 
  }
   
  
  /*** Section ***/

  void gdml_writer::xml_header (ostream & out_, 
				const string & version_, 
				const string & encoding_)
  {
    out_ << "<?xml version=" << '"' << version_ << '"' 
	 << " encoding=" << '"' << encoding_ << '"' 
	 << " ?>" << endl;

  }

  void gdml_writer::gdml_begin (ostream & out_, 
				const string & schema_)
  {

    out_ << "<gdml xmlns:gdml=" << '"' << "http://cern.ch/2001/Schemas/GDML" << '"'
	 << ' ' << endl
	 << "      xmlns:xsi=" << '"' << "http://www.w3.org/2001/XMLSchema-instance" << '"'
	 << ' ' << endl
	 << "      xsi:noNamespaceSchemaLocation="
	 << '"' << schema_ << '"' 
	 << " >" << endl;

    /*
      out_ << "<gdml " 
	 << "xsi:noNamespaceSchemaLocation="
	 << '"' << schema_ << '"' 
	 << " >" << endl;
    */
  }

  void gdml_writer::gdml_end (ostream & out_)
  {
    out_ << "</gdml>" << endl;
  }

  void gdml_writer::gdml_section_begin (ostream & out_, 
					const string & section_)
  {
    out_ << "<" << section_ << ">" << endl << endl;
  }

  void gdml_writer::gdml_section_end (ostream & out_, 
					const string & section_)
  {
    out_ << "</" << section_ << ">" << endl << endl;
  }
  
  /*** Utilities ***/
   
  void gdml_writer::full_write (ostream & out_,
			   const string & version_,
			   const string & encoding_,
			   const string & schema_)
  {
    if (! out_)
      {
	throw runtime_error ("gdml_writer::full_write: Output stream is invalid !");
      }
    xml_header (out_, version_, encoding_);
    gdml_begin (out_, schema_);
    out_ << endl;

    gdml_section_begin (out_, DEFINE_SECTION);
    out_ << _get_stream (DEFINE_SECTION).str ();
    gdml_section_end (out_, DEFINE_SECTION);
    out_ << endl;

    gdml_section_begin (out_, MATERIALS_SECTION);
    if (has_external_materials_stream ())
      {
	out_ << __external_materials_stream->str ();
      }
    out_ << _get_stream (MATERIALS_SECTION).str ();
    gdml_section_end (out_, MATERIALS_SECTION);
    out_ << endl;

    gdml_section_begin (out_, SOLIDS_SECTION);
    out_ << _get_stream (SOLIDS_SECTION).str ();
    gdml_section_end (out_, SOLIDS_SECTION);
    out_ << endl;

    gdml_section_begin (out_, STRUCTURE_SECTION);
    out_ << _get_stream (STRUCTURE_SECTION).str ();
    gdml_section_end (out_, STRUCTURE_SECTION);
    out_ << endl;

    //gdml_section_begin (out_, SETUP_SECTION);
    out_ << _get_stream (SETUP_SECTION).str ();
    //gdml_section_end (out_, SETUP_SECTION);
    out_ << endl;

    gdml_end (out_);
    out_ << endl;

    return;
  }
 
  void gdml_writer::attach_external_materials (const ostringstream & oss_)
  {
    __external_materials_stream = &oss_;
  }
  
  void gdml_writer::save_file (const string & filename_,
			       const string & version_,
			       const string & encoding_,
			       const string & schema_)
  {
    ofstream fout;
    string filename = filename_;
    datatools::utils::fetch_path_with_env (filename);
    fout.open (filename.c_str ());
    if (! fout)
      {
	ostringstream message;
	message << "gdml_writer::save_file: "
		<< "Cannot open GDML file '"
		<< filename
		<< "' !";
	throw runtime_error (message.str ());
      }
    full_write (fout, version_, encoding_, schema_);
    return;
  }
 
  void gdml_writer::dump (ostream & out_) const
  {
    out_ << "gdml_writer::dump: " << endl;
    out_ << endl << "*** Define section *** " << endl;
    out_ << _get_stream (DEFINE_SECTION).str ();
    out_ << endl << "*** Materials section *** " << endl;
    out_ << _get_stream (MATERIALS_SECTION).str ();
    out_ << endl << "*** Solids section *** " << endl;
    out_ << _get_stream (SOLIDS_SECTION).str ();
    out_ << endl << "*** Structure section *** " << endl;
    out_ << _get_stream (STRUCTURE_SECTION).str ();
    out_ << endl << "*** Setup section *** " << endl;
    out_ << _get_stream (SETUP_SECTION).str ();
    out_ << endl;
  }
  
} // end of namespace geomtools

// end of gdml_writer.cc
