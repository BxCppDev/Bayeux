// -*- mode: c++; -*- 
/* primary_particle.cc
 */

#include <genbb_help/primary_particle.h>

namespace genbb {

  const std::string primary_particle::SERIAL_TAG = "__genbb::primary_particle__";

  bool primary_particle::is_valid () const
  {
    return type != UNDEF;
  }
  
  int primary_particle::get_type () const
  {
    return type;
  }
  
  void  primary_particle::set_type (int type_)
  {
    type = type_;
  }
  
  double primary_particle::get_time () const
  {
    return time;
  }
  
  void  primary_particle::set_time (double time_)
  {
    time = time_;
  }
  
  void primary_particle::set_momentum (const geomtools::vector_3d & m_) 
  {
    momentum = m_;
  }
  
  const geomtools::vector_3d & primary_particle::get_momentum () const
  {
    return momentum;
  }
  
  void primary_particle::reset ()
  {
    type = UNDEF;
    time = 0.0;
    geomtools::invalidate (momentum);
  }

  // ctor:
  primary_particle::primary_particle ()
  {
    reset ();
  }

  // ctor:
  primary_particle::primary_particle (int32_t type_, 
				      double time_, 
				      const geomtools::vector_3d & mom_)
  {
    type = type_;
    time = time_;
    momentum = mom_;
  }

  // dtor:
  primary_particle::~primary_particle ()
  {
  }

  const std::string & primary_particle::get_serial_tag () const
  {
    return primary_particle::SERIAL_TAG;
  }

  double primary_particle::get_charge () const
  {
    if (is_positron ()) return +1;
    if (is_electron ()) return -1;
    if (is_alpha ()) return +2;
    if (is_gamma ()) return 0.0;

    throw std::runtime_error ("primary_particle::get_charge: Unknown particle !");
  }

  double primary_particle::get_mass () const
  {
    if (is_positron () || is_electron ())
      {
	return CLHEP::electron_mass_c2; 
      }
    if (is_alpha ()) 
      {
	return 3.727417 * CLHEP::GeV;
      }
    if (is_gamma ()) return 0.0;

    throw std::runtime_error ("primary_particle::get_mass: Unknown particle !");
  }

  double primary_particle::get_beta () const
  {
    return momentum.mag () / get_total_energy ();
  }

  double primary_particle::get_kinetic_energy () const
  {
    double mass = get_mass ();
    double kinetic_energy 
      = std::sqrt (momentum.mag () * momentum.mag () + mass * mass) - mass;
    return kinetic_energy;
  }

  double primary_particle::get_total_energy () const
  {
    return get_kinetic_energy () + get_mass ();
  }

  void primary_particle::dump (std::ostream & out_, const std::string & indent_) const
  {
    std::string indent = indent_;
    
    out_ << indent << "genbb::primary_particle:" << std::endl;
    if (is_valid ())
      {
	double mass = get_mass ();
	double energy = get_kinetic_energy ();

	out_ << indent << "|-- type: " << type << " (" << get_label (type) << ')'
	     << std::endl;
	std::ostringstream time_oss;
	time_oss.precision (15);
	time_oss << time / CLHEP::ns;
	out_ << indent << "|-- time: " << time_oss.str ()
	     << " ns" << std::endl;
	out_ << indent << "|-- kinetic energy: " << energy / CLHEP::MeV 
	     << " MeV" << std::endl;
	out_ << indent << "`-- momentum: " 
	     << momentum / CLHEP::MeV 
	     << " MeV" << std::endl;
      }
    else
      {
	out_ << indent<< "`-- type: " << "<invalid>" << std::endl;
      }
  }
  
  std::string primary_particle::get_label (int type_)
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
 
} // end of namespace genbb

// end of primary_particle.cc
