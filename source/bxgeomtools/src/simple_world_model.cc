// -*- mode: c++ ; -*- 
/* simple_world_model.cc
 */

#include <geomtools/simple_world_model.h>

namespace geomtools {

  using namespace std;

  const string simple_world_model::SETUP_LABEL = "setup";
  
  // register this creator:   
  geomtools::i_model::creator_registration<simple_world_model> simple_world_model::__CR;
  
  const geomtools::box & simple_world_model::get_solid () const
  {
    return __solid;
  }
  
  string simple_world_model::get_model_id () const
  {
    return "geomtools::simple_world_model";
  }
  
    // ctor:
  simple_world_model::simple_world_model () : geomtools::i_model ()
  {
    __setup_model = 0;
  }
  
  simple_world_model::~simple_world_model ()
  {
  }
  
  void simple_world_model::_at_construct (const string & name_,
					const datatools::utils::properties & config_,
					models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: simple_world_model::_at_construct: Entering..." << endl;
    set_name (name_);
    string material = "vacuum";
    string setup_model_name;
    double setup_phi   = 0.;
    double setup_theta = 0.;
    double setup_x     = 0.;
    double setup_y     = 0.;
    double setup_z     = 0.;
    double world_x, world_y, world_z;
    datatools::utils::invalidate (world_x);
    datatools::utils::invalidate (world_y);
    datatools::utils::invalidate (world_z);
    double length_unit = CLHEP::mm;
    double angle_unit = CLHEP::degree;

    if (config_.has_key ("material.ref"))
      {
	material = config_.fetch_string ("material.ref");
      }

    if (config_.has_key ("length_unit"))
      {
	string length_unit_str = config_.fetch_string ("length_unit");
	length_unit = datatools::utils::units::get_length_unit_from (length_unit_str);
      }

    if (config_.has_key ("angle_unit"))
      {
	string angle_unit_str = config_.fetch_string ("angle_unit");
	angle_unit = datatools::utils::units::get_angle_unit_from (angle_unit_str);
      }

    if (config_.has_key ("world.x"))
      {
	world_x = config_.fetch_real ("world.x");
	world_x *= length_unit;
      }

    if (config_.has_key ("world.y"))
      {
	world_y = config_.fetch_real ("world.y");
	world_y *= length_unit;
      }

    if (config_.has_key ("world.z"))
      {
	world_z = config_.fetch_real ("world.z");
	world_z *= length_unit;
      }

    if (config_.has_key ("setup.x"))
      {
	setup_x = config_.fetch_real ("setup.x");
	setup_x *= length_unit;
      }

    if (config_.has_key ("setup.y"))
      {
	setup_y = config_.fetch_real ("setup.y");
	setup_y *= length_unit;
      }

    if (config_.has_key ("setup.z"))
      {
	setup_z = config_.fetch_real ("setup.y");
	setup_z *= length_unit;
      }
 
    if (config_.has_key ("setup.theta"))
      {
	setup_theta = config_.fetch_real ("setup.theta");
	setup_theta *= angle_unit;
      }

    if (config_.has_key ("setup.phi"))
      {
	setup_phi = config_.fetch_real ("setup.phi");
	setup_phi *= angle_unit;
      }

    if (material::has_key (config_, material::make_key (material::MATERIAL_REF_PROPERTY)))
      {
	if (devel) clog << "DEVEL: simple_world_model::_at_construct: key= 'material'..." << endl;
	material = config_.fetch_string (material::make_key (material::MATERIAL_REF_PROPERTY));
      }

    if (config_.has_key ("setup.model"))
      {
	setup_model_name = config_.fetch_string ("setup.model");
      }
    else
      {
	ostringstream message;
	message << "simple_world_model::_at_construct: "
		<< "Missing 'setup_model' property !"; 
	throw runtime_error (message.str ());	
      }
     
    if (! models_)
      {
	ostringstream message;
	message << "simple_world_model::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw runtime_error (message.str ());
      }

    // Setup model:
    {
      models_col_t::const_iterator found = models_->find (setup_model_name);
      if (found != models_->end ())
	{
	  //__setup_model = (dynamic_cast<const test_model_2 *> (found->second));
	  __setup_model = found->second;
	}
      else
	{
	  ostringstream message;
	  message << "simple_world_model::_at_construct: "
		  << "Cannot find model with name '" 
		  << setup_model_name << "' !";
	  throw runtime_error (message.str ());
	}
    }

    vector_3d setup_pos (setup_x, setup_y, setup_z);
    //create_xyz (setup_pos, 0.0, 0.0, 0.0);
    __setup_placement.set_translation (setup_pos);
    __setup_placement.set_orientation (setup_phi, setup_theta, 0.0);

    __solid.reset ();

    double setup_sx = 0.;
    double setup_sy = 0.;
    double setup_sz = 0.;
    if (__setup_model->get_logical ().get_shape ().get_shape_name () == "box")
      {
	const geomtools::box * b = 0;
	b = dynamic_cast<const geomtools::box *> (&(__setup_model->get_logical ()).get_shape ());
	setup_sx = b->get_x ();
	setup_sy = b->get_y ();
	setup_sz = b->get_z ();
      }
    
    double size = setup_sx;
    if (setup_sy > size) size = setup_sy;
    if (setup_sz > size) size = setup_sz;
    size *= 2;
    __world_x = size;
    __world_y = size;
    __world_z = size;
 
    if (datatools::utils::is_valid (world_x))
      {
	__world_x = world_x;
      }
    if (datatools::utils::is_valid (world_y))
      {
	__world_y = world_y;
      }
    if (datatools::utils::is_valid (world_z))
      {
	__world_z = world_z;
      }

    __solid.set_x (__world_x);
    __solid.set_y (__world_y);
    __solid.set_z (__world_z);
    if (!__solid.is_valid ())
      {
	throw runtime_error ("simple_world_model::_at_construct: Invalid solid !");
      }
    __material = material;
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (__material);

    if (devel) clog << "DEVEL: simple_world_model::_at_construct: Install physicals..." << endl;
    __setup_phys.set_name (i_model::make_physical_volume_name (SETUP_LABEL));
    __setup_phys.set_placement (__setup_placement);
    __setup_phys.set_logical (__setup_model->get_logical ());
    __setup_phys.set_mother (_logical);
    if (devel) clog << "DEVEL: simple_world_model::_at_construct: Physicals are installed. " << endl;
    
    if (devel) clog << "DEVEL: simple_world_model::_at_construct: Exiting." << endl;
    return;
  }

  void simple_world_model::tree_dump (ostream & out_, 
				const string & title_ , 
				const string & indent_, 
				bool inherit_) const
  {
     namespace du = datatools::utils;
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

     
     {
       // Setup model:
        if (__setup_model)
          {
            out_ << indent << i_tree_dumpable::tag 
                 << "Setup model : " << endl;
            {
              ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << du::i_tree_dumpable::skip_tag;
              __setup_model->tree_dump (out_, "", indent_oss.str ());
            }   
          }
        else
          {
            out_ << indent << i_tree_dumpable::tag 
                 << "Setup model : " << "<missing>" << endl;
          }
      }

      {
	out_ << indent << i_tree_dumpable::tag 
	     << "Setup placement: " << endl;
	{
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::skip_tag;
	  __setup_placement.tree_dump (out_, "", indent_oss.str ());
	}   
      }

      {
	out_ << indent << i_tree_dumpable::tag 
	     << "Setup physical : " << endl;
	{
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::skip_tag;
	  __setup_phys.tree_dump (out_, "", indent_oss.str ());
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
       
} // end of namespace geomtools

// end of simple_world_model.cc
