// -*- mode: c++ ; -*- 
/* factory.cc
 */

#include <materials/factory.h>

namespace mat {

  
  bool factory::is_debug () const
  {
    return __debug;
  }
  
  void factory::set_debug (bool new_value_)
  {
    __debug = new_value_;
  }
  
  
  // ctor:
  factory::factory ()
  {
    __debug = false;
  }
  
  // dtor:
  factory::~factory ()
  {
  }
  
  isotope * factory::create_isotope (const string & name_, 
				     const datatools::utils::properties & config_) const
  {
    int z = 0;
    int a = 0;
    int i = isotope::GROUND_STATE;

    if (config_.has_key ("z"))
      {
	z =  config_.fetch_integer ("z");
      }
    else
      {
	throw runtime_error ("factory::create_isotope: Missing 'z' property !");
      }
    
    if (config_.has_key ("a"))
      {
	a =  config_.fetch_integer ("a");
      }
    else
      {
	throw runtime_error ("factory::create_isotope: Missing 'a' property !");
      }

    if (config_.has_key ("I"))
      {
	string i_str = config_.fetch_string ("I");
	if (i_str == "GS")
	  {
	    i = isotope::GROUND_STATE;
	  }
	else if (i_str == "M")
	  {
	    i = isotope::ISOMERIC_M;
	  }
	else if (i_str == "N")
	  {
	    i = isotope::ISOMERIC_N;
	  }
	else if (i_str == "O")
	  {
	    i = isotope::ISOMERIC_O;
	  }
	else
	  {
	    ostringstream message;
	    message << "factory::create_isotope: Invalid 'I' property label: '" << i_str << "' !";
	    throw runtime_error (message.str ());
	  }
      }
    isotope * iso = new isotope (name_, z, a, i);
    iso->build ();
    return iso;
  }

  
  element * factory::create_element (const string & name_,
				     const datatools::utils::properties & config_, 
				     const isotope_dict_t & isotopes_) const
  {

    int z = 0;

    if (config_.has_key ("z"))
      {
	z =  config_.fetch_integer ("z");
      }
    else
      {
	throw runtime_error ("factory::create_element: Missing 'z' property !");
      }
   
    vector<string> isotopes;
    vector<double> weights;

    if (! config_.has_key ("isotope.names"))
      {
	throw runtime_error ("factory::create_element: Missing 'isotope.names' property !");
      }
    else
      {
	config_.fetch ("isotope.names", isotopes);
      }
    
    if (! config_.has_key ("isotope.weights"))
      {
	throw runtime_error ("factory::create_element: Missing 'isotope.weights' property !");
      }
    else
      {
	config_.fetch ("isotope.weights", weights);
      }
    
    if (isotopes.size () != weights.size ())
      {
	throw runtime_error ("factory::create_element: Unmatching isotopes/weights vector size !");
      }
     
    element * elmt = new element (name_, z);
    // add isotopes in element:
    for (int i = 0; i < isotopes.size (); i++)
      { 
	isotope_dict_t::const_iterator found = isotopes_.find (isotopes[i]);
	if (found == isotopes_.end ())
	  {
	    ostringstream message;
	    message << "factory::create_element: Isotope '" << isotopes[i] 
		    << "' not foudn in map of isotopes !";
	    throw runtime_error (message.str ());
	  }
	const isotope & iso = found->second.get_ref ();
	elmt->add_isotope (iso, weights[i]);
      }
    elmt->build ();
    return elmt;
  }
  
  material * factory::create_material (const string & name_, 
				       const datatools::utils::properties & config_, 
				       const element_dict_t & elements_, 
				       const material_dict_t & materials_) const
  {

    double density = 1.0;
    double density_unit = material::g_per_cm3;
    cerr << "DEVEL: factory::create_material: 1 g=" << CLHEP::g << endl;
    cerr << "DEVEL: factory::create_material: 1 cm3=" << CLHEP::cm3 << endl;

    if (config_.has_key ("density"))
      {
	density = config_.fetch_real ("density");
      }
    else
      {
	throw runtime_error ("factory::create_element: Missing 'density' property !");
      }

    if (config_.has_key ("density.unit"))
      {
	string density_unit_str =  config_.fetch_string ("density.unit");
	density_unit = datatools::utils::units::get_density_unit_from (density_unit_str);
      }

    material * matl = new material ();
    matl->set_name (name_);
    matl->set_density (density * density_unit);
    cerr << "DEVEL: factory::create_material: density=" << density << endl;
    cerr << "DEVEL: factory::create_material: density unit =" << density_unit << endl;
    //matl->build ();
    return matl;
  }
 
} // end of namespace mat

// end of factory.cc
