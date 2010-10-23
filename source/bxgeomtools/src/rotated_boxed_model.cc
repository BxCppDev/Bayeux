// -*- mode: c++ ; -*- 
/* rotated_boxed_model.cc
 */

#include <geomtools/rotated_boxed_model.h>

namespace geomtools {

  using namespace std;
  
  const geomtools::box & rotated_boxed_model::get_box () const
  {
    return __solid;
  }
  
  const geomtools::box & rotated_boxed_model::get_solid () const
  {
    return __solid;
  }

  void rotated_boxed_model::set_boxed_model (const i_model & model_)
  {
    assert_unconstructed("rotated_boxed_model::set_boxed_model");

    // check if model has a logical volume with a box shape:
    const i_shape_3d & shape = model_.get_logical ().get_shape ();
    /*
    if (shape.get_shape_name () != box::BOX_LABEL)
      {
	ostringstream message;
	message << "rotated_boxed_model::set_boxed_model: "
		<< "Model has no 'box' shape ! "
		<< "Found '" << shape.get_shape_name () << "' !";
	throw runtime_error (message.str ());
      }
    */
    __boxed_model = &model_;
  }

  const i_model & rotated_boxed_model::get_boxed_model () const
  {
    return *__boxed_model;
  }
 
  string rotated_boxed_model::get_model_id () const
  {
    return "geomtools::rotated_boxed_model";
  }
  
  rotated_boxed_model::rotated_boxed_model () : i_boxed_model ()
  {
    __boxed_model = 0;
  }
  
  rotated_boxed_model::~rotated_boxed_model ()
  {
  }
  
  void rotated_boxed_model::_at_construct (const string & name_,
					   const datatools::utils::properties & config_,
					   models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel)
      {
	clog << "DEVEL: rotated_boxed_model::_at_construct: Entering..." << endl;
      }
    set_name (name_);
    string boxed_model_name;
    string rotation_axis_label = "";
    string special_rotation_angle_label = "";
    string rotated_label = "rotated";

    if (config_.has_key ("rotated_label"))
      {
	rotated_label = config_.fetch_string ("rotated_label");
      }  

    if (config_.has_key ("rotation_axis"))
      {
	rotation_axis_label = config_.fetch_string ("rotation_axis");
      }  
    else
      {
	ostringstream message;
	message << "rotated_boxed_model::_at_construct: "
		<< "Missing 'rotation_axis' property !"; 
	throw runtime_error (message.str ());		
      }

    if (config_.has_key ("rotation_angle"))
      {
	special_rotation_angle_label = config_.fetch_string ("rotation_angle");
      }  
    else
      {
	ostringstream message;
	message << "rotated_boxed_model::_at_construct: "
		<< "Missing 'rotation_angle' property !"; 
	throw runtime_error (message.str ());		
      }
     
    if (config_.has_key ("boxed_model"))
      {
	boxed_model_name = config_.fetch_string ("boxed_model");
      }  
    else
      {
	ostringstream message;
	message << "rotated_boxed_model::_at_construct: "
		<< "Missing 'boxed_model' property !"; 
	throw runtime_error (message.str ());	
      }

    int rotation_axis = 
      get_rotation_axis_from_label (rotation_axis_label);
    if (! check_rotation_axis (rotation_axis))
      {
	ostringstream message;
	message << "rotated_boxed_model::_at_construct: "
		<< "Invalid rotation axis !"; 
	throw runtime_error (message.str ());	
      }

    int special_rotation_angle = 
      get_special_rotation_angle_from_label (special_rotation_angle_label);
    if (! check_special_rotation_angle (special_rotation_angle))
      {
	ostringstream message;
	message << "rotated_boxed_model::_at_construct: "
		<< "Invalid rotation angle !"; 
	throw runtime_error (message.str ());	
      }

    if (! models_)
      {
	ostringstream message;
	message << "rotated_boxed_model::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw runtime_error (message.str ());
      }
    
    // Boxed model:
    {
      models_col_t::const_iterator found = 
	models_->find (boxed_model_name);
      if (found != models_->end ())
	{
	  // check if the model is stackable:
	  if (! i_shape_3d::is_stackable (found->second->get_logical ().get_shape ()))
	    {
	      ostringstream message;
	      message << "stacked_boxed_model::_at_construct: "
		      << "The rotating model '" 
		      << found->second->get_name () 
		      << "' is not stackable !"; 
	      throw runtime_error (message.str ());
	    }
	  set_boxed_model (*(found->second));
	}
      else
	{
	  ostringstream message;
	  message << "rotated_boxed_model::_at_construct: "
		  << "Cannot find model with name '" 
		  << boxed_model_name << "' !";
	  throw runtime_error (message.str ());
	}
    }
 
    const i_shape_3d & the_shape = __boxed_model->get_logical ().get_shape ();

    // try to get a stackable data from the shape:
    stackable_data the_SD;
    if (! i_shape_3d::pickup_stackable (the_shape, the_SD))
      {
	    ostringstream message;
	    message << "rotated_boxed_model::_at_construct: "
		    << "Cannot stack '" 
		    << the_shape.get_shape_name () << "' shape !";
	    throw runtime_error (message.str ());
      }
    double gxmin = the_SD.get_xmin ();
    double gxmax = the_SD.get_xmax ();
    double gymin = the_SD.get_ymin ();
    double gymax = the_SD.get_ymax ();
    double gzmin = the_SD.get_zmin ();
    double gzmax = the_SD.get_zmax ();

    /*   
    if (__boxed_model->get_logical ().get_shape ().get_shape_name () !=
	"box")
      {
	  ostringstream message;
	  message << "rotated_boxed_model::_at_construct: "
		  << "The embedded model is not a 'boxed' one !"; 
	  throw runtime_error (message.str ());
      }
    const box & b = 
      dynamic_cast<const box &>(__boxed_model->get_logical ().get_shape ());
    */
    double x0, y0, z0;
    x0 = y0 = z0 = 0.0;

    __boxed_placement.set_translation (x0, y0, z0);
    __boxed_placement.set_orientation (rotation_axis, 
				       get_special_rotation_angle (special_rotation_angle));

    stackable_data the_new_SD;    
 
    double xmin, xmax, ymin, ymax, zmin, zmax;
    the_new_SD.xmin = gxmin;
    the_new_SD.xmax = gxmax;
    the_new_SD.ymin = gymin;
    the_new_SD.ymax = gymax;
    the_new_SD.zmin = gzmin;
    the_new_SD.zmax = gzmax;
    the_new_SD.tree_dump (cerr, "rotated_boxed_model::_at_construct: ", "DEVEL: ");
    if (rotation_axis == ROTATION_AXIS_X) 
      {
	if (special_rotation_angle ==  ROTATION_ANGLE_90)
	  {
	    the_new_SD.ymax = -gzmin;
	    the_new_SD.zmax =  gymax;
	    the_new_SD.ymin = -gzmax;
	    the_new_SD.zmin =  gymin;
	  } 
	if (special_rotation_angle ==  ROTATION_ANGLE_180)
	  {
	    the_new_SD.ymax = -gymin;
	    the_new_SD.zmax = -gzmin;
	    the_new_SD.ymin = -gymax;
	    the_new_SD.zmin = -gzmax;
	  } 
	if (special_rotation_angle ==  ROTATION_ANGLE_270)
	  {
	    the_new_SD.ymax =  gzmax;
	    the_new_SD.zmax = -gymin;
	    the_new_SD.ymin =  gzmin;
	    the_new_SD.zmin = -gymax;
	  } 
       }

    if (rotation_axis == ROTATION_AXIS_Y) 
      {
	if (special_rotation_angle ==  ROTATION_ANGLE_90)
	  {
	    the_new_SD.zmax = -gxmin;
	    the_new_SD.xmax =  gzmax;
	    the_new_SD.zmin = -gxmax;
	    the_new_SD.xmin =  gzmin;
	  } 
	if (special_rotation_angle ==  ROTATION_ANGLE_180)
	  {
	    the_new_SD.zmax = -gzmin;
	    the_new_SD.xmax = -gxmin;
	    the_new_SD.zmin = -gzmax;
	    the_new_SD.xmin = -gxmax;
	  } 
	if (special_rotation_angle ==  ROTATION_ANGLE_270)
	  {
	    the_new_SD.zmax =  gxmax;
	    the_new_SD.xmax = -gzmin;
	    the_new_SD.zmin =  gxmin;
	    the_new_SD.xmin = -gzmax;
	  } 
        }

    if (rotation_axis == ROTATION_AXIS_Z) 
      {
	if (special_rotation_angle ==  ROTATION_ANGLE_90)
	  {
	    the_new_SD.xmax = -gymin;
	    the_new_SD.ymax =  gxmax;
	    the_new_SD.xmin = -gymax;
	    the_new_SD.ymin =  gxmin;
	  } 
	if (special_rotation_angle ==  ROTATION_ANGLE_180)
	  {
	    the_new_SD.xmax = -gxmin;
	    the_new_SD.ymax = -gymin;
	    the_new_SD.xmin = -gxmax;
	    the_new_SD.ymin = -gymax;
	  } 
	if (special_rotation_angle ==  ROTATION_ANGLE_270)
	  {
	    the_new_SD.xmax =  gymax;
	    the_new_SD.ymax = -gxmin;
	    the_new_SD.xmin =  gymin;
	    the_new_SD.ymin = -gxmax;
	  } 
        }

    __solid.reset ();
    __solid.set_x (the_new_SD.xmax - the_new_SD.xmin);
    __solid.set_y (the_new_SD.ymax - the_new_SD.ymin);
    __solid.set_z (the_new_SD.zmax - the_new_SD.zmin);
    if (! __solid.is_valid ())
      {
	throw runtime_error ("rotated_boxed_model::_at_construct: Invalid solid !");
      }

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    string material_name = material::MATERIAL_REF_DEFAULT;
    if (__boxed_model->get_logical ().has_material_ref ())
      {
	material_name = __boxed_model->get_logical ().get_material_ref ();
      }
    get_logical ().set_material_ref (material_name);
    __boxed_phys.set_name (i_model::make_physical_volume_name (rotated_label));
    __boxed_phys.set_placement (__boxed_placement);
    __boxed_phys.set_logical (__boxed_model->get_logical ());
    __boxed_phys.set_mother (_logical);

    if (devel) clog << "DEVEL: rotated_boxed_model::_at_construct: Exiting." << endl;
    return;
  }

  void rotated_boxed_model::tree_dump (ostream & out_, 
					  const string & title_ , 
					  const string & indent_, 
					  bool inherit_) const
  {
     namespace du = datatools::utils;
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

     {
       out_ << indent << i_tree_dumpable::tag 
	    << "Rotated placement (box) : " << endl;
       {
	 ostringstream indent_oss;
	 indent_oss << indent;
	 indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	 __boxed_placement.tree_dump (out_, "", indent_oss.str ());
       }   
     }
     
     {
       out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	    << "Solid : " << endl;
       {
	 ostringstream indent_oss;
	 indent_oss << indent;
	 indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	 __solid.tree_dump (out_, "", indent_oss.str ());
       }   
     }

     return;
  }

  
  // register this creator:   
  geomtools::i_model::creator_registration<rotated_boxed_model> rotated_boxed_model::__CR;
       
} // end of namespace geomtools

// end of rotated_boxed_model.cc
