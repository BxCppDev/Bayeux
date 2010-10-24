// -*- mode: c++ ; -*- 
/* gdml_writer.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 * 
 * License: 
 * 
 * Description: 
 *   GDML writer
 * 
 * History: 
 * 
 *
 * To do:
 * Add auxiliary properties for volumes...
 *
 */

#ifndef __geomtools__gdml_writer_h
#define __geomtools__gdml_writer_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <list>
#include <map>

#include <boost/algorithm/string.hpp>

#include <datatools/utils/utils.h>
#include <datatools/utils/properties.h>

#include <geomtools/utils.h>
//#include <geomtools/units.h>
#include <datatools/utils/units.h>

#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/sphere.h>
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>

namespace geomtools {

  using namespace std;

  class gdml_writer 
  {
  public:

    typedef map<string, ostringstream *> __streams_col_t;

    static const string DEFAULT_XML_VERSION;
    static const string DEFAULT_XML_ENCODING;
    static const string DEFAULT_XSI;
    static const string DEFAULT_GDML_SCHEMA;
    static const string DEFAULT_REMOTE_GDML_SCHEMA;

    static const string DEFINE_SECTION;
    static const string MATERIALS_SECTION;
    static const string SOLIDS_SECTION;
    static const string STRUCTURE_SECTION;
    static const string SETUP_SECTION;

  private:
    bool                  __initialized;
    bool                  __verbose;
    __streams_col_t       __streams;
    const ostringstream * __external_materials_stream;

    static bool __g_using_html_symbols;

  public:

    static void set_using_html_symbols (bool u_);
    static bool is_using_html_symbols ();
    static string to_html (const string &);
    static string to_ascii (const string &);

    bool is_initialized () const;

    bool is_verbose () const;

    void set_verbose (bool);

    const ostringstream & get_stream (const string & section_) const;

  protected:

    const ostringstream & _get_stream (const string & section_) const;

    ostringstream & _get_stream (const string & section_);

  public:

    void attach_external_materials (const ostringstream & oss_);

    bool has_external_materials_stream () const;

    void reset_external_materials_stream ();

    gdml_writer ();

    virtual ~gdml_writer ();

    void init ();

    void initialize ();

    void reset ();

    /*************** Defines section *******************/

    void add_constant (const string & name_, 
		       double value_);

    void add_quantity (const string & name_, 
		       const string & quantity_type_, 
		       const string & unit_str_, 
		       double value_);

    void add_variable (const string & name_, 
		       double value_);

    void add_variable (const string & name_, 
		       const string & expr_value_);

    void add_position (const string & name_, 
		       double x_, double y_, double z_, 
		       const string & unit_str_);

    void add_position (const string & name_, 
		       const string & x_str_,
		       const string & y_str_,
		       const string & z_str_,
		       const string & unit_str_);

    void add_position (const string & name_, 
		       const vector_3d & v_, 
		       const string & unit_str_);

    void add_rotation (const string & name_, 
		       const string & axis_, 
		       double angle_,
		       const string & unit_str_);

    // rotation using GDML convention through XYZ Euler angles:
    void add_rotation (const string & name_, 
		       const rotation_3d & rot_, 
		       const string & unit_str_);

    /************* Materials section *******************/

    void add_isotope (const string & name_, 
		      size_t atomic_number_,
		      size_t number_of_nucleons_,
		      double a_);

    void add_element (const string & name_, 
		      size_t atomic_number_,
		      const string & formula_,
		      double a_);

    void add_element (const string & name_, 
		      const map<string, double> & fractions_);

    void add_element (const string & name_, 
		      const string & ref_);

    void add_element (const string & name_, 
		      const string & ref1_, double fraction1_, 
		      const string & ref2_, double fraction2_);

    void add_element (const string & name_, 
		      const string & ref1_, double fraction1_, 
		      const string & ref2_, double fraction2_, 
		      const string & ref3_, double fraction3_);

    void add_element (const string & name_, 
		      const string & ref1_, double fraction1_, 
		      const string & ref2_, double fraction2_, 
		      const string & ref3_, double fraction3_, 
		      const string & ref4_, double fraction4_);

    void add_element (const string & name_, 
		      const string & ref1_, double fraction1_, 
		      const string & ref2_, double fraction2_, 
		      const string & ref3_, double fraction3_, 
		      const string & ref4_, double fraction4_, 
		      const string & ref5_, double fraction5_);

    void add_material (const string & name_, 
		       double atomic_number_,
		       double density_,
		       double a_);

    void add_material (const string & name_, 
		       const string & formula_,
		       double density_, 
		       const map<string, size_t> & composites_);

    void add_material (const string & name_, 
		       const string & formula_,
		       double density_, 
		       const map<string, double> & fractions_);

    /**************** Solids section *******************/
 
    static bool solid_type_is_valid (const string & solid_type_);
 
    static bool solid_type_is_supported (const string & solid_type_);

    /*
    void add_solid (const string & name_, 
		    const string & solid_type_,
		    const datatools::utils::properties & params_);
    */

    void add_gdml_box (const string & name_, 
		       double x_, double y_, double z_, 
		       const string & lunit_str_ = "mm");
	    
    void add_gdml_tube (const string & name_, 
			double rmin_, double rmax, double z_, 
			double start_phi_, double delta_phi_,
			const string & lunit_str_ = "mm",
			const string & aunit_str_ = "radian");
	    
    void add_gdml_orb (const string & name_, 
		       double r_,
		       const string & lunit_str_ = "mm");
	    
    void add_gdml_sphere (const string & name_, 
			  double rmin_, double rmax, 
			  double start_phi_, double delta_phi_,
			  double start_theta_, double delta_theta_,
			  const string & lunit_str_ = "mm",
			  const string & aunit_str_ = "radian");
	    
    void add_gdml_polycone (const string & name_, 
			    map<double, pair<double, double> > zplanes_, 
			    double start_phi_, double delta_phi_,
			    const string & lunit_str_ = "mm",
			    const string & aunit_str_ = "radian");
	    
    void add_gdml_polyhedra (const string & name_, 
			     size_t num_sides_,
			     map<double, pair<double, double> > zplanes_, 
			     double start_phi_, double delta_phi_,
			     const string & lunit_str_ = "mm",
			     const string & aunit_str_ = "radian");
	    
    void add_gdml_boolean (const string & name_, 
			   const string & boolean_type_, 
			   const string & first_ref_, 
			   const string & second_ref_, 
			   const string & position_ref_, 
			   const string & rotation_ref_);

    void add_gdml_union (const string & name_, 
			 const string & first_ref_, 
			 const string & second_ref_, 
			 const string & position_ref_, 
			 const string & rotation_ref_);

    void add_gdml_subtraction (const string & name_, 
			       const string & first_ref_, 
			       const string & second_ref_, 
			       const string & position_ref_, 
			       const string & rotation_ref_);

    void add_gdml_intersection (const string & name_, 
				const string & first_ref_, 
				const string & second_ref_, 
				const string & position_ref_, 
				const string & rotation_ref_);
    
    void add_box (const string & name_, 
		  const box & b_,
		  const string & lunit_str_ = "mm");
	    
    void add_cylinder (const string & name_, 
		       const cylinder & c_,
		       const string & lunit_str_ = "mm",
		       const string & aunit_str_ = "radian");

    void add_tube (const string & name_, 
		   const tube & t_,
		   const string & lunit_str_ = "mm",
		   const string & aunit_str_ = "radian");

    void add_orb (const string & name_, 
		  const sphere & s_,
		  const string & lunit_str_ = "mm",
		  const string & aunit_str_ = "radian");

    void add_sphere (const string & name_, 
		     const sphere & s_,
		     const string & lunit_str_ = "mm",
		     const string & aunit_str_ = "radian");

    void add_polycone (const string & name_, 
		       const polycone & s_,
		       const string & lunit_str_ = "mm",
		       const string & aunit_str_ = "radian");

    void add_polyhedra (const string & name_, 
		       const polyhedra & s_,
		       const string & lunit_str_ = "mm",
		       const string & aunit_str_ = "radian");

    /**************** Structures section *******************/

    class physvol
    {
    public:
      string volumeref;
      string positionref;
      string rotationref;
      string scaleref;
      physvol (const string & volumeref_ = "",
	       const string & positionref_ = "",
	       const string & rotationref_ = "",
	       const string & scaleref_ = "");
    };


    class replicavol
    {
    public:
      static const string REPLICATED_ALONG_AXIS;
      string volumeref;
      size_t number;
      string mode;
      string direction;
      double width;
      double offset;
    };

    void add_volume_auxiliaries (const map<string,string> & aux_);

    void add_volume (const string & name_,
		     const string & material_ref_,
		     const string & solid_ref_,
		     const map<string, string> & aux_);

    void add_volume (const string & name_,
		     const string & material_ref_,
		     const string & solid_ref_);

    void add_volume (const string & name_,
		     const string & material_ref_,
		     const string & solid_ref_,
		     const list<physvol> & phys_vols_,
		     const map<string, string> & aux_);

    void add_volume (const string & name_,
		     const string & material_ref_,
		     const string & solid_ref_,
		     const list<physvol> & phys_vols_);

    void add_replica_volume (const string & name_,
			     const string & material_ref_,
			     const string & solid_ref_,
			     const replicavol & replicavol_,
			     const string & lunit_str_,
			     const string & aunit_str_,
			     const map<string, string> & aux_);
    
    void add_replica_volume (const string & name_,
			     const string & material_ref_,
			     const string & solid_ref_,
			     const replicavol & replicavol_,
			     const string & lunit_str_ = "mm",
			     const string & aunit_str_ = "radian");

    /**************** Setups section *******************/
 
    void add_setup (const string & name_, 
		    const string & world_ref_,
		    const string & version_ = "1.0");


   
    /**************** Header/Section  *******************/

    void xml_header (ostream &, 
		     const string & version_ = DEFAULT_XML_VERSION,
		     const string & encoding_ = DEFAULT_XML_ENCODING,
		     bool standalone_ = false);

    void gdml_begin (ostream &, 
		     const string & schema_ = DEFAULT_GDML_SCHEMA,
		     const string & xsi_ = DEFAULT_XSI);

    void gdml_end (ostream &);

    void gdml_section_begin (ostream &, 
			     const string & section_);
    void gdml_section_end (ostream &, 
			   const string & section_);
   
   
    /**************** Utilities *******************/
   
    void full_write (ostream & out_,
		     const string & version_  = DEFAULT_XML_VERSION,
		     const string & encoding_ = DEFAULT_XML_ENCODING,
		     const string & schema_   = DEFAULT_REMOTE_GDML_SCHEMA,
		     const string & xsi_      = DEFAULT_XSI);
   
    void save_file (const string & filename_,
		    const string & version_  = "1.0",
		    const string & encoding_ = "UTF-8",
		    const string & schema_   = DEFAULT_REMOTE_GDML_SCHEMA,
		    const string & xsi_      = DEFAULT_XSI);


    void dump (ostream &) const;

  };


} // end of namespace geomtools

#endif // __geomtools__gdml_writer_h

// end of gdml_writer.h
