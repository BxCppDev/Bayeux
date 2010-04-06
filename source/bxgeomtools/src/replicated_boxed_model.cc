// -*- mode: c++ ; -*- 
/* replicated_boxed_model.cc
 */

#include <geomtools/replicated_boxed_model.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<replicated_boxed_model> replicated_boxed_model::__CR;
  
  const geomtools::box & replicated_boxed_model::get_box () const
  {
    return __solid;
  }
  
  const geomtools::box & replicated_boxed_model::get_solid () const
  {
    return __solid;
  }

  void replicated_boxed_model::set_number_of_items (size_t n_)
  {
    __number_of_items = n_;
  }
  
  size_t replicated_boxed_model::get_number_of_items ()
  {
    return __number_of_items;
  }
  
  double replicated_boxed_model::get_x () const
  {
    return __x;
  }
  
  double replicated_boxed_model::get_y () const
  {
    return __y;
  }
  
  double replicated_boxed_model::get_z () const
  {
    return __z;
  }
 
  void replicated_boxed_model::set_boxed_model (const i_model & model_)
  {
    // check if model has a logical volume with a box shape:
    const i_shape_3d & shape = model_.get_logical ().get_shape ();
    if (shape.get_shape_name () != box::BOX_LABEL)
      {
	ostringstream message;
	message << "replicated_boxed_model::set_boxed_model: "
		<< "Model has no 'box' shape ! "
		<< "Found '" << shape.get_shape_name () << "' !";
	throw runtime_error (message.str ());
      }
    __boxed_model = &model_;
  }

  const i_model & replicated_boxed_model::get_boxed_model () const
  {
    return *__boxed_model;
  }
 
  string replicated_boxed_model::get_model_id () const
  {
    return "geomtools::replicated_boxed_model";
  }
  
  replicated_boxed_model::replicated_boxed_model () : geomtools::i_model ()
  {
    __boxed_model = 0;
    __x = numeric_limits<double>::quiet_NaN ();
    __y = numeric_limits<double>::quiet_NaN ();
    __z = numeric_limits<double>::quiet_NaN ();
    __number_of_items = 0;
  }
  
  replicated_boxed_model::~replicated_boxed_model ()
  {
  }
  
  void replicated_boxed_model::_at_construct (const string & name_,
				    const datatools::utils::properties & config_,
				    models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: replicated_boxed_model::_at_construct: Entering..." << endl;
    set_name (name_);
    double x;
    double y;
    double z;
    string material_name = material::MATERIAL_REF_DEFAULT;
    string boxed_model_name;
    size_t number_of_items = 0;
    string replicant_axis_label = "";

    if (config_.has_key ("replicant_axis"))
      {
	replicant_axis_label = config_.fetch_string ("replicant_axis");
      }  
    else
      {
	ostringstream message;
	message << "replicated_boxed_model::_at_construct: "
		<< "Missing 'replicant_axis' property !"; 
	throw runtime_error (message.str ());		
      }

    if (config_.has_key ("number_of_items"))
      {
	number_of_items = config_.fetch_integer ("number_of_items");
      }  
    else
      {
	ostringstream message;
	message << "replicated_boxed_model::_at_construct: "
		<< "Missing 'number_of_items' property !"; 
	throw runtime_error (message.str ());		
      }
     
    if (config_.has_key ("boxed_model"))
      {
	boxed_model_name = config_.fetch_string ("boxed_model");
      }  
    else
      {
	ostringstream message;
	message << "replicated_boxed_model::_at_construct: "
		<< "Missing 'boxed_model' property !"; 
	throw runtime_error (message.str ());	
      }

    if (number_of_items == 0)
      {
	ostringstream message;
	message << "replicated_boxed_model::_at_construct: "
		<< "Number of items is zero !"; 
	throw runtime_error (message.str ());	
      }
    set_number_of_items (number_of_items);

    bool axis_ok = false;
    if (replicant_axis_label == "x") axis_ok = true;
    else if (replicant_axis_label == "y") axis_ok = true;
    else if (replicant_axis_label == "z") axis_ok = true;
    if (! axis_ok)
      {
	ostringstream message;
	message << "replicated_boxed_model::_at_construct: "
		<< "Invalid replicant axis !"; 
	throw runtime_error (message.str ());	
      }

    if (! models_)
      {
	ostringstream message;
	message << "replicated_boxed_model::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw runtime_error (message.str ());
      }
    
    // Boxed model:
    {
      models_col_t::const_iterator found = 
	models_->find (boxed_model_name);
      if (found != models_->end ())
	{
	  set_boxed_model (dynamic_cast<const i_model &>(*(found->second)));
	}
      else
	{
	  ostringstream message;
	  message << "replicated_boxed_model::_at_construct: "
		  << "Cannot find model with name '" 
		  << boxed_model_name << "' !";
	  throw runtime_error (message.str ());
	}
    }
    
    const box & b = 
      dynamic_cast<const box &>(__boxed_model->get_logical ().get_shape ());
    __x = b.get_x ();
    __y = b.get_y ();
    __z = b.get_z ();
    double x0, y0, z0;
    x0 = y0 = z0 = 0.0;
    if (replicant_axis_label == "x") 
      {
	__boxed_replica_placement.set_replicant_step_x (b.get_x ());
	__x = b.get_x ()* get_number_of_items ();
	x0 = -0.5 * __x + 0.5 * b.get_x ();
      }
    if (replicant_axis_label == "y") 
      {
	__boxed_replica_placement.set_replicant_step_y (b.get_y ());
	__y = b.get_y ()* get_number_of_items ();
	y0 = -0.5 * __y + 0.5 * b.get_y ();
      }
    if (replicant_axis_label == "z") 
      {
	__boxed_replica_placement.set_replicant_step_z (b.get_z ());
	__z = b.get_z () * get_number_of_items ();
	z0 = -0.5 * __z + 0.5 * b.get_z ();
      }
    __boxed_replica_placement.set_number_of_items (get_number_of_items ());

    __solid.reset ();
    __solid.set_x (__x);
    __solid.set_y (__y);
    __solid.set_z (__z);
    if (! __solid.is_valid ())
      {
	throw runtime_error ("replicated_boxed_model::_at_construct: Invalid solid !");
      }
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    if (__boxed_model->get_logical ().has_material_ref ())
      {
	material_name = __boxed_model->get_logical ().get_material_ref ();
      }
    get_logical ().set_material_ref (material_name);
    geomtools::visibility::extract (config_, _logical.parameters ());

    placement basic_p;
    basic_p.set (x0, y0, z0, 0, 0, 0);
    __boxed_replica_placement.set_basic_placement (basic_p);
    __boxed_phys.set_name (i_model::make_physical_volume_name ("replicated_boxed", get_number_of_items ()));
    __boxed_phys.set_placement (__boxed_replica_placement);
    __boxed_phys.set_logical (__boxed_model->get_logical ());
    __boxed_phys.set_mother (_logical);

    if (devel) clog << "DEVEL: replicated_boxed_model::_at_construct: Exiting." << endl;
    return;
  }

  void replicated_boxed_model::tree_dump (ostream & out_, 
					  const string & title_ , 
					  const string & indent_, 
					  bool inherit_) const
  {
     namespace du = datatools::utils;
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

     out_ << indent << i_tree_dumpable::tag 
	  << "X : " << get_x () / CLHEP::mm << " mm" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Y : " << get_y () / CLHEP::mm << " mm" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Z : " << get_z () / CLHEP::mm << " mm" << endl;

     out_ << indent << i_tree_dumpable::tag 
	  << "Number of replicated items : " << __number_of_items << endl;

     {
       out_ << indent << i_tree_dumpable::tag 
	    << "Replicated placement (box) : " << endl;
       {
	 ostringstream indent_oss;
	 indent_oss << indent;
	 indent_oss << du::i_tree_dumpable::skip_tag (inherit_);
	 __boxed_replica_placement.tree_dump (out_, "", indent_oss.str ());
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

// end of replicated_boxed_model.cc
