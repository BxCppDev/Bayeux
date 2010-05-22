// -*- mode: c++ ; -*- 
/* test_model_1.cc
 */

#include <geomtools/test/test_model_1.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<test_model_1> test_model_1::__CR;
  
  const geomtools::box & test_model_1::get_solid () const
  {
    return __solid;
  }
  
  string test_model_1::get_model_id () const
  {
    return "geomtools::test::test_model_1";
  }
  
  test_model_1::test_model_1 () : geomtools::i_model ()
  {
  }
  
  test_model_1::~test_model_1 ()
  {
  }
  
  void test_model_1::_at_construct (const string & name_,
				    const datatools::utils::properties & config_,
				    models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: test_model_1::_at_construct: Entering..." << endl;
    set_name (name_);
    double width;
    double height;
    string material;
    
    if (config_.has_key ("width"))
      {
	if (devel) clog << "DEVEL: test_model_1::_at_construct: key= 'width'..." << endl;
	width = config_.fetch_real ("width");
	width *= CLHEP::mm;
      }  
    if (config_.has_key ("height"))
      {
	if (devel) clog << "DEVEL: test_model_1::_at_construct: key= 'height'..." << endl;
	height = config_.fetch_real ("height");
	height *= CLHEP::mm;
      }
    if (config_.has_key ("material.ref"))
      {
	if (devel) clog << "DEVEL: test_model_1::_at_construct: key= 'material'..." << endl;
	material = config_.fetch_string ("material.ref");
      }
    else
      {
	ostringstream message;
	message << "test_model_1::_at_construct: "
		<< "Missing 'material.ref' property !";
	throw runtime_error (message.str ());
      }
    
    __solid.reset ();
    __solid.set_x (width);
    __solid.set_y (width);
    __solid.set_z (height);
    if (! __solid.is_valid ())
      {
	throw runtime_error ("test_model_1::_at_construct: Invalid solid !");
      }
    
    // initialize the 'logical_volume' of this model:
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (material);

    if (devel) clog << "DEVEL: test_model_1::_at_construct: Exiting." << endl;
    return;
  }

  void test_model_1::tree_dump (ostream & out_, 
				const string & title_ , 
				const string & indent_, 
				bool inherit_) const
  {
     namespace du = datatools::utils;
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

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

// end of test_model_1.cc
