// -*- mode: c++ ; -*-
/* polycone.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2010-03-14
 *
 * License:
 *
 * Description:
 *   Polycone 3D shape
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_POLYCONE_H_
#define GEOMTOOLS_POLYCONE_H_ 1

#include <iostream>
#include <string>
#include <map>
#include <limits>

#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  class polycone : public i_shape_3d, public i_stackable
  {
  public:
    static const std::string & polycone_label();

    enum faces_mask_type
      {
        FACE_NONE         = geomtools::FACE_NONE,
        FACE_INNER_SIDE   = datatools::bit_mask::bit00,
        FACE_OUTER_SIDE   = datatools::bit_mask::bit01,
        FACE_BOTTOM       = datatools::bit_mask::bit02,
        FACE_TOP          = datatools::bit_mask::bit03,
        FACE_ALL    = (FACE_INNER_SIDE
                       | FACE_OUTER_SIDE
                       | FACE_BOTTOM
                       | FACE_TOP)
      };

    enum datafile_column_mode {
      RMIN_RMAX = 0,
      IGNORE_RMIN = 1,
      RMIN_AS_RMAX = 2
    };

    struct r_min_max
    {
      double rmin, rmax;
    };

    struct frustrum_data {
      double z1, a1, b1;
      double z2, a2, b2;
    };

    typedef std::map<double, r_min_max> rz_col_type;

  public:

    bool is_extruded () const;

    /* stackable interface */
    double get_xmin () const;

    double get_xmax () const;

    double get_ymin () const;

    double get_ymax () const;

    double get_zmin () const;

    double get_zmax () const;

    double get_z () const;
    /* end of stackable interface */

  private:

    void _compute_surfaces_ ();

    void _compute_volume_ ();

    void _compute_limits_ ();

    void _compute_all_ ();

  public:

    unsigned int number_of_frustra() const;

    void get_frustrum(int i_, frustrum_data &) const;

    const rz_col_type & points () const;

    /// Constructor
    polycone ();

    // Destructor
    virtual ~polycone ();

    virtual std::string get_shape_name () const;

    bool is_valid () const;

    void reset ();

    void add (double z_, double rmax_, bool compute_ = true);

    void add (double z_, double rmin_, double rmax_, bool compute_ = true);

    void initialize ();

    void initialize (const std::string & filename_);

    /** Initialize the polycone from data in a file.
     * Format (ASCII) consists in lines with the
     * (Z,Rmax) format or the (Z,Rmin,Rmax) format.
     * Special metadata may be specified at the beginning
     * of the file:
     *
     * All data are given with the following length unit:
     * \code
     * #@length_unit mm
     * \endcode
     *
     * Data (Rmin) in the second column out of three columns are ignored:
     * \code
     * #@ignore_rmin
     * \endcode
     *
     * The thickness of the polycone:
     * \code
     * #@skin_thickness 0.001
     * \endcode
     *
     * The step of the polycone:
     * \code
     * #@skin_step 5.0
     * \endcode
     *
     * Example:
     * \code
     *  #@length_unit mm
     *  z1 rmin1 rmax1
     *  z2 rmin2 rmax2
     *  z3       rmax3
     *  z4 rmin4 rmax4
     * \endcode
     *
     */
    void initialize (const std::string & filename_,
                     double zmin_ = std::numeric_limits<double>::quiet_NaN (),
                     double zmax_ = std::numeric_limits<double>::quiet_NaN (),
                     int mode_ = RMIN_RMAX);

    /// Main initilalization method from a container of configuration parameters
    void initialize (const datatools::properties & setup_);

  private:

    // interpolation:
    void _build_from_envelope_and_skin_ (double thickness_,
                                         double step_,
                                         double zmin_,
                                         double zmax_);

    void _build_from_envelope_and_skin_ (double thickness_, double step_ = 0.0);

  public:

    void compute_inner_polycone (polycone & ip_);

    void compute_outer_polycone (polycone & op_);

    virtual double get_volume (uint32_t flags_ = 0) const;

    virtual double get_surface (uint32_t mask_ = FACE_ALL_BITS) const;

    double get_z_min () const;

    double get_z_max () const;

    double get_r_max () const;

    double get_parameter ( const std::string & flag_ ) const;

    virtual bool is_inside (const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool is_on_surface (const vector_3d & ,
                                int mask_    = FACE_ALL ,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    friend std::ostream & operator<< (std::ostream &, const polycone &);

    friend std::istream & operator>> (std::istream &, polycone &);

    virtual bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  private:

    rz_col_type _points_;
    double  _top_surface_;
    double  _bottom_surface_;
    double  _outer_side_surface_;
    double  _inner_side_surface_;
    double  _outer_volume_;
    double  _inner_volume_;
    double  _volume_;
    double  _z_min_;
    double  _z_max_;
    double  _r_max_;
    bool    _extruded_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_POLYCONE_H_

// end of polycone.h
