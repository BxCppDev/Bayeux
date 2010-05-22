// -*- mode: c++ ; -*- 
/* test_model_2.cc
 */

#include <geomtools/test/test_model_2.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<test_model_2> test_model_2::__CR;
  
  const geomtools::box & test_model_2::get_solid () const
  {
    return __solid;
  }
  
  string test_model_2::get_model_id () const
  {
    return "geomtools::test::test_model_2";
  }
  
    // ctor:
  test_model_2::test_model_2 () : geomtools::i_model ()
  {
    __sub1_model = 0;
    __sub2_model = 0;
  }
  
  test_model_2::~test_model_2 ()
  {
  }
  
  void test_model_2::_at_construct (const string & name_,
				    const datatools::utils::properties & config_,
				    models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: test_model_2::_at_construct: Entering..." << endl;
    set_name (name_);
    double gas_pressure = 1. * CLHEP::atmosphere;
    double gas_temperature = 300. * CLHEP::kelvin;
    double distance = 50. * CLHEP::mm;
    double phi = 0. * CLHEP::degree;
    double theta = 0. * CLHEP::degree;
    string material = "air";
    string sub1_model_name;
    string sub2_model_name;

    if (config_.has_key ("gas_pressure"))
      {
	if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'gas_pressure'..." << endl;
	gas_pressure = config_.fetch_real ("gas_pressure");
	gas_pressure *= CLHEP::atmosphere;
      }  

    if (config_.has_key ("gas_temperature"))
      {
	if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'gas_temperature'..." << endl;
	gas_temperature = config_.fetch_real ("gas_temperature");
	gas_temperature *= CLHEP::kelvin;
      }  

    if (config_.has_key ("distance"))
      {
	if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'distance'..." << endl;
	distance = config_.fetch_real ("distance");
	distance *= CLHEP::mm;
      }  

    if (config_.has_key ("theta"))
      {
	if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'theta'..." << endl;
	theta = config_.fetch_real ("theta");
	theta *= CLHEP::degree;
      }

    if (config_.has_key ("phi"))
      {
	if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'phi'..." << endl;
	phi = config_.fetch_real ("phi");
	phi *= CLHEP::degree;
      }

    if (config_.has_key ("material.ref"))
      {
	if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'material'..." << endl;
	material = config_.fetch_string ("material.ref");
      }

    if (config_.has_key ("sub1_model"))
      {
	sub1_model_name = config_.fetch_string ("sub1_model");
      }
    else
      {
	ostringstream message;
	message << "test_model_2::_at_construct: "
		<< "Missing 'sub1_model' property !"; 
	throw runtime_error (message.str ());	
      }

    if (config_.has_key ("sub2_model"))
      {
	sub2_model_name = config_.fetch_string ("sub2_model");
      }
    else
      {
	ostringstream message;
	message << "test_model_2::_at_construct: "
		<< "Missing 'sub2_model' property !"; 
	throw runtime_error (message.str ());	
      }
      
    if (! models_)
      {
	ostringstream message;
	message << "test_model_2::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw runtime_error (message.str ());
      }

    // Sub1:
    {
      models_col_t::const_iterator found = models_->find (sub1_model_name);
      if (found != models_->end ())
	{
	  __sub1_model = (dynamic_cast<const test_model_1 *> (found->second));
	}
      else
	{
	  ostringstream message;
	  message << "test_model_2::_at_construct: "
		  << "Cannot find model with name '" 
		  << sub1_model_name << "' !";
	  throw runtime_error (message.str ());
	}
    }

    // Sub2:
    {
      models_col_t::const_iterator found = models_->find (sub2_model_name);
      if (found != models_->end ())
	{
	  __sub2_model = (dynamic_cast<const test_model_1 *> (found->second));
	}
      else
	{
	  ostringstream message;
	  message << "test_model_2::_at_construct: "
		  << "Cannot find model with name '" 
		  << sub2_model_name << "' !";
	  throw runtime_error (message.str ());
	}
    }

    __distance = distance;
    __phi = phi;
    __theta = theta;

    vector_3d sub1_pos;
    double r1 = 0.5 * distance + __sub1_model->get_solid ().get_half_z ();
    create_spherical (sub1_pos, r1, phi, theta);
    __sub1_placement.set_translation (sub1_pos);
    __sub1_placement.set_orientation (phi, theta + M_PI, 0.0);

    vector_3d sub2_pos;
    double r2 = 0.5 * distance + __sub2_model->get_solid ().get_half_z ();
    create_spherical (sub2_pos, r2, phi, theta + M_PI);
    __sub2_placement.set_translation (sub2_pos);
    __sub2_placement.set_orientation (phi, theta, 0.0);

    __solid.reset ();
    double size = 
      distance 
      + 2 * __sub1_model->get_solid ().get_x ()
      + 2 * __sub2_model->get_solid ().get_x ();
    __solid.set_x (size);
    __solid.set_y (size);
    __solid.set_z (size);
    if (!__solid.is_valid ())
      {
	throw runtime_error ("test_model_2::_at_construct: Invalid solid !");
      }
    
    // initialize the 'logical_volume' of this model:
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (material);

    if (devel) clog << "DEVEL: test_model_2::_at_construct: Install physicals..." << endl;
    __sub1_phys.set_name (i_model::make_physical_volume_name ("sub1"));
    __sub1_phys.set_placement (__sub1_placement);
    __sub1_phys.set_logical (__sub1_model->get_logical ());
    __sub1_phys.set_mother (_logical);

    __sub2_phys.set_name (i_model::make_physical_volume_name ("sub2"));
    __sub2_phys.set_placement (__sub2_placement);
    __sub2_phys.set_logical (__sub2_model->get_logical ());
    __sub2_phys.set_mother (_logical);
    if (devel) clog << "DEVEL: test_model_2::_at_construct: Physicals are installed. " << endl;
    
    if (devel) clog << "DEVEL: test_model_2::_at_construct: Exiting." << endl;
    return;
  }

  void test_model_2::tree_dump (ostream & out_, 
				const string & title_ , 
				const string & indent_, 
				bool inherit_) const
  {
     namespace du = datatools::utils;
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

     
      {
        // Sub-model #1:
        if (__sub1_model)
          {
            out_ << indent << i_tree_dumpable::tag 
                 << "Sub model 1 : " << endl;
            {
              ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << du::i_tree_dumpable::skip_tag;
              __sub1_model->tree_dump (out_, "", indent_oss.str ());
            }   
          }
        else
          {
            out_ << indent << i_tree_dumpable::tag 
                 << "Sub model 1 : " << "<missing>" << endl;
          }
      }

      {
        // Sub-model #2:
        if (__sub2_model)
          {
            out_ << indent << i_tree_dumpable::tag 
                 << "Sub model 2 : " << endl;
            {
              ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << du::i_tree_dumpable::skip_tag;
              __sub2_model->tree_dump (out_, "", indent_oss.str ());
            }   
          }
        else
          {
            out_ << indent << i_tree_dumpable::tag 
                 << "Sub model 2 : " << "<missing>" << endl;
          }
      }

      {
	out_ << indent << i_tree_dumpable::tag 
	     << "Placement 1 : " << endl;
	{
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::skip_tag;
	  __sub1_placement.tree_dump (out_, "", indent_oss.str ());
	}   
      }

      {
	out_ << indent << i_tree_dumpable::tag 
	     << "Placement 2 : " << endl;
	{
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::skip_tag;
	  __sub2_placement.tree_dump (out_, "", indent_oss.str ());
	}   
      }

      {
	out_ << indent << i_tree_dumpable::tag 
	     << "Sub physical 1 : " << endl;
	{
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::skip_tag;
	  __sub1_phys.tree_dump (out_, "", indent_oss.str ());
	}   
      }

      {
	out_ << indent << i_tree_dumpable::tag 
	     << "Sub physical 2 : " << endl;
	{
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::skip_tag;
	  __sub2_phys.tree_dump (out_, "", indent_oss.str ());
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

// end of test_model_2.cc
