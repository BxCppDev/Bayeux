// -*- mode: c++ ; -*- 
/* smart_id_locator.cc
 */

#include <geomtools/smart_id_locator.h>

namespace geomtools {

  using namespace std;

  void smart_id_locator::set_debug (bool d_)
  {
    __debug = d_;
  }

  bool smart_id_locator::is_debug () const
  {
    return __debug;
  }

  const list<const geom_info *> & smart_id_locator::get_ginfos () const
  {
    return __ginfos;
  }

  void smart_id_locator::_construct ()
  {
    for (geom_info_dict_t::const_iterator i 
	   = __gmap->get_geom_infos ().begin ();
	 i != __gmap->get_geom_infos ().end ();
	 i++)
      {
	uint32_t gtype = i->first.get_type ();
	bool valid = false;

	// search matching type IDs:
	if (__type != geom_id::INVALID_TYPE)
	  {
	    if (gtype == __type)
	      {
		valid = true;
	      }
	  }
	else if (__idsel.is_initialized ())
	  {
	    if (__idsel.match (i->first))
	      {
		valid = true;
	      }
	  }
	else
	  {
	    ostringstream message;
	    message << "smart_id_locator::_construct: "
		    << "Missing criterion for building the smart list of IDs !";
	    throw runtime_error (message.str ());	    
	  }

	if (valid)
	  {
	    const geom_info * ginfo = &(i->second);
	    __ginfos.push_back (ginfo);
	  }
      }

    if (__type != geom_id::INVALID_TYPE)
      {
	if (is_debug ())
	  {
	    clog << datatools::utils::io::debug 
		 << "smart_id_locator::_construct: "
		 << "Number of entries with type (" << __type << ") = "
		 << __ginfos.size ()
		 << endl;
	  }
      }
    if (__idsel.is_initialized ())
      {
	if (is_debug ())
	  {
	    clog << datatools::utils::io::debug 
		 << "smart_id_locator::_construct: "
		 << "Number of entries selected by the ID selector = "
		 << __ginfos.size ()
		 << endl;
	  }
      }
    return;
  }

  void smart_id_locator::set_gmap (const geom_map & gmap_)
  {
    if (is_initialized ())
      {
	ostringstream message;
	message << "smart_id_locator::set_gmap: Operation prohibited ! "
		<< "Locator is already initialized !";
	throw runtime_error (message.str ());
      }	
    __gmap = &gmap_;
  }

  bool smart_id_locator::is_initialized () const
  {
    return __mode != MODE_INVALID;
  }
  
  void smart_id_locator::reset ()
  {
    __mode = MODE_INVALID;
    __type = geom_id::INVALID_TYPE;
    __idsel.reset ();
    __last_found = 0;
    __ginfos.clear ();
  }

  void smart_id_locator::initialize (const string & rules_, 
				     int mode_)
  {
    if (is_initialized ())
      {
	ostringstream message;
	message << "smart_id_locator::initialize: "
		<< "Locator is already initialized !";
	throw runtime_error (message.str ());
      }
    if (__gmap == 0)
      {
	ostringstream message;
	message << "smart_id_locator::initialize: "
		<< "Missing geoemtry map ! Use the 'set_gmap' method before !";
	throw runtime_error (message.str ());	
      }
    __idsel.set_id_mgr (__gmap->get_id_manager ());
    try
      {
	__idsel.initialize (rules_);
      }
    catch (exception & x)
      {
	ostringstream message;
	message << "smart_id_locator::initialize: "
		<< "ID selector failed: " << x.what ();
	throw runtime_error (message.str ());
      }
    
    __mode = mode_;
    __last_found = 0;

    _construct ();
  
    return;
  }

  void smart_id_locator::initialize (uint32_t type_, 
				     int mode_)
  {
    if (is_initialized ())
      {
	ostringstream message;
	message << "smart_id_locator::initialize: "
		<< "Locator is already initialized !";
	throw runtime_error (message.str ());
      }

    if (type_ == geom_id::INVALID_TYPE)
      {
	ostringstream message;
	message << "smart_id_locator::_construct: "
		<< "Invalid geometry info type (" << type_ << ")";
	throw runtime_error (message.str ());
      }
    
    __mode = mode_;
    __type = type_;
    __last_found = 0;

    _construct ();
  
    return;
  }

  // ctor:
  smart_id_locator::smart_id_locator ()
  {
    __debug= false;
    __gmap = 0;
    __mode = MODE_INVALID;
    __type = geom_id::INVALID_TYPE;
    __last_found = 0;
    return;
  }

  // ctor:
  smart_id_locator::smart_id_locator (const geom_map & gmap_)
  {
    __debug= false;
    __gmap = 0;
    __mode = MODE_INVALID;
    __type = geom_id::INVALID_TYPE;
    __last_found = 0;

    set_gmap (gmap_);
    return;
  }
   
  // ctor:
  smart_id_locator::smart_id_locator (const geom_map & gmap_, 
				      uint32_t type_,
				      int mode_)
  {
    __mode = MODE_INVALID;
    __type = geom_id::INVALID_TYPE;
    __last_found = 0;

    set_gmap (gmap_);
    initialize (type_, mode_);
    return;
  }
   
  // ctor:
  smart_id_locator::smart_id_locator (const geom_map & gmap_, 
				      const string & selection_rules_,
				      int mode_)
  {
    __mode = MODE_INVALID;
    __type = geom_id::INVALID_TYPE;
    __last_found = 0;

    set_gmap (gmap_);
    initialize (selection_rules_, mode_);
    return;
  }

  // dtor:
  smart_id_locator::~smart_id_locator ()
  {
    if (is_initialized ()) 
      {
	reset ();
      }
  }

  bool smart_id_locator::validate_id (const geom_id & id_) const
  {
    return __gmap->validate_id (id_);
  }

  const geom_info & smart_id_locator::get_geom_info (const geom_id & id_) const
  {
    return __gmap->get_geom_info (id_);
  }
  
  const geom_id & smart_id_locator::get_geom_id (const vector_3d & world_position_, 
						 int type_, 
						 double tolerance_) const
  {

    if (__mode & MODE_LAST)
      {
	// first search if one matched the last found object:

	if (__last_found != 0)
	  {
	    bool located = false;
	    located = geom_map::check_inside (*__last_found,
					      world_position_,
					      tolerance_);
	    if (located)
	      {
		if (is_debug ())
		  {
		    clog << datatools::utils::io::debug 
			 << "smart_id_locator::get_geom_id: "
			 << "Last found entry match ("
			 << __last_found->get_id () << ")"
			 << endl;
		  }
		return __last_found->get_id ();
	      }
	    
	  }
      }

    for (list<const geom_info *>::const_iterator i
	   = __ginfos.begin ();
	 i != __ginfos.end ();
	 i++)
      {
	const geom_info * ginfo = *i;
	if (__last_found != 0)
	  {
	    // no need to check twice the last found location:
	    if (ginfo == __last_found)
	      {
		continue;
	      }
	  }
	bool located = false;
	located = geom_map::check_inside (*ginfo,
					  world_position_,
					  tolerance_);
	if (located)
	  {
	    if (__mode & MODE_LAST)
	      {
		smart_id_locator * non_const_this = const_cast<smart_id_locator *> (this);
		non_const_this->__last_found = ginfo;
	      }
	    if (is_debug ())
	      {
		clog << datatools::utils::io::debug 
		     << "smart_id_locator::get_geom_id: "
		     << "New entry match ("
		     << ginfo->get_id () << ")"
		     << endl;
	      }
	    return ginfo->get_id ();
	  }
     }

    // 
    return __gmap->get_invalid_geom_id ();
  }

} // end of namespace geomtools

// end of smart_id_locator.cc
