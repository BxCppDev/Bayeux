// -*- mode: c++ ; -*- 
/* factory.cc
 */

#include <materials/factory.h>
#include <datatools/utils/clhep_units.h>
#include <datatools/utils/units.h>

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
    double a = -1.0;     
    vector<string> isotopes;
    vector<double> weights;

    if (config_.has_key ("z"))
      {
	z = config_.fetch_integer ("z");
      }
    else
      {
	throw runtime_error ("factory::create_element: Missing 'z' property !");
      }

    if (config_.has_key ("a"))
      {
	a = config_.fetch_real ("a");
      }
    else
      {
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
      }
     
    element * elmt = new element (name_, z);

    if (a > 0.0)
      {
	elmt->set_molar_mass (a);
      }
    else
      {
	// add isotopes in element:
	for (int i = 0; i < isotopes.size (); i++)
	  { 
	    isotope_dict_t::const_iterator found = isotopes_.find (isotopes[i]);
	    if (found == isotopes_.end ())
	      {
		ostringstream message;
		message << "factory::create_element: Isotope '" << isotopes[i] 
			<< "' not found in map of isotopes !";
		throw runtime_error (message.str ());
	      }
	    const isotope & iso = found->second.get_ref ();
	    elmt->add_isotope (iso, weights[i]);
	  }
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
    //cerr << "DEVEL: factory::create_material: 1 g=" << CLHEP::g << endl;
    //cerr << "DEVEL: factory::create_material: 1 cm3=" << CLHEP::cm3 << endl;
    string formula;
    double temperature = -1.0;
    double temperature_unit = CLHEP::kelvin;
    double pressure = -1.0;
    double pressure_unit = CLHEP::bar;
    string state = "";

    if (config_.has_key ("density"))
      {
	density = config_.fetch_real ("density");
      }
    else
      {
	throw runtime_error ("factory::create_material: Missing 'density' property !");
      }

    if (config_.has_key ("density.unit"))
      {
	string density_unit_str =  config_.fetch_string ("density.unit");
	density_unit = datatools::utils::units::get_density_unit_from (density_unit_str);
      }

    if (config_.has_key ("temperature"))
      {
	temperature = config_.fetch_real ("temperature");
	temperature *= CLHEP::kelvin;
      }

    if (config_.has_key ("temperature.unit"))
      {
	string temperature_unit_str =  config_.fetch_string ("temperature.unit");
	temperature_unit = datatools::utils::units::get_temperature_unit_from (temperature_unit_str);
      }

    if (config_.has_key ("pressure"))
      {
	pressure = config_.fetch_real ("pressure");
      }

    if (config_.has_key ("pressure.unit"))
      {
	string pressure_unit_str =  config_.fetch_string ("pressure.unit");
	pressure_unit = datatools::utils::units::get_pressure_unit_from (pressure_unit_str);
      }
 
    // apply units:
    density *= density_unit;
    temperature *= temperature_unit;
    pressure *= pressure_unit;

    if (config_.has_key ("formula"))
      {
	formula = config_.fetch_string ("formula");
      }

    if (config_.has_key ("state"))
      {
	state = config_.fetch_string ("state");
      }

    vector<string> composition_names;
    vector<int>    composition_nb_of_atoms;
    vector<double> composition_fraction_mass;
    string         composition_mode_label;
    int            composition_mode = -1;
    double         mean_z = -1.0;
    double         mean_a = -1.0;

    if (config_.has_key ("composition.mode"))
      {
	composition_mode_label =  config_.fetch_string ("composition.mode");
      }
    else
      {
	throw runtime_error ("factory::create_material: Missing 'composition.mode' property !");
      }

    if (composition_mode_label == "number_of_atoms")
      {
	composition_mode = material::NUMBER_OF_ATOMS;
      }
    else if (composition_mode_label == "fraction_mass")
      {
	composition_mode = material::FRACTION_MASS;
      }
    else if (composition_mode_label == "mean_za")
      {
	composition_mode = material::MEAN_ZA;
      }
    else
      {
	ostringstream message;
	message << "factory::create_material: "
		<< "Invalid 'composition.mode' property ('" << composition_mode_label << "') !";
	throw runtime_error (message.str ());
      }

    if (composition_mode == material::MEAN_ZA)
      {
	if (config_.has_key ("mean_z"))
	  {
	    mean_z = config_.fetch_real ("mean_z");
	  }
	else
	  {
	    throw runtime_error ("factory::create_material: Missing 'mean_z' property !");
	  }
	if (config_.has_key ("mean_a"))
	  {
	    mean_a = config_.fetch_real ("mean_a");
	  }
	else
	  {
	    throw runtime_error ("factory::create_material: Missing 'mean_a' property !");
	  }
      }
    else
      {
	if (config_.has_key ("composition.names"))
	  {
	    config_.fetch ("composition.names", composition_names);
	    if (composition_names.size () == 0)
	      {
		ostringstream message;
		message << "factory::create_material: Empty list of compounds for material '" << name_ << "' !";
		throw runtime_error (message.str ());
	      } 
	  }
	else
	  {
	    ostringstream message;
	    message << "factory::create_material: Missing 'composition.names' property for material '" << name_ << "' !";
	    throw runtime_error (message.str ());
	  }

	if (composition_mode == material::NUMBER_OF_ATOMS)
	  {
	    if (config_.has_key ("composition.number_of_atoms"))
	      {
		config_.fetch ("composition.number_of_atoms", composition_nb_of_atoms);
		if (composition_names.size () != composition_nb_of_atoms.size ())
		  {
		    throw runtime_error ("factory::create_material: Unmatching sizes of list of compounds/number of atoms !");
		  }
	      }
	    else
	      {
		ostringstream message;
		message << "factory::create_material: Missing 'composition.number_of_atoms' property for material '" << name_ << "' !";
		throw runtime_error (message.str ());
	      }
	  }


	if (composition_mode == material::FRACTION_MASS)
	  {
	    if (config_.has_key ("composition.fraction_mass"))
	      {
		config_.fetch ("composition.fraction_mass", composition_fraction_mass);
		if (composition_names.size () != composition_fraction_mass.size ())
		  {
		    throw runtime_error ("factory::create_material: Unmatching sizes of list of compounds/fraction mass !");
		  }
	      }
	    else
	      {
		ostringstream message;
		message << "factory::create_material: Missing 'composition.fraction_mass' property for element '" << name_ << "' !";
		throw runtime_error (message.str ());
	      }
	  }
      }
    
    material * matl = new material ();
    try 
      {
	matl->set_name (name_);
	matl->set_density (density);

	if (composition_mode == material::MEAN_ZA)
	  {
	    matl->set_mean_z_a (mean_z, mean_a);
	  }

	if (composition_mode == material::NUMBER_OF_ATOMS)
	  {
	    for (int i = 0; i < composition_names.size (); i++)
	      { 
		element_dict_t::const_iterator found = elements_.find (composition_names[i]);
		if (found ==  elements_.end ())
		  {
		    ostringstream message;
		    message << "factory::create_material: "
			    << "Unknown element '" << composition_names[i] << "') !";
		    throw runtime_error (message.str ());	
		  }
		const element & elmt = found->second.get_ref ();
		int nb_atoms = composition_nb_of_atoms[i];
		matl->add_element_by_nb_of_atoms (elmt , nb_atoms);
	      }
	  }

	if (composition_mode == material::FRACTION_MASS)
	  {
	    for (int i = 0; i < composition_names.size (); i++)
	      { 
		const element * a_elmt = 0;
		const material * a_matl = 0;
		if (composition_names[i] == name_)
		  {
		    ostringstream message;
		    message << "factory::create_material: "
			    << "Self-referenced material with name '" 
			    << composition_names[i] << "' !";
		    delete matl;
		    throw runtime_error (message.str ());	
		  }
		element_dict_t::const_iterator found = elements_.find (composition_names[i]);
		if (found != elements_.end ())
		  {
		    a_elmt = found->second.get_ptr ();
		  }
		else
		  {
		    material_dict_t::const_iterator found2 = materials_.find (composition_names[i]);
		    if (found2 != materials_.end ())
		      {
			a_matl = found2->second.get_ptr ();
		      }
		    else
		      {
			ostringstream message;
			message << "factory::create_material: "
				<< "Unknown element or material '" << composition_names[i] 
				<< "' for material '" << name_ << "' !";
		throw runtime_error (message.str ());

			delete matl;
			throw runtime_error (message.str ());	
		      }
		  }
		double f_mass = composition_fraction_mass[i];
		if (a_elmt !=  0)
		  {
		    matl->add_element_by_mass (*a_elmt, f_mass);
		  }
		else
		  {
		    matl->add_material_by_mass (*a_matl, f_mass);
		  }
	      }
	  }

	// Some auxiliary properties:
	if (! formula.empty ())
	  {
	    matl->grab_properties ().store ("formula", formula);
	  }

	if (! state.empty ())
	  {
	    matl->grab_properties ().store ("state", state);
	  }

	if (temperature > 0.0)
	  {
	    matl->grab_properties ().store ("temperature", temperature);
	  }

	if (pressure > 0.0)
	  {
	    matl->grab_properties ().store ("pressure", pressure);
	  }

	matl->build ();
      }
    catch (exception & x)
      {
	cerr << "factory::create_material: " << "Exception : " << x.what () << endl;
	delete matl;
	throw x;
      }
    return matl;
  }
 
} // end of namespace mat

// end of factory.cc
