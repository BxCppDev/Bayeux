// -*- mode: c++ ; -*-
/* gdml_export.cc
 */

#include <iomanip>
#include <stdexcept>
#include <algorithm>

#include <geomtools/gdml_export.h>
#include <geomtools/union_3d.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>

#include <geomtools/detail/model_tools.h>
#include <geomtools/model_factory.h>
#include <geomtools/gdml_writer.h>
#include <geomtools/units.h>
#include <geomtools/utils.h>
#include <geomtools/material.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/regular_linear_placement.h>

namespace geomtools {

  using namespace std;

  bool gdml_export::g_devel = false;

  const string gdml_export::DEFAULT_LENGTH_UNIT  = "mm";
  const string gdml_export::DEFAULT_ANGLE_UNIT   = "deg";
  const string gdml_export::DEFAULT_DENSITY_UNIT = "g/cm3";

  bool gdml_export::is_debug () const
  {
    return _debug_;
  }

  void gdml_export::set_debug (bool d_)
  {
    _debug_ = d_;
    return;
  }

  bool gdml_export::is_auxiliary_supported () const
  {
    return _support_auxiliary_;
  }

  void gdml_export::add_auxiliary_support (bool s_)
  {
    _support_auxiliary_ = s_;
    return;
  }


  bool gdml_export::is_replica_supported () const
  {
    return _support_replica_;
  }

  void gdml_export::add_replica_support (bool s_)
  {
    _support_replica_ = s_;
    return;
  }

  const datatools::utils::properties & gdml_export::parameters () const
  {
    return _parameters_;
  }

  datatools::utils::properties & gdml_export::parameters ()
  {
    return _parameters_;
  }

  // ctor:
  gdml_export::gdml_export ()
  {
    _debug_ = false;
    _factory_ = 0;
    _length_unit_ = DEFAULT_LENGTH_UNIT;
    _angle_unit_ = DEFAULT_ANGLE_UNIT;
    _density_unit_ = DEFAULT_DENSITY_UNIT;
    _external_materials_stream_ = 0;
    _support_auxiliary_ = true;
    _support_replica_ = false;
    return;
  }

  // dtor:
  gdml_export::~gdml_export ()
  {
    return;
  }

  void gdml_export::attach_external_materials (const ostringstream & oss_)
  {
    _external_materials_stream_ = &oss_;
    return;
  }

  void gdml_export::export_gdml (const string & filename_,
				 const model_factory & factory_,
				 const string & model_name_)
  {
    bool devel = g_devel;
    //devel = true; // XXX
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
    //devel = true; // XXX
    if (devel)
      {
	clog << "DEVEL: gdml_export::export_gdml: Entering..." << endl;
      }
    _factory_ = &factory_;
    const models_col_t & models = _factory_->get_models ();
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
    if (_external_materials_stream_ != 0)
      {
	clog << "NOTICE: gdml_export::export_gdml: Attach a stream with materials external definitions." << endl;
	_writer_.attach_external_materials (*_external_materials_stream_);
      }
    _writer_.init ();
    _export_gdml_model (top_model);

    string xml_version  = gdml_writer::DEFAULT_XML_VERSION;
    string xml_encoding = gdml_writer::DEFAULT_XML_ENCODING;
    string xsi          = gdml_writer::DEFAULT_XSI;
    string gdml_schema  = gdml_writer::DEFAULT_REMOTE_GDML_SCHEMA;

    if (_parameters_.has_key ("xml_version"))
      {
	xml_version = _parameters_.fetch_string ("xml_version");
      }

    if (_parameters_.has_key ("xml_encoding"))
      {
	xml_encoding = _parameters_.fetch_string ("xml_encoding");
      }

    if (_parameters_.has_key ("xsi"))
      {
	xsi = _parameters_.fetch_string ("xsi");
      }

    if (_parameters_.has_key ("gdml_schema"))
      {
	gdml_schema = _parameters_.fetch_string ("gdml_schema");
      }

    if (_parameters_.has_key ("length_unit"))
      {
	_length_unit_ = _parameters_.fetch_string ("length_unit");
      }

    if (_parameters_.has_key ("angle_unit"))
      {
	_angle_unit_ = _parameters_.fetch_string ("angle_unit");
      }

    if (_parameters_.has_key ("density_unit"))
      {
	_density_unit_ = _parameters_.fetch_string ("density_unit");
      }

    // add a fake material:
    _writer_.add_material (material::MATERIAL_REF_DEFAULT,
			   1.0,
			   1. * CLHEP::g / CLHEP::cm3,
			   1.00);
    _writer_.add_material (material::MATERIAL_REF_UNKNOWN,
			   1.0,
			   1. * CLHEP::g / CLHEP::cm3,
			   1.00);
    _writer_.add_material (material::MATERIAL_REF_VACUUM,
			   1.0,
			   1.e-15 * CLHEP::g / CLHEP::cm3,
			   1.00);

    _writer_.add_setup ("Setup", top_model.get_logical ().get_name ());

    _writer_.full_write (out_, xml_version, xml_encoding, gdml_schema, xsi);
    _writer_.reset ();

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
    //devel = true;
    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_solid: Entering..." << endl;
      }
    string shape_name = shape_.get_shape_name ();

    if (! gdml_writer::solid_type_is_valid (shape_name))
      {
	ostringstream message;
	message << "gdml_export::_export_gdml_solid: "
		<< "Solid type '"
		<< shape_name
		<< "' is not valid !";
	throw runtime_error (message.str ());
     }
    if (! gdml_writer::solid_type_is_supported (shape_name))
      {
	ostringstream message;
	message << "gdml_export::_export_gdml_solid: "
		<< "Solid type '"
		<< shape_name
		<< "' is not supported !";
	throw runtime_error (message.str ());
     }
    bool composite = false;

    if (shape_.is_composite ())
      {
	/* GDML constraints:
	 * One should check if placement of shape 1 in any composite
	 * solid is NULL (translation & rotation).
	 *
	 */
	if (shape_name == "union_3d")
	  {
	    const union_3d & u = static_cast<const union_3d &> (shape_);
	    string shape_ref_1 = solid_name_ + ".union.first_ref";
	    string shape_ref_2 = solid_name_ + ".union.second_ref";;
	    string pos_ref = solid_name_ + ".union.pos_ref";
	    string rot_ref = solid_name_ + ".union.rot_ref";;
	    // only stores the solid #2 placement:
	    _writer_.add_position (pos_ref,
				   u.get_shape2 ().get_placement ().get_translation (),
				   _length_unit_);
	    _writer_.add_rotation (rot_ref,
				   u.get_shape2 ().get_placement ().get_rotation (),
				   _angle_unit_);
	    this->_export_gdml_solid (u.get_shape1 ().get_shape (), shape_ref_1);
	    this->_export_gdml_solid (u.get_shape2 ().get_shape (), shape_ref_2);

	    _writer_.add_gdml_union (solid_name_,
				     shape_ref_1,
				     shape_ref_2,
				     pos_ref,
				     rot_ref);
	  }
	else if (shape_name == "subtraction_3d")
	  {
	    const subtraction_3d & s = static_cast<const subtraction_3d &> (shape_);
	    string shape_ref_1 = solid_name_ + ".subtraction.first_ref";
	    string shape_ref_2 = solid_name_ + ".subtraction.second_ref";;
	    string pos_ref = solid_name_ + ".subtraction.pos_ref";
	    string rot_ref = solid_name_ + ".subtraction.rot_ref";;

	    // only stores the solid #2 placement:
	    _writer_.add_position (pos_ref,
				   s.get_shape2 ().get_placement ().get_translation (),
				   _length_unit_);
	    _writer_.add_rotation (rot_ref,
				   s.get_shape2 ().get_placement ().get_rotation (),
				   _angle_unit_);
	    this->_export_gdml_solid (s.get_shape1 ().get_shape (), shape_ref_1);
	    this->_export_gdml_solid (s.get_shape2 ().get_shape (), shape_ref_2);

	    _writer_.add_gdml_subtraction (solid_name_,
					   shape_ref_1,
					   shape_ref_2,
					   pos_ref,
					   rot_ref);
	  }
	else if (shape_name == "intersection_3d")
	  {
	    const intersection_3d & i = static_cast<const intersection_3d &> (shape_);
	    string shape_ref_1 = solid_name_ + ".intersection.first_ref";
	    string shape_ref_2 = solid_name_ + ".intersection.second_ref";;
	    string pos_ref = solid_name_ + ".intersection.pos_ref";
	    string rot_ref = solid_name_ + ".intersection.rot_ref";;

	    // only stores the solid #2 placement:
	    _writer_.add_position (pos_ref,
				   i.get_shape2 ().get_placement ().get_translation (),
				   _length_unit_);
	    _writer_.add_rotation (rot_ref,
				   i.get_shape2 ().get_placement ().get_rotation (),
				   _angle_unit_);
	    this->_export_gdml_solid (i.get_shape1 ().get_shape (), shape_ref_1);
	    this->_export_gdml_solid (i.get_shape2 ().get_shape (), shape_ref_2);

	    _writer_.add_gdml_intersection (solid_name_,
					    shape_ref_1,
					    shape_ref_2,
					    pos_ref,
					    rot_ref);
	  }
	else
	  {
	    ostringstream message;
	    message << "gdml_export::_export_gdml_solid: "
		    << "Boolean solid type '"
		    << shape_name
		    << "' is not supported yet !";
	    throw runtime_error (message.str ());
	  }
      }
    else
      {
	if (shape_name == "box")
	  {
	    const box & b = static_cast<const box &> (shape_);
	    _writer_.add_box (solid_name_, b, _length_unit_);
	  }
	else if (shape_name == "cylinder")
	  {
	    const cylinder & c = static_cast<const cylinder &> (shape_);
	    _writer_.add_cylinder (solid_name_, c, _length_unit_, _angle_unit_);
	  }
	else if (shape_name == "tube")
	  {
	    const tube & t = static_cast<const tube &> (shape_);
	    _writer_.add_tube (solid_name_, t, _length_unit_, _angle_unit_);
	  }
	else if (shape_name == "sphere")
	  {
	    const sphere & s = static_cast<const sphere &> (shape_);
	    _writer_.add_orb (solid_name_, s, _length_unit_, _angle_unit_);
	  }
	else if (shape_name == "polycone")
	  {
	    const polycone & p = static_cast<const polycone &> (shape_);
	    _writer_.add_polycone (solid_name_, p, _length_unit_, _angle_unit_);
	  }
	else if (shape_name == "polyhedra")
	  {
	    const polyhedra & p = static_cast<const polyhedra &> (shape_);
	    _writer_.add_polyhedra (solid_name_, p, _length_unit_, _angle_unit_);
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
    _solid_refs_.push_back (solid_name_);

    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_solid: Exiting." << endl;
      }
    return;
  }

  void gdml_export::_export_gdml_logical (const logical_volume & lv_)
  {
    bool devel = g_devel;
    //devel = true; // XXX
    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_logical: Entering..." << endl;
      }

    const geomtools::logical_volume & logical = lv_;
    const string & log_name = logical.get_name ();

    if (find (_volumes_refs_.begin (),
              _volumes_refs_.end (),
              log_name) != _volumes_refs_.end ())
      {
	if (devel)
	  {
	    clog << "DEVEL: gdml_export::_export_gdml_logical: "
		 << "Logical '" << log_name << "' is already exported !" << endl;
	  }
	return;
      }

    // export solid shape:
    const i_shape_3d & log_solid = logical.get_shape ();
    ostringstream solid_name_oss;
    solid_name_oss << log_name << i_model::SOLID_SUFFIX;
    string solid_name = solid_name_oss.str ();
    _export_gdml_solid (log_solid, solid_name);

    // prepare volume export
    string material_ref = material::MATERIAL_REF_UNKNOWN;
    string solid_ref = solid_name;
    if (devel) logical.tree_dump (clog, "Logical:", "DEVEL: ");
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
    if (is_auxiliary_supported ())
      {
	logical.parameters ().export_to_string_based_dictionary (auxprops, false);
      }

    if (! skip && logical.get_physicals ().size () == 0)
      {
	_writer_.add_volume (log_name,
			     material_ref,
			     solid_ref,
			     auxprops);
	skip = true;
      }

    // there is a replica children:
    if (! skip && (_support_replica_ && logical.is_replica ()))
      {
	if (devel)
	  {
	    clog << endl << "DEVEL: gdml_export::_export_gdml_logical: "
		 << "**************"
		 << " REPLICA "
		 << "**************"
		 << endl
		 << endl;
	  }
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

	_writer_.add_replica_volume (log_name,
				     material_ref,
				     solid_ref,
				     a_replicavol,
				     _length_unit_,
				     _angle_unit_,
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
		if (multiple) ref_rot_name_oss << "__" << '0' << ".." << (nitems - 1) << "__";
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
		if (multiple) pos_name_oss << "__" << i << "__";
		pos_name_oss << io::POSITION_SUFFIX;
		_writer_.add_position (pos_name_oss.str (),
				       p.get_translation (),
				       _length_unit_);

		if (devel) clog << "DEVEL: gdml_export::_export_gdml_logical: "
				<< "add rotation..." << endl;
		// register the rotation of item 'i':
		//   default rotation name:
		ostringstream rot_name_oss;
		rot_name_oss << log_name << '.' << phys.get_name ();
		if (multiple) rot_name_oss << "__" << i << "__";
		rot_name_oss << io::ROTATION_SUFFIX;
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
			_writer_.add_rotation (rot_name,
					       p.get_rotation (),
					       _angle_unit_);
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
	_writer_.add_volume (log_name,
			     material_ref,
			     solid_ref,
			     physvols,
			     auxprops);
	skip = true;
      } // there are children:

    _volumes_refs_.push_back (log_name);
    if (devel)
      {
	clog << "DEVEL: gdml_export::_export_gdml_logical: Exiting." << endl;
      }
    return;
  }

  void gdml_export::_export_gdml_model (const i_model & model_)
  {
    bool devel = g_devel;
    //devel = true; // XXX
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
