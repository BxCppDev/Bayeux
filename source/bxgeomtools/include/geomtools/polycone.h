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
    static const std::string POLYCONE_LABEL;

    enum faces_mask_type
      {
        FACE_NONE   = FACE_NONE_BIT,
        FACE_INNER_SIDE   = 0x1,
        FACE_OUTER_SIDE   = 0x2,
        FACE_BOTTOM = 0x4,
        FACE_TOP    = 0x8,
        FACE_ALL    = (FACE_INNER_SIDE
                       | FACE_OUTER_SIDE
                       | FACE_BOTTOM 
                       | FACE_TOP)
      };  

    struct r_min_max
    {
      double rmin, rmax;
    };

    typedef std::map<double, r_min_max> rz_col_type;
  
  public:

    bool is_extruded () const;
    
    double get_xmin () const;
    
    double get_xmax () const;
    
    double get_ymin () const;
    
    double get_ymax () const;
    
    double get_zmin () const;
    
    double get_zmax () const;
    
    double get_z () const;

  private:

    void _compute_surfaces_ ();

    void _compute_volume_ ();

    void _compute_limits_ ();

    void _compute_all_ ();
    
  public: 

    const rz_col_type & points () const;

    // ctor:
    polycone ();

    // dtor:
    virtual ~polycone ();
  
    // methods:
      
    virtual std::string get_shape_name () const;

    bool is_valid () const;

    void reset ();

    void add (double z_, double rmax_, bool compute_ = true);

    void add (double z_, double rmin_, double rmax_, bool compute_ = true);

    void initialize ();


    void initialize (const std::string & filename_);

    /**
     * Initialize the polycone from data in a file.
     * Format (ASCII) consists in lines with the
     * (Z,Rmax) format or the (Z,Rmin,Rmax) format.
     * Special comments may be inserted at the beginning
     * of the file:
     *
     * All data are given with the following lenght unit:
     *    #@length_unit mm
     *
     * Data (Rmin) in the second out of three columns are ignored:
     *    #@ignore_rmin
     *
     * The thickness of the polycone:
     *    #@skin_thickness 1
     *
     * The step of the polycone:
     *    #@skin_step 5.0
     *
     * Example:
     *  >>>
     *  #@length_unit mm
     *  z1 rmin1 rmax1
     *  z2 rmin2 rmax2
     *  z3       rmax3
     *  z4 rmin4 rmax4
     *  <<<
     *
     */
    void initialize (const std::string & filename_, 
                     double zmin_ = std::numeric_limits<double>::quiet_NaN (),
                     double zmax_ = std::numeric_limits<double>::quiet_NaN ());

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

    double get_volume () const;

    double get_surface (int mask_ = FACE_ALL) const;

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
