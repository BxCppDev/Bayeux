// -*- mode: c++; -*- 
/* primary_event.cc
 */

#include <genbb_help/primary_event.h>

namespace genbb {

  const std::string primary_event::SERIAL_TAG = "__genbb::primary_event__";

  const std::string & primary_event::get_serial_tag () const
  {
    return primary_event::SERIAL_TAG;
  }

  bool primary_event::is_valid () const
  {
    return time >= 0.;
  }

  void primary_event::reset ()
  {
    time = -1.;
    particles.clear ();
    classification = "";
  }

  double primary_event::get_total_kinetic_energy () const
  {
    double tke = 0.;
    for (particles_col_t::const_iterator i = particles.begin ();
	 i != particles.end ();
	 i++)
      {
	const primary_particle & p = *i;
	tke += p.get_kinetic_energy ();
      }
    return tke;
  }
  
  // ctor:
  primary_event::primary_event ()
  {
    time = -1.;
  }
  
  // dtor:
  primary_event::~primary_event ()
  {
    
  }

  void primary_event::set_time (double t_)
  {
    time = t_;
  }

  double primary_event::get_time () const
  {
    return time;
  }

  const primary_event::particles_col_t & primary_event::get_particles () const
  {
    return particles;
  }

  primary_event::particles_col_t & primary_event::get_particles ()
  {
    return particles;
  }

  void primary_event::add_particle (const primary_particle & p_)
  {
    particles.push_back (p_);
  }

  const std::string & primary_event::get_classification () const
  {
    return classification;
  }

  void primary_event::set_classification (const std::string & c_)
  {
    classification = c_;
  }

  void primary_event::reset_classification ()
  {
    classification = "";
  }

  void primary_event::compute_classification ()
  {
    size_t n_eminus = 0;
    size_t n_eplus = 0;
    size_t n_gamma = 0;
    size_t n_alpha = 0;
    size_t n_others = 0;
    for (particles_col_t::const_iterator i = particles.begin ();
	 i != particles.end ();
	 i++)
      {
	const primary_particle & p = *i;
	if (p.is_electron ()) n_eminus++;
	else if (p.is_positron ()) n_eplus++;
	else if (p.is_gamma ()) n_gamma++;
	else if (p.is_alpha ()) n_alpha++;
	else n_others++;
      }
    std::ostringstream cl_ss;
      
    cl_ss << n_eminus << 'e' 
	  << n_eplus << 'p' 
	  << n_gamma << 'g' 
	  << n_alpha << 'a' 
	  << n_others << 'X'; 
    set_classification (cl_ss.str ());
  }

  void primary_event::dump (std::ostream & out_,
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
  
} // end of namespace genbb

// end of primary_event.cc
