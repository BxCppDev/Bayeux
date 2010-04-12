// -*- mode: c++; -*- 
/* genbb_mgr.cc
 */

#include <genbb_help/genbb_mgr.h>

namespace genbb {

  // ctor:
  genbb_mgr::genbb_mgr (int format_)
  {
    __debug = false;
    __initialized = false;
    __in = 0;
    __format = FORMAT_GENBB;
    set_format (format_);
  }
  
  // dtor:
  genbb_mgr::~genbb_mgr ()
  {
    reset ();
  }

  void genbb_mgr::set_format (int format_)
  {
    if (__initialized)
      {
	throw std::runtime_error ("genbb_mgr::set: Operation not allowed! Manager is locked!");
      }
    if ((format_ != FORMAT_GENBB) && (format_ != FORMAT_BOOST))
      {
	throw std::runtime_error ("genbb_mgr::set: Invalid format!");
      }
    __format = format_;
  }

  void genbb_mgr::set (const std::string & filename_)
  {
    if (__initialized)
      {
	throw std::runtime_error ("genbb_mgr::set: Operation not allowed! Manager is locked!");
      }
    __filenames.push_back (filename_);
  }

  bool genbb_mgr::has_next ()
  {
    return __current.is_valid ();
  }
  
  void genbb_mgr::load_next (primary_event & event_, 
			     bool compute_classification_)
  {
    event_ = __current;
    if (compute_classification_) event_.compute_classification ();
    __load_next ();
  }

  void genbb_mgr::__load_next_boost ()
  {
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

  void genbb_mgr::__load_next ()
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
    
  void genbb_mgr::__load_next_genbb ()
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
    double part_time = 0.0;
    for (int i = 0; i < npart; i++)
      {
	primary_particle pp;
	int part_type;
	double x, y ,z, time_shift;
	// 2009-07-14 FM: Vladimir Tretyak email about particles' time shifts: 
	//*__in >> std::ws >> pp.type >> x >> y >> z >> pp.time; 
	*__in >> std::ws >> part_type >> x >> y >> z >> time_shift;
	part_time += time_shift; 
	pp.set_type (part_type);
	pp.set_time (part_time * CLHEP::second); // GENBB unit is s
	
	if (! *__in)
	  {
	    std::ostringstream message;
	    message << "genbb_mgr::__load_next: format error!";
	    __fin.close ();
	    __in = 0;
	    throw std::runtime_error (message.str ());
	  }
	geomtools::vector_3d p (x, y, z);
	p *= CLHEP::MeV; // GENBB unit is MeV/c
	pp.set_momentum (p);
	__current.add_particle (pp);
      }
    *__in >> std::ws;
    if (__fin.eof ())
      {
	__fin.close ();
	__in = 0;
      }
  }

  void genbb_mgr::dump (std::ostream & out_) const
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

  void genbb_mgr::init ()
  {
    if (__initialized) return;
    __load_next ();
    __initialized = true;
  }

  void genbb_mgr::reset ()
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
