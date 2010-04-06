// -*- mode: c++ ; -*- 
/* gdml_export.cc
 */

#include <geomtools/gdml_export.h>

namespace geomtools {

  using namespace std;

  bool gdml_export::g_devel = false;

  const string gdml_export::DEFAULT_LENGTH_UNIT = "millimeter";
  const string gdml_export::DEFAULT_ANGLE_UNIT  = "degree";

  bool gdml_export::is_debug () const
  {
    return __debug;
  }

  void gdml_export::set_debug (bool d_)
  {
    __debug = d_;
  }

  bool gdml_export::is_replica_supported () const
  {
    return __support_replica;
  }

  void gdml_export::add_replica_support (bool s_)
  {
    __support_replica = s_;
  }

  const datatools::utils::properties & gdml_export::parameters () const
  {
    return __parameters;
  }

  datatools::utils::properties & gdml_export::parameters ()
  {
    return __parameters;
  }

  // ctor:
  gdml_export::gdml_export ()
  {
    __debug = false;
    __factory = 0;
    __length_unit = DEFAULT_LENGTH_UNIT;
    __angle_unit = DEFAULT_ANGLE_UNIT;
    __external_materials_stream = 0;
    __support_replica = false;
  }
  
  // dtor:
  gdml_export::~gdml_export ()
  {
  }

  void gdml_export::attach_external_materials (const ostringstream & oss_)
  {
    __external_materials_stream = &oss_;
  }

  void gdml_export::export_gdml (const string & filename_, 
				 const model_factory & factory_,
				 const string & model_name_)
  {
    bool devel = g_devel;
    devel = true; // XXX
    if (devel)
      {
	clog << "DEVEL: gdml_export::export_gdml: Entering..." << endl;
      }

    if (! factory_.is_locked ())
      {
	throw runtime_error ("gdml_export::export_gdml: Factory is not locked !");
      }
    ofstream fout;
    string gdml_filename = filename_;
    datatools::utils::fetch_path_with_env (gdml_filename);
    fout.open (gdml_filename.c_str ());
    if (! fout)
      {
	ostringstream message;
	message << "gdml_export::export_gdml: "
		<< "Cannot open GDML file '"
		<< filename_
		<< "' !";
	throw runtime_error (message.str ());
      }
    _export_gdml (fout, factory_, model_name_);
    if (devel)
      {
	clog << "DEVEL: gdml_export::export_gdml: Exiting." << endl;
      }
    return;
  }

  void gdml_export::_export_gdml (ostream & out_, 
				  const model_factory & factory_,
				  const string & model_name_)
  {
    bool devel = g_devel;
    devel = true; // XXX
    if (devel)
      {
	clog << "DEVEL: gdml_export::export_gdml: Entering..." << endl;
      }
    __factory = &factory_;
    const models_col_t & models = __factory->get_models ();
    models_col_t::const_iterator found = models.find (model_name_);
    if (found == models.end ())
      {
	ostringstream message;
	message << "gdml_export::_export_gdml: "
		<< "Cannot find model '"
		<< model_name_
		<< "' !";
	throw runtime_error (message.str ());	
      }
    const i_model & top_model = *(found->second);
    if (__external_materials_stream != 0)
      {
	clog << "WARNING: gdml_export::export_gdml: Attach a stream with materials external definitions." << endl;
	__writer.attach_external_materials (*__external_materials_stream);
      }
    __writer.init ();
    _export_gdml_model (top_model);

    string xml_version  = gdml_writer::DEFAULT_XML_VERSION;
    string xml_encoding = gdml_writer::DEFAULT_XML_ENCODING;
    string gdml_schema  = gdml_writer::DEFAULT_GDML_SCHEMA;

    if (__parameters.has_key ("xml_version"))
      {
	xml_version = __parameters.fetch_string ("xml_version");
      }

    if (__parameters.has_key ("xml_encoding"))
      {
	xml_encoding = __parameters.fetch_string ("xml_encoding");
      }

    if (__parameters.has_key ("gdml_schema"))
      {
	gdml_schema = __parameters.fetch_string ("gdml_schema");
      }

    if (__parameters.has_key ("length_unit"))
      {
	__length_unit = __parameters.fetch_string ("length_unit");
      }

    if (__parameters.has_key ("angle_unit"))
      {
	__angle_unit = __parameters.fetch_string ("angle_unit");
      }

    // add a fake material:
    __writer.add_material (material::MATERIAL_REF_DEFAULT,
			   1.0,
			   1. * CLHEP::g / CLHEP::cm3,
			   1.00); 
    __writer.add_material (material::MATERIAL_REF_UNKWOWN,
			   1.0,
			   1. * CLHEP::g / CLHEP::cm3,
			   1.00); 
    __writer.add_material (material::MATERIAL_REF_VACUUM,
			   1.0,
			   1.e-15 * CLHEP::g / CLHEP::cm3,
			   1.00); 

    __writer.add_setup ("Setup", top_model.get_name ());

    __writer.full_write (out_, xml_version, xml_encoding, gdml_schema);
    __writer.reset ();

    if (devel)
      {
	clog << "DEVEL: gdml_export::export_gdml: Exiting." << endl;
      }
    return;
  }

  void gdml_export::_export_gdml_solid (const i_shape_3d & shape_,
					const string & solid_name_)
  {
    bool devel = g_devel;
    devel = true; // XXX
    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_solid: Entering..." << endl;
      }
    string shape_name = shape_.get_shape_name ();

    bool composite = false;
    /*
    if (__solid_refs.find)
      {
      }
    */
    if (shape_.is_composite ())
      {
	/*
	if (shape_name == "union")
	  {
	    const union_3d & u = static_cast<const union_3d &> (shape_);
	    //__writer.add_union (solid_name_, bu __length_unit);
	  }
	else
	*/ 
	  {
	    ostringstream message;
	    message << "gdml_export::_export_gdml_solid: "
		    << "Boolean solid type '"
		    << shape_name
		    << "' is not supported !";
	    throw runtime_error (message.str ());	
	  }	
      } 
    else 
      {
	if (shape_name == "box")
	  {
	    const box & b = static_cast<const box &> (shape_);
	    __writer.add_box (solid_name_, b, __length_unit);
	  }
	else if (shape_name == "cylinder")
	  {
	    const cylinder & c = static_cast<const cylinder &> (shape_);
	    __writer.add_cylinder (solid_name_, c, __length_unit, __angle_unit);
	  }
	else if (shape_name == "tube")
	  {
	    const tube & t = static_cast<const tube &> (shape_);
	    __writer.add_tube (solid_name_, t, __length_unit, __angle_unit);
	  }
	else if (shape_name == "sphere")
	  {
	    const sphere & s = static_cast<const sphere &> (shape_);
	    __writer.add_sphere (solid_name_, s, __length_unit, __angle_unit);
	  }
	else 
	  {
	    ostringstream message;
	    message << "gdml_export::_export_gdml_solid: "
		    << "Simple solid type '"
		    << shape_name
		    << "' is not supported !";
	    throw runtime_error (message.str ());	
	  }
      }
    __solid_refs.push_back (solid_name_);

    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_solid: Exiting." << endl;
      }
    return;
  }
  
  void gdml_export::_export_gdml_logical (const logical_volume & lv_)
  {
    bool devel = g_devel;
    devel = true; // XXX
    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_logical: Entering..." << endl;
      }

    const geomtools::logical_volume & logical = lv_;
    const string & log_name = logical.get_name ();

    if (find (__volumes_refs.begin (),
	      __volumes_refs.end (),
	      log_name) != __volumes_refs.end ())
      {
	clog << "DEVEL: gdml_export::_export_gdml_logical: "
	     << "Logical '" << log_name << "' is already exported !" << endl;
	return;
      }

    // export solid shape:
    const i_shape_3d & log_solid = logical.get_shape ();
    ostringstream solid_name_oss;
    solid_name_oss << log_name << i_model::SOLID_SUFFIX;
    string solid_name = solid_name_oss.str ();
    _export_gdml_solid (log_solid, solid_name);

    // prepare volume export
    string material_ref = material::MATERIAL_REF_UNKWOWN;
    string solid_ref = solid_name;
    logical.tree_dump (cerr, "Logical:", " *** ");
    if (logical.has_material_ref ())
      {
	material_ref = logical.get_material_ref ();
      }
    else
      {
	ostringstream message;
	message << "gdml_export::_export_gdml_logical: "
		<< "Logical volume '"
		<< log_name
		<< "' has no material !";
	throw runtime_error (message.str ());		
      }

    bool skip = false;

    // export a dictionary of auxiliary properties:
    map<string, string> auxprops;
    logical.parameters ().export_to_string_based_dictionary (auxprops, false);
    
    if (! skip && logical.get_physicals ().size () == 0)
      {
	__writer.add_volume (log_name, 
			     material_ref,
			     solid_ref, 
			     auxprops);
	skip = true;
      }

    // there is a replica children:
    if (! skip && (__support_replica && logical.is_replica ()))
      {
	const physical_volume & phys = *(logical.get_physicals ().begin ()->second);
	if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
			<< "replica phys=" << phys.get_name () << endl;
	const logical_volume & log_child = phys.get_logical ();
	if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
			<< "replica log_child=" << log_child.get_name () << endl;
	    
	_export_gdml_logical (log_child);
	const i_placement * pp = &(phys.get_placement ());

	gdml_writer::replicavol a_replicavol;
	// only support for 'regular_linear_placement':
	const regular_linear_placement * RLP = 0;
	RLP = dynamic_cast<const regular_linear_placement *> (pp);
	if (RLP == 0)
	  {
	    throw runtime_error ("gdml_export::_export_gdml_logical: GDML replica support is for 'regular_linear_placement' only !");
	  }
	a_replicavol.volumeref = log_child.get_name ();
	a_replicavol.number = pp->get_number_of_items ();
	a_replicavol.mode = "replicate_along_axis";
	if (RLP->is_replicant_x_axis ()) 
	  {
	    a_replicavol.direction = "x";
	    a_replicavol.width = RLP->get_step ().x ();
	  }
	else if (RLP->is_replicant_y_axis ()) 
	  {
	    a_replicavol.direction = "y";
	    a_replicavol.width = RLP->get_step ().y ();
	  }
	else if (RLP->is_replicant_z_axis ()) 
	  {
	    a_replicavol.direction = "z";
	    a_replicavol.width = RLP->get_step ().z ();
	  }
	a_replicavol.offset = 0.0;
	if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
			<< "add volume '" << log_name << "' (replica)..." << endl;
	
	__writer.add_replica_volume (log_name, 
				     material_ref,
				     solid_ref, 
				     a_replicavol,
				     __length_unit,
				     __angle_unit,
				     auxprops);
	skip = true;
      }

    // there are children:
    if (! skip && (logical.get_physicals ().size () > 0))
      {
	if (devel) 
	  {
	    clog << "DEVEL: gdml_export::_export_gdml_logical: Here we should export the daughter physicals..." << endl;
	    clog << "DEVEL: gdml_export::_export_gdml_logical: List of daughter physicals:" << endl;

	    for (logical_volume::physicals_col_t::const_iterator i 
		   = logical.get_physicals ().begin (); 
		 i != logical.get_physicals ().end (); 
		 i++)
	      {
		const string & name = (i->first);
		const physical_volume & phys = *(i->second);
		clog << "DEVEL: gdml_export::_export_gdml_logical: "
		     << "    name=" << name << "     " << "phys->name=" << phys.get_name () << endl;
	      }
	  }
	
	list<gdml_writer::physvol> physvols;
	for (logical_volume::physicals_col_t::const_iterator i 
	       = logical.get_physicals ().begin (); 
	     i != logical.get_physicals ().end (); 
	     i++)
	  {
	    const physical_volume & phys = *(i->second);
	    if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
			    << "phys=" << phys.get_name () << endl;
	    const logical_volume & log_child = phys.get_logical ();
	    if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
			    << "log_child=" << log_child.get_name () << endl;
	    
	    _export_gdml_logical (log_child);
	    
	    const i_placement * pp = &(phys.get_placement ());
	    bool multiple = false;
	    size_t nitems = pp->get_number_of_items ();
	    bool only_one_rotation = pp->has_only_one_rotation ();
	    multiple = (nitems > 1);
	    
	    if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
			    << "No replica... for '" << log_name << "'..." << endl;
	    // XXX UUU
	    rotation_3d ref_rot;
	    invalidate_rotation_3d (ref_rot);
	    ostringstream ref_rot_name_oss;
	    if (only_one_rotation)
	      {
		ref_rot_name_oss << log_name << '.' << phys.get_name ();
		if (multiple) ref_rot_name_oss << '{' << '0' << '-' << (nitems - 1) << '}';
		ref_rot_name_oss << ".rot";
	      }
	    if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
			    << "add placements for " << nitems 
			    << " items..." << endl;
	    for (int i = 0; i < nitems; i++)
	      {
		// extract placement for item 'i':
		placement p;
		pp->get_placement (i, p);

		// register the position of item 'i':
		ostringstream pos_name_oss;
		pos_name_oss << log_name << '.' << phys.get_name ();
		if (multiple) pos_name_oss << '[' << i << ']';
		pos_name_oss << ".pos";
		__writer.add_position (pos_name_oss.str (), 
				       p.get_translation (), 
				       __length_unit);
		
		if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
				<< "add rotation..." << endl;
		// register the rotation of item 'i':
		//   default rotation name:
		ostringstream rot_name_oss;
		rot_name_oss << log_name << '.' << phys.get_name ();
		if (multiple) rot_name_oss << '[' << i << ']';
		rot_name_oss << ".rot";
		string rot_name = rot_name_oss.str ();
		bool add_rot = false;
		// XXX YYY ZZZ
		if (only_one_rotation)
		  {
		    rot_name = ref_rot_name_oss.str ();
		    if (! is_valid_rotation_3d (ref_rot))
		      {
			ref_rot = p.get_rotation ();
			add_rot= true;
		      }
		  }
		else
		  {
		    // Force add_rot:
		    add_rot = true;
		  }
		if (add_rot)
		  { 
		    if (! is_identity (p.get_rotation ()))
		      {
			__writer.add_rotation (rot_name, 
					       p.get_rotation (),
					       __angle_unit);
		      }
		  }
		if (is_identity (p.get_rotation ()))
		  {
		    rot_name = "";
		  }
		physvols.push_back (gdml_writer::physvol (log_child.get_name (), 
							  pos_name_oss.str (), 
							  rot_name));
	      } // for ... items
	    if (devel) 
		  {
		    clog << "DEVEL: gdml_export::_export_gdml_logical: "
			 << "add volume '" << log_name << "' with physvols=";
		    for (list<gdml_writer::physvol>::const_iterator jj = physvols.begin ();
			 jj != physvols.end ();
			 jj++)
		      {
			clog << '"' << jj->volumeref << '"' << ' '; 
		      }
		    clog  << endl;
		  }
	      }
	__writer.add_volume (log_name, 
			     material_ref,
			     solid_ref, 
			     physvols,
			     auxprops);
	skip = true;
      } // there are children:
    
    __volumes_refs.push_back (log_name);
    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_logical: Exiting." << endl;
      }
    return;
    return;
  }
  
  void gdml_export::_export_gdml_model (const i_model & model_)
  {
    bool devel = g_devel;
    devel = true; // XXX
    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_model: Entering..." << endl;
      }
    const i_model & model = model_;

    if (! model.is_constructed ())
      {
	ostringstream message;
	message << "gdml_export::_export_gdml_model: "
		<< "Model '"
		<< model.get_name ()
		<< "' is not constructed !";
	throw runtime_error (message.str ());	
      }
    _export_gdml_logical (model.get_logical ());

    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_model: Exiting." << endl;
      }
    return;
  }
  
} // end of namespace geomtools

// end of gdml_export.cc
