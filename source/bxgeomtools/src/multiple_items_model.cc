// -*- mode: c++ ; -*- 
/* multiple_items_model.cc
 */

#include <geomtools/multiple_items_model.h>

namespace geomtools {

  using namespace std;  

  /*
  multiple_items_model::item::item ()
  {
    __model = 0;
  }

  void multiple_items_model::item::set_label (const string & label_)
  {
    __label = label_;
  }

  const string & multiple_items_model::item::get_label () const
  {
    return __label;
  }

  void multiple_items_model::item::set_model (const i_model & model_)
  {
    __model = &model_;
  }

  const i_model & multiple_items_model::item::get_model () const
  {
    if (__model == 0)
      {
	ostringstream message;
	message << "multiple_items_model::item::get_model: "
		<< "Missing model !";
	throw runtime_error (message.str ());
      }
    return *__model;
  }

  void multiple_items_model::item::set_placement (const placement & pl_)
  {
    __placement = pl_;
  }

  const placement & 
  multiple_items_model::item::get_placement () const
  {
    return __placement;
  }

  const physical_volume & 
  multiple_items_model::item::get_physical_volume () const
  {
    return __phys;
  }

  physical_volume & 
  multiple_items_model::item::get_physical_volume ()
  {
    return __phys;
  }
  */

  /***************************************/

  MWIM & multiple_items_model::get_internals ()
  {
    return __internals;
  }

  const MWIM & multiple_items_model::get_internals () const
  {
    return __internals;
  }
    
  string multiple_items_model::get_model_id () const
  {
    return "geomtools::multiple_items_model";
  }
   
  const box & multiple_items_model::get_box () const
  {
    return __solid;
  }
 
  const box & multiple_items_model::get_solid () const
  {
    return __solid;
  }

  void multiple_items_model::set_material_name (const string & m_)
  {
    assert_unconstructed ("multiple_items_model::set_material_name");
    __material_name = m_;
    return;
  }

  const string & multiple_items_model::get_material_name () const
  {
    assert_constructed ("multiple_items_model::get_material_name");
    return __get_material_name ();
  }
    
  const string & multiple_items_model::__get_material_name () const
  {
    return __material_name;
  }
    
  /*
  bool multiple_items_model::has_item (const string & label_) const
  {
    return __items.find (label_) != __items.end ();      
  }
    
  const multiple_items_model::item & 
  multiple_items_model::get_item (const string & label_) const
  {
    item_dict_t::const_iterator found = __items.find (label_);
    if (found == __items.end ())
      {
	ostringstream message;
	message << "multiple_items_model::get_item: "
		<< "No item with label '" << label_ << "'"
		<< endl;
	throw runtime_error (message.str ());
      }
    return found->second;
  }

  multiple_items_model::item & 
  multiple_items_model::get_item (const string & label_)
  {
    item_dict_t::iterator found = __items.find (label_);
    if (found == __items.end ())
      {
	ostringstream message;
	message << "multiple_items_model::get_item: "
		<< "No item with label '" << label_ << "' !"
		<< endl;
	throw runtime_error (message.str ());
      }
    return found->second;
  }

  void multiple_items_model::add_item (const string & label_, 
				       const i_model & model_,
				       const placement & placement_)
  {
    assert_unconstructed ("multiple_items_model::add_item");
    if (has_item (label_))
      {
	ostringstream message;
	message << "multiple_items_model::add_item: "
		<< "There is already an item with label '" << label_ << "' !"
		<< endl;
	throw runtime_error (message.str ());
      }
      
    __items[label_] = item ();
    item & the_item = __items[label_];
    the_item.set_label (label_);
    the_item.set_model (model_);
    the_item.set_placement (placement_);
  }

  size_t multiple_items_model::get_number_of_items () const
  {
    return __items.size ();
  }

  const multiple_items_model::item_dict_t & 
  multiple_items_model::get_items () const
  {
    return __items;
  }

  multiple_items_model::item_dict_t & multiple_items_model::get_items ()
  {
    return __items;
  }

  */

  
  // ctor:
  multiple_items_model::multiple_items_model ()
    : i_boxed_model ()
  {
    return;
  }
  
  // dtor:
  multiple_items_model::~multiple_items_model ()
  {
    //__items.clear ();
    return;
  }

  void multiple_items_model::_at_construct (const string & name_,
					    const datatools::utils::properties & config_,
					    models_col_t * models_)
  {
    if (g_devel) clog << "DEVEL: multiple_items_model::_at_construct: Entering..." << endl;

    set_name (name_);

    /*** box dimensions ***/
    double x, y, z;
    string length_unit = "mm";
    double lunit = CLHEP::mm;
 
    if (config_.has_key ("length_unit"))
      {
	string length_unit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (length_unit_str);
      }
    
    if (config_.has_key ("x"))
      {
	x = config_.fetch_real ("x");
	x *= lunit;
      }

    if (config_.has_key ("y"))
      {
	y = config_.fetch_real ("y");
	y *= lunit;
      }

    if (config_.has_key ("z"))
      {
	z = config_.fetch_real ("z");
	z *= lunit;
      }

    /*** material ***/
    {
      string material;
      if (config_.has_key ("material.ref"))
	{
	  material = config_.fetch_string ("material.ref");
	}
      else
	{
	  ostringstream message;
	  message << "multiple_items_model::_at_construct: "
		  << "Missing 'material.ref' property !"; 
	  throw runtime_error (message.str ());
	}
      set_material_name (material);
    }

    /*
    vector<string> labels;
    if (config_.has_key ("items.labels"))
      {
	config_.fetch ("items.labels", labels);
      }

    if (! models_)
      {
	ostringstream message;
	message << "multiple_items_model::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw runtime_error (message.str ());
      }
    */

    __solid.reset ();
    __solid.set_x (x);
    __solid.set_y (y);
    __solid.set_z (z);
    if (!__solid.is_valid ())
      {
	throw runtime_error ("multiple_items_model::_at_init: Invalid solid !");
      }

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (__get_material_name ());

    __internals.plug_internal_models (config_, get_logical (), models_);

    /*
    for (vector<string>::const_iterator i = labels.begin ();
	 i != labels.end ();
	 i++)
      {
	string item_label = *i;
	string item_model_name;
	const i_model * item_model = 0;
	placement item_placement;
	// extract placement:
	{
	  string item_placement_str;
	  ostringstream item_placement_oss;
	  item_placement_oss << "placement." << item_label;
	  if (config_.has_key (item_placement_oss.str ()))
	    {
	      item_placement_str = config_.fetch_string (item_placement_oss.str ());
	    }
	  else
	    {
	      ostringstream message;
	      message << "multiple_items_model::_at_construct: "
		      << "Missing '" << item_placement_oss.str () << "' property !"; 
	      throw runtime_error (message.str ());
	    }
	  // parse placement:
	  bool res_parsing = true;
	  try
	    {
	      res_parsing = placement::from_string (item_placement_str, item_placement);
	    }
	  catch (...)
	    {
	      res_parsing = false;
	    }
	  if (! res_parsing)
	    {
	      ostringstream message;
	      message << "multiple_items_model::_at_construct: "
		      << "Item's placement parsing failed !"; 
	      throw runtime_error (message.str ());
	    }
	}

	// extract model:
	{
	  ostringstream item_model_key_oss;
	  item_model_key_oss << "model." << item_label;
	  if (config_.has_key (item_model_key_oss.str ()))
	    {
	      item_model_name = config_.fetch_string (item_model_key_oss.str ());
	    }
	  else
	    {
	      ostringstream message;
	      message << "multiple_items_model::_at_construct: "
		      << "Missing 'item_model_key_oss.str ()' property !"; 
	      throw runtime_error (message.str ());
	    }
	  {
	    models_col_t::const_iterator found = 
	      models_->find (item_model_name);
	    if (found != models_->end ())
	      {
		item_model = found->second;
	      }
	    else
	      {
		ostringstream message;
		message << "multiple_items_model::_at_construct: "
			<< "Cannot find model with name '" 
			<< item_model_name << "' !";
		throw runtime_error (message.str ());
	      }
	  }
	}

	
 	add_item (item_label, *item_model, item_placement);
	physical_volume & item_phys = get_item (item_label).get_physical_volume ();
	const placement & item_plcmt = get_item (item_label).get_placement ();
	item_phys.set_name (i_model::make_physical_volume_name (item_label));
	item_phys.set_placement (item_plcmt);
	item_phys.set_logical (item_model->get_logical ());
	item_phys.set_mother (get_logical ());
	//item_phys.tree_dump (clog, "Item phys: ", "DEVEL: ");

	//clog << "******* DEVEL: Item '" << item_label << "' is done..." << endl;
      }
    */

    if (g_devel) clog << "DEVEL: multiple_items_model::_at_construct: Exiting." << endl;
    return;
  }
      

  void multiple_items_model::tree_dump (ostream & out_, 
					const string & title_, 
					const string & indent_, 
					bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty()) indent = indent_;      
    i_model::tree_dump (out_, title_, indent, true);
      
    {
      // Material:
      out_ << indent << i_tree_dumpable::tag 
	   << "Material : " << __get_material_name () << std::endl;
    }
     
    {
      // Items:
      out_ << indent << i_tree_dumpable::tag 
	   << "Internal items : " << get_internals ().get_items ().size () << std::endl;
    }

    {
      // Solid:
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	   << "Solid : " << std::endl;
      {
	std::ostringstream indent_oss;
	indent_oss << indent;
	indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	__solid.tree_dump (out_,"",indent_oss.str ());
      }   
    }   

    return;
  }

  i_model::creator_registration<multiple_items_model> multiple_items_model::__CR;

} // end of namespace geomtools

// end of multiple_items_model.cc
