// -*- mode: c++ ; -*- 
/* placement_vg.cc
 */

#include <genvtx/placement_vg.h>

namespace genvtx {

  
  const geomtools::placement & placement_vg::get_placement () const
  {
    return __placement;
  }
  
  geomtools::placement & placement_vg::get_placement ()
  {
    return __placement;
  }
  
  void placement_vg::set_placement (const geomtools::placement & new_value_)
  {
    __placement = new_value_;
    return;
  }

  bool placement_vg::has_vg () const
  {
    return __vg != 0;
  }

  bool placement_vg::is_owned () const
  {
    return __owned;
  }
  
  const i_vertex_generator & placement_vg::get_vg () const
  {
    if (__vg == 0)
      {
	throw runtime_error ("genvtx::placement_vg::get_vg: Missing vertex generator !");
      }
    return *__vg;
  }

  void placement_vg::__clear_vg ()
  {
    if (__vg != 0)
      {
	if (__owned)
	  {
	    delete __vg;
	  }
	__vg = 0;
      }
    __owned = false;
    return;
  }

  void placement_vg::set_vg (i_vertex_generator & new_value_)
  {
    __clear_vg ();
    __owned = false;
    __vg = &new_value_;
    return;
  }
  
  void  placement_vg::set_vg (i_vertex_generator * new_value_)
  {
    __clear_vg ();
    __owned = true;
    __vg = new_value_;
    return;
  }
  
  
  // ctor/dtor:
  placement_vg::placement_vg ()
  {
    __owned = false;
    __vg = 0;
    return;
  }
  
  placement_vg::~placement_vg ()
  {
    __clear_vg ();
    return;
  }
  
  void placement_vg::_shoot_vertex (mygsl::rng & random_, 
				    geomtools::vector_3d & vertex_)
  {
    bool devel = g_debug;
    if (__vg == 0)
      {
	throw runtime_error ("genvtx::placement_vg::_shoot_vertex: Missing vertex generator !");
      }
    if (! __placement.is_valid ())
      {
	throw runtime_error ("genvtx::placement_vg::_shoot_vertex: Invalid placement !");	
      }
    geomtools::invalidate (vertex_);
    geomtools::vector_3d vertex;
    geomtools::invalidate (vertex);
    __vg->shoot_vertex (random_, vertex);
    if (geomtools::is_valid (vertex))
      {
	__placement.child_to_mother (vertex, vertex_);
      }
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  placement_vg::create (const properties & configuration_, void * user_)
  {
    cerr << "DEVEL: genvtx::placement_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "genvtx::placement_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    double x, y, z;
    double phi, theta, delta;
    x = y = z = 0.0;
    phi = theta = delta = 0.0;
    i_vertex_generator * vg = 0;

    // parameters of the cut:
    string generator_id;

    if (configuration_.has_key ("generator.id"))
      {
	generator_id = configuration_.fetch_string ("generator.id");
	if (! get_vertex_generator_db ().has_vertex_generator (generator_id))
	  {
	    ostringstream message;
	    message << "genvtx::placement_vg::create: Invalid vertex generator id '"
		    << generator_id << "' !";
	    throw runtime_error (message.str ());
	  }
	vg_creator_t vgc = get_vertex_generator_db ().get_vertex_generator (generator_id);
	// vg = 
      }

    if (configuration_.has_key ("placement.x"))
      {
	x = configuration_.fetch_real ("placement.x");
      }

    if (configuration_.has_key ("placement.y"))
      {
	y = configuration_.fetch_real ("placement.y");
      }

    if (configuration_.has_key ("placement.z"))
      {
	z = configuration_.fetch_real ("placement.z");
      }

    if (configuration_.has_key ("placement.phi"))
      {
	phi = configuration_.fetch_real ("placement.phi");
      }

    if (configuration_.has_key ("placement.theta"))
      {
	theta = configuration_.fetch_real ("placement.theta");
      }

    if (configuration_.has_key ("placement.delta"))
      {
	delta = configuration_.fetch_real ("placement.delta");
      }

    // create a new parameterized 'placement_vg' instance:
    placement_vg * ptr = new placement_vg;
    geomtools::placement pl (x, y, z, phi, theta, delta);
    ptr->set_placement (pl);
    ptr->set_vg (vg);
    return ptr;	
  }

  string placement_vg::vg_id () const
  {
    return "genvtx::placement_vg";
  }

  vg_creator_t placement_vg::vg_creator () const
  {
    return placement_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<placement_vg> placement_vg::__CR;
  
} // end of namespace genvtx

// end of placement_vg.cc
