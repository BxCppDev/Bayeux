// -*- mode: c++; -*- 
/* material.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-25
 * Last modified: 2008-05-25
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geom_tools__material_h
#define __geom_tools__material_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <cmath>

#include <boost/scoped_ptr.hpp>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/utils.h>
#include <geomtools/element.h>

namespace geomtools {

  class material :
    public datatools::utils::i_tree_dumpable
  {
  public:    

    enum state_type
      {
	STATE_UNDEFINED = -1,
	STATE_SOLID     = 0,
	STATE_LIQUID    = 1,
	STATE_GAS       = 2
      };

    enum compositeness_type
      {
	COMPOSITENESS_UNDEFINED           = -1,
	COMPOSITENESS_PER_NUMBER_OF_ATOMS = 0,
	COMPOSITENESS_PER_FRACTION_MASS   = 1
      };

    static const double DEFAULT_DENSITY;
    static const double DEFAULT_TEMPERATURE;
    static const double DEFAULT_PRESSURE;
    static const double NO_RADIATION_LENGTH;
    static const double NO_IONIZATION_POTENTIAL;
    static const double GAS_DENSITY_THRESHOLD;

    struct component_entry
    {
      const element * element_ptr;
      double fraction_mass;
      int    number_of_atoms;
    public:
      const element & 
      get_element () const
      {
	if (element_ptr == 0)
	  {
	    std::ostringstream message;
	    message << "material::component_entry:get_element: "
		    << "Null element internal pointer!";
	    throw std::runtime_error (message.str ());
	  }
	return *element_ptr;
      }

      int 
      get_number_of_atoms () const
      {
	return number_of_atoms;
      }

      double 
      get_fraction_mass () const
      {
	return fraction_mass;
      }

      component_entry ()
      {
	element_ptr     = 0;
	fraction_mass   = -1.0;
	number_of_atoms = -1;
      }

      component_entry (const element & element_, 
		      int number_of_atoms_)
      {
	element_ptr     = &element_;
	fraction_mass   = -1.0;
	number_of_atoms = number_of_atoms_;
      }

      component_entry (const element & element_, 
		      double fraction_mass_)
      {
	element_ptr     = &element_;
	fraction_mass   = fraction_mass_;
	number_of_atoms = -1;
      }
    };

    typedef std::vector<component_entry> component_col;

  private:
    bool        __locked;
    std::string __name;
    state_type  __state;

    // compositeness:
    compositeness_type __compositeness;
    component_col      __components;

    /*
    double      __A;
    double      __Z;
    */
    double      __density;
    double      __temperature;
    double      __pressure;
    double      __radiation_length;
    double      __ionization_potential;

    datatools::utils::properties __properties;

  public:
    typedef std::map<std::string,material> material_dict;

    class table
    {
      material_dict __materials;

    public:
      table ();

      virtual 
      ~table ();

      const material_dict & get_dict () const
      {
	return __materials;
      }

      material &
      access (const std::string & name_);

      const material &
      get (const std::string & name_) const;

      void 
      add (const material & material_);

      bool 
      has (const std::string & name_) const;

    };

    static const material &
    get (const std::string & name_);

  private:
    static boost::scoped_ptr<table> g__table;

  public:
      
    static table & 
    get_table ();

  public:
    bool 
    is_locked () const;

    void 
    lock ();

    void 
    unlock ();

    void 
    assert_lock (const std::string & where_ = "");

    double
    get_A () const;

    /*
    void 
    set_A (double);
    */

    double
    get_Z () const;

    /*
    void 
    set_Z (double);
    */

    double
    get_density () const;

    void 
    set_density (double);

    double
    get_temperature () const;

    void 
    set_temperature (double);

    double
    get_pressure () const;

    void 
    set_pressure (double);

    double
    get_radiation_length () const;

    void 
    set_radiation_length (double = -1.0);

    double
    get_ionization_potential () const;

    void 
    set_ionization_potential (double = -1.0);

    const std::string &
    get_name () const;

    void 
    set_name (const std::string & name_);

    state_type 
    get_state () const;

    void 
    set_state (state_type state_);

    bool 
    is_solid () const;

    bool 
    is_liquid () const;

    bool 
    is_gas () const;

    bool 
    is_composed () const;

    bool 
    is_composed_per_number_of_atoms () const;

    bool 
    is_composed_per_fraction_mass () const;

    void
    add_element (const std::string & element_name_,
		int number_of_atoms_);

    void
    add_element (const std::string & element_name_,
		double fraction_mass_);

    int
    get_number_of_elements () const;

    const component_entry & 
    get_component (int i_) const;

    const datatools::utils::properties &
    properties () const;

    datatools::utils::properties &
    properties ();

  private:
    void
    __set_default_values ();

  public:

    // ctor/dtor:
    material ();

    ~material ();

  public:
    static void 
    create (const std::string & name_,
	   const std::string & element_name_,
	   double density_,
	   state_type state_ = STATE_UNDEFINED,
	   double temperature_ = DEFAULT_TEMPERATURE,
	   double pressure_ = DEFAULT_PRESSURE,
	   double radiation_length_ = NO_RADIATION_LENGTH);

    static void 
    create (const std::string & name_,
	   int number_of_elements_,
	   double density_,
	   state_type state_ = STATE_UNDEFINED,
	   double temperature_ = DEFAULT_TEMPERATURE,
	   double pressure_ = DEFAULT_PRESSURE,
	   double radiation_length_ = NO_RADIATION_LENGTH);

    static void 
    create (const std::string & name_,
	   std::vector<std::string> elements_names_,
	   std::vector<int> number_of_atoms_,
	   double density_,
	   state_type state_ = STATE_UNDEFINED,
	   double temperature_ = DEFAULT_TEMPERATURE,
	   double pressure_ = DEFAULT_PRESSURE,
	   double radiation_length_ = NO_RADIATION_LENGTH);
 
    static void 
    print_table_of_materials (std::ostream & out_);
     
    static std::string
    get_state_label (state_type);

    static double 
    compute_ionization_potential (double z_);

    static double 
    compute_radiation_length (double z_, double a_);

    virtual void 
    tree_dump (std::ostream & out_         = std::cerr , 
	      const std::string & title_  = "" ,
	      const std::string & indent_ = "",
	      bool inherit_               = false) const;
      
  };

} // end of namespace geomtools

#endif // __geomtools__material_h

// end of material.h
