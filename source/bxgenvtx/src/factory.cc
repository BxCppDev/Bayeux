// -*- mode: c++ ; -*- 
/* factory.cc
 */

#include <genvtx/factory.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>

namespace genvtx {
 
  using namespace std;

  bool factory::g_devel = false;
  
  bool factory::is_debug () const
  {
    return __debug;
  }
  
  void factory::set_debug (bool new_value_)
  {
    __debug = new_value_;
    return;
  }

  void factory::do_register (const vg_creator_t & creator_,
			     const string & vg_id_)
  {
    bool abort_on_error = false;
    vertex_generator_creator_dict_t::const_iterator found = __creators.find (vg_id_);
    if (found != __creators.end ())
      {
	ostringstream message;
	message << "genvtx::factory::do_register: " 
		<< "Vertex generator ID '" << vg_id_ << "' is already used "
		<< "within the factory dictionary !";
	if (abort_on_error)
	  {
	    throw runtime_error (message.str ());
	  }
	else 
	  {
	    message << ' ' << "Overwriting current value...";
	    clog << "WARNING: " << message.str () << endl;
	  }
      }
    string vertex_generator_id = vg_id_;
    if (vertex_generator_id.empty ())
      {
	ostringstream message;
	message << "genvtx::factory::do_register: " 
		<< "Missing vertex generator ID !";
	throw runtime_error (message.str ());
      }
    __creators [vertex_generator_id] = creator_;
    return;
  }

  // ctor:
  factory::factory (bool preload_)
  {
    __debug = false;
    bool devel = g_devel;
    //devel = true;
    if (devel)
      {
	clog << "DEVEL: genvtx::factory::ctor: Initial vertex generators..." << endl;
	dump_vertex_generators (clog);
      }
    if (preload_)
      {
	clog << "NOTICE: genvtx::factory::ctor: " 
	     << "preload the global vertex generator dictionary!" << endl;
	// preload local vertex generator dictionary with global vertex generator dictionary:
	__creators = i_vertex_generator::get_vertex_generator_db ().get_dict ();
	if (devel)
	  {
	    clog << "DEVEL: genvtx::factory::ctor: " 
		 << "Available vertex generators after preload:" << endl;
	    dump_vertex_generators (clog);
	  }
      }
    return;
  }
  
  // dtor:
  factory::~factory ()
  {
    __creators.clear ();
    return;
  }
  
  i_vertex_generator * factory::create_vertex_generator (const string & vg_id_, 
							 const datatools::utils::properties & vg_configuration_,
							 void * user_)
  {
    using namespace std;
    bool devel = g_devel;
    //devel = true;

    if (devel) 
      {
	cerr << "DEVEL: genvtx::factory::create_vertex_generator: " 
	     << "VERTEX GENERATOR ID == '" << vg_id_ << "' " << endl;
      }
    // search for the vertex generator's label in the creators dictionary:
    vertex_generator_creator_dict_t::iterator found = __creators.find (vg_id_);
    if (found != __creators.end ())
      {
	vg_creator_t a_creator = found->second;
	i_vertex_generator * vg = a_creator (vg_configuration_, user_);
	return vg;
      }
    else
      {
	if (devel) 
	  {
	    cerr << "DEVEL: genvtx::factory::create_vertex_generator: "
		 << "NULL VERTEX GENERATOR !" << endl;
	  }
      }
    /*
      else
      {
      i_vertex_generator * a_vg = base_factory::create_vertex_generator (vertex_generator_id_,
      vg_configuration_,user_);
      if (a_vg != 0) 
      {
      return (a_vg);
      }
      }
    */

    return (0);
  }

  void factory::dump_vertex_generators (ostream & out_) const
  {
    out_ << "genvtx::factory::dump_vertex_generators: " << endl
	 << "List of vertex generator creators in 'factory::__creators': ";
    size_t sz = __creators.size ();
    out_ << sz << " element(s)" << endl;
    size_t count = 0; 
    for (vertex_generator_creator_dict_t::const_iterator it = __creators.begin ();
	 it != __creators.end ();
	 it++) 
      {
	count++;
	if (count == sz) out_ << "`-- "; 
	else out_ << "|-- ";
	out_ << it->first << ": " << hex << (void *) it->second 
	     << dec << endl;
      }
    out_ << "end." << endl;
    return;
  }
  
} // end of namespace genvtx

// end of factory.cc
