// -*- mode: c++ ; -*- 
/* polyhedra.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-08
 * Last modified: 2010-10-14
 * 
 * License: 
 * 
 * Description: 
 *   Polyhedra 3D shape with regular polygon 
 *   sections (fr: équilatéral, inscribed, circonscriptible)
 *   See also: http://en.wikipedia.org/wiki/Frustum
 *
 * History: 
 * 
 */

#ifndef __geomtools__polyhedra_h
#define __geomtools__polyhedra_h 1

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <datatools/utils/utils.h>
#include <datatools/utils/units.h>

#include <geomtools/i_shape_3d.h>
#include <geomtools/regular_polygon.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  using namespace std;

  class polyhedra : public i_shape_3d, public i_stackable
  {
  public:

    static const string POLYHEDRA_LABEL;
    static const size_t MIN_NUMBER_OF_SIDES = 3;

    enum faces_mask_t
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

    typedef map<double, r_min_max> rz_col_t;

  private:

    size_t   __n_sides;
    rz_col_t __points;
    double  __top_surface;
    double  __bottom_surface;
    double  __side_surface;
    double  __outer_side_surface;
    double  __inner_side_surface;
    double  __outer_volume;
    double  __inner_volume;
    double  __volume;
    double  __z_min;
    double  __z_max;
    double  __r_max;
    bool    __extruded;
    
  public: 

    bool is_extruded () const;
    
    double get_xmin () const
    {
      return -__r_max;
    }
    
    double get_xmax () const
    {
      return +__r_max;
    }
    
    double get_ymin () const
    {
      return -__r_max;
    }
    
    double get_ymax () const
    {
      return +__r_max;
    }
    
    double get_zmin () const
    {
      return __z_min;
    }
    
    double get_zmax () const
    {
      return __z_max;
    }

  private:

    void __compute_surfaces ();

    void __compute_volume ();

    void __compute_limits ();

    void __compute_all ();

  public:

    void set_n_sides (size_t n_sides_);

    size_t get_n_sides () const;

    const rz_col_t & points () const;

    // ctor:
    polyhedra ();

    // dtor:
    virtual ~polyhedra ();
  
    // methods:
      
    virtual string get_shape_name () const;

    bool is_valid () const;

    void reset ();

    void add (double z_, double rmax_, bool compute_ = true);

    void add (double z_, double rmin_, double rmax_, bool compute_ = true);

    void initialize (const datatools::utils::properties & setup_); 

    void initialize (const string & filename_);

    void compute_inner_polyhedra (polyhedra & ip_);

    void compute_outer_polyhedra (polyhedra & op_);

    double get_volume () const;

    double get_surface (int mask_ = FACE_ALL) const;

    double get_z_min () const;

    double get_z_max () const;

    double get_r_max () const;

    double get_parameter ( const string & flag_ ) const;

    virtual bool is_inside (const vector_3d &, 
			    double skin_ = USING_PROPER_SKIN) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool 
    is_on_surface (const vector_3d & , 
		   int mask_    = FACE_ALL , 
		   double skin_ = USING_PROPER_SKIN) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    friend ostream & operator<< (ostream &, const polyhedra &);

    friend istream & operator>> (istream &, polyhedra &);
      
    virtual bool find_intercept (const vector_3d & from_, 
				 const vector_3d & direction_,
				 intercept_t & intercept_,
				 double skin_ = USING_PROPER_SKIN) const;

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;

  };

} // end of namespace geomtools

#endif // __geomtools__polyhedra_h

// end of polyhedra.h
