// -*- mode: c++ ; -*- 
/* cylinder_vg.cc
 */

#include <genvtx/cylinder_vg.h>

namespace genvtx {

  bool cylinder_vg::g_debug = false;

  void cylinder_vg::__assert_lock (const string & where_)
  {
    if (__initialized)
      {
	ostringstream message;
	message << "cylinder_vg::__assert_lock: " << where_ << ": "
		<< "Object is locked !";
	throw runtime_error (message.str());
      }
  }

  bool cylinder_vg::is_initialized () const
  {
    return __initialized;
  }

  int cylinder_vg::get_mode () const
  {
    return __mode;
  }

  void cylinder_vg::set_mode (int mode_)
  {
    if ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE))
      {
	throw runtime_error ("cylinder_vg::set_mode: Invalid mode !");
      }
    __mode = mode_;
  }

  void cylinder_vg::set_surface_mask (int surface_mask_)
  {
    __assert_lock ("cylinder_vg::set_surface_mask");
    __surface_mask = surface_mask_;
  }

  void cylinder_vg::set_skin_skip (double skin_skip_)
  {
    __assert_lock ("cylinder_vg::set_surface_mask");
    __skin_skip = skin_skip_;
  }

  void cylinder_vg::set_skin_thickness (double skin_thickness_)
  {
    __assert_lock ("cylinder_vg::set_skin_thickness");
    __skin_thickness = skin_thickness_;
  }

  void cylinder_vg::set_bulk (double skin_thickness_)
  {
    __assert_lock ("cylinder_vg::set_bulk");
    __mode = MODE_BULK;
  }

  void cylinder_vg::set_surface (int surface_mask_)
  {
    __assert_lock ("cylinder_vg::set_surface");
    __mode = MODE_SURFACE;
    set_surface_mask (surface_mask_);
  }

  void cylinder_vg::set_cylinder (const geomtools::cylinder & cylinder_)
  {
    __assert_lock ("cylinder_vg::set_cylinder");
    __cylinder = cylinder_;
  }

  const geomtools::cylinder & cylinder_vg::get_cylinder () const
  {
    return __cylinder;
  }
  
  // ctor:
  cylinder_vg::cylinder_vg ()
  {
    __initialized = false;
    __reset ();
  }
  
  // dtor:
  cylinder_vg::~cylinder_vg ()
  {
  }

  void cylinder_vg::init ()
  {
    if (__initialized)
      {
	throw runtime_error ("cylinder_vg::init: Already initialized !");
      }
    __init ();
    __initialized = true;
  }

  void cylinder_vg::reset ()
  {
    if (! __initialized)
      {
	throw runtime_error ("cylinder_vg::reset: Not initialized !");
      }
    __reset ();
    __initialized = false;
   }

  void cylinder_vg::__init ()
  {
    bool devel = g_debug;
    if (__mode == MODE_SURFACE) 
      {
	if (__surface_mask == 0)
	  {
	    throw runtime_error ("cylinder_vg::__init: Surface mask is null !");
	  }
	double s = __cylinder.get_surface (__surface_mask);
	if (devel) clog << "DEVEL: cylinder_vg::__init: Total surface = " << s << endl;
	__sum_weight[0] = __cylinder.get_surface (__surface_mask & geomtools::cylinder::FACE_SIDE);
	__sum_weight[1] = __cylinder.get_surface (__surface_mask & geomtools::cylinder::FACE_BOTTOM);
	__sum_weight[2] = __cylinder.get_surface (__surface_mask & geomtools::cylinder::FACE_TOP);
	for (int i = 0; i < 3; i++) 
	   {
	    __sum_weight[i] /= s;
	    if (i > 0) 
	      {
		__sum_weight[i] += __sum_weight[i - 1];
	      }
	    if (devel) clog << "DEVEL: cylinder_vg::__init: Surface weight [" << i << "] = " << __sum_weight[i] << endl;
	  }
      }
  }

  void cylinder_vg::__reset ()
  {
    __mode = MODE_DEFAULT;
    __surface_mask = geomtools::cylinder::FACE_ALL;
    __skin_skip = 0.0;
    __skin_thickness = 0.0;
    for (int i = 0; i < 3; i++)
      {
	__sum_weight[i] = 0.0;
      }
  }

  void cylinder_vg::dump (ostream & out_) const
  {
    out_ << "cylinder_vg::dump: " << endl;
    out_ << "|-- " << "Cylinder: " << __cylinder << endl;
    out_ << "|-- " << "Mode: " << __mode << endl;
    out_ << "|-- " << "Surface mask: " << __surface_mask << endl;
    out_ << "|-- " << "Skin skip: " << __skin_skip << endl;
    out_ << "`-- " << "Skin thickness: " << __skin_thickness << endl;
  }
  
  void cylinder_vg::_shoot_vertex (mygsl::rng & random_, 
			      geomtools::vector_3d & vertex_)
  {
    bool devel = g_debug;
    if (! __initialized)
      {
	init ();
      }
    geomtools::invalidate (vertex_);
    double r, t;
    double t_max = 2. * M_PI;
    t = random_.uniform () * t_max;
    double x, y, z;
    if (__mode == MODE_BULK) 
      {
	if (devel) 
	  {
	    clog << "DEVEL: cylinder_vg::_shoot_vertex: "
		 << "Bulk mode..." << endl;
	  }
	double r_max = __cylinder.get_r () - 0.5 * __skin_thickness;
	r = sqrt (random_.uniform ()) * r_max;
	z = (random_.uniform () - 0.5) * (__cylinder.get_z () - __skin_thickness);
      }

    if (__mode == MODE_SURFACE) 
      {
	if (devel) 
	  {
	    clog << "DEVEL: cylinder_vg::_shoot_vertex: "
		 << "Surface mode..." << endl;
	  }
	double r0 = random_.uniform ();

	double delta_thick = 0.0;
	if (__skin_thickness > 0.0) 
	  {
	    double r3 = random_.uniform ();
	    delta_thick = (r3 - 0.5) * __skin_thickness;
	  }
	
	if (r0 < __sum_weight[0]) 
	  {
	    double r_min  = __cylinder.get_r () 
	      + __skin_skip
	      - 0.5 * __skin_thickness;
	    double r_max  = r_min + __skin_thickness;
	    double r_min2 = r_min * r_min;
	    double r_max2 = r_max * r_max;
	    r = (sqrt (r_min2 + random_.uniform () * (r_max2 - r_min2)));
	    z = +(random_.uniform () - 0.5) * (__cylinder.get_z ());
	  }
	else if (r0 < __sum_weight[1]) 
	  {
	    double r_max = __cylinder.get_r ();
	    r = sqrt (random_.uniform ()) * r_max;
	    z = -(__cylinder.get_half_z () + __skin_skip + delta_thick);
	  }
	else if (r0 < __sum_weight[2]) 
	  {
	    double r_max = __cylinder.get_r ();
	    r = sqrt (random_.uniform ()) * r_max;
	    z = +(__cylinder.get_half_z () + __skin_skip + delta_thick);
	  }
      }
    x = r * cos (t); 
    y = r * sin (t); 
    vertex_.set (x,y, z);
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  cylinder_vg::create (const properties & configuration_)
  {
    cerr << "DEVEL: cylinder_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "cylinder_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the cylinder vertex generator:
    double x, y, z;
    x = y = z = 1.0;
    int mode =  cylinder_vg::MODE_DEFAULT;
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
	    message << "cylinder_vg::create: Invalid mode '"
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

    // create a new parameterized 'cylinder_vg' instance:
    cylinder_vg * ptr = new cylinder_vg;
    //ptr->set_spot (x, y, z);
    return ptr;	
  }

  string cylinder_vg::vg_id () const
  {
    return "genvtx::cylinder_vg";
  }

  vg_creator_t cylinder_vg::vg_creator () const
  {
    return cylinder_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<cylinder_vg> cylinder_vg::__CR;
  
} // end of namespace genvtx

// end of cylinder_vg.cc
