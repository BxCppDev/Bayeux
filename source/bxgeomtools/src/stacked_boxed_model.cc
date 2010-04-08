// -*- mode: c++ ; -*- 
/* stacked_boxed_model.cc
 */

#include <geomtools/stacked_boxed_model.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<stacked_boxed_model> stacked_boxed_model::__CR;

  const string stacked_boxed_model::STACKED_LABEL = "stacked";
  const string stacked_boxed_model::LABEL_PROPERTY_PREFIX = "label";

  const string & stacked_boxed_model::get_material_name () const
  {
    assert_constructed ("stacked_boxed_model::get_material_name");
    return __get_material_name ();
  }

  const string & stacked_boxed_model::__get_material_name () const
  {
    return __material_name;
  }

  void stacked_boxed_model::set_material_name (const string & mn_)
  {
    assert_unconstructed("stacked_boxed_model::set_material_name");

    __material_name = mn_;
  }

  void stacked_boxed_model::set_stacking_axis (int a_)
  {
    assert_unconstructed("stacked_boxed_model::set_stacking_axis");

    if ((a_ < STACKING_ALONG_X) || (a_ > STACKING_ALONG_Z))
      {
	throw runtime_error ("stacked_boxed_model::set_staking_axis: Invalid stacking axis !");
      }
    __stacking_axis = a_;
  }

  int stacked_boxed_model::get_stacking_axis () const
  {
    assert_constructed ("stacked_boxed_model::get_stacking_axis");
    return __get_stacking_axis ();
  }

  int stacked_boxed_model::__get_stacking_axis () const
  {
    return __stacking_axis;
  }

  bool stacked_boxed_model::is_stacking_along_x () const
  {
    return __get_stacking_axis () == STACKING_ALONG_X;
  }
  
  bool stacked_boxed_model::is_stacking_along_y () const
  {
    return __get_stacking_axis () == STACKING_ALONG_Y;
  }
  
  bool stacked_boxed_model::is_stacking_along_z () const
  {
    return __get_stacking_axis () == STACKING_ALONG_Z;
  }
  
  const geomtools::box & stacked_boxed_model::get_box () const
  {
    assert_constructed ("stacked_boxed_model::get_box");
    return __solid;
  }
  
  const geomtools::box & stacked_boxed_model::get_solid () const
  {
    assert_constructed ("stacked_boxed_model::get_solid");
    return __solid;
  }

  void stacked_boxed_model::add_boxed_model (int i_, const i_model & model_, const string & label_)
  {
    assert_unconstructed("stacked_boxed_model::add_boxed_model");

    boxed_dict_t::const_iterator found = __boxed_models.find (i_);
    if (found != __boxed_models.end ())
      {
	ostringstream message;
	message << "stacked_boxed_model::add_boxed_model: "
		<< "Dictionary already has a model with "
		<< "index '" << i_ << "' !";
	throw runtime_error (message.str ());
      }
    boxed_item bi;
    __boxed_models[i_] = bi;
    __boxed_models[i_].model = &model_;
    __boxed_models[i_].placmt.invalidate ();
    string label = label_;
    if (label.empty ())
      {
	ostringstream label_oss;
	label_oss << STACKED_LABEL << "[" << i_ << ']';
	label = label_oss.str ();
      }
    else
      {
	if (__labels.find (label) != __labels.end ())
	  {
	    ostringstream message;
	    message << "stacked_boxed_model::add_boxed_model: "
		    << "Label '" << label << "' is already used !";
	    throw runtime_error (message.str ());
	  }
      }
    __boxed_models[i_].label = label;
    __labels[label] = i_;
    return;
  }

  const stacked_boxed_model::boxed_dict_t & stacked_boxed_model::get_models () const
  {
    return __boxed_models;
  }

  const stacked_boxed_model::labels_dict_t & stacked_boxed_model::get_labels () const
  {
    return __labels;
  }

  size_t stacked_boxed_model::get_number_of_boxed_models () const
  {
    return __boxed_models.size ();
  }

  bool stacked_boxed_model::has_boxed_model (const string & label_) const
  {
    labels_dict_t::const_iterator found = __labels.find (label_);
    return found != __labels.end ();
  }

  const stacked_boxed_model::boxed_item & 
  stacked_boxed_model::get_boxed_item (const string & label_) const
  {
    labels_dict_t::const_iterator found = __labels.find (label_);
    if (found == __labels.end ())
      {
	ostringstream message;
	message << "stacked_boxed_model::get_boxed_model: "
		<< "Dictionary has no model with "
		<< "label '" << label_ << "' !";
	throw runtime_error (message.str ());
      }
    return (this->get_boxed_item (found->second));
  }

  const stacked_boxed_model::boxed_item & 
  stacked_boxed_model::get_boxed_item (int i_) const
  {
    boxed_dict_t::const_iterator found = __boxed_models.find (i_);
    if (found == __boxed_models.end ())
      {
	ostringstream message;
	message << "stacked_boxed_model::get_boxed_item: "
		<< "Dictionary has no model item with "
		<< "index '" << i_ << "' !";
	throw runtime_error (message.str ());
      }
    return found->second;
  }

  const i_model & stacked_boxed_model::get_boxed_model (const string & label_) const
  {
    labels_dict_t::const_iterator found = __labels.find (label_);
    if (found == __labels.end ())
      {
	ostringstream message;
	message << "stacked_boxed_model::get_boxed_model: "
		<< "Dictionary has no model with "
		<< "label '" << label_ << "' !";
	throw runtime_error (message.str ());
      }
    return this->get_boxed_model (found->second);
  }

  const i_model & stacked_boxed_model::get_boxed_model (int i_) const
  {
    boxed_dict_t::const_iterator found = __boxed_models.find (i_);
    if (found == __boxed_models.end ())
      {
	ostringstream message;
	message << "stacked_boxed_model::get_boxed_model: "
		<< "Dictionary has no model with "
		<< "index '" << i_ << "' !";
	throw runtime_error (message.str ());
      }
    return *(found->second.model);
  }

  string stacked_boxed_model::get_model_id () const
  {
    return "geomtools::stacked_boxed_model";
  }
  
  // ctor:
  stacked_boxed_model::stacked_boxed_model () : geomtools::i_model ()
  {
    __material_name = "";
    __stacking_axis = STACKING_ALONG_INVALID;
  }
  
  // dtor:
  stacked_boxed_model::~stacked_boxed_model ()
  {
  }
  
  void stacked_boxed_model::_at_construct (const string & name_,
					   const datatools::utils::properties & config_,
					   models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel)
      {
	clog << "DEVEL: stacked_boxed_model::_at_construct: Entering..." << endl;
      }
    set_name (name_);
    size_t number_of_items;
    string boxed_model_name;
    string label_name;
    string stacking_axis_label = "";
    int    stacking_axis;
    string material_name;
    double x, y, z;

    /*** material ***/
    if (config_.has_key ("material.ref"))
      {
	material_name = config_.fetch_string ("material.ref");
      }  
    else
      {
	ostringstream message;
	message << "stacked_boxed_model::_at_construct: "
		<< "Missing 'material.ref' property !"; 
	throw runtime_error (message.str ());		
      }
    set_material_name (material_name);

    /*** stacking axis ***/
    if (config_.has_key ("stacking_axis"))
      {
	stacking_axis_label = config_.fetch_string ("stacking_axis");
      }  
    else
      {
	ostringstream message;
	message << "stacked_boxed_model::_at_construct: "
		<< "Missing 'stacking_axis' property !"; 
	throw runtime_error (message.str ());		
      }
    if (stacking_axis_label == "x")
      {
	stacking_axis = STACKING_ALONG_X;
      }
    else if (stacking_axis_label == "y")
      {
	stacking_axis = STACKING_ALONG_Y;
      }
    else if (stacking_axis_label == "z")
      {
	stacking_axis = STACKING_ALONG_Z;
      }
    set_stacking_axis (stacking_axis);

    /*** number of stacked boxed models ***/
    if (config_.has_key ("number_of_items"))
      {
	number_of_items = config_.fetch_integer ("number_of_items");
      }  
    else
      {
	ostringstream message;
	message << "stacked_boxed_model::_at_construct: "
		<< "Missing 'number_of_items' property !"; 
	throw runtime_error (message.str ());		
      }

    /*** check models ***/
    if (! models_)
      {
	ostringstream message;
	message << "stacked_boxed_model::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw runtime_error (message.str ());
      }

    /*** loop over models to be stacked ***/
    for (int i = 0; i < number_of_items; i++)
      {
	string boxed_model_name;
	string label_name;
	ostringstream boxed_item_prop;
	boxed_item_prop << "boxed_model_" << i;
	if (config_.has_key (boxed_item_prop.str ()))
	  {
	    boxed_model_name = config_.fetch_string (boxed_item_prop.str ());
	  }  
	else
	  {
	    ostringstream message;
	    message << "stacked_boxed_model::_at_construct: "
		    << "Missing '" << boxed_item_prop.str () << "' property !"; 
	    throw runtime_error (message.str ());	
	  }
	// attempt to extract a user defined label:
	ostringstream label_item_prop;
	label_item_prop << LABEL_PROPERTY_PREFIX << "_" << i;
	if (config_.has_key (label_item_prop.str ()))
	  {
	    label_name = config_.fetch_string (label_item_prop.str ());
	  }  

	models_col_t::const_iterator found = 
	  models_->find (boxed_model_name);
	if (found != models_->end ())
	  {
	    // check if the model is box-shaped:
	    if (found->second->get_logical ().get_shape ().get_shape_name () !=
		"box")
	      {
		ostringstream message;
		message << "stacked_boxed_model::_at_construct: "
			<< "The embedded model '" << found->second->get_name () 
			<< "' is not a 'boxed' one !"; 
		throw runtime_error (message.str ());
	      }
	    add_boxed_model (i, *(found->second), label_name);
	  }
	else
	  {
	    ostringstream message;
	    message << "stacked_boxed_model::_at_construct: "
		    << "Cannot find model with name '" 
		    << boxed_model_name << "' !";
	    throw runtime_error (message.str ());
	  }
      }

    /*** compute main box dimensions ***/
    mygsl::min_max mmx;
    mygsl::min_max mmy;
    mygsl::min_max mmz;
    double stacked_x = 0.0;
    double stacked_y = 0.0;
    double stacked_z = 0.0;
    for (boxed_dict_t::const_iterator i = __boxed_models.begin ();
	 i != __boxed_models.end ();
	 i++)
      {
	const boxed_item & bi = i->second;
	const i_model * model = bi.model;
	const box & b = 
	  dynamic_cast<const box &>(model->get_logical ().get_shape ());
	mmx.add (b.get_x ());
	mmy.add (b.get_y ());
	mmz.add (b.get_z ());
	if (is_stacking_along_x ())
	  {
	    stacked_x += b.get_x ();
	  }
	else if (is_stacking_along_y ())
	  {
	    stacked_y += b.get_y ();
	  }
	else if (is_stacking_along_z ())
	  {
	    stacked_z += b.get_z ();
	  }
      }

    double max = -1.0;
    if (is_stacking_along_x ())
      {
	stacked_y = mmy.get_max ();
	stacked_z = mmz.get_max ();
      }
    else if (is_stacking_along_y ())
      {
	stacked_x = mmx.get_max ();
	stacked_z = mmz.get_max ();
      }
    else if (is_stacking_along_z ())
      {
	stacked_x = mmx.get_max ();
	stacked_y = mmy.get_max ();
      }

    double dim_x = stacked_x;
    double dim_y = stacked_y;
    double dim_z = stacked_z;
    double lunit = CLHEP::mm;

    if (config_.has_key ("x"))
      {
	x = config_.fetch_real ("x");
	x *= lunit;
	if (x < stacked_x)
	  {
	    ostringstream message;
	    message << "stacked_boxed_model::_at_construct: "
		    << "Enforced X dimension '" << x / CLHEP::mm << "' mm (<" << stacked_x / CLHEP::mm << ") is too small for stacked components to fit !"; 
	    throw runtime_error (message.str ());    
	  }
        dim_x = x;
      }  

    if (config_.has_key ("y"))
      {
	y = config_.fetch_real ("y");
	y *= lunit;
	if (y < stacked_y)
	  {
	    ostringstream message;
	    message << "stacked_boxed_model::_at_construct: "
		    << "Enforced Y dimension '" << y / CLHEP::mm << "' mm (<" << stacked_y / CLHEP::mm << ") is too small for stacked components to fit !"; 
	    throw runtime_error (message.str ());    
	  }
        dim_y = y;
      }  

    if (config_.has_key ("z"))
      {
	z = config_.fetch_real ("z");
	z *= lunit;
	if (z < stacked_z)
	  {
	    ostringstream message;
	    message << "stacked_boxed_model::_at_construct: "
		    << "Enforced Z dimension '" << z / CLHEP::mm << "' mm (<" << stacked_z / CLHEP::mm << ") is too small for stacked components to fit !"; 
	    throw runtime_error (message.str ());    
	  }
        dim_z = z;
      }  

    __solid.reset ();
    __solid.set_x (dim_x);
    __solid.set_y (dim_y);
    __solid.set_z (dim_z);
    if (! __solid.is_valid ())
      {
	throw runtime_error ("stacked_boxed_model::_at_construct: Invalid solid !");
      }

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (__get_material_name ());
    geomtools::visibility::extract (config_, _logical.parameters ());

    double pos;
    if (is_stacking_along_x ())
      {
	pos = -0.5 * stacked_x;
      }
    else if (is_stacking_along_y ())
      {
	pos = -0.5 * stacked_y;
      }
    else if (is_stacking_along_z ())
      {
	pos = -0.5 * stacked_z;
      }
    int j = 0;
    for (boxed_dict_t::iterator i = __boxed_models.begin ();
	 i != __boxed_models.end ();
	 i++)
      {
	boxed_item & bi = i->second;
	const i_model * model = bi.model;
	double xi, yi, zi;
	xi = yi = zi = 0.0;
	const box & b = 
	  dynamic_cast<const box &>(model->get_logical ().get_shape ());
	if (is_stacking_along_x ())
	  {
	    xi = pos + 0.5 * b.get_x ();
	    pos += b.get_x ();
	  }
	else if (is_stacking_along_y ())
	  {
	    yi = pos + 0.5 * b.get_y ();
	    pos += b.get_y ();
	  }
	else if (is_stacking_along_z ())
	  {
	    zi = pos + 0.5 * b.get_z ();
	    pos += b.get_z ();
	  }
	bi.placmt.set (xi, yi, zi, 0.0, 0.0);
	bi.phys.set_name (i_model::make_physical_volume_name (bi.label));
	bi.phys.set_placement (bi.placmt);
	bi.phys.set_logical (bi.model->get_logical ());
	bi.phys.set_mother (_logical);
	j++;
      }

    if (devel) clog << "DEVEL: stacked_boxed_model::_at_construct: Exiting." << endl;
    return;
  }

  void stacked_boxed_model::tree_dump (ostream & out_, 
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
	    << "Material : " << get_material_name () << endl;
     }

     {
       out_ << indent << i_tree_dumpable::tag 
	    << "Stacking axis : " << get_stacking_axis () << endl;
     }
     
     {
       for (labels_dict_t::const_iterator i = __labels.begin ();
	    i != __labels.end ();
	    i++)
	 {
	   out_ << indent << i_tree_dumpable::tag 
		<< "Stacked model : " << "'" << i->first << "'" << " [rank==" << i->second << "]" << endl;
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

// end of stacked_boxed_model.cc
