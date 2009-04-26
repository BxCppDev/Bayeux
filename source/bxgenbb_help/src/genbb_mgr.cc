// -*- mode: c++; -*- 
/* genbb_mgr.cc
 */

#include <genbb_help/genbb_mgr.h>

namespace genbb {

  const std::string primary_particle::SERIAL_TAG = "__genbb::primary_particle__";

  void primary_particle::reset ()
  {
    type = UNDEF;
    time = 0.0;
    geomtools::invalidate (momentum);
  }

  primary_particle::primary_particle ()
  {
    reset ();
  }

  primary_particle::primary_particle (int32_t type_, 
				      double time_, 
				      const geomtools::vector_3d & mom_)
  {
    type = type_;
    time = time_;
    momentum = mom_;
  }

  const std::string & primary_particle::get_serial_tag () const
  {
    return primary_particle::SERIAL_TAG;
  }

  void 
  primary_particle::dump (std::ostream & out_,
			  const std::string & indent_) const
  {
    std::string indent = indent_;
    
    out_ << indent << "genbb::primary_particle:" << std::endl;
    if (is_valid ())
      {
	out_ << indent<< "|-- type: " << type << " (" << get_label (type) << ')'
	     << std::endl;
	out_ << indent<< "|-- time: " << time / CLHEP::ns 
	     << " ns" << std::endl;
	out_ << indent<< "`-- momentum: " 
	     << momentum / CLHEP::MeV 
	     << " MeV" << std::endl;
      }
    else
      {
	out_ << indent<< "`-- type: " << "<invalid>" << std::endl;
      }
  }
  
  std::string 
  primary_particle::get_label (int type_)
  {
    switch (type_)
      {
      case 1: return "gamma";
      case 2: return "e+";
      case 3: return "e-";
      case 47: return "alpha";
      }
    return "<unknown>";
  }
 
  /************************************************************/

  const std::string primary_event::SERIAL_TAG = "__genbb::primary_event__";

  const std::string & primary_event::get_serial_tag () const
  {
    return primary_event::SERIAL_TAG;
  }

  void 
  primary_event::dump (std::ostream & out_,
		       const std::string & indent_) const
  {
    std::string indent = indent_;
    out_ << indent << "genbb::primary_event:" << std::endl;
    if (is_valid ())
      {
	out_ << indent << "|-- time: " << time << std::endl;
	out_ << indent << "|-- #particles: " << particles.size () << std::endl;
	for (particles_col_t::const_iterator it = particles.begin ();
	     it != particles.end ();
	     it++)
	  {
	    it->dump (out_, (indent + "|   ")); 
	  }
	out_ << indent << "`-- classification: " << get_classification () << std::endl;
      }
    else
      {
	out_ << indent << "`-- status: " << "<invalid>" << std::endl;
      }
  }
 
  /************************************************************/

  // ctor/dtor:
  genbb_mgr::genbb_mgr (int format_)
  {
    __debug = false;
    __initialized = false;
    __in = 0;
    __format = FORMAT_GENBB;
    set_format (format_);
  }
  
  genbb_mgr::~genbb_mgr ()
  {
    reset ();
  }

  void 
  genbb_mgr::set_format (int format_)
  {
    if (__initialized)
      {
	throw std::runtime_error ("genbb_mgr::set: Operation not allowed! Manager is locked!");
      }
    if ((format_ != FORMAT_GENBB) && (format_ != FORMAT_BOOST))
      {
	throw std::runtime_error ("genbb_mgr::set: Invalid format!");
      }
    /*
    if (format_ == FORMAT_BOOST)
      {
	throw std::runtime_error ("genbb_mgr::set: Boost format is not supported yet!");
      }
    */
    __format = format_;
  }

  void 
  genbb_mgr::set (const std::string & filename_)
  {
    if (__initialized)
      {
	throw std::runtime_error ("genbb_mgr::set: Operation not allowed! Manager is locked!");
      }
    __filenames.push_back (filename_);
  }

  bool 
  genbb_mgr::has_next ()
  {
    return __current.is_valid ();
  }
  
  void 
  genbb_mgr::load_next (primary_event & event_, 
			bool compute_classification_)
  {
    event_ = __current;
    if (compute_classification_) event_.compute_classification ();
    __load_next ();
  }

  void 
  genbb_mgr::__load_next_boost ()
  {
    /*
      throw std::runtime_error ("genbb_mgr::__load_next_boost: Not implemented yet!");  
    */

    if (! __reader.is_initialized ())
      {
	std::string filename;

	if (__filenames.size () == 0)
	  { 
	    if (is_debug ()) std::clog << "genbb_mgr::__load_next_boost: no more filenames!" << std::endl;
	    return;
	  }

	filename = __filenames.front ();
	__filenames.pop_front ();
	if (filename.empty ()) 
	  {
	    if (is_debug ()) std::clog << "DEVEL: genbb_mgr::__load_next_boost: filename = '" << filename << "'" << std::endl;
	    return;
	  }

	__reader.init (filename, 
		       datatools::serialization::using_multi_archives);

      }

    if (! __reader.is_initialized ())
      {
	 std::runtime_error ("genbb_mgr::__load_next_boost: Reader is not initialized!");  
      }

    if (! __reader.has_record_tag ())
      {
	 std::runtime_error ("genbb_mgr::__load_next_boost: Reader has no data!");  
      }
    if (__reader.record_tag_is (primary_event::SERIAL_TAG)) 
      {
	__reader.load (__current);
      }
    if (! __reader.has_record_tag ())
      {
	__reader.reset ();
      }
  }

  void 
  genbb_mgr::__load_next ()
  {
    __current.reset ();
    if (__format == FORMAT_GENBB)
      {
	__load_next_genbb ();
      }
    if (__format == FORMAT_BOOST)
      {
	__load_next_boost ();
      }
  }
    
  void 
  genbb_mgr::__load_next_genbb ()
  {
    if (__in == 0)
      {
	if (is_debug ()) std::clog << "genbb_mgr::__load_next: no input stream!" << std::endl;
	std::string filename;

	if (__filenames.size () == 0)
	  { 
	    if (is_debug ()) std::clog << "genbb_mgr::__load_next: no more filenames!" << std::endl;
	    return;
	  }

	filename = __filenames.front ();
	__filenames.pop_front ();
	if (filename.empty ()) 
	  {
	    if (is_debug ()) std::clog << "DEVEL: genbb_mgr::__load_next: filename = '" << filename << "'" << std::endl;
	    return;
	  }

	if (is_debug ()) std::clog << "DEVEL: genbb_mgr::__load_next: filename = '" << filename << "'" << std::endl;
	__fin.close ();
	__fin.open (filename.c_str ());
	if (! __fin)
	  {
	    std::ostringstream message;
	    message << "genbb_mgr::__load_next: cannot open file '"
		    << filename << "'!";
	    throw std::runtime_error (message.str ());
	  }
	__in = &__fin;
      }
  
    if (is_debug ()) std::clog << "DEVEL: genbb_mgr::__load_next: __in = " << __in << std::endl;

    if (! *__in)
      {
	std::ostringstream message;
	message << "genbb_mgr::__load_next: cannot load event!";
	throw std::runtime_error (message.str ());
      }
    int evnum;
    double time;
    int npart;
    *__in >> std::ws >> evnum >> time >> npart;
    if (! *__in)
      {
	std::ostringstream message;
	message << "genbb_mgr::__load_next: format error!";
	throw std::runtime_error (message.str ());
      }
    __current.time = time * CLHEP::second;
    for (int i = 0; i < npart; i++)
      {
	primary_particle pp;
	double x, y ,z;
	*__in >> std::ws >> pp.type 
	      >> x >> y >> z >> pp.time; 
	if (! *__in)
	  {
	    std::ostringstream message;
	    message << "genbb_mgr::__load_next: format error!";
	    __fin.close ();
	    __in = 0;
	    throw std::runtime_error (message.str ());
	  }
	pp.time *= CLHEP::second;
	pp.momentum.set (x, y, z);
	pp.momentum *= CLHEP::MeV;
	__current.particles.push_back (pp);
      }
    *__in >> std::ws;
    if (__fin.eof ())
      {
	__fin.close ();
	__in = 0;
      }
  }

  void 
  genbb_mgr::dump (std::ostream & out_) const
  {
    out_ << "genbb_mgr::dump: " << std::endl;
    out_ << "|-- debug : " << __debug << std::endl;
    out_ << "|-- initialized : " << __initialized << std::endl;
    out_ << "|-- format : " << (__format == FORMAT_GENBB? "GENBB": "Boost")<< std::endl;
    for (std::list<std::string>::const_iterator it = __filenames.begin ();
	 it != __filenames.end ();
	 it++)
      {
	out_ << "|-- filename : '" << *it << "'" << std::endl; 
      }
    if (__format == FORMAT_GENBB)
      {
	out_ << "|-- *in : " << std::hex << __in 
	     << std::dec << std::endl;
      }
    if (__format == FORMAT_BOOST)
      {
	out_ << "|-- Boost reader : " 
	     << (__reader.is_initialized ()? "Yes": "No") << std::endl;
      }
    out_ << "`-- current event: " << std::endl;
    __current.dump (out_, "    "); 

  }

  void 
  genbb_mgr::init ()
  {
    if (__initialized) return;

    __load_next ();
    __initialized = true;
  }

  void
  genbb_mgr::reset ()
  {
    if (! __initialized) return;
    __filenames.clear ();
    if (__format == FORMAT_GENBB)
      {
	if (__in != 0)
	  {
	    __in = 0; 
	    __fin.close ();
	  }
      }
    if (__format == FORMAT_BOOST)
      {
	if (__reader.is_initialized ())
	  {
	    __reader.reset ();
	  }
      }
    __format = FORMAT_GENBB;

    __initialized = false;
  }
  
} // end of namespace genbb

// end of genbb_mgr.cc
