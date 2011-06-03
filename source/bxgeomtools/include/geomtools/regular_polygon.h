// -*- mode: c++ ; -*- 
/* regular_polygon.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 * 
 * License: 
 * 
 * Description: 
 *   A regular_polygon in x-y plane
 * 
 * History: 
 * 
 */

#ifndef __geomtools__regular_polygon_h
#define __geomtools__regular_polygon_h 1

#include <string>

#include <geomtools/i_shape_2d.h>

namespace geomtools {

  using namespace std;

  class regular_polygon : public i_shape_2d
  {
    
  public:
    static const string REGULAR_POLYGON_LABEL;
 
    enum mode_t
      {
	BUILD_BY_RADIUS  = 1,
	BUILD_BY_APOTHEM = 2
      };   

  private: 
    /*
     *       C         B
     *        *-------*
     *       /       / \
     *      /       /   * M         
     *     /    O  /     \
     *  D *       * <-r-> *-- -----------------> y
     *     \      :      / A
     *      \     :     /
     *       \    :    /
     *        *-------*
     *       E    |    F
     *            |
     *            V x
     *
     *   A, B... : vertices
     *   OA = OB : radius (or circumradius)
     *   OM      : apothem (http://en.wikipedia.org/wiki/Apothem)
     *   AB      : side's length
     *   (A0B)   : reference angle
     */

    uint32_t __n_sides; //! The number of sides/vertices of the polygon
    double   __r;       //! The radius of the polygon, i.e. the distance from center to vertices (circumradius)

  public: 

    bool is_valid () const;

    uint32_t get_n_sides () const;

    void set_n_sides (uint32_t);

    double get_apothem () const;

    double get_perimeter () const;

    double get_side_length () const;

    double get_reference_angle () const;

    double get_r () const;

    double get_radius () const;

    void set_r (double);

    void set_diameter (double);
 
    double get_diameter () const;
 
    double get_surface () const;
 
    void get_vertex (int n_, double & x_, double & y_) const;
 
    void get_vertex (int n_, vector_3d & vertex_) const;

  public: 
    // ctor:
    regular_polygon ();

    regular_polygon (uint32_t n_sides_, double r_, int mode_ = BUILD_BY_RADIUS);

    // dtor:
    virtual ~regular_polygon ();
  
    // methods:
    virtual string get_shape_name () const;
      
    virtual bool is_on_surface (const vector_3d &,
				double tolerance_ = USING_PROPER_TOLERANCE) const; 
    
    virtual vector_3d get_normal_on_surface (const vector_3d & position_,
					     bool up_ = true) const;
    
    virtual bool find_intercept (const vector_3d & from_, 
				 const vector_3d & direction_,
				 intercept_t & intercept_,
				 double tolerance_ = USING_PROPER_TOLERANCE) const;

    virtual void tree_dump (ostream & out_ = clog, 
			    const string & title_ = "", 
			    const string & indent_ = "", 
			    bool inherit_= false) const;
 
  };

} // end of namespace geomtools

#endif // __geomtools__regular_polygon_h

// end of regular_polygon.h
