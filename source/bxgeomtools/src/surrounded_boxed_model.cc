// -*- mode: c++ ; -*- 
/* surrounded_boxed_model.cc
 */

#include <geomtools/surrounded_boxed_model.h>

namespace geomtools {

  using namespace std;

  const string surrounded_boxed_model::SURROUNDED_LABEL      = "surrounded";
  const string surrounded_boxed_model::SURROUNDING_LABEL     = "surrounding";
  const string surrounded_boxed_model::LABEL_PROPERTY_PREFIX = "label";

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
    return;
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
    return;
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
    return;
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

  bool surrounded_boxed_model::is_debug () const
  {
    return __debug;
  }
    
  void surrounded_boxed_model::set_debug (bool d_)
  {
    __debug = d_;
    return;
  }
  
  // ctor:
  surrounded_boxed_model::surrounded_boxed_model () : i_boxed_model ()
  {
    __debug = false;
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
    return;
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
    if (config_.has_flag ("devel"))
      {
	devel = true;
      }  
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


    if (config_.has_flag ("debug"))
      {
	set_debug (true);
      }  

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
	if (devel) cerr << "DEVEL: surrounded_boxed_model::_at_construct: X-centered" << endl; 
	__centered_x = true;
      }
    if (config_.has_flag ("centered.y"))
      {
	if (devel) cerr << "DEVEL: surrounded_boxed_model::_at_construct: Y-centered" << endl; 
	__centered_y = true;
      }
    if (config_.has_flag ("centered.z"))
      {
	if (devel) cerr << "DEVEL: surrounded_boxed_model::_at_construct: Z-centered" << endl; 
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
	  // check if the model is stackable:
	  if (! i_shape_3d::is_stackable (found->second->get_logical ().get_shape ()))
	    {
	      ostringstream message;
	      message << "stacked_boxed_model::_at_construct: "
		      << "The surrounded model '" << found->second->get_name () 
		      << "' is not stackable !"; 
	      throw runtime_error (message.str ());
	    }
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
		  clog << datatools::utils::io::devel << message.str () << endl;	
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
	      // check if the model is stackable:
	      if (! i_shape_3d::is_stackable (found->second->get_logical ().get_shape ()))
		{
		  ostringstream message;
		  message << "stacked_boxed_model::_at_construct: "
			  << "The " << *ilabel << " surrounding model '" 
			  << found->second->get_name () 
			  << "' is not stackable !"; 
		  throw runtime_error (message.str ());
		}
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
    mygsl::min_max mmx0;
    mygsl::min_max mmy0;
    mygsl::min_max mmz0;
    mygsl::min_max mmx1;
    mygsl::min_max mmy1;
    mygsl::min_max mmz1;
    const i_shape_3d & the_shape = __surrounded_model->get_logical ().get_shape ();

    // try to get a stackable data from the shape:
    stackable_data the_SD;
    if (! i_shape_3d::pickup_stackable (the_shape, the_SD))
      {
	ostringstream message;
	message << "stacked_boxed_model::_at_construct: "
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

    // border coordinates:
    double x0 = gxmin;
    double x1 = gxmax;
    double y0 = gymin;
    double y1 = gymax;
    double z0 = gzmin;
    double z1 = gzmax;

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
	const i_shape_3d & the_surrounding_shape = model->get_logical ().get_shape ();

	// try to get a stackable data from the shape:
	stackable_data the_SD2;
	if (! i_shape_3d::pickup_stackable (the_surrounding_shape, the_SD2))
	  {
	    ostringstream message;
	    message << "stacked_boxed_model::_at_construct: "
		    << "Cannot stack the '" 
		    << the_surrounding_shape.get_shape_name () << "' surrounding shape !";
	    throw runtime_error (message.str ());
	  }
	double g2xmin = the_SD2.get_xmin ();
	double g2xmax = the_SD2.get_xmax ();
	double g2ymin = the_SD2.get_ymin ();
	double g2ymax = the_SD2.get_ymax ();
	double g2zmin = the_SD2.get_zmin ();
	double g2zmax = the_SD2.get_zmax ();
	
	if (position == BACK)
	  {
	    dx0 = abs (g2xmax - g2xmin);
	    x0 -= dx0;
	    mmy0.add (g2ymin);
	    mmy1.add (g2ymax);
	    mmz0.add (g2zmin);
	    mmz1.add (g2zmax);
	  }
	if (position == FRONT)
	  {
	    dx1 = abs (g2xmax - g2xmin);
	    x1 += dx1;
	    mmy0.add (g2ymin);
	    mmy1.add (g2ymax);
	    mmz0.add (g2zmin);
	    mmz1.add (g2zmax);
	  }
	if (position == LEFT)
	  {
	    dy0 = abs (g2ymax - g2ymin);
	    y0 -= dy0;
	    mmx0.add (g2xmin);
	    mmx1.add (g2xmax);
	    mmz0.add (g2zmin);
	    mmz1.add (g2zmax);
	  }
	if (position == RIGHT)
	  {
	    dy1 = abs (g2ymax - g2ymin);
	    y1 += dy1;
	    mmx0.add (g2xmin);
	    mmx1.add (g2xmax);
	    mmz0.add (g2zmin);
	    mmz1.add (g2zmax);
	  }
	if (position == BOTTOM)
	  {
	    dz0 = abs (g2zmax - g2zmin);
	    z0 -= dz0;
	    mmx0.add (g2xmin);
	    mmx1.add (g2xmax);
	    mmy0.add (g2ymin);
	    mmy1.add (g2ymax);
	  }
	if (position == TOP)
	  {
	    dz1 = abs (g2zmax - g2zmin);
	    z1 += dz1;
	    mmx0.add (g2xmin);
	    mmx1.add (g2xmax);
	    mmy0.add (g2ymin);
	    mmy1.add (g2ymax);
	  }
      }
    
    if (mmx0.get_min () < x0) x0 = mmx0.get_min ();
    if (mmx1.get_max () > x1) x1 = mmx1.get_max ();
    if (mmy0.get_min () < y0) y0 = mmy0.get_min ();
    if (mmy1.get_max () > y1) y1 = mmy1.get_max ();
    if (mmz0.get_min () < z0) z0 = mmz0.get_min ();
    if (mmz1.get_max () > z1) z1 = mmz1.get_max ();
    double surrounded_x = 0;
    double surrounded_y = 0;
    double surrounded_z = 0;
    double dim_x = 2 * max (x1, abs (x0));
    double dim_y = 2 * max (y1, abs (y0));
    double dim_z = 2 * max (z1, abs (z0));
    if (! __centered_x)
      {
	if (devel) cerr << "DEVEL: geomtools::surrounded_boxed_model: ! X-centered" << endl; 
	dim_x = x1 - x0;
	surrounded_x = -0.5 * dim_x - x0;
      }
    else
      {
	if (devel) cerr << "DEVEL: geomtools::surrounded_boxed_model: X-centered" << endl; 
      }
    if (! __centered_y)
      {
	if (devel) cerr << "DEVEL: geomtools::surrounded_boxed_model: ! Y-centered" << endl; 
	dim_y = y1 - y0;
	surrounded_y = -0.5 * dim_y - y0;
      }
    else
      {
	if (devel) cerr << "DEVEL: geomtools::surrounded_boxed_model: Y-centered" << endl; 
      }
    if (! __centered_z)
      {
	if (devel) cerr << "DEVEL: geomtools::surrounded_boxed_model: ! Z-centered" << endl; 
	dim_z = z1 - z0;
	surrounded_z = -0.5 * dim_z + z0;
      }
    else
      {
	if (devel) cerr << "DEVEL: geomtools::surrounded_boxed_model: Z-centered" << endl; 
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
    if (devel) 
      {
	__solid.tree_dump (cerr, "surrounded_boxed_model::_at_construct: Solid: ", "DEVEL: ");
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
	const i_shape_3d & the_surrounding_shape = model->get_logical ().get_shape ();
	double xi, yi, zi;
	xi = surrounded_x;
	yi = surrounded_y;
	zi = surrounded_z;

	// try to get a stackable data from the shape:
	stackable_data the_SD2;
	i_shape_3d::pickup_stackable (the_surrounding_shape, the_SD2);
	double g2xmin = the_SD2.get_xmin ();
	double g2xmax = the_SD2.get_xmax ();
	double g2ymin = the_SD2.get_ymin ();
	double g2ymax = the_SD2.get_ymax ();
	double g2zmin = the_SD2.get_zmin ();
	double g2zmax = the_SD2.get_zmax ();
	if (position == BACK)
	  {
	    xi += the_SD.get_xmin ();
	    xi -= g2xmax;
	  }
	if (position == FRONT)
	  {
	    xi += the_SD.get_xmax ();
	    xi -= g2xmin;
	  }
	if (position == LEFT)
	  {
	    yi += the_SD.get_ymin ();
	    yi -= g2ymax;
	  }
	if (position == RIGHT)
	  {
	    yi += the_SD.get_ymax ();
	    yi -= g2ymin;
	  }
	if (position == BOTTOM)
	  {
	    zi += the_SD.get_zmin ();
	    zi -= g2zmax;
	  }
	if (position == TOP)
	  {
	    zi += the_SD.get_zmax ();
	    zi -= g2zmin;
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
  
  // register this creator:   
  geomtools::i_model::creator_registration<surrounded_boxed_model> surrounded_boxed_model::__CR;
       
} // end of namespace geomtools

// end of surrounded_boxed_model.cc
