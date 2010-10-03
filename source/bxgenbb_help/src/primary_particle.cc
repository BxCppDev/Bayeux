// -*- mode: c++; -*- 
/* primary_particle.cc
 */

#include <genbb_help/primary_particle.h>

namespace genbb {

  using namespace std;
  
  const string primary_particle::SERIAL_TAG = "__genbb::primary_particle__";

  bool primary_particle::is_valid () const
  {
    return type != UNDEF;
  }

  const string & primary_particle::get_particle_label () const
  {
    return particle_label;
  }

  void primary_particle::set_particle_label (const string & pl_)
  {
    if (type == UNDEF)
      {
	particle_label = pl_;
      }
    return;
  }
  
  int primary_particle::get_type () const
  {
    return type;
  }
  
  void  primary_particle::set_type (int type_)
  {
    type = type_;
    particle_label = get_particle_label_from_type (type);
    return;
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

  const string & primary_particle::get_serial_tag () const
  {
    return primary_particle::SERIAL_TAG;
  }

  double primary_particle::get_charge () const
  {
    if (is_positron ()) return +1;
    if (is_electron ()) return -1;
    if (is_alpha ()) return +2;
    if (is_gamma ()) return 0.0;

    throw runtime_error ("primary_particle::get_charge: Unknown particle !");
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

    if (is_gamma ()) 
      {
	return 0.0;
      }

    if (type == NEUTRON) 
      {
	return 939.565560 * CLHEP::MeV; 
      }

    if (type == PROTON) 
      {
	return 938.272013 * CLHEP::MeV; 
      }

    if (type == MUON_PLUS || type == MUON_MINUS)
      {
	return 105.658369 * CLHEP::MeV; 
      } 

    if (type == NEUTRINO) 
      {
	return 0.0 * CLHEP::MeV; 
      }
    double a_mass;
    datatools::utils::invalidate (a_mass);
    return a_mass;
    //throw runtime_error ("primary_particle::get_mass: Unknown particle !");
  }

  double primary_particle::get_beta () const
  {
    return momentum.mag () / get_total_energy ();
  }

  double primary_particle::get_kinetic_energy () const
  {
    double mass = get_mass ();
    double kinetic_energy 
      = sqrt (momentum.mag () * momentum.mag () + mass * mass) - mass;
    return kinetic_energy;
  }

  double primary_particle::get_total_energy () const
  {
    return get_kinetic_energy () + get_mass ();
  }

  void primary_particle::dump (ostream & out_, const string & indent_) const
  {
    string indent = indent_;
    
    out_ << indent << "genbb::primary_particle:" << endl;
    double mass = get_mass ();
    double energy = get_kinetic_energy ();
    
    out_ << indent << "|-- Type: " << type;
    if (type != UNDEF) 
      {
	out_ << " (" << get_particle_label_from_type (type) << ')';
      }
    out_ << endl;
    out_ << indent << "|-- Particle label: '" << particle_label << "'"
	 << endl;
    ostringstream time_oss;
    time_oss.precision (15);
    time_oss << time / CLHEP::ns;
    out_ << indent << "|-- Time: " << time_oss.str ()
	 << " ns" << endl;
    out_ << indent << "|-- Kinetic energy: " << energy / CLHEP::MeV 
	 << " MeV" << endl;
    out_ << indent << "`-- Momentum: " 
	 << momentum / CLHEP::MeV 
	 << " MeV" << endl;
    return;
  }
  
  string primary_particle::get_label (int type_)
  {
    switch (type_)
      {
      case 1: return "gamma";
      case 2: return "e+";
      case 3: return "e-";
      case 4: return "neutrino";
      case 5: return "mu+";
      case 6: return "mu-";
      case 7: return "pi0";
      case 8: return "pi+";
      case 9: return "pi-";
      case 13: return "neutron";
      case 14: return "proton";
      case 45: return "deuteron";
      case 46: return "tritium";
      case 47: return "alpha";
      case 49: return "he3";
      }
    return "<unknown>";
  }

  string primary_particle::get_particle_label_from_type (int type_)
  {
    return primary_particle::get_label (type_);
  }
    
  int primary_particle::get_particle_type_from_label (const string & label_)
  {
    if (label_ == "electron" || label_ == "e-")
      {
	return primary_particle::ELECTRON;
      }
    else if (label_ == "positron" || label_ == "e+")
      {
	return primary_particle::POSITRON;
      }
    else if (label_ == "gamma")
      {
	return primary_particle::GAMMA;
      }
    else if (label_ == "neutrino")
      {
	return primary_particle::NEUTRINO;
      }
    else if (label_ == "mu+")
      {
	return primary_particle::MUON_PLUS;
      }
    else if (label_ == "mu-")
      {
	return primary_particle::MUON_MINUS;
      }
    else if (label_ == "pi+")
      {
	return primary_particle::PION_PLUS;
      }
    else if (label_ == "pi-")
      {
	return primary_particle::PION_MINUS;
      }
    else if (label_ == "pi0")
      {
	return primary_particle::PION_0;
      }
    else if (label_ == "neutron")
      {
	return primary_particle::NEUTRON;
      }
    else if (label_ == "proton")
      {
	return primary_particle::PROTON;
      }
    else if (label_ == "deuteron")
      {
	return primary_particle::DEUTERON;
      }
    else if (label_ == "tritium")
      {
	return primary_particle::TRITIUM;
      }
    else if (label_ == "alpha")
      {
	return primary_particle::ALPHA;
      }
    else if (label_ == "he3")
      {
	return primary_particle::HE3;
      }
    else return primary_particle::UNDEF;
  }
 
} // end of namespace genbb

// end of primary_particle.cc
