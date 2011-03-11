// -*- mode: c++ ; -*- 
/* spot_vertex_generator.cc
 */

#include <genvtx/spot_vertex_generator.h>

namespace genvtx {

  const geomtools::vector_3d &  spot_vertex_generator::get_spot () const
  {
    return __spot;
  }
  
  void spot_vertex_generator::set_spot (const geomtools::vector_3d & new_value_)
  {
    __spot = new_value_;
    return;
  }
  
  void spot_vertex_generator::set_spot (double x_, double y_, double z_)
  {
    __spot.set (x_, y_, z_);
    return;
  }
  
  // ctor:
  spot_vertex_generator::spot_vertex_generator ()
  {
    return;
  }

  spot_vertex_generator::spot_vertex_generator (double x_, double y_, double z_)
  {
    set_spot (x_, y_, z_);
    return;
  }

  spot_vertex_generator::spot_vertex_generator (const geomtools::vector_3d & spot_)
  {
    __spot = spot_;
    return;
  }
  
  // dtor:
  spot_vertex_generator::~spot_vertex_generator ()
  {
    return;
  }
  
  void spot_vertex_generator::_shoot_vertex (mygsl::rng & random_, 
			       geomtools::vector_3d & vertex_)
  {
    vertex_ = __spot;
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  spot_vertex_generator::create (const properties & configuration_, void * user_)
  {
    cerr << "DEVEL: genvtx::spot_vertex_generator::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "genvtx::spot_vertex_generator::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the spot vertex generator:
    double x, y, z;
    x = y = z = 0.0;
    double lunit = 1.0;
    string lunit_str;;

    if (configuration_.has_key ("spot.x"))
      {
	x = configuration_.fetch_real ("spot.x");
      }

    if (configuration_.has_key ("spot.y"))
      {
	y = configuration_.fetch_real ("spot.y");
      }

    if (configuration_.has_key ("spot.z"))
      {
	z = configuration_.fetch_real ("spot.z");
      }

    if (configuration_.has_key ("spot.length_unit"))
      {
	lunit_str = configuration_.fetch_string ("spot.length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }

    if (! lunit_str.empty ())
      {
	x *= lunit;
	y *= lunit;
	z *= lunit;
      }

    // create a new parameterized 'spot_vertex_generator' instance:
    spot_vertex_generator * ptr = new spot_vertex_generator;
    ptr->set_spot (x, y, z);
    return ptr;	
  }

  string spot_vertex_generator::vg_id () const
  {
    return "genvtx::spot_vertex_generator";
  }

  vg_creator_t spot_vertex_generator::vg_creator () const
  {
    return spot_vertex_generator::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<spot_vertex_generator> spot_vertex_generator::__CR;

} // end of namespace genvtx

// end of spot_vertex_generator.cc
