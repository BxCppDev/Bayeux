// -*- mode: c++ ; -*- 
/* box_vg.cc
 */

#include <genvtx/box_vg.h>

namespace genvtx {

  bool box_vg::g_debug = false;

  void box_vg::__assert_lock (const string & where_)
  {
    if (__initialized)
      {
	ostringstream message;
	message << "box_vg::__assert_lock: " << where_ << ": "
		<< "Object is locked !";
	throw runtime_error (message.str());
      }
  }

  bool box_vg::is_initialized () const
  {
    return __initialized;
  }

  int box_vg::get_mode () const
  {
    return __mode;
  }

  void box_vg::set_mode (int mode_)
  {
    if ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE))
      {
	throw runtime_error ("box_vg::set_mode: Invalid mode !");
      }
    __mode = mode_;
  }

  void box_vg::set_surface_mask (int surface_mask_)
  {
    __assert_lock ("box_vg::set_surface_mask");
    __surface_mask = surface_mask_;
  }

  void box_vg::set_skin_skip (double skin_skip_)
  {
    __assert_lock ("box_vg::set_surface_mask");
    __skin_skip = skin_skip_;
  }

  void box_vg::set_skin_thickness (double skin_thickness_)
  {
    __assert_lock ("box_vg::set_skin_thickness");
    __skin_thickness = skin_thickness_;
  }

  void box_vg::set_bulk (double skin_thickness_)
  {
    __assert_lock ("box_vg::set_bulk");
    __mode = MODE_BULK;
  }

  void box_vg::set_surface (int surface_mask_)
  {
    __assert_lock ("box_vg::set_surface");
    __mode = MODE_SURFACE;
    set_surface_mask (surface_mask_);
  }

  void box_vg::set_box (const geomtools::box & box_)
  {
    __assert_lock ("box_vg::set_box");
    __box = box_;
  }

  const geomtools::box & box_vg::get_box () const
  {
    return __box;
  }
  
  // ctor:
  box_vg::box_vg ()
  {
    __initialized = false;
    __reset ();
  }
  
  // dtor:
  box_vg::~box_vg ()
  {
  }

  void box_vg::init ()
  {
    if (__initialized)
      {
	throw runtime_error ("box_vg::init: Already initialized !");
      }
    __init ();
    __initialized = true;
  }

  void box_vg::reset ()
  {
    if (! __initialized)
      {
	throw runtime_error ("box_vg::reset: Not initialized !");
      }
    __reset ();
    __initialized = false;
   }

  void box_vg::__init ()
  {
    bool devel = g_debug;
    if (__mode == MODE_SURFACE) 
      {
	if (__surface_mask == 0)
	  {
	    throw runtime_error ("box_vg::__init: Surface mask is null !");
	  }
	double s = __box.get_surface (__surface_mask);
	if (devel) clog << "DEVEL: box_vg::__init: Total surface = " << s << endl;
	__sum_weight[0] = __box.get_surface (__surface_mask & geomtools::box::FACE_BACK);
	__sum_weight[1] = __box.get_surface (__surface_mask & geomtools::box::FACE_FRONT);
	__sum_weight[2] = __box.get_surface (__surface_mask & geomtools::box::FACE_LEFT);
	__sum_weight[3] = __box.get_surface (__surface_mask & geomtools::box::FACE_RIGHT);
	__sum_weight[4] = __box.get_surface (__surface_mask & geomtools::box::FACE_BOTTOM);
	__sum_weight[5] = __box.get_surface (__surface_mask & geomtools::box::FACE_TOP);
	for (int i = 0; i < 6; i++) 
	   {
	    __sum_weight[i] /= s;
	    if (i > 0) 
	      {
		__sum_weight[i] += __sum_weight[i - 1];
	      }
	    if (devel) clog << "DEVEL: box_vg::__init: Surface weight [" << i << "] = " << __sum_weight[i] << endl;
	  }
      }
  }

  void box_vg::__reset ()
  {
    __mode = MODE_DEFAULT;
    __surface_mask = geomtools::box::FACE_ALL;
    __skin_skip = 0.0;
    __skin_thickness = 0.0;
    for (int i = 0; i < 6; i++)
      {
	__sum_weight[i] = 0.0;
      }
  }

  void box_vg::dump (ostream & out_) const
  {
    out_ << "box_vg::dump: " << endl;
    out_ << "|-- " << "Box: " << __box << endl;
    out_ << "|-- " << "Mode: " << __mode << endl;
    out_ << "|-- " << "Surface mask: " << __surface_mask << endl;
    out_ << "|-- " << "Skin skip: " << __skin_skip << endl;
    out_ << "`-- " << "Skin thickness: " << __skin_thickness << endl;
  }
  
  void box_vg::_shoot_vertex (mygsl::rng & random_, 
			      geomtools::vector_3d & vertex_)
  {
    bool devel = g_debug;
    if (! __initialized)
      {
	init ();
      }
    geomtools::invalidate (vertex_);
    double x,y,z;
    if (__mode == MODE_BULK) 
      {
	if (devel) 
	  {
	    clog << "DEVEL: box_vg::_shoot_vertex: "
		 << "Bulk mode..." << endl;
	  }
	x = (random_.uniform () - 0.5) * (__box.get_x () - __skin_thickness);
	y = (random_.uniform () - 0.5) * (__box.get_y () - __skin_thickness);
	z = (random_.uniform () - 0.5) * (__box.get_z () - __skin_thickness);
      }

    if (__mode == MODE_SURFACE) 
      {
	if (devel) 
	  {
	    clog << "DEVEL: box_vg::_shoot_vertex: "
		 << "Surface mode..." << endl;
	  }
	double r0 = random_.uniform ();
	double r1 = random_.uniform ();
	double r2 = random_.uniform ();
	double r3 = 0.0;
	double delta_thick = 0.0;
	if (__skin_thickness > 0.0) 
	  {
	    r3 = random_.uniform ();
	    delta_thick = (r3 - 0.5) * __skin_thickness;
	  }
	
	if (r0 < __sum_weight[0]) 
	  {
	    y = (r1 - 0.5) * __box.get_y ();
	    z = (r2 - 0.5) * __box.get_z ();
	    x = -(__box.get_half_x () + __skin_skip + delta_thick);
	  }
	else if (r0 < __sum_weight[1]) 
	  {
	    y = (r1 - 0.5) * __box.get_y ();
	    z = (r2 - 0.5) * __box.get_z ();
	    x = +(__box.get_half_x () + __skin_skip + delta_thick);
	  }
	else if (r0 < __sum_weight[2]) 
	  {
	    x = (r1 - 0.5) * __box.get_x ();
	    z = (r2 - 0.5) * __box.get_z ();
	    y = -(__box.get_half_y () + __skin_skip + delta_thick);
	  }
	else if (r0 < __sum_weight[3]) 
	  {
	    x = (r1 - 0.5) * __box.get_x ();
	    z = (r2 - 0.5) * __box.get_z ();
	    y = +(__box.get_half_y () + __skin_skip + delta_thick);
	  }	
	else if (r0 < __sum_weight[4]) 
	  {
	    x = (r1 - 0.5) * __box.get_x ();
	    y = (r2 - 0.5) * __box.get_y ();
	    z = -(__box.get_half_z () + __skin_skip + delta_thick);
	  }
	else 
	  {
	    x = (r1 - 0.5) * __box.get_x ();
	    y = (r2 - 0.5) * __box.get_y ();
	    z = +(__box.get_half_z () + __skin_skip + delta_thick);
	  }	
      }
    vertex_.set (x,y, z);
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  box_vg::create (const properties & configuration_)
  {
    cerr << "DEVEL: box_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "box_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the box vertex generator:
    double x, y, z;
    x = y = z = 1.0;
    int mode =  box_vg::MODE_DEFAULT;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    int surface_mask = 0;

    if (configuration_.has_key ("mode"))
      {
	string mode_str = configuration_.fetch_string ("mode");
	if (mode_str == "surface")
	  {
	    mode = MODE_SURFACE;
	  }
	else if (mode_str == "bulk")
	  {
	    mode = MODE_BULK;
	  }
	else
	  {
	    ostringstream message;
	    message << "box_vg::create: Invalid mode '"
		    << mode_str << "' !";
	    throw runtime_error (message.str ());
	  }
      }

    if (configuration_.has_key ("skin_skip"))
      {
	skin_skip = configuration_.fetch_real ("skin_skip");
      }

    if (configuration_.has_key ("skin_thickness"))
      {
	skin_thickness = configuration_.fetch_real ("skin_thickness");
      }

    // create a new parameterized 'box_vg' instance:
    box_vg * ptr = new box_vg;
    //ptr->set_spot (x, y, z);
    return ptr;	
  }

  string box_vg::vg_id () const
  {
    return "genvtx::box_vg";
  }

  vg_creator_t box_vg::vg_creator () const
  {
    return box_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<box_vg> box_vg::__CR;
  
} // end of namespace genvtx

// end of box_vg.cc
