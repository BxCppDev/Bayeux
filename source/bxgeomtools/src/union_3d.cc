// -*- mode: c++; -*- 
/* union_3d.cc
 */

#include <geomtools/union_3d.h>

namespace geomtools {

  const std::string union_3d::UNION_3D_LABEL = "union_3d";

  std::string 
  union_3d::get_shape_name () const
  {
    return UNION_3D_LABEL;
  }

  union_3d::union_3d () : i_composite_shape_3d ()
  {
  }
  
  union_3d::~union_3d ()
  {
  }
  
  bool 
  union_3d::is_inside (const vector_3d & position_, 
		       double skin_) const
  {
    const shape_t & sh1 = get_shape1 ();
    const shape_t & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    vector_3d pos1 = p1.mother_to_child (position_);
    vector_3d pos2 = p2.mother_to_child (position_);
    bool res = false;
    res = sh3d1.is_inside (pos1, skin_) 
      || sh3d2.is_inside (pos2, skin_);
    return res;
  }
    
  bool 
  union_3d::is_on_surface (const vector_3d & position_, 
			   int mask_, 
			   double skin_) const
  {
    const shape_t & sh1 = get_shape1 ();
    const shape_t & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    vector_3d pos1 = p1.mother_to_child (position_);
    vector_3d pos2 = p2.mother_to_child (position_);
    bool res = false;
    res = (sh3d1.is_on_surface (pos1, ALL_SURFACES, skin_) 
	   && ! sh3d2.is_inside (pos2, skin_))
      || (sh3d2.is_on_surface (pos2, ALL_SURFACES, skin_)
	  && ! sh3d1.is_inside (pos1, skin_));
    return res;
  }
  
  bool 
  union_3d::find_intercept (const vector_3d & from_, 
			    const vector_3d & direction_,
			    vector_3d & intercept_,
			    int & face_,
			    double skin_) const
  {
    bool debug = false;
    //debug = true;
    if (debug)
      {
	std::clog << "union_3d::find_intercept: entering..." << std::endl;
      }
    const shape_t & sh1 = get_shape1 ();
    const shape_t & sh2 = get_shape2 ();
    if (debug)
      {
	sh1.dump (std::clog);
	sh2.dump (std::clog);
      }
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    if (debug)
      {
	std::clog << "union_3d::find_intercept: shape1 is '" 
		  << sh3d1.get_shape_name () << "'" << std::endl;
	std::clog << "union_3d::find_intercept: shape2 is '" 
		  << sh3d2.get_shape_name () << "'" << std::endl;
      }
    vector_3d pos1 = p1.mother_to_child (from_);
    vector_3d pos2 = p2.mother_to_child (from_);
    vector_3d dir1 = p1.mother_to_child_direction (direction_);
    vector_3d dir2 = p2.mother_to_child_direction (direction_);
    if (debug)
      {
	std::clog << "union_3d::find_intercept: pos1=" << pos1 << std::endl;
	std::clog << "union_3d::find_intercept: pos2=" << pos2 << std::endl;
	std::clog << "union_3d::find_intercept: dir1=" << dir1 << std::endl;
	std::clog << "union_3d::find_intercept: dir2=" << dir2 << std::endl;
      }

    // XXX wrong algorithme: needs corrections...
    vector_3d i1, i2;
    int f1, f2;
    bool ok1;
    ok1 = sh3d1.find_intercept (pos1, dir1, i1, f1, skin_);
    bool ok2;
    ok2 = sh3d2.find_intercept (pos2, dir2, i2, f2, skin_);
    //ok1 = false;
    //ok2 = false;
    if (debug)
      {
	std::clog << "union_3d::find_intercept: ok1=" << ok1 << std::endl;
	std::clog << "union_3d::find_intercept: ok2=" << ok2 << std::endl;
      }
    if (ok1 && ! ok2)
      {
	face_ = f1;
	vector_3d intercept1 = p1.child_to_mother (i1);
	intercept_ = intercept1;
	return true;
      }
    if (ok2 && ! ok1)
      {
	face_ = f2;
	vector_3d intercept2 = p2.child_to_mother (i2);
	intercept_ = intercept2;
	return true;
      }
    if (ok1 && ok2)
      {
	vector_3d intercept1 = p1.child_to_mother (i1);
	face_ = f1;
	intercept_ = intercept1;
	double dist1 = (intercept1 - from_).mag ();
	vector_3d intercept2 = p2.child_to_mother (i2);
	double dist2 = (intercept2 - from_).mag ();
	if (dist2 < dist1)
	  {
	    face_ = f2;
	    intercept_ = intercept2;
	  }
	return true;
      }
    
    face_ = face_3d::FACE_NONE_BIT;
    return false; // there is no solution
  }


} // end of namespace geomtools

// end of union_3d.cc
