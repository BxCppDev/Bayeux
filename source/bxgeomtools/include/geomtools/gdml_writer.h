/// \file geomtools/gdml_writer.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2015-02-04
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

#ifndef GEOMTOOLS_GDML_WRITER_H
#define GEOMTOOLS_GDML_WRITER_H 1

// Standard library:
#include <iostream>
#include <sstream>
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
// - Bayeux/materials:
#include <materials/material.h>

// This project:
#include <geomtools/utils.h>

namespace geomtools {

  // Forward class declarations:
  class box;
  class right_circular_conical_frustrum;
  class ellipsoid;
  class elliptical_cylinder;
  class sphere;
  class polycone;
  class polyhedra;
  class right_polygonal_frustrum;
  class tube;
  class cylinder;
  class tessellated_solid;
  class wall_solid;

  /// \brief GDML writer class used by the geomtools GDML export functionalities
  class gdml_writer
  {
  public:

    /// Dictionary of output stream associated by section name
    typedef std::map<std::string, std::ostringstream *> streams_col_type;

    /// Return default XML version
    static const std::string & default_xml_version();

    /// Return default XML encoding
    static const std::string & default_xml_encoding();

    /// Return default XSI
    static const std::string & default_xsi();

    /// Return default GDML schema
    static const std::string & default_gdml_schema();

    /// Return default remote GDML schema
    static const std::string & default_remote_gdml_schema();

    /// Return define section tag
    static const std::string & define_section();

    /// Return materials section tag
    static const std::string & materials_section();

    /// Return solids section tag
    static const std::string & solids_section();

    /// Return structure section tag
    static const std::string & structure_section();

    /// Return setup section tag
    static const std::string & setup_section();

    /// Convert material state (gas, liquid, solid...) to GDML supported label
    static const std::string & state_convert(::materials::material::state_type);

    /// Convert string to HTML
    static std::string to_html(const std::string &,
                               bool using_html_symbols_ = false);

    /// Convert string to ASCII
    static std::string to_ascii(const std::string &,
                                bool using_html_symbols_ = true);

    /// Check initialization flag
    bool is_initialized() const;

    /// Check verbosity flag
    bool is_verbose() const;

    /// Set verbosity flag
    void set_verbose(bool);

    /// Set flag for using HTML symbol
    void set_using_html_symbols(bool u_);

    /// Check flag for using HTML symbol
    bool is_using_html_symbols() const;

    /// Return output stream associated to a GDML section
    const std::ostringstream & get_stream(const std::string & section_) const;

    /// Attach an external output stream dedicated to GDML description of materials
    void attach_external_materials(const std::ostringstream & oss_);

    /// Check if an external output stream dedicated to GDML description of materials is defined
    bool has_external_materials_stream() const;

    /// Detach the external output stream dedicated to GDML description of materials
    void reset_external_materials_stream();

    /// Default constructor
    gdml_writer();

    /// Destructor
    virtual ~gdml_writer();

    /// Initialize
    void init();

    /// Initialize
    void initialize();

    /// Reset
    void reset();

    /* ************** Defines section ****************** */

    void add_constant(const std::string & name_,
                      double value_);

    void add_quantity(const std::string & name_,
                      const std::string & quantity_type_,
                      const std::string & unit_str_,
                      double value_);

    void add_variable(const std::string & name_,
                      double value_);

    void add_variable(const std::string & name_,
                      const std::string & expr_value_);

    void add_position(const std::string & name_,
                      double x_, double y_, double z_,
                      const std::string & unit_str_);

    void add_position(const std::string & name_,
                      const std::string & x_str_,
                      const std::string & y_str_,
                      const std::string & z_str_,
                      const std::string & unit_str_);

    void add_position(const std::string & name_,
                      const vector_3d & v_,
                      const std::string & unit_str_);

    void add_rotation(const std::string & name_,
                      const std::string & axis_,
                      double angle_,
                      const std::string & unit_str_);

    // Rotation using GDML convention through XYZ Euler angles:
    void add_rotation(const std::string & name_,
                      const rotation_3d & rot_,
                      const std::string & unit_str_);

    void add_scale(const std::string & name_,
                   double x_, double y_, double z_);

    /* ************ Materials section ****************** */

    void add_isotope(const std::string & name_,
                     size_t atomic_number_,
                     size_t number_of_nucleons_,
                     double a_);

    void add_element(const std::string & name_,
                     size_t atomic_number_,
                     const std::string & formula_,
                     double a_);

    void add_element(const std::string & name_,
                     const std::map<std::string, double> & fractions_);

    void add_element(const std::string & name_,
                     const std::string & ref_);

    void add_element(const std::string & name_,
                     const std::string & ref1_, double fraction1_,
                     const std::string & ref2_, double fraction2_);

    void add_element(const std::string & name_,
                     const std::string & ref1_, double fraction1_,
                     const std::string & ref2_, double fraction2_,
                     const std::string & ref3_, double fraction3_);

    void add_element(const std::string & name_,
                     const std::string & ref1_, double fraction1_,
                     const std::string & ref2_, double fraction2_,
                     const std::string & ref3_, double fraction3_,
                     const std::string & ref4_, double fraction4_);

    void add_element(const std::string & name_,
                     const std::string & ref1_, double fraction1_,
                     const std::string & ref2_, double fraction2_,
                     const std::string & ref3_, double fraction3_,
                     const std::string & ref4_, double fraction4_,
                     const std::string & ref5_, double fraction5_);

    void add_material(const std::string & name_,
                      double atomic_number_,
                      double density_,
                      double a_,
                      const std::string & state_label_ = "",
                      double temperature_ = std::numeric_limits<double>::quiet_NaN(),
                      double pressure_ = std::numeric_limits<double>::quiet_NaN()
                      );

    void add_material(const std::string & name_,
                      const std::string & formula_,
                      double density_,
                      const std::map<std::string, size_t> & composites_,
                      const std::string & state_label_ = "",
                      double temperature_ = std::numeric_limits<double>::quiet_NaN(),
                      double pressure_ = std::numeric_limits<double>::quiet_NaN()
                      );

    void add_material(const std::string & name_,
                      const std::string & formula_,
                      double density_,
                      const std::map<std::string, double> & fractions_,
                      const std::string & state_label_ = "",
                      double temperature_ = std::numeric_limits<double>::quiet_NaN(),
                      double pressure_ = std::numeric_limits<double>::quiet_NaN()
                      );

    /* *************** Solids section ****************** */
    // GDML USER’S GUIDE Version 2.3 page 14

    /// Check if a solid type given by name is valid
    static bool solid_type_is_valid(const std::string & solid_type_);

    /// Check if a solid type given by name is supported
    static bool solid_type_is_supported(const std::string & solid_type_);

    // GDML USER’S GUIDE Version 2.3 page 14
    void add_gdml_box(const std::string & name_,
                      double x_, double y_, double z_,
                      const std::string & lunit_str_ = "mm");

    void add_box(const std::string & name_,
                 const box & b_,
                 const std::string & lunit_str_ = "mm");

    // GDML USER’S GUIDE Version 2.3 page 14
    void add_gdml_cone_segment(const std::string & name_,
                               double rmin1_, double rmax1_,
                               double rmin2_, double rmax2_,
                               double z_,
                               double startphi_, double deltaphi_,
                               const std::string & lunit_str_ = "mm",
                               const std::string & aunit_str_ = "radian");

    void add_cone_segment(const std::string & name_,
                          const right_circular_conical_frustrum & cf_,
                          const std::string & lunit_str_ = "mm",
                          const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 14
    void add_gdml_ellipsoid(const std::string & name_,
                            double rx_, double ry, double rz_,
                            double bottom_z_, double top_z_,
                            const std::string & lunit_str_ = "mm",
                            const std::string & aunit_str_ = "radian");

    void add_ellipsoid(const std::string & name_,
                       const ellipsoid & e_,
                       const std::string & lunit_str_ = "mm",
                       const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 14
    void add_gdml_elliptical_tube(const std::string & name_,
                                  double rx_, double ry, double z_,
                                  const std::string & lunit_str_ = "mm",
                                  const std::string & aunit_str_ = "radian");

    void add_elliptical_tube(const std::string & name_,
                             const elliptical_cylinder & t_,
                             const std::string & lunit_str_ = "mm",
                             const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 15
    void add_gdml_orb(const std::string & name_,
                      double r_,
                      const std::string & lunit_str_ = "mm");

    void add_orb(const std::string & name_,
                 const sphere & s_,
                 const std::string & lunit_str_ = "mm",
                 const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 17
    void add_gdml_polycone(const std::string & name_,
                           std::map<double, std::pair<double, double> > zplanes_,
                           double start_phi_, double delta_phi_,
                           const std::string & lunit_str_ = "mm",
                           const std::string & aunit_str_ = "radian");

    void add_polycone(const std::string & name_,
                      const polycone & s_,
                      const std::string & lunit_str_ = "mm",
                      const std::string & aunit_str_ = "radian");


    // GDML USER’S GUIDE Version 2.3 page 17
    void add_gdml_generic_polycone(const std::string & name_,
                                   std::map<double, double> rzpoints_,
                                   double start_phi_, double delta_phi_,
                                   const std::string & lunit_str_ = "mm",
                                   const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 19
    void add_gdml_polyhedra(const std::string & name_,
                            size_t num_sides_,
                            std::map<double, std::pair<double, double> > zplanes_,
                            double start_phi_, double delta_phi_,
                            const std::string & lunit_str_ = "mm",
                            const std::string & aunit_str_ = "radian");

    void add_polyhedra(const std::string & name_,
                       const polyhedra & s_,
                       const std::string & lunit_str_ = "mm",
                       const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 17
    void add_gdml_generic_polyhedra(const std::string & name_,
                                    size_t num_sides_,
                                    std::map<double, double> rzpoints_,
                                    double start_phi_, double delta_phi_,
                                    const std::string & lunit_str_ = "mm",
                                    const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 19
    void add_gdml_sphere(const std::string & name_,
                         double rmin_, double rmax,
                         double start_phi_, double delta_phi_,
                         double start_theta_, double delta_theta_,
                         const std::string & lunit_str_ = "mm",
                         const std::string & aunit_str_ = "radian");

    void add_sphere(const std::string & name_,
                    const sphere & s_,
                    const std::string & lunit_str_ = "mm",
                    const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 22
    void add_gdml_tube(const std::string & name_,
                       double rmin_, double rmax, double z_,
                       double start_phi_, double delta_phi_,
                       const std::string & lunit_str_ = "mm",
                       const std::string & aunit_str_ = "radian");

    void add_tube(const std::string & name_,
                  const tube & t_,
                  const std::string & lunit_str_ = "mm",
                  const std::string & aunit_str_ = "radian");

    void add_cylinder(const std::string & name_,
                      const cylinder & c_,
                      const std::string & lunit_str_ = "mm",
                      const std::string & aunit_str_ = "radian");

    // GDML USER’S GUIDE Version 2.3 page 26
    void add_tessellated(const std::string & name_,
                         const tessellated_solid & ts_,
                         const std::string & lunit_str_ = "mm");

    // GDML USER’S GUIDE Version 2.3 page 26
    void add_wall(const std::string & name_,
                  const wall_solid & ts_,
                  const std::string & lunit_str_ = "mm");

    // GDML USER’S GUIDE Version 2.3 page 28
    void add_gdml_boolean(const std::string & name_,
                          const std::string & boolean_type_,
                          const std::string & first_ref_,
                          const std::string & second_ref_,
                          const std::string & position_ref_,
                          const std::string & rotation_ref_);

    void add_gdml_union(const std::string & name_,
                        const std::string & first_ref_,
                        const std::string & second_ref_,
                        const std::string & position_ref_,
                        const std::string & rotation_ref_);

    void add_gdml_subtraction(const std::string & name_,
                              const std::string & first_ref_,
                              const std::string & second_ref_,
                              const std::string & position_ref_,
                              const std::string & rotation_ref_);

    void add_gdml_intersection(const std::string & name_,
                               const std::string & first_ref_,
                               const std::string & second_ref_,
                               const std::string & position_ref_,
                               const std::string & rotation_ref_);

    /* *************** Structures section ****************** */
    // GDML USER’S GUIDE Version 2.3 page 29

    /// \brief Description of a physical volume
    class physvol
    {
    public:
      std::string volumeref;
      std::string positionref;
      std::string rotationref;
      std::string scaleref;
      physvol(const std::string & volumeref_ = "",
              const std::string & positionref_ = "",
              const std::string & rotationref_ = "",
              const std::string & scaleref_ = "");
    };

    /// \brief Description of a replica volume
    class replicavol
    {
    public:
      static const std::string & replicated_along_axis();
      std::string volumeref;
      size_t number;
      std::string mode;
      std::string direction;
      double width;
      double offset;
    };

    void add_volume_auxiliaries(const std::map<std::string, std::string> & aux_);

    void add_volume(const std::string & name_,
                    const std::string & material_ref_,
                    const std::string & solid_ref_,
                    const std::map<std::string, std::string> & aux_);

    void add_volume(const std::string & name_,
                    const std::string & material_ref_,
                    const std::string & solid_ref_);

    void add_volume(const std::string & name_,
                    const std::string & material_ref_,
                    const std::string & solid_ref_,
                    const std::list<physvol> & phys_vols_,
                    const std::map<std::string, std::string> & aux_);

    void add_volume(const std::string & name_,
                    const std::string & material_ref_,
                    const std::string & solid_ref_,
                    const std::list<physvol> & phys_vols_);

    void add_replica_volume(const std::string & name_,
                            const std::string & material_ref_,
                            const std::string & solid_ref_,
                            const replicavol & replicavol_,
                            const std::string & lunit_str_,
                            const std::string & aunit_str_,
                            const std::map<std::string, std::string> & aux_);

    void add_replica_volume(const std::string & name_,
                            const std::string & material_ref_,
                            const std::string & solid_ref_,
                            const replicavol & replicavol_,
                            const std::string & lunit_str_ = "mm",
                            const std::string & aunit_str_ = "radian");

    /* *************** Setups section ****************** */

    void add_setup(const std::string & name_,
                   const std::string & world_ref_,
                   const std::string & version_ = "1.0");



    /* *************** Header/Section  ****************** */

    void xml_header(std::ostream &,
                    const std::string & xml_version_ = "",
                    const std::string & xml_encoding_ = "",
                    bool standalone_ = false);

    void gdml_begin(std::ostream &,
                    const std::string & schema_ = "",
                    const std::string & xsi_ = "<default>");

    void gdml_end(std::ostream &);

    void gdml_section_begin(std::ostream &,
                            const std::string & section_);
    void gdml_section_end(std::ostream &,
                          const std::string & section_);


    /* *************** Utilities ****************** */

    /// Write the full GDML description in an output stream
    void full_write(std::ostream & out_,
                    const std::string & version_  = "",
                    const std::string & encoding_ = "",
                    const std::string & schema_   = "",
                    const std::string & xsi_      = "<default>");

    /// Write the full GDML description in a file
    void save_file(const std::string & filename_,
                   const std::string & version_  = "",
                   const std::string & encoding_ = "",
                   const std::string & schema_   = "",
                   const std::string & xsi_      = "<default>");


    /// Basic print
    void dump(std::ostream &) const;

  protected:

    /// Allocate the output streams associated to GDML sections
    void _allocate_streams();

    /// Deallocate the output streams associated to GDML sections
    void _free_streams();

    /// Return a const reference to an output stream associated to a given GDML section
    const std::ostringstream & _get_stream(const std::string & section_) const;

    /// Return a mutable reference to an output stream associated to a given GDML section
    std::ostringstream & _get_stream(const std::string & section_);

  private:

    bool                       _initialized_; //!< Initialization flag
    bool                       _verbose_; //!< Verbosity flag
    streams_col_type           _streams_; //!< Dictionary of output streams
    const std::ostringstream * _external_materials_stream_; //!< Handle to an external output stream dedicated to materials' GDML export
    bool                       _using_html_symbols_; //!< Flag to use HTML symbol

  };


} // end of namespace geomtools

#endif // GEOMTOOLS_GDML_WRITER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
