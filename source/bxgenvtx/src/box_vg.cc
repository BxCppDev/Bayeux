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
	message << "genvtx::box_vg::__assert_lock: " << where_ << ": "
		<< "Object is locked !";
	throw runtime_error (message.str());
      }
    return;
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
	throw runtime_error ("genvtx::box_vg::set_mode: Invalid mode !");
      }
    __mode = mode_;
    return;
  }

  void box_vg::set_surface_mask (int surface_mask_)
  {
    __assert_lock ("genvtx::box_vg::set_surface_mask");
    __surface_mask = surface_mask_;
    return;
  }

  void box_vg::set_skin_skip (double skin_skip_)
  {
    __assert_lock ("genvtx::box_vg::set_surface_mask");
    __skin_skip = skin_skip_;
    return;
  }

  void box_vg::set_skin_thickness (double skin_thickness_)
  {
    __assert_lock ("genvtx::box_vg::set_skin_thickness");
    __skin_thickness = skin_thickness_;
    return;
  }

  void box_vg::set_bulk (double skin_thickness_)
  {
    __assert_lock ("genvtx::box_vg::set_bulk");
    __mode = MODE_BULK;
    return;
  }

  void box_vg::set_surface (int surface_mask_)
  {
    __assert_lock ("genvtx::box_vg::set_surface");
    __mode = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  bool box_vg::has_box_ref () const
  {
    return __box_ref != 0;
  }

  void box_vg::set_box_ref (const geomtools::box & box_)
  {
    __assert_lock ("genvtx::box_vg::set_box_ref");
    __box_ref = &box_;
    return;
  }

  void box_vg::set_box (const geomtools::box & box_)
  {
    __assert_lock ("genvtx::box_vg::set_box");
    __box = box_;
    return;
  }

  const geomtools::box & box_vg::get_box_safe () const
  {
    if (has_box_ref ())
      {
	return get_box_ref ();
      }
    return get_box ();
  }

  const geomtools::box & box_vg::get_box_ref () const
  {
    return *__box_ref;
  }

  const geomtools::box & box_vg::get_box () const
  {
    return __box;
  }
  
  // ctor:
  box_vg::box_vg ()
  {
    __initialized = false;
    __box_ref = 0;
    __reset ();
    return;
  }
  
  // dtor:
  box_vg::~box_vg ()
  {
    return;
  }

  void box_vg::init ()
  {
    if (__initialized)
      {
	throw runtime_error ("genvtx::box_vg::init: Already initialized !");
      }
    __init ();
    __initialized = true;
    return;
  }

  void box_vg::reset ()
  {
    if (! __initialized)
      {
	throw runtime_error ("genvtx::box_vg::reset: Not initialized !");
      }
    __reset ();
    __initialized = false;
    return;
  }

  void box_vg::__init ()
  {
    bool devel = g_debug;
    if (__mode == MODE_SURFACE) 
      {
	if (__surface_mask == 0)
	  {
	    throw runtime_error ("genvtx::box_vg::__init: Surface mask is null !");
	  }
	double s = __box.get_surface (__surface_mask);
	if (devel) clog << "DEVEL: genvtx::box_vg::__init: Total surface = " << s << endl;
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
	    if (devel) clog << "DEVEL: genvtx::box_vg::__init: Surface weight [" << i << "] = " << __sum_weight[i] << endl;
	  }
      }
    return;
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
    return;
  }

  void box_vg::dump (ostream & out_) const
  {
    out_ << "genvtx::box_vg::dump: " << endl;
    if (has_box_ref ())
      {
	out_ << "|-- " << "Box ref: " << *__box_ref << " [" << __box_ref << ']' << endl;
      }
    else
      {
	out_ << "|-- " << "Box: " << __box << endl;
      }
    out_ << "|-- " << "Mode: " << __mode << endl;
    out_ << "|-- " << "Surface mask: " << __surface_mask << endl;
    out_ << "|-- " << "Skin skip: " << __skin_skip << endl;
    out_ << "`-- " << "Skin thickness: " << __skin_thickness << endl;
    return;
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
    double x, y, z;
    const geomtools::box * the_box = &__box;
    if (has_box_ref ())
      {
	the_box = __box_ref;
      }

    if (__mode == MODE_BULK) 
      {
	if (devel) 
	  {
	    clog << "DEVEL: genvtx::box_vg::_shoot_vertex: "
		 << "Bulk mode..." << endl;
	  }
	x = (random_.uniform () - 0.5) * (the_box->get_x () - __skin_thickness);
	y = (random_.uniform () - 0.5) * (the_box->get_y () - __skin_thickness);
	z = (random_.uniform () - 0.5) * (the_box->get_z () - __skin_thickness);
      }

    if (__mode == MODE_SURFACE) 
      {
	if (devel) 
	  {
	    clog << "DEVEL: genvtx::box_vg::_shoot_vertex: "
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
	    y = (r1 - 0.5) * the_box->get_y ();
	    z = (r2 - 0.5) * the_box->get_z ();
	    x = -(the_box->get_half_x () + __skin_skip + delta_thick);
	  }
	else if (r0 < __sum_weight[1]) 
	  {
	    y = (r1 - 0.5) * the_box->get_y ();
	    z = (r2 - 0.5) * the_box->get_z ();
	    x = +(the_box->get_half_x () + __skin_skip + delta_thick);
	  }
	else if (r0 < __sum_weight[2]) 
	  {
	    x = (r1 - 0.5) * the_box->get_x ();
	    z = (r2 - 0.5) * the_box->get_z ();
	    y = -(the_box->get_half_y () + __skin_skip + delta_thick);
	  }
	else if (r0 < __sum_weight[3]) 
	  {
	    x = (r1 - 0.5) * the_box->get_x ();
	    z = (r2 - 0.5) * the_box->get_z ();
	    y = +(the_box->get_half_y () + __skin_skip + delta_thick);
	  }	
	else if (r0 < __sum_weight[4]) 
	  {
	    x = (r1 - 0.5) * the_box->get_x ();
	    y = (r2 - 0.5) * the_box->get_y ();
	    z = -(the_box->get_half_z () + __skin_skip + delta_thick);
	  }
	else 
	  {
	    x = (r1 - 0.5) * the_box->get_x ();
	    y = (r2 - 0.5) * the_box->get_y ();
	    z = +(the_box->get_half_z () + __skin_skip + delta_thick);
	  }	
      }
    vertex_.set (x,y, z);
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  box_vg::create (const properties & configuration_, void * user_)
  {
    cerr << "DEVEL: genvtx::box_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "genvtx::box_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the box vertex generator:
    double box_x, box_y, box_z;
    box_x = box_y = box_z = 1.0;
    int mode = box_vg::MODE_DEFAULT;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    int surface_mask = geomtools::box::FACE_NONE;
    double lunit = CLHEP::mm;
    string lunit_str;

    string box_user_ref = vg_tools::SHAPE_REF_NONE;
    string box_user_ref_name;

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
	    message << "genvtx::box_vg::create: Invalid mode '"
		    << mode_str << "' !";
	    throw runtime_error (message.str ());
	  }
      }

    if (configuration_.has_key ("box_ref"))
      {
	box_user_ref = configuration_.fetch_string ("box_ref");;
      }

    if (box_user_ref == vg_tools::SHAPE_REF_NONE)
      {
	if (configuration_.has_key ("box.x"))
	  {
	    box_x = configuration_.fetch_real ("box.x");
	  }
	
	if (configuration_.has_key ("box.y"))
	  {
	    box_y = configuration_.fetch_real ("box.y");
	  }
	
	if (configuration_.has_key ("box.z"))
	  {
	    box_z = configuration_.fetch_real ("box.z");
	  }	
      }

    if (box_user_ref == vg_tools::SHAPE_REF_GETTER)
      {
	if (configuration_.has_key ("box_ref_name"))
	  {
	    box_user_ref_name = configuration_.fetch_string ("box_ref_name");
	  }
      }

    if (configuration_.has_key ("length_unit"))
      {
	lunit_str = configuration_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }

    if (configuration_.has_key ("skin_skip"))
      {
	skin_skip = configuration_.fetch_real ("skin_skip");
      }

    if (configuration_.has_key ("skin_thickness"))
      {
	skin_thickness = configuration_.fetch_real ("skin_thickness");
      }

    vector<string> surfaces;
    if (mode == MODE_SURFACE)
      {
	
	if (configuration_.has_key ("surfaces"))
	  {
	    configuration_.fetch ("surfaces", surfaces);
	  }

	for (int i = 0; i < surfaces.size (); i++)
	  {
	    if (surfaces[i] == "all")
	      {
		surface_mask = geomtools::box::FACE_ALL;
	      }
	    else if (surfaces[i] == "back")
	      {
		surface_mask |= geomtools::box::FACE_BACK;
	      }
	    else if (surfaces[i] == "front")
	      {
		surface_mask |= geomtools::box::FACE_FRONT;
	      }
	    else if (surfaces[i] == "left")
	      {
		surface_mask |= geomtools::box::FACE_LEFT;
	      }
	    else if (surfaces[i] == "right")
	      {
		surface_mask |= geomtools::box::FACE_RIGHT;
	      }
	    else if (surfaces[i] == "bottom")
	      {
		surface_mask |= geomtools::box::FACE_BOTTOM;
	      }
	    else if (surfaces[i] == "top")
	      {
		surface_mask |= geomtools::box::FACE_TOP;
	      }
	  }
	
      }

    // create a new parameterized 'box_vg' instance:
    box_vg * ptr = new box_vg;
    ptr->set_mode (mode);
    ptr->set_skin_skip (skin_skip);
    ptr->set_skin_thickness (skin_thickness);
    ptr->set_surface_mask (surface_mask);

    // use an embedded box:
    if (box_user_ref == vg_tools::SHAPE_REF_NONE)
      {
	box_x *= lunit;
	box_y *= lunit;
	box_z *= lunit;
	geomtools::box box (box_x, box_y, box_z);
	ptr->set_box (box);
      }
    else
      // use an referenced external box extracted from:
      {
	if (user_ == 0)
	  {
	    throw runtime_error ("genvtx::box_vg::create: No user data is provided !");
	  }

	const geomtools::box * ext_box_ref = 0;
	    
	if (box_user_ref == vg_tools::SHAPE_REF_PLAIN)
	  {
	    try
	      {
		ext_box_ref = static_cast<const geomtools::box *> (user_);
	      }
	    catch (...)
	      {
		throw runtime_error ("genvtx::box_vg::create: Cannot reference a 'geomtools::box' from 'user' pointer !");
	      }
	  }
	else if (box_user_ref == vg_tools::SHAPE_REF_GETTER)
	  {
	    geomtools::i_object_3d::i_getter * getter = 0;
	    try
	      {
		getter = static_cast<geomtools::i_object_3d::i_getter *> (user_);
	      }
	    catch (...)
	      {
		throw runtime_error ("genvtx::box_vg::create: Cannot reference a 'i_object_3d::i_getter' from 'user' pointer !");
	      }

	    const geomtools::i_object_3d * o3d = 0;
	    try
	      {
		o3d = getter->get (box_user_ref_name, configuration_);
	      }
	    catch (...)
	      {
		throw runtime_error ("genvtx::box_vg::create: Cannot reference a 'i_object_3d' object from the 'getter' instance !");
	      }

	    if (o3d->get_shape_name () != "box")
	      {
		throw runtime_error ("genvtx::box_vg::create: Reference 3D object is not a 'box' shape !");
	      }

	    try
	      {
		ext_box_ref = dynamic_cast <const geomtools::box *> (o3d);
	      }
	    catch (...)
	      {
		throw runtime_error ("genvtx::box_vg::create: Cannot reference a 'geomtools::box' from the generic 3D shape !");
	      }
	  }
	else 
	  {
	    throw runtime_error ("genvtx::box_vg::create: Unknown mode for external shape referencing !");	    
	  }

	if (ext_box_ref == 0)
	  {
	    throw runtime_error ("genvtx::box_vg::create: Cannot reference a box from 'user' pointer !");
	  }
	ptr->set_box_ref (*ext_box_ref);
      }

    return ptr;	
  }
	

	    /*
	    const geomtools::i_model * model = 0;
	    if (box_user_ref == vg_tools::SHAPE_REF_MODEL)
	      {
		try
		  {
		    model = static_cast<const geomtools::i_model *> (user_);
		  }
		catch (...)
		  {
		    throw runtime_error ("box_vg::create: Cannot reference a 'geomtools::i_model' from 'user' pointer !");
		  }
	      }
	    else
	      {
		const geomtools::models_col_t * models = 0;
		try
		  {
		    models = static_cast<const geomtools::models_col_t *> (user_);
		  }
		catch (...)
		  {
		    throw runtime_error ("box_vg::create: Cannot reference a 'geomtools::models_col_t' dictionary from 'user' pointer !");
		  }
		geomtools::models_col_t::const_iterator found 
		  = models.find (box_user_ref);
		if (found == models.end ())
		  {
		    ostringstream message:
		    message << "box_vg::create: Cannot find a model with name '"
			    << user_ref << "' in the dictionary of models !";
		    throw runtime_error (message.str ());
		  }
		model = (found->second)
		

	      }
	    if (! model->is_constructed ())
	      {
		throw runtime_error ("box_vg::create: Extracted model is not constructed !");
	      }
	    if (! model->get_logical ().has_shape ())
	      {
		throw runtime_error ("box_vg::create: Logical from extracted model has no shape !");
	      } 
	    const geomtools::i_shape3d & shape = model->get_logical ().get_shape ();
	    if (shape.get_shape_name () != "box")
	      {
		throw runtime_error ("box_vg::create: Extracted 3D shape is not a box !");		
	      }
	    ext_box_ref = static_cast<const geomtools::box *> (&shape);
	  }
	else
	  {
	    
	    /*
	      {
	      const geomtools::models_col_t * models = 0;
	      
	      
	      }
	    */

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
