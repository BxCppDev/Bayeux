// -*- mode: c++ ; -*- 
/* mapping.cc 
 */

#include <geomtools/mapping.h>

namespace geomtools {
  
  using namespace std;

  bool mapping::g_devel = false;

  /*** Properties manipulation ***/
  const string mapping::MAPPING_PREFIX = "mapping.";

  string mapping::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << mapping::MAPPING_PREFIX << key_;
    return key_oss.str ();
  }
  
  void mapping::extract (const datatools::utils::properties & source_,
			 datatools::utils::properties & target_)
  {
    source_.export_starting_with (target_, mapping::MAPPING_PREFIX);
    return;
  }
 
  bool mapping::has_flag (const datatools::utils::properties & config_,
			  const string & flag_)
  {
    return (config_.has_flag (mapping::make_key (flag_)));
  }

  bool mapping::has_key (const datatools::utils::properties & config_,
			 const string & key_)
  {
    return (config_.has_key (mapping::make_key (key_)));
  }

  /******/

  bool mapping::is_initialized () const
  {
    return __initialized;
  }
 
  bool mapping::is_mode_none () const
  {
    return __mode == MODE_NONE;
  }
 
  bool mapping::is_mode_only () const
  {
    return __mode == MODE_ONLY;
  }
 
  bool mapping::is_mode_excluded () const
  {
    return __mode == MODE_EXCLUDED;
  }

  void mapping::add_only (const string & category_)
  {
    if (is_initialized ())
      {
	throw runtime_error ("mapping::add_only: Already initialized !");
      }
    if (! is_mode_only ())
      {
	__only_excluded_list.clear ();
	__mode = MODE_ONLY;
      }
    __only_excluded_list.push_back (category_);
    return;
  }

  void mapping::add_excluded (const string & category_)
  {
    if (is_initialized ())
      {
	throw runtime_error ("mapping::add_excluded: Already initialized !");
      }
    if (! is_mode_excluded ())
      {
	__only_excluded_list.clear ();
	__mode = MODE_EXCLUDED;
      }
    __only_excluded_list.push_back (category_);
    return;
  }

  void mapping::initialize (const datatools::utils::properties & config_)
  {
    if (is_initialized ())
      {
	throw runtime_error ("mapping::initialize: Already initialized !");
      }

    if (config_.has_key ("mapping.max_depth"))
      {
	int mdepth = config_.fetch_integer ("mapping.max_depth");
	if (mdepth > 0)
	  {
	    set_max_depth (mdepth);
	  }
      }

    bool has_only = false;
    if (config_.has_key ("mapping.only_categories"))
      {
	has_only = true;
	vector<string> only;
	config_.fetch ("mapping.only_categories", only);
	for (int i = 0; i < only.size (); i++)
	  {
	    add_only (only[i]);
	  }
      }

    if (config_.has_key ("mapping.excluded_categories"))
      {
	if (has_only)
	  {
	    throw runtime_error ("mapping::initialize: The 'mapping.excluded_categories' property is not compatible with 'mapping.only_categories' property !");
	  }
	vector<string> excluded;
	config_.fetch ("mapping.excluded_categories", excluded);
	for (int i = 0; i < excluded.size (); i++)
	  {
	    add_excluded (excluded[i]);
	  }
      }

    __initialized = true;
  }

  // ctor:
  mapping::mapping () : geom_map ()
  {
    __initialized = false;
    __depth       = 0;
    __max_depth   = NO_MAX_DEPTH;
    __factory     = 0;
    __top_logical = 0;
    __mode        = MODE_NONE;
    return;
  }

  // dtor:
  mapping::~mapping ()
  {
  }

  void mapping::set_max_depth (size_t max_depth_)
  {
    if (is_initialized ())
      {
	throw runtime_error ("mapping::set_max_depth: Already initialized !");
      }
    __max_depth = max_depth_;
    return;
  }

  size_t mapping::get_max_depth () const
  {
    return __max_depth;
  }

  void mapping::build_from (const model_factory & factory_, const string & mother_)
  {
    bool devel = g_devel;
    if (devel)
      {
	clog << datatools::utils::io::devel << "mapping::build_from: Entering..." << endl;
      }

    if (! is_initialized ())
      {
	__initialized = true;
	//throw runtime_error ("mapping::build_from: Not initialized !");
      }

    if (! factory_.is_locked ())
      {
	throw runtime_error ("mapping::build_from: Factory is not locked !");
      }
    __factory = &factory_;
    models_col_t::const_iterator found 
      = __factory->get_models ().find (mother_);
    if (found == __factory->get_models ().end ())
      {
	ostringstream message;
	message << "mapping::build_from: Cannot find model '"
		<< mother_ << "' !";
	throw runtime_error (message.str ());
      }
    const i_model & top_model = *(found->second);
    __top_logical = &(top_model.get_logical ());
    __build ();
    if (devel)
      {
	clog << datatools::utils::io::devel 
	     << "mapping::build_from: Exiting." << endl;
      }
    return;
  }

  void mapping::dump_dictionnary (ostream & out_) const
  {
    out_ << "--- Geometry ID mapping --- " << endl;
    for (geom_info_dict_t::const_iterator i
	   = _get_geom_infos ().begin ();
	 i != _get_geom_infos ().end ();
	 i++)
      {
	out_ << i->first << ": LOG=`";
	if (i->second.has_logical ())
	  {
	    out_ << i->second.get_logical ().get_name ();
	  }
	else
	  {
	    out_ << "<no logical>";
	  }
	out_ << "'";
	out_ << " WP=`" << i->second.get_world_placement () << "'";
	out_ << endl;
      }
    out_ << "--------------------------- " <<  endl;
    return;
  }

  void mapping::__build ()
  {
    bool devel = g_devel;

    if (devel)
      {
	clog << datatools::utils::io::devel 
	     << "mapping::__build: Entering..." << endl;
      }
    string world_cat_name = "world";
    if (! _get_id_manager ().has_category_info (world_cat_name))
      {
	ostringstream message;
	message << "mapping::__build: Unknown category '"
		<< world_cat_name << "' !";
	throw runtime_error (message.str ());
      }
    {
      const id_mgr::category_info & world_cat_info
	= _get_id_manager ().get_category_info (world_cat_name);
      if (devel) world_cat_info.tree_dump (clog, "Category info: ");
      geom_id world_id;
      world_cat_info.create (world_id);
      world_id.set_address (0);
      if (devel)
	{
	  clog << datatools::utils::io::devel 
	       <<  "mapping::__build:"<< "World ID = " << world_id << ' ' 
	       << (world_id.is_valid () ? "[Valid]": "[Invalid]")<< endl;
	}
      placement top_placement (0.0, 0.0, 0.0);
	
      //	const logical_volume * world_log 
      //  = __factory->get_logicals ().find ()->second;
	
      geom_info world_gi (world_id, 
			  top_placement, 
			  *__top_logical);

      // Add 
      _get_geom_infos ()[world_id] = world_gi;
      //dump_dictionnary (clog);
      __build_logical_children (*__top_logical, top_placement, world_id);
    }
    //dump_dictionnary (clog);
    if (devel)
      {
	clog << datatools::utils::io::devel 
	     << "mapping::__build: Exiting." << endl;
      }
    return;
  }
 
  void mapping::__build_logical_children (const logical_volume & log_, 
					  const placement & mother_world_placement_,
					  const geom_id & mother_id_)
  {
    bool devel = g_devel;
    namespace du = datatools::utils;
    __indenter (++__depth);

    if (devel) clog << du::io::devel << __indenter << "mapping::__build_logical_children: Entering..." << endl;

    const logical_volume & log = log_;
    if (devel) clog << du::io::devel << __indenter << "mapping::__build_logical_children: Log = `" << log.get_name () << "'" << endl;
    if (log.get_physicals ().size () == 0)
      {
	if (devel) clog << du::io::devel << __indenter << "mapping::__build_logical_children: Exiting." << endl;
	__indenter (--__depth);
	return;
      }
    if (devel)
      { 
	log.parameters ().tree_dump (clog, 
				     "***** logical's parameters *****", 
				     "DEVEL: ");
      }

    // Loop on children physical volumes:
    for (logical_volume::physicals_col_t::const_iterator i 
	   = log.get_physicals ().begin ();
	 i != log.get_physicals ().end ();
	 i++)
      {
	const string & phys_name = i->first;
	const physical_volume & phys_vol = *(i->second);
	if (devel) clog << du::io::devel << __indenter << "Physical '" << phys_name << "' : "
			<< "'" << phys_vol.get_name () << "' " << endl;
	  
	const logical_volume & phys_logical = phys_vol.get_logical ();
	  

	/*
	  if (devel) phys_logical.parameters ().tree_dump (clog, 
	  "***** logical's parameters *****", 
	  "DEVEL: mapping::__build_logical_children: ");
	*/

	string daughter_label 
	  = i_model::extract_label_from_physical_volume_name (phys_vol.get_name ());
	if (devel) clog << du::io::devel << __indenter 
			<< "Daughter label = '" 
			<< daughter_label << "' " << endl;
	string daughter_category_info;
	if (mapping_utils::has_daughter_id (log.parameters (),
					    daughter_label))
	  {
	    mapping_utils::fetch_daughter_id (log.parameters (),
					      daughter_label,
					      daughter_category_info);
	    if (devel)
	      {
		clog << du::io::devel << __indenter 
		     << "Found daughter ID info for physical '" 
		     << phys_name << "' " << endl;
	      }
	  }
	else
	  {
	    if (devel)
	      {
		clog << du::io::devel << __indenter 
		     << "No daughter ID info for physical '" 
		     << phys_name << "' " << endl;
	      }
	  }

	const i_placement & phys_placement = phys_vol.get_placement ();
	if (devel) clog << du::io::devel << __indenter << "-> Number of items: " << phys_placement.get_number_of_items () << endl;
	// Loop on replicated children physical volumes:
	for (int item = 0; 
	     item < phys_placement.get_number_of_items (); 
	     item++)
	  {
	    if (devel) clog << du::io::devel << __indenter << "-> item #" << item << ": " << endl;

	    // get the current item placement 
	    // in the mother coordinates system:
	    placement item_placement;
	    phys_placement.get_placement (item, item_placement);
	    {
	      string tmp;
	      placement::to_string (tmp, item_placement);
	      if (devel) clog << du::io::devel << __indenter << "-> child placement " << tmp << " " << endl; 
	    }

	    // compute the current item placement
	    // in the world coordinates system: 
	    placement world_item_placement;
	    mother_world_placement_.child_to_mother (item_placement, 
						     world_item_placement);
	    {
	      string tmp;
	      placement::to_string (tmp, world_item_placement);
	      if (devel) clog << du::io::devel << __indenter << "-> world placement " << tmp << " " << endl;
	    }
	    int address = item;
	    geom_id propagated_world_id = mother_id_;
	    if (! daughter_category_info.empty ())
	      {
		geom_id item_id;

		// compute the vector of sub-addresses:
		vector<uint32_t> items_index;
		phys_placement.compute_index_map (items_index, item);
		_get_id_manager ().compute_id_from_info (item_id, 
							 mother_id_,
							 daughter_category_info, 
							 items_index);
	  
		if (_get_id_manager ().validate_id (item_id))
		  {
		    if (devel) clog << du::io::devel << __indenter << "-> Item ID " << item_id << " is added to the map: " << endl;
		    geom_info item_gi (item_id, 
				       world_item_placement, 
				       phys_logical);


		    bool add_it = true;
		    if (! is_mode_none ())
		      {
			// get the category associated to the item ID:
			const string & category 
			  = _get_id_manager ().get_category_info (item_id.get_type ()).get_category ();
			if (devel) clog << du::io::devel << __indenter << "mapping::__build_logical_children: "
			     << "Category = '" << category << "' (from ID=" << item_id << ")"
			     << endl;
			if (is_mode_only ())
			  {
			    // the list contains only the categories to be registered:
			    list<string>::const_iterator found = find (__only_excluded_list.begin (),
								       __only_excluded_list.end (),
								       category);
			    if (found == __only_excluded_list.end ())
			      {
				add_it = false;
			      } 
			  }
			else if (is_mode_excluded ())
			  {
			    // the list contains only the categories to be excluded:
			    list<string>::const_iterator found = find (__only_excluded_list.begin (),
								       __only_excluded_list.end (),
								       category);
			    if (found != __only_excluded_list.end ())
			      {
				add_it = false;
			      } 
			  }
		      }
		    if (add_it)
		      {
			_get_geom_infos ()[item_id] = item_gi;
		      }
		    propagated_world_id = item_id;
		  }
	      }

	    // DEBUG: stop after traverse down to depth==max_depth:
	    bool build_it = true;
	    if (__max_depth > 0)
	      {
		if (__depth > __max_depth)
		  {
		    build_it = false;
		  }
	      }
	    if (build_it)
	      {
		__build_logical_children (phys_logical, 
					  world_item_placement, 
					  propagated_world_id);
	      }
	    else
	      {
		if (devel) 
		  {
		    clog << du::io::devel << __indenter << "-> DO NOT TRAVERSE THE GEOMETRY TREE FURTHER."
			 << endl;
		  }
	      }
	  }
      }

    if (devel) 
      {
	clog << du::io::devel 
	     << __indenter << "mapping::__build_logical_children: Exiting." 
	     << endl;
      }
    __indenter (--__depth);
    return;
  }

} // end of namespace geomtools

// end of mapping.cc
