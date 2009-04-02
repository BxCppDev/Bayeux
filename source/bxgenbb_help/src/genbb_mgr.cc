// -*- mode: c++; -*- 
/* genbb_mgr.cc
 */

#include <genbb_help/genbb_mgr.h>

namespace genbb {

  void 
  primary_particle::dump (std::ostream & out_,
			  const std::string & indent_) const
  {
    std::string indent = indent_;
    
    out_ << indent << "genbb::primary_particle:" << std::endl;
    out_ << indent<< "|-- type: " << type << " (" << get_label (type) << ')'
	 << std::endl;
    out_ << indent<< "|-- time: " << time / CLHEP::ns 
	 << " ns" << std::endl;
    out_ << indent<< "`-- momentum: " 
	 << momentum / CLHEP::MeV 
	 << " MeV" << std::endl;
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
  //#include <unistd.h>

  void 
  primary_event::dump (std::ostream & out_,
		       const std::string & indent_) const
  {
    std::string indent = indent_;
    out_ << indent << "genbb::primary_event:" << std::endl;
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
 
  /************************************************************/

  // ctor/dtor:
  genbb_mgr::genbb_mgr ()
  {
    __debug = false;
    __initialized = false;
    __in = 0;
  }
  
  genbb_mgr::~genbb_mgr ()
  {
    reset ();
  }

  void 
  genbb_mgr::set (const std::string & filename_)
  {
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
  genbb_mgr::__load_next ()
  {
    __current.reset ();
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
    for (std::list<std::string>::const_iterator it = __filenames.begin ();
	 it != __filenames.end ();
	 it++)
      {
	out_ << "|-- filename : '" << *it << "'" << std::endl; 
      }
    out_ << "|-- *in : " << std::hex << __in 
	 << std::dec << std::endl;
    out_ << "`-- current : " << std::endl;
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

    

    __initialized = false;
  }
  
} // end of namespace genbb

// end of genbb_mgr.cc
