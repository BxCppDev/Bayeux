// -*- mode: c++ ; -*- 
/* test_world_model.cc
 */

#include <geomtools/test/test_world_model.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<test_world_model> test_world_model::__CR;
  
  const geomtools::box & test_world_model::get_solid () const
  {
    return __solid;
  }
  
  string test_world_model::get_model_id () const
  {
    return "geomtools::test::test_world_model";
  }
  
    // ctor:
  test_world_model::test_world_model () : geomtools::i_model ()
  {
    __setup_model = 0;
  }
  
  test_world_model::~test_world_model ()
  {
  }
  
  void test_world_model::_at_construct (const string & name_,
					const datatools::utils::properties & config_,
					models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: test_world_model::_at_construct: Entering..." << endl;
    set_name (name_);
    string material = "vacuum";
    string visibility = "invisible";
    string setup_model_name;
    double phi = 0. * CLHEP::degree;
    double theta = 0. * CLHEP::degree;

    if (config_.has_key ("theta"))
      {
	 if (devel) clog << "DEVEL: test_world_model::_at_construct: key= 'theta'..." << endl;
	theta = config_.fetch_real ("theta");
	theta *= CLHEP::degree;
      }

    if (config_.has_key ("phi"))
      {
	 if (devel) clog << "DEVEL: test_world_model::_at_construct: key= 'phi'..." << endl;
	phi = config_.fetch_real ("phi");
	phi *= CLHEP::degree;
      }

    if (config_.has_key ("material"))
      {
	if (devel) clog << "DEVEL: test_world_model::_at_construct: key= 'material'..." << endl;
	material = config_.fetch_string ("material");
      }

    if (config_.has_key ("visibility"))
      {
	if (devel) clog << "DEVEL: test_world_model::_at_construct: key= 'visibility'..." << endl;
	visibility = config_.fetch_string ("visibility");
      }

    if (config_.has_key ("setup_model"))
      {
	setup_model_name = config_.fetch_string ("setup_model");
      }
    else
      {
	ostringstream message;
	message << "test_world_model::_at_construct: "
		<< "Missing 'setup_model' property !"; 
	throw runtime_error (message.str ());	
      }
     
    if (! models_)
      {
	ostringstream message;
	message << "test_world_model::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw runtime_error (message.str ());
      }

    // Setup model:
    {
      models_col_t::const_iterator found = models_->find (setup_model_name);
      if (found != models_->end ())
	{
	  __setup_model = (dynamic_cast<const test_model_2 *> (found->second));
	}
      else
	{
	  ostringstream message;
	  message << "test_world_model::_at_construct: "
		  << "Cannot find model with name '" 
		  << setup_model_name << "' !";
	  throw runtime_error (message.str ());
	}
    }

    vector_3d setup_pos;
    create_xyz (setup_pos, 0, 0, 0);
    __setup_placement.set_translation (setup_pos);
    __setup_placement.set_orientation (0.0, 0.0, 0.0);

    __solid.reset ();
    double setup_x = __setup_model->get_solid ().get_x ();
    double setup_y = __setup_model->get_solid ().get_y ();
    double setup_z = __setup_model->get_solid ().get_z ();
    double size = setup_x;
    if (setup_y > size) size = setup_y;
    if (setup_z > size) size = setup_z;
    size *= 2;
    __solid.set_x (size);
    __solid.set_y (size);
    __solid.set_z (size);
    if (!__solid.is_valid ())
      {
	throw runtime_error ("test_world_model::_at_construct: Invalid solid !");
      }
    
    // initialize the 'logical_volume' of this model:
    ostringstream name_log_oss;
    name_log_oss << name_ << LOGICAL_SUFFIX;
    get_logical ().set_name (name_log_oss.str ());
    get_logical ().set_shape (__solid);
    get_logical ().parameters ().store ("material", material);
    get_logical ().parameters ().store ("visibility", visibility);

    if (devel) clog << "DEVEL: test_world_model::_at_construct: Install physicals..." << endl;
    __setup_phys.set_name ("setup_phys");
    __setup_phys.set_placement (__setup_placement);
    __setup_phys.set_logical (__setup_model->get_logical ());
    __setup_phys.set_mother (_logical);
    if (devel) clog << "DEVEL: test_world_model::_at_construct: Physicals are installed. " << endl;
    
    if (devel) clog << "DEVEL: test_world_model::_at_construct: Exiting." << endl;
    return;
  }

  void test_world_model::tree_dump (ostream & out_, 
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

// end of test_world_model.cc
