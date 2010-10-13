// -*- mode: c++; -*- 
/* i_shape_3d.cc
 */

#include <geomtools/i_shape_3d.h>

namespace geomtools {

  using namespace std;

  const double i_shape_3d::DEFAULT_SKIN = i_object_3d::DEFAULT_TOLERANCE;
  const double i_shape_3d::USING_PROPER_SKIN = i_object_3d::USING_PROPER_TOLERANCE;
  
  double i_shape_3d::get_skin () const
  {
    return get_tolerance ();
  }
   
  void i_shape_3d::set_skin (double skin_)
  {
    //clog << "DEVEL: i_shape_3d::set_skin: skin_ = " << skin_ << endl;
    set_tolerance (skin_);
  }
 
  int i_shape_3d::get_dimensional () const
  {
    return DIMENSIONAL_3;
  }

  bool i_shape_3d::has_stackable_data () const
  {
    return __stackable_data != 0;
  }

  const stackable_data & i_shape_3d::get_stackable_data () const
  {
    return *__stackable_data;
  }

  void i_shape_3d::set_stackable_data (const stackable_data & sd_)
  {
    if (__stackable_data != 0)
      {
	if (&sd_ == __stackable_data) return;
	if (__owns_stackable_data)
	  {
	    delete __stackable_data;
	    __stackable_data = 0;
	  }
      }
    __owns_stackable_data = false;
    __stackable_data = &sd_;
    return;
  }

  void i_shape_3d::set_stackable_data (const stackable_data * sd_)
  {
    if (__stackable_data != 0)
      {
	if (sd_ == __stackable_data) return;
	if (__owns_stackable_data)
	  {
	    delete __stackable_data;
	    __stackable_data = 0;
	  }
      }
    __owns_stackable_data = true;
    __stackable_data = sd_;
    return;
  }
     
  // ctor:
  i_shape_3d::i_shape_3d () : i_object_3d ()
  {
    __owns_stackable_data = false;
    __stackable_data = 0;
  }
     
  // ctor:
  i_shape_3d::i_shape_3d (double skin_) : i_object_3d (skin_)
  {
    __owns_stackable_data = false;
    __stackable_data = 0;
    /*
    clog << "DEVEL: i_shape_3d::i_shape_3d: i_object_3d::DEFAULT_TOLERANCE = " 
	 << i_object_3d::DEFAULT_TOLERANCE << endl;
    clog << "DEVEL: i_shape_3d::i_shape_3d: i_shape_3d::DEFAULT_SKIN = " 
	 << i_shape_3d::DEFAULT_SKIN << endl;
    clog << "DEVEL: i_shape_3d::i_shape_3d: skin_ = " << skin_ << endl;
    */
  }
  
  // dtor:
  i_shape_3d::~i_shape_3d ()
  {
    if (__stackable_data == 0)
      {
	if (__owns_stackable_data)
	  {
	    delete __stackable_data;
	    __stackable_data = 0;
	  }
      }
  }

  bool i_shape_3d::is_outside (const vector_3d & pos_, 
			       double skin_) const
  {
    return ! is_inside (pos_, skin_) 
      && ! is_on_surface (pos_, ALL_SURFACES, skin_);
  }

} // end of namespace geomtools

// end of i_shape_3d.cc
