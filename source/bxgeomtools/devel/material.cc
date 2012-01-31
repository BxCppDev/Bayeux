// -*- mode: c++; -*- 
/* material.cc
 */

#include <geomtools/material.h>

namespace geomtools {
 
  boost::scoped_ptr<material::table> material::g__table;

  const double material::DEFAULT_DENSITY     = 1.0 * CLHEP::gram / CLHEP::cm3;
  const double material::DEFAULT_TEMPERATURE = CLHEP::STP_Temperature;
  const double material::DEFAULT_PRESSURE    = CLHEP::STP_Pressure;
  const double material::NO_RADIATION_LENGTH = -1.0;
  const double material::NO_IONIZATION_POTENTIAL = -1.0;
  const double material::GAS_DENSITY_THRESHOLD = CLHEP::kGasThreshold;

  material::table::table ()
  {
  }

  material::table::~table ()
  {
  }

  material &
  material::table::access (const std::string & name_)
  {
    material_dict::iterator found = 
      __materials.find (name_);
    if  (found == __materials.end ())
      {
	std::ostringstream message;
	message << "material::table::access: "
		<< "Cannot find material with name '" << name_ << "' in table";
	throw std::runtime_error (message.str ());
      }
    return  (found->second);
  }
	
  const material &
  material::table::get (const std::string & name_) const
  {
    material_dict::const_iterator found = 
      __materials.find (name_);
    if  (found == __materials.end ())
      {
	std::ostringstream message;
	message << "material::table::get: "
		<< "Cannot find material with name '" << name_ << "' in table";
	throw std::runtime_error (message.str ());
      }
    return  (found->second);
  }

  bool 
  material::table::has (const std::string & name_) const
  {
    return  __materials.find (name_) != __materials.end ();
  }
  
  void 
  material::table::add (const material & material_)
  {
    material_dict::const_iterator found = 
      __materials.find (material_.get_name ());
    if  (found != __materials.end ())
      {
	std::cerr << "WARNING: material::table::add: "
		  << "material '" << material_.get_name () 
		  << "' already exists in the table!"
		  << std::endl;
      }
    __materials[material_.get_name ()] = material_;
  }

  material::table & 
  material::get_table ()
  {
    if  (g__table.get () == 0) 
      {
	material::table * ptr = new material::table;
        g__table.reset (ptr);
      }
    return * (g__table.get ());
  }

  const material & 
  material::get (const std::string & name_)
  {
    return material::get_table ().get (name_);
  }
  
  /******************************************/

  bool 
  material::is_locked () const
  {
    return __locked;
  }

  void 
  material::lock ()
  {
    __locked = true;
  }
  
  void 
  material::unlock ()
  {
    __locked = false;
  }

  void 
  material::assert_lock (const std::string & where_)
  {
    if  (is_locked ())
      {
	std::ostringstream message;
	message << "material::assert_lock: ";
	if  (!where_.empty ())
	  {
	    message << where_ << ": ";
	  }
	message << "Object is locked!";
	throw std::runtime_error (message.str ());
      }
  }

  double
  material::get_A () const
  {
    if (! is_composed ()) return get_component (0).get_element ().get_A ();

    if  (is_composed_per_number_of_atoms ())
      {
	double sum_a = 0.0;
	for  (int i = 0; i < get_number_of_elements (); i++)
	  {
	    sum_a = get_component (0).get_element ().get_A ();
	  }
	return sum_a / get_number_of_elements ();
      }

    return 0.0;
  }

  double
  material::get_Z () const
  {
    if (! is_composed ()) return get_component (0).get_element ().get_Z ();

    if  (is_composed_per_number_of_atoms ())
      {
	double sum_z = 0.0;
	for  (int i = 0; i < get_number_of_elements (); i++)
	  {
	    sum_z = get_component (0).get_element ().get_Z ();
	  }
	return sum_z / get_number_of_elements ();
      }

    return 0.0;
  }

  double
  material::get_density () const
  {
    return __density;
  }

  void 
  material::set_density (double d_)
  {
    assert_lock (__FUNCTION__);
    if  (d_ < CLHEP::universe_mean_density)
      {
	d_ = CLHEP::universe_mean_density;
      }
    __density = d_;
  }

  double
  material::get_temperature () const
  {
    return __temperature;
  }

  void 
  material::set_temperature (double t_)
  {
    assert_lock (__FUNCTION__);
    __temperature = t_;
  }

  double
  material::get_pressure () const
  {
    return __pressure;
  }

  void 
  material::set_pressure (double p_)
  {
    assert_lock (__FUNCTION__);
    __pressure = p_;
  }

  double 
  material::compute_ionization_potential (double z_)
  {
    const double I0 = 16. * CLHEP::eV;
    return I0 * std::pow (0.9, z_);
  }
  
  double 
  material::compute_radiation_length (double z_ , double a_)
  {
    double A       = a_;
    double Z       = z_;
    const double C = 716.4 * CLHEP::g / CLHEP::cm2;
    double x0      =  (C * A) /  (Z *  (Z + 1) * std::log (287. / std::sqrt (Z)));
    if  (Z == 2) x0 *= 1.05;
    return x0;
  }

  void 
  material::set_ionization_potential (double ip_)
  {
    assert_lock (__FUNCTION__);
    if  (ip_ < 0.0)
      {
	__ionization_potential = compute_ionization_potential (get_Z ());
      }
    else
      {
	__ionization_potential = ip_;
      }
  }

  double
  material::get_ionization_potential () const
  {
    return __ionization_potential;
  }

  double
  material::get_radiation_length () const
  {
    return __radiation_length;
  }

  void 
  material::set_radiation_length (double rl_)
  {
    assert_lock (__FUNCTION__);
    if  (rl_ < 0.0)
      {
	__radiation_length = compute_radiation_length (get_Z (), get_A ());
      }
    else
      {
	__radiation_length = rl_;
      }
  }

  const std::string &
  material::get_name () const
  {
    return __name;
  }

  void 
  material::set_name (const std::string & name_)
  {
    assert_lock (__FUNCTION__);
    __name = name_;
  }

  material::state_type 
  material::get_state () const
  {
    if  (__state == STATE_UNDEFINED)
      {
	if (__density <= GAS_DENSITY_THRESHOLD) return STATE_GAS;
	else return STATE_SOLID;
      }
    return __state;
  }

  void 
  material::set_state (material::state_type state_)
  {
    assert_lock (__FUNCTION__);
    __state = state_;
  }

  bool 
  material::is_solid () const
  {
    return get_state () == STATE_SOLID;
  }

  bool 
  material::is_liquid () const
  {
    return get_state () == STATE_LIQUID;
  }

  bool 
  material::is_gas () const
  {
    return get_state () == STATE_GAS;
  }
  
  int
  material::get_number_of_elements () const
  {
    return __components.size ();
  }

  const material::component_entry & 
  material::get_component (int i_) const
  {
    if  (i_ < 0 || i_ >= __components.size ())
      {
	throw std::runtime_error ("material::get_component: invalid component index!");
      }
    return __components[i_];
  }

  const datatools::utils::properties &
  material::properties () const
  {
    return __properties;
  }
  
  datatools::utils::properties &
  material::properties ()
  {
    return __properties;
  }

  void
  material::__set_default_values ()
  {
    unlock ();
    set_name ("<no name>");
    set_state (STATE_UNDEFINED);

    __compositeness = COMPOSITENESS_UNDEFINED;
    __components.clear ();

    set_density (DEFAULT_DENSITY);
    set_temperature (DEFAULT_TEMPERATURE);
    set_pressure (DEFAULT_PRESSURE);
    set_radiation_length (NO_RADIATION_LENGTH);
  }

  // ctor/dtor:
  material::material ()
  {
    __set_default_values ();
  }

  material::~material ()
  {
  }
  
  bool 
  material::is_composed () const
  {
    return get_number_of_elements () > 1;
  }

  bool 
  material::is_composed_per_number_of_atoms () const
  {
    return __compositeness == COMPOSITENESS_PER_NUMBER_OF_ATOMS;
  }
  
  bool 
  material::is_composed_per_fraction_mass () const
  {
    return __compositeness == COMPOSITENESS_PER_FRACTION_MASS;
  }


  void
  material::add_element (const std::string & element_name_, 
			int number_of_atoms_)
  {
    assert_lock (__FUNCTION__);
    if  (__compositeness == COMPOSITENESS_PER_FRACTION_MASS)
      {
	std::ostringstream message;
	message << "material::add_element: "
		<< "This material is composed per fraction mass!";
	throw std::runtime_error (message.str ());
      }
    const element * elmt = element::get (element_name_);
    if  (elmt == 0)
      {
	std::ostringstream message;
	message << "material::add_element: "
		<< "Cannot find element '" 
		<< element_name_
		<< "'!";
	throw std::runtime_error (message.str ());
      }
    __compositeness = COMPOSITENESS_PER_NUMBER_OF_ATOMS;
    __components.push_back (component_entry (*elmt, number_of_atoms_));
  }

  void
  material::add_element (const std::string & element_name_, 
			double fraction_mass_)
  {
    assert_lock (__FUNCTION__);
    if  (__compositeness == COMPOSITENESS_PER_NUMBER_OF_ATOMS)
      {
	std::ostringstream message;
	message << "material::add_element: "
		<< "This material is composed per number of atoms!";
	throw std::runtime_error (message.str ());
      }
    const element * elmt = element::get (element_name_);
    if  (elmt == 0)
      {
	std::ostringstream message;
	message << "material::add_element: "
		<< "Cannot find element '" 
		<< element_name_
		<< "'!";
	throw std::runtime_error (message.str ());
      }
    __compositeness = COMPOSITENESS_PER_FRACTION_MASS;
    __components.push_back (component_entry (*elmt, fraction_mass_));
  }
  
  void 
  material::create (const std::string & name_, 
		   std::vector<std::string> elements_names_, 
		   std::vector<int> number_of_atoms_, 
		   double density_, 
		   state_type state_, 
		   double temperature_, 
		   double pressure_, 
		   double radiation_length_)
  {
    material mat;
    mat.set_name (name_);
    mat.set_state (state_);
    if  (elements_names_.size () == 0)
      {
	std::ostringstream message;
	message << "material::create: "
		<< "size of vector of elements' names is zero!";
	throw std::runtime_error (message.str ()); 	
      }
    if  (elements_names_.size () != number_of_atoms_.size ())
      {
	std::ostringstream message;
	message << "material::create: "
		<< "size of vector of elements' names "
		<< "and number of atoms do not match!";
	throw std::runtime_error (message.str ()); 
      }
    for  (int i = 0; i < elements_names_.size (); i++)
      {

	mat.add_element (elements_names_[i], 
			number_of_atoms_[i]);
      }
    mat.set_density (density_);
    mat.set_temperature (temperature_);
    mat.set_pressure (pressure_);
    mat.set_radiation_length (radiation_length_);

    material::get_table ().add (mat);
  }

  void 
  material::create (const std::string & name_, 
		   const std::string & element_name_, 
		   double density_, 
		   state_type state_, 
		   double temperature_, 
		   double pressure_, 
		   double radiation_length_)
  {
    material mat;
    mat.set_name (name_);
    mat.set_state (state_);
    mat.add_element (element_name_, 1.0);
    mat.set_density (density_);
    mat.set_temperature (temperature_);
    mat.set_pressure (pressure_);
    mat.set_radiation_length (radiation_length_);

    material::get_table ().add (mat);
  }

  /*
  void 
  material::create (const std::string & name_, 
		   double z_, 
		   double a_, 
		   double density_, 
		   state_type state_, 
		   double temperature_, 
		   double pressure_, 
		   double radiation_length_)
  {
    material mat;
    mat.set_name (name_);
    mat.set_state (state_);
    mat.set_Z (z_);
    mat.set_A (a_);
    mat.set_density (density_);
    mat.set_temperature (temperature_);
    mat.set_pressure (pressure_);
    mat.set_radiation_length (radiation_length_);

    material::get_table ().add (mat);
  }
  */

  std::string
  material::get_state_label (state_type st_)
  {
    if  (st_ == STATE_SOLID) return "solid";
    if  (st_ == STATE_LIQUID) return "liquid";
    if  (st_ == STATE_GAS) return "gas";
    return "undefined";
  }

  void 
  material::tree_dump (std::ostream & out_, 
		      const std::string & title_, 
		      const std::string & indent_, 
		      bool inherit_) const
  {
    std::string indent;
    if  ( !indent_.empty () ) indent = indent_;
    if  ( !title_.empty () ) 
      {
	out_ << indent << title_ << std::endl;
      }
    namespace du = datatools::utils;

    out_ << indent << du::i_tree_dumpable::tag 
         << "name    = '" << get_name () << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "compositeness = "
	 <<  (is_composed ()?"Yes":"No")
	 << std::endl;

    for  (int i = 0; i < get_number_of_elements (); i++)
      {
	out_ << indent << du::i_tree_dumpable::tag
	     << "Element '" << __components[i].get_element ().get_name ()
	     << "' ";
	if  (is_composed_per_number_of_atoms ())
	  {
	    out_ << " # atoms=" 
		 << __components[i].get_number_of_atoms ();
	  }
	else 
	  {
	    out_ << " % mass=" 
		 << __components[i].get_fraction_mass () * 100.;
	  }
	out_ << std::endl;
      }

    out_ << indent << du::i_tree_dumpable::tag 
         << "state   = '"  
	 << material::get_state_label (get_state ()) 
	 << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "density = "  
         << get_density () /  (CLHEP::g / CLHEP::cm3) 
	 << " g/cm3" << std::endl; 

    out_ << indent << du::i_tree_dumpable::tag 
         << "temperature      = "  
         << get_temperature () / CLHEP::kelvin 
	 << " K" << std::endl; 

    out_ << indent << du::i_tree_dumpable::tag 
         << "pressure         = "  
         << get_pressure () / CLHEP::atmosphere
	 << " atm" << std::endl; 

    out_ << indent << du::i_tree_dumpable::tag 
         << "A         = "  
         << get_A () /  (CLHEP::g / CLHEP::mole)
	 << " g/mol" << std::endl; 
 
    out_ << indent << du::i_tree_dumpable::tag 
         << "Z         = "  
         << get_Z ()
	 << std::endl; 

    out_ << indent << du::i_tree_dumpable::tag 
         << "radiation length = ";
    if  (get_radiation_length () <= 0.0 )
      {
	out_ << "undefined";
      }
    else 
      {
	out_ << get_radiation_length () / CLHEP::cm 
	     << " cm";
      }
    out_ << std::endl;
 
    out_ << indent << du::i_tree_dumpable::tag 
         << "ionization potential = ";
    if  (get_ionization_potential () <= 0.0 )
      {
	out_ << "undefined";
      }
    else 
      {
	out_ << get_ionization_potential () / CLHEP::cm 
	     << " cm";
      }
    out_ << std::endl;
 
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	 << "Properties : ";
    if  ( __properties.size () == 0 ) { out_ << "<empty>"; }
    out_ << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
      __properties.tree_dump (out_, "", indent_oss.str ());
    }
  }

  void 
  material::print_table_of_materials (std::ostream & out_)
  {
    out_ << "Table of materials: " << std::endl;
    const material_dict & d = material::get_table ().get_dict ();

    for  (material_dict::const_iterator it = d.begin ();
	 it != d.end ();
	 it++)
      {
	it->second.tree_dump (out_, it->first);
      }
  }
  
} // end of namespace geomtools

// end of material.cc
