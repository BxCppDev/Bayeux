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

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <geomtools/utils.h>

#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/sphere.h>
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>

namespace geomtools {

  class gdml_writer 
  {
  public:

    typedef std::map<std::string, std::ostringstream *> streams_col_type;

    static const std::string DEFAULT_XML_VERSION;
    static const std::string DEFAULT_XML_ENCODING;
    static const std::string DEFAULT_XSI;
    static const std::string DEFAULT_GDML_SCHEMA;
    static const std::string DEFAULT_REMOTE_GDML_SCHEMA;

    static const std::string DEFINE_SECTION;
    static const std::string MATERIALS_SECTION;
    static const std::string SOLIDS_SECTION;
    static const std::string STRUCTURE_SECTION;
    static const std::string SETUP_SECTION;

  public:

    static void set_using_html_symbols (bool u_);
    static bool is_using_html_symbols ();
    static std::string to_html (const std::string &);
    static std::string to_ascii (const std::string &);

    bool is_initialized () const;

    bool is_verbose () const;

    void set_verbose (bool);

    const std::ostringstream & get_stream (const std::string & section_) const;

  protected:

    const std::ostringstream & _get_stream (const std::string & section_) const;

    std::ostringstream & _get_stream (const std::string & section_);

  public:

    void attach_external_materials (const std::ostringstream & oss_);

    bool has_external_materials_stream () const;

    void reset_external_materials_stream ();

    gdml_writer ();

    virtual ~gdml_writer ();

    void init ();

    void initialize ();

    void reset ();

    /*************** Defines section *******************/

    void add_constant (const std::string & name_, 
                       double value_);

    void add_quantity (const std::string & name_, 
                       const std::string & quantity_type_, 
                       const std::string & unit_str_, 
                       double value_);

    void add_variable (const string & name_, 
                       double value_);

    void add_variable (const std::string & name_, 
                       const std::string & expr_value_);

    void add_position (const std::string & name_, 
                       double x_, double y_, double z_, 
                       const std::string & unit_str_);

    void add_position (const std::string & name_, 
                       const std::string & x_str_,
                       const std::string & y_str_,
                       const std::string & z_str_,
                       const std::string & unit_str_);

    void add_position (const std::string & name_, 
                       const vector_3d & v_, 
                       const std::string & unit_str_);

    void add_rotation (const std::string & name_, 
                       const std::string & axis_, 
                       double angle_,
                       const std::string & unit_str_);

    // rotation using GDML convention through XYZ Euler angles:
    void add_rotation (const std::string & name_, 
                       const rotation_3d & rot_, 
                       const std::string & unit_str_);

    /************* Materials section *******************/

    void add_isotope (const std::string & name_, 
                      size_t atomic_number_,
                      size_t number_of_nucleons_,
                      double a_);

    void add_element (const std::string & name_, 
                      size_t atomic_number_,
                      const std::string & formula_,
                      double a_);

    void add_element (const std::string & name_, 
                      const std::map<std::string, double> & fractions_);

    void add_element (const std::string & name_, 
                      const std::string & ref_);

    void add_element (const std::string & name_, 
                      const std::string & ref1_, double fraction1_, 
                      const std::string & ref2_, double fraction2_);

    void add_element (const std::string & name_, 
                      const std::string & ref1_, double fraction1_, 
                      const std::string & ref2_, double fraction2_, 
                      const std::string & ref3_, double fraction3_);

    void add_element (const std::string & name_, 
                      const std::string & ref1_, double fraction1_, 
                      const std::string & ref2_, double fraction2_, 
                      const std::string & ref3_, double fraction3_, 
                      const std::string & ref4_, double fraction4_);

    void add_element (const std::string & name_, 
                      const std::string & ref1_, double fraction1_, 
                      const std::string & ref2_, double fraction2_, 
                      const std::string & ref3_, double fraction3_, 
                      const std::string & ref4_, double fraction4_, 
                      const std::string & ref5_, double fraction5_);

    void add_material (const std::string & name_, 
                       double atomic_number_,
                       double density_,
                       double a_);

    void add_material (const std::string & name_, 
                       const std::string & formula_,
                       double density_, 
                       const std::map<std::string, size_t> & composites_);

    void add_material (const std::string & name_, 
                       const std::string & formula_,
                       double density_, 
                       const std::map<std::string, double> & fractions_);

    /**************** Solids section *******************/
 
    static bool solid_type_is_valid (const std::string & solid_type_);
 
    static bool solid_type_is_supported (const std::string & solid_type_);

    /*
      void add_solid (const std::string & name_, 
      const std::string & solid_type_,
      const datatools::utils::properties & params_);
    */

    void add_gdml_box (const std::string & name_, 
                       double x_, double y_, double z_, 
                       const std::string & lunit_str_ = "mm");
            
    void add_gdml_tube (const std::string & name_, 
                        double rmin_, double rmax, double z_, 
                        double start_phi_, double delta_phi_,
                        const std::string & lunit_str_ = "mm",
                        const std::string & aunit_str_ = "radian");
            
    void add_gdml_orb (const std::string & name_, 
                       double r_,
                       const std::string & lunit_str_ = "mm");
            
    void add_gdml_sphere (const std::string & name_, 
                          double rmin_, double rmax, 
                          double start_phi_, double delta_phi_,
                          double start_theta_, double delta_theta_,
                          const std::string & lunit_str_ = "mm",
                          const std::string & aunit_str_ = "radian");
            
    void add_gdml_polycone (const std::string & name_, 
                            std::map<double, pair<double, double> > zplanes_, 
                            double start_phi_, double delta_phi_,
                            const std::string & lunit_str_ = "mm",
                            const std::string & aunit_str_ = "radian");
            
    void add_gdml_polyhedra (const std::string & name_, 
                             size_t num_sides_,
                             std::map<double, pair<double, double> > zplanes_, 
                             double start_phi_, double delta_phi_,
                             const std::string & lunit_str_ = "mm",
                             const std::string & aunit_str_ = "radian");
            
    void add_gdml_boolean (const std::string & name_, 
                           const std::string & boolean_type_, 
                           const std::string & first_ref_, 
                           const std::string & second_ref_, 
                           const std::string & position_ref_, 
                           const std::string & rotation_ref_);

    void add_gdml_union (const std::string & name_, 
                         const std::string & first_ref_, 
                         const std::string & second_ref_, 
                         const std::string & position_ref_, 
                         const std::string & rotation_ref_);

    void add_gdml_subtraction (const std::string & name_, 
                               const std::string & first_ref_, 
                               const std::string & second_ref_, 
                               const std::string & position_ref_, 
                               const std::string & rotation_ref_);

    void add_gdml_intersection (const std::string & name_, 
                                const std::string & first_ref_, 
                                const std::string & second_ref_, 
                                const std::string & position_ref_, 
                                const std::string & rotation_ref_);
    
    void add_box (const std::string & name_, 
                  const box & b_,
                  const std::string & lunit_str_ = "mm");
            
    void add_cylinder (const std::string & name_, 
                       const cylinder & c_,
                       const std::string & lunit_str_ = "mm",
                       const std::string & aunit_str_ = "radian");

    void add_tube (const std::string & name_, 
                   const tube & t_,
                   const std::string & lunit_str_ = "mm",
                   const std::string & aunit_str_ = "radian");

    void add_orb (const std::string & name_, 
                  const sphere & s_,
                  const std::string & lunit_str_ = "mm",
                  const std::string & aunit_str_ = "radian");

    void add_sphere (const std::string & name_, 
                     const sphere & s_,
                     const std::string & lunit_str_ = "mm",
                     const std::string & aunit_str_ = "radian");

    void add_polycone (const std::string & name_, 
                       const polycone & s_,
                       const std::string & lunit_str_ = "mm",
                       const std::string & aunit_str_ = "radian");

    void add_polyhedra (const std::string & name_, 
                        const polyhedra & s_,
                        const std::string & lunit_str_ = "mm",
                        const std::string & aunit_str_ = "radian");

    /**************** Structures section *******************/

    class physvol
    {
    public:
      std::string volumeref;
      std::string positionref;
      std::string rotationref;
      std::string scaleref;
      physvol (const std::string & volumeref_ = "",
               const std::string & positionref_ = "",
               const std::string & rotationref_ = "",
               const std::string & scaleref_ = "");
    };


    class replicavol
    {
    public:
      static const std::string REPLICATED_ALONG_AXIS;
      std::string volumeref;
      size_t number;
      std::string mode;
      std::string direction;
      double width;
      double offset;
    };

    void add_volume_auxiliaries (const std::map<std::string,string> & aux_);

    void add_volume (const std::string & name_,
                     const std::string & material_ref_,
                     const std::string & solid_ref_,
                     const std::map<std::string, string> & aux_);

    void add_volume (const std::string & name_,
                     const std::string & material_ref_,
                     const std::string & solid_ref_);

    void add_volume (const std::string & name_,
                     const std::string & material_ref_,
                     const std::string & solid_ref_,
                     const std::list<physvol> & phys_vols_,
                     const std::map<std::string, string> & aux_);

    void add_volume (const std::string & name_,
                     const std::string & material_ref_,
                     const std::string & solid_ref_,
                     const std::list<physvol> & phys_vols_);

    void add_replica_volume (const std::string & name_,
                             const std::string & material_ref_,
                             const std::string & solid_ref_,
                             const replicavol & replicavol_,
                             const std::string & lunit_str_,
                             const std::string & aunit_str_,
                             const std::map<std::string, std::string> & aux_);
    
    void add_replica_volume (const std::string & name_,
                             const std::string & material_ref_,
                             const std::string & solid_ref_,
                             const replicavol & replicavol_,
                             const std::string & lunit_str_ = "mm",
                             const std::string & aunit_str_ = "radian");

    /**************** Setups section *******************/
 
    void add_setup (const std::string & name_, 
                    const std::string & world_ref_,
                    const std::string & version_ = "1.0");


   
    /**************** Header/Section  *******************/

    void xml_header (std::ostream &, 
                     const std::string & version_ = DEFAULT_XML_VERSION,
                     const std::string & encoding_ = DEFAULT_XML_ENCODING,
                     bool standalone_ = false);

    void gdml_begin (std::ostream &, 
                     const std::string & schema_ = DEFAULT_GDML_SCHEMA,
                     const std::string & xsi_ = DEFAULT_XSI);

    void gdml_end (std::ostream &);

    void gdml_section_begin (std::ostream &, 
                             const std::string & section_);
    void gdml_section_end (std::ostream &, 
                           const std::string & section_);
   
   
    /**************** Utilities *******************/
   
    void full_write (std::ostream & out_,
                     const std::string & version_  = DEFAULT_XML_VERSION,
                     const std::string & encoding_ = DEFAULT_XML_ENCODING,
                     const std::string & schema_   = DEFAULT_REMOTE_GDML_SCHEMA,
                     const std::string & xsi_      = DEFAULT_XSI);
   
    void save_file (const std::string & filename_,
                    const std::string & version_  = "1.0",
                    const std::string & encoding_ = "UTF-8",
                    const std::string & schema_   = DEFAULT_REMOTE_GDML_SCHEMA,
                    const std::string & xsi_      = DEFAULT_XSI);


    void dump (std::ostream &) const;

  private:

    bool                       _initialized_;
    bool                       _verbose_;
    streams_col_type           _streams_;
    const std::ostringstream * _external_materials_stream_;

    static bool _g_using_html_symbols_;

  };


} // end of namespace geomtools

#endif // __geomtools__gdml_writer_h

// end of gdml_writer.h
