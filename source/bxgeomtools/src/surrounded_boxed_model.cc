// -*- mode: c++ ; -*- 
/* surrounded_boxed_model.cc
 */

#include <geomtools/surrounded_boxed_model.h>

namespace geomtools {

  using namespace std;
  
  // register this creator:   
  geomtools::i_model::creator_registration<surrounded_boxed_model> surrounded_boxed_model::__CR;

  const string surrounded_boxed_model::SURROUNDED_LABEL      = "surrounded";
  const string surrounded_boxed_model::SURROUNDING_LABEL     = "surrounding";
  const string surrounded_boxed_model::LABEL_PROPERTY_PREFIX = "label";

  //list<string> surrounded_boxed_model::__g_position_labels;

  const string & surrounded_boxed_model::get_material_name () const
  {
    assert_constructed ("surrounded_boxed_model::get_material_name");
    return __get_material_name ();
  }

  const string & surrounded_boxed_model::__get_material_name () const
  {
    return __material_name;
  }

  void surrounded_boxed_model::set_material_name (const string & mn_)
  {
    assert_unconstructed("surrounded_boxed_model::set_material_name");

    __material_name = mn_;
  }

  const geomtools::box & surrounded_boxed_model::get_box () const
  {
    assert_constructed ("surrounded_boxed_model::get_box");
    return __solid;
  }
  
  const geomtools::box & surrounded_boxed_model::get_solid () const
  {
    assert_constructed ("surrounded_boxed_model::get_solid");
    return __solid;
  }

  /*** surrounded model ***/
  void surrounded_boxed_model::set_surrounded_label (const string & label_)
  {
    __surrounded_label = label_;
  }

  const string & surrounded_boxed_model::get_surrounded_label () const
  {
    assert_constructed ("surrounded_boxed_model::get_surrounded_label");
    return __surrounded_label;
  }

  const i_model & surrounded_boxed_model::get_surrounded_model () const
  {
    assert_constructed ("surrounded_boxed_model::get_surrounded_model");
    return *__surrounded_model;
  }

  void surrounded_boxed_model::set_surrounded_model (const i_model & model_)
  {
    __surrounded_model = &model_;
  }

  /*** surrounding models ***/
  size_t surrounded_boxed_model::get_number_of_surrounding_items () const
  {
    return __surrounding_items.size ();
  }

  bool surrounded_boxed_model::has_surrounding_model (const string & label_) const
  {
    labels_dict_t::const_iterator found = __surrounding_labels.find (label_);
    return found != __surrounding_labels.end ();
  }

  void surrounded_boxed_model::add_surrounding_model (int i_, 
						      const i_model & model_, 
						      const string & label_)
  {
    assert_unconstructed("surrounded_boxed_model::add_surrounding_model");

    surrounding_dict_t::const_iterator found = __surrounding_items.find (i_);
    if (found != __surrounding_items.end ())
      {
	ostringstream message;
	message << "surrounded_boxed_model::add_surrounding_model: "
		<< "Dictionary already has a model at position "
		<< " '" << i_ << "' !";
	throw runtime_error (message.str ());
      }
    surrounding_item si;
    __surrounding_items[i_] = si;
    __surrounding_items[i_].model = &model_;
    __surrounding_items[i_].placmt.invalidate ();
    string label = label_;
    if (label.empty ())
      {
	ostringstream label_oss;
	label_oss << SURROUNDING_LABEL << "__" << i_ << "__";
	label = label_oss.str ();
      }
    else
      {
	if (__surrounding_labels.find (label) != __surrounding_labels.end ())
	  {
	    ostringstream message;
	    message << "surrounded_boxed_model::add_surrounding_model: "
		    << "Label '" << label << "' is already used !";
	    throw runtime_error (message.str ());
	  }
      }
    __surrounding_items[i_].label = label;
    __surrounding_labels[label] = i_;
    return;
  }

  const surrounded_boxed_model::surrounding_dict_t & surrounded_boxed_model::get_surrounding_items () const
  {
    return __surrounding_items;
  }

  const surrounded_boxed_model::labels_dict_t & surrounded_boxed_model::get_surrounding_labels () const
  {
    return __surrounding_labels;
  }

  const surrounded_boxed_model::surrounding_item & 
  surrounded_boxed_model::get_surrounding_item (const string & label_) const
  {
    labels_dict_t::const_iterator found = __surrounding_labels.find (label_);
    if (found == __surrounding_labels.end ())
      {
	ostringstream message;
	message << "surrounded_boxed_model::get_surrounding_item: "
		<< "Dictionary has no surrounding model with "
		<< "label '" << label_ << "' !";
	throw runtime_error (message.str ());
      }
    return (this->get_surrounding_item (found->second));
  }

  const surrounded_boxed_model::surrounding_item & 
  surrounded_boxed_model::get_surrounding_item (int i_) const
  {
    surrounding_dict_t::const_iterator found = __surrounding_items.find (i_);
    if (found == __surrounding_items.end ())
      {
	ostringstream message;
	message << "surrounded_boxed_model::get_surrounding_item: "
		<< "Dictionary has no surrounding model item with "
		<< "index '" << i_ << "' !";
	throw runtime_error (message.str ());
      }
    return found->second;
  }

  const i_model & surrounded_boxed_model::get_surrounding_model (const string & label_) const
  {
    labels_dict_t::const_iterator found = __surrounding_labels.find (label_);
    if (found == __surrounding_labels.end ())
      {
	ostringstream message;
	message << "surrounded_boxed_model::get_boxed_model: "
		<< "Dictionary has no surrounding model with "
		<< "label '" << label_ << "' !";
	throw runtime_error (message.str ());
      }
    return this->get_surrounding_model (found->second);
  }

  const i_model & surrounded_boxed_model::get_surrounding_model (int i_) const
  {
    surrounding_dict_t::const_iterator found = __surrounding_items.find (i_);
    if (found == __surrounding_items.end ())
      {
	ostringstream message;
	message << "surrounded_boxed_model::get_boxed_model: "
		<< "Dictionary has no surrounding model with "
		<< "index '" << i_ << "' !";
	throw runtime_error (message.str ());
      }
    return *(found->second.model);
  }

  /*** model ID ***/

  string surrounded_boxed_model::get_model_id () const
  {
    return "geomtools::surrounded_boxed_model";
  }
  
  // ctor:
  surrounded_boxed_model::surrounded_boxed_model () : i_boxed_model ()
  {
    if (__position_labels.size () == 0)
      {
	__position_labels.push_back ("back");
	__position_labels.push_back ("front");
	__position_labels.push_back ("left");
	__position_labels.push_back ("right");
	__position_labels.push_back ("bottom");
	__position_labels.push_back ("top");
      }
    __material_name = "";
    __surrounded_model = 0;
    __centered_x = false;
    __centered_y = false;
    __centered_z = false;
  }
  
  // dtor:
  surrounded_boxed_model::~surrounded_boxed_model ()
  {
  }
  
  void surrounded_boxed_model::_at_construct (const string & name_,
					   const datatools::utils::properties & config_,
					   models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (devel)
      {
	clog << datatools::utils::io::devel 
	     << "surrounded_boxed_model::_at_construct: Entering..." << endl;
      }
    set_name (name_);
    string surrounded_model_name;
    string label_name;
    string material_name;
    double x, y, z;
    map<string,string> surrounding_model_names;
    double lunit = CLHEP::mm;

    /*** material ***/
    if (config_.has_key ("material.ref"))
      {
	material_name = config_.fetch_string ("material.ref");
      }  
    else
      {
	ostringstream message;
	message << "surrounded_boxed_model::_at_construct: "
		<< "Missing 'material.ref' property !"; 
	throw runtime_error (message.str ());		
      }
    set_material_name (material_name);

    /*** length unit ***/
    if (config_.has_key ("length_unit"))
      {
	string length_unit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (length_unit_str);
      }  

    /*** Surrounded model ***/
    if (config_.has_key ("surrounded_model"))
      {
	surrounded_model_name = config_.fetch_string ("surrounded_model");
      }  
    else
      {
	ostringstream message;
	message << "surrounded_boxed_model::_at_construct: "
		<< "Missing 'surrounded_model' property !"; 
	throw runtime_error (message.str ());		
      }

    /*** Surrounded label ***/
    __surrounded_label = SURROUNDED_LABEL;
    if (config_.has_key ("surrounded_label"))
      {
	set_surrounded_label (config_.fetch_string ("surrounded_label"));
      }  
    

    /*** Centering of the surrounded item ***/
    if (config_.has_flag ("centered.x"))
      {
	if (devel) cerr << "DEVEL: X-centered" << endl; 
	__centered_x = true;
      }
    if (config_.has_flag ("centered.y"))
      {
	if (devel) cerr << "DEVEL: Y-centered" << endl; 
	__centered_y = true;
      }
    if (config_.has_flag ("centered.z"))
      {
	if (devel) cerr << "DEVEL: Z-centered" << endl; 
	__centered_z = true;
      }

    /*** check models ***/
    if (! models_)
      {
	ostringstream message;
	message << "surrounded_boxed_model::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw runtime_error (message.str ());
      }

    /*** check if surrounded model exists ***/
    {
      models_col_t::const_iterator found = 
	models_->find (surrounded_model_name);
      if (found != models_->end ())
	{
	  // check if the model is box-shaped:
	  /*
	  if (found->second->get_logical ().get_shape ().get_shape_name () !=
	      "box")
	    {
	      ostringstream message;
	      message << "surrounded_boxed_model::_at_construct: "
		      << "The surrounded model '" << found->second->get_name () 
		      << "' is not a 'boxed' one !"; 
	      throw runtime_error (message.str ());
	    }
	  */
	  set_surrounded_model (*(found->second));
	}
      else
	{
	  ostringstream message;
	  message << "surrounded_boxed_model::_at_construct: "
		  << "Cannot find surrounded model with name '" 
		  << surrounded_model_name << "' !";
	  throw runtime_error (message.str ());
	}
    }

    /*** loop over surrounding models ***/
    {
      int ipos = BACK;
      for (list<string>::const_iterator ilabel = __position_labels.begin ();
	   ilabel != __position_labels.end ();
	   ilabel++, ipos++)
	{
	  string surrounding_model_name;
	  string label_name;
	  ostringstream surrounding_item_prop;
	  surrounding_item_prop << "surrounding_model." << *ilabel;
	  if (config_.has_key (surrounding_item_prop.str ()))
	    {
	      surrounding_model_name = config_.fetch_string (surrounding_item_prop.str ());
	    }  
	  else
	    {
	      if (devel)
		{
		  ostringstream message;
		  message << "surrounded_boxed_model::_at_construct: "
			  << "No '" << surrounding_item_prop.str () << "' property !"; 
		  clog << datatools::utils::io::warning << message.str () << endl;	
		}
	      continue;
	    }
	  // attempt to extract a user defined label:
	  ostringstream label_item_prop;
	  label_item_prop << "surrounding_" << LABEL_PROPERTY_PREFIX << "." << *ilabel;
	  if (config_.has_key (label_item_prop.str ()))
	    {
	      label_name = config_.fetch_string (label_item_prop.str ());
	    }  
	  
	  models_col_t::const_iterator found = 
	    models_->find (surrounding_model_name);
	  if (found != models_->end ())
	    {
	      // check if the model is box-shaped:
	      /*
	      if (found->second->get_logical ().get_shape ().get_shape_name () !=
		  "box")
		{
		  ostringstream message;
		  message << "surrounded_boxed_model::_at_construct: "
			  << "The surrounding model '" << found->second->get_name () 
			  << "' is not a 'boxed' one !"; 
		  throw runtime_error (message.str ());
		}
	      */
	      add_surrounding_model (ipos, *(found->second), label_name);
	    }
	  else
	    {
	      ostringstream message;
	      message << "surrounded_boxed_model::_at_construct: "
		      << "Cannot find surrounding model with name '" 
		      << surrounding_model_name << "' !";
	      throw runtime_error (message.str ());
	    }
	}
    }
    
    /*** compute main box dimensions ***/
    mygsl::min_max mmx;
    mygsl::min_max mmy;
    mygsl::min_max mmz;
    const i_shape_3d & the_shape = __surrounded_model->get_logical ().get_shape ();
    const i_stackable * the_stackable = 0;
    if (the_shape.has_stackable_data ())
      {
	the_stackable = &(the_shape.get_stackable_data ());
      }
    else
      {
	the_stackable = dynamic_cast<const i_stackable *> (&the_shape);
      }
    const i_stackable & surrounded_box = *the_stackable;
    /*
    const box & surrounded_box = 
	  dynamic_cast<const box &>(__surrounded_model->get_logical ().get_shape ());
    */
    double dx = surrounded_box.get_x ();
    double dy = surrounded_box.get_y ();
    double dz = surrounded_box.get_z ();
    double x0 = -0.5 * dx;
    double x1 = +0.5 * dx;
    double y0 = -0.5 * dy;
    double y1 = +0.5 * dy;
    double z0 = -0.5 * dz;
    double z1 = +0.5 * dz;
    double dx0 = 0.0;
    double dx1 = 0.0;
    double dy0 = 0.0;
    double dy1 = 0.0;
    double dz0 = 0.0;
    double dz1 = 0.0;
    for (surrounding_dict_t::const_iterator i = __surrounding_items.begin ();
	 i != __surrounding_items.end ();
	 i++)
      {
	int position = i->first;
	const surrounding_item & si = i->second;
	const i_model * model = si.model;
	const i_shape_3d & the_shape = model->get_logical ().get_shape ();
	const i_stackable * the_stackable = 0;
	if (the_shape.has_stackable_data ())
	  {
	    the_stackable = &(the_shape.get_stackable_data ());
	  }
	else
	  {
	    the_stackable = dynamic_cast<const i_stackable *> (&the_shape);
	  }
	const i_stackable & surrounding_box = *the_stackable;
	/*
	  const box & surrounding_box = 
	  dynamic_cast<const box &>(model->get_logical ().get_shape ());
	*/
	if (position == BACK)
	  {
	    dx0 = surrounding_box.get_x ();
	    x0 -= dx0;
	    mmy.add (surrounding_box.get_y ());
	    mmz.add (surrounding_box.get_z ());
	  }
	if (position == FRONT)
	  {
	    dx1 = surrounding_box.get_x ();
	    x1 += dx1;
	    mmy.add (surrounding_box.get_y ());
	    mmz.add (surrounding_box.get_z ());
	  }
	if (position == LEFT)
	  {
	    dy0 = surrounding_box.get_y ();
	    y0 -= dy0;
	    mmx.add (surrounding_box.get_x ());
	    mmz.add (surrounding_box.get_z ());
	  }
	if (position == RIGHT)
	  {
	    dy1 = surrounding_box.get_y ();
	    y1 += dy1;
	    mmx.add (surrounding_box.get_x ());
	    mmz.add (surrounding_box.get_z ());
	  }
	if (position == BOTTOM)
	  {
	    dz0 = surrounding_box.get_z ();
	    z0 -= dz0;
	    mmx.add (surrounding_box.get_x ());
	    mmy.add (surrounding_box.get_y ());
	  }
	if (position == TOP)
	  {
	    dz1 = surrounding_box.get_z ();
	    z1 += dz1;
	    mmx.add (surrounding_box.get_x ());
	    mmy.add (surrounding_box.get_y ());
	  }
      }
    
    if (0.5 * mmx.get_max () > abs (x0)) x0 = -0.5 * mmx.get_max ();
    if (0.5 * mmx.get_max () > x1) x1 = +0.5 * mmx.get_max ();
    if (0.5 * mmy.get_max () > abs (y0)) y0 = -0.5 * mmy.get_max ();
    if (0.5 * mmy.get_max () > y1) y1 = +0.5 * mmy.get_max ();
    if (0.5 * mmz.get_max () > abs (z0)) z0 = -0.5 * mmz.get_max ();
    if (0.5 * mmz.get_max () > z1) z1 = +0.5 * mmz.get_max ();
    double surrounded_x = 0;
    double surrounded_y = 0;
    double surrounded_z = 0;
    double dim_x = 2 * max (x1, abs (x0));
    double dim_y = 2 * max (y1, abs (y0));
    double dim_z = 2 * max (z1, abs (z0));
    if (! __centered_x)
      {
	//cerr << "DEVEL: ! X-centered" << endl; 
	dim_x = x1 - x0;
	surrounded_x = -0.5 * dim_x + dx0 + 0.5 * dx;
      }
    else
      {
	//cerr << "DEVEL: X-centered" << endl; 
      }
    if (! __centered_y)
      {
	//cerr << "DEVEL: ! Y-centered" << endl; 
	dim_y = y1 - y0;
	surrounded_y = -0.5 * dim_y + dy0 + 0.5 * dy;
      }
    else
      {
	//cerr << "DEVEL: Y-centered" << endl; 
      }
    if (! __centered_z)
      {
	//cerr << "DEVEL: ! Z-centered" << endl; 
	dim_z = z1 - z0;
	surrounded_z = -0.5 * dim_z + dz0 + 0.5 * dz;
      }
    else
      {
	//cerr << "DEVEL: Z-centered" << endl; 
      }

    if (config_.has_key ("x"))
      {
	x = config_.fetch_real ("x");
	x *= lunit;
	if (x < dim_x)
	  {
	    ostringstream message;
	    message << "surrounded_boxed_model::_at_construct: "
		    << "Enforced X dimension '" << x / CLHEP::mm 
		    << "' mm (<" << dim_x / CLHEP::mm << ") is too small for surrounded components to fit !"; 
	    throw runtime_error (message.str ());    
	  }
        dim_x = x;
      }  

    if (config_.has_key ("y"))
      {
	y = config_.fetch_real ("y");
	y *= lunit;
	if (y < dim_y)
	  {
	    ostringstream message;
	    message << "surrounded_boxed_model::_at_construct: "
		    << "Enforced Y dimension '" << y / CLHEP::mm << "' mm (<" << dim_y / CLHEP::mm << ") is too small for surrounded components to fit !"; 
	    throw runtime_error (message.str ());    
	  }
        dim_y = y;
      }  

    if (config_.has_key ("z"))
      {
	z = config_.fetch_real ("z");
	z *= lunit;
	if (z < dim_z)
	  {
	    ostringstream message;
	    message << "surrounded_boxed_model::_at_construct: "
		    << "Enforced Z dimension '" << z / CLHEP::mm << "' mm (<" << dim_z / CLHEP::mm << ") is too small for surrounded components to fit !"; 
	    throw runtime_error (message.str ());    
	  }
        dim_z = z;
      }  

    // define the enclosing solid box:
    __solid.reset ();
    __solid.set_x (dim_x);
    __solid.set_y (dim_y);
    __solid.set_z (dim_z);
    if (! __solid.is_valid ())
      {
	throw runtime_error ("surrounded_boxed_model::_at_construct: Invalid solid !");
      }

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (__get_material_name ());

    // placement of the surrounded solid: 
    __surrounded_placmt.set (surrounded_x, surrounded_y, surrounded_z, 
			     0.0, 0.0);
    __surrounded_phys.set_name (i_model::make_physical_volume_name (__surrounded_label));
    __surrounded_phys.set_placement (__surrounded_placmt);
    __surrounded_phys.set_logical (__surrounded_model->get_logical ());
    __surrounded_phys.set_mother (get_logical ());


    // placement of the surrounding solids: 
    for (surrounding_dict_t::iterator i = __surrounding_items.begin ();
	 i != __surrounding_items.end ();
	 i++)
      {
	int position = i->first;
	surrounding_item & si = i->second;
	const i_model * model = si.model;
	double xi, yi, zi;
	xi = surrounded_x;
	yi = surrounded_y;
	zi = surrounded_z;
	const box & b = 
	  dynamic_cast<const box &>(model->get_logical ().get_shape ());
	if (position == BACK)
	  {
	    xi -= 0.5 * dx;
	    xi -= 0.5 * dx0;
	  }
	if (position == FRONT)
	  {
	    xi += 0.5 * dx;
	    xi += 0.5 * dx1;
	  }
	if (position == LEFT)
	  {
	    yi -= 0.5 * dy;
	    yi -= 0.5 * dy0;
	  }
	if (position == RIGHT)
	  {
	    yi += 0.5 * dy;
	    yi += 0.5 * dy1;
	  }
	if (position == BOTTOM)
	  {
	    zi -= 0.5 * dz;
	    zi -= 0.5 * dz0;
	  }
	if (position == TOP)
	  {
	    zi += 0.5 * dz;
	    zi += 0.5 * dz1;
	  }
	si.placmt.set (xi, yi, zi, 0.0, 0.0);
	si.phys.set_name (i_model::make_physical_volume_name (si.label));
	si.phys.set_placement (si.placmt);
	si.phys.set_logical (si.model->get_logical ());
	si.phys.set_mother (get_logical ());
      }
    
    if (devel) clog << datatools::utils::io::devel 
		    << "surrounded_boxed_model::_at_construct: Exiting." << endl;
    return;
  }
  
  void surrounded_boxed_model::tree_dump (ostream & out_, 
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
	    << "X-centered : " << __centered_x << endl;
       out_ << indent << i_tree_dumpable::tag 
	    << "Y-centered : " << __centered_y << endl;
       out_ << indent << i_tree_dumpable::tag 
	    << "Z-centered : " << __centered_z << endl;
     }

     {
       out_ << indent << i_tree_dumpable::tag 
	    << "Surrounded model : " << __surrounded_label << endl;
     }
     
     {
       for (labels_dict_t::const_iterator i = __surrounding_labels.begin ();
	    i != __surrounding_labels.end ();
	    i++)
	 {
	   out_ << indent << i_tree_dumpable::tag 
		<< "Surrounding model : " << "'" << i->first << "'" << " [rank==" << i->second << "]" << endl;
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

// end of surrounded_boxed_model.cc
