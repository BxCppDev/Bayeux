// -*- mode: c++; -*-
// primary_particle.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 *
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <genbb_help/primary_particle.h>

#include <genbb_help/genbb_help_config.h>

// Special backward compatibility support for serialization :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(genbb::primary_particle,"genbb::primary_particle")
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(genbb::primary_particle,"__genbb::primary_particle__")

namespace genbb {

  using namespace std;

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(primary_particle,"genbb::primary_particle")

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

  bool primary_particle::is_gamma () const
  {
    return get_type () == GAMMA;
  }
  
  bool primary_particle::is_positron () const
  {
    return get_type () == POSITRON;
  }
  
  bool primary_particle::is_electron () const
  {
    return get_type () == ELECTRON;
  }
  
  bool primary_particle::is_alpha () const
  {
    return get_type () == ALPHA;
  }
  
  bool primary_particle::is_neutron () const
  {
    return get_type () == NEUTRON;
  }
    
  bool primary_particle::is_muon_plus () const
  {
    return get_type () == MUON_PLUS;
  }

  bool primary_particle::is_muon_minus () const
  {
    return get_type () == MUON_MINUS;
  }

  double primary_particle::get_time () const
  {
    return time;
  }

  void  primary_particle::set_time (double time_)
  {
    time = time_;
    return;
  }

  void primary_particle::set_momentum (const geomtools::vector_3d & v_)
  {
    momentum = v_;
    return;
  }

  const geomtools::vector_3d & primary_particle::get_momentum () const
  {
    return momentum;
  }

  void primary_particle::set_vertex (const geomtools::vector_3d & m_)
  {
    vertex = m_;
    return;
  }

  const geomtools::vector_3d & primary_particle::get_vertex () const
  {
    return vertex;
  }

  bool primary_particle::has_vertex () const
  {
    return geomtools::is_valid (vertex);
  }

  void primary_particle::invalidate_vertex ()
  {
    geomtools::invalidate_vector_3d (vertex);
    return;
  }

  void primary_particle::reset ()
  {
    type = UNDEF;
    time = 0.0;
    geomtools::invalidate (momentum);
    this->invalidate_vertex ();
    return;
  }

  // ctor:
  primary_particle::primary_particle ()
  {
    reset ();
    return;
  }

  // ctor:
  primary_particle::primary_particle (int32_t type_,
                                      double time_,
                                      const geomtools::vector_3d & mom_)
  {
    type = type_;
    time = time_;
    momentum = mom_;
    return;
  }

  // dtor:
  primary_particle::~primary_particle ()
  {
    return;
  }


  double primary_particle::get_charge () const
  {
    double c = 0.0;
    if (is_gamma ()) c = 0.0;
    else if (is_positron ()) c = +1.0;
    else if (is_electron ()) c = -1.0;
    else if (get_type () == NEUTRINO) c = 0.0;
    else if (is_muon_plus ()) c = +1.0;
    else if (is_muon_minus ()) c = -1.0;
    else if (get_type () == PION_0) c = 0.0;
    else if (get_type () == PION_PLUS) c = +1.0;
    else if (get_type () == PION_MINUS) c = -1.0;
    else if (get_type () == NEUTRON) c = 0.0;
    else if (get_type () == PROTON) c = +1.0;
    else if (get_type () == DEUTERON) c = +1.0;
    else if (get_type () == TRITIUM) c = +1.0;
    else if (is_alpha ()) c = +2.0;
    else if (get_type () == HE3) c = +2.0;
    else
      {
        throw logic_error ("primary_particle::get_charge: Unknown particle !");
      }
    return c;
  }

  bool primary_particle::mass_is_known () const
  {
    return datatools::is_valid (get_mass ());
  }

  double primary_particle::get_mass () const
  {
    double a_mass;
    datatools::invalidate (a_mass);

    if (is_positron () || is_electron ())
      {
        a_mass = CLHEP::electron_mass_c2;
      }

    if (is_alpha ())
      {
        a_mass = 3.727417 * CLHEP::GeV;
      }

    if (is_gamma ())
      {
        a_mass = 0.0;
      }

    if (type == NEUTRON)
      {
        a_mass = 939.565560 * CLHEP::MeV;
      }

    if (type == PROTON)
      {
        a_mass = 938.272013 * CLHEP::MeV;
      }

    if (type == MUON_PLUS || type == MUON_MINUS)
      {
        a_mass = 105.658369 * CLHEP::MeV;
      }

    if (type == NEUTRINO)
      {
        a_mass = 0.0 * CLHEP::MeV;
      }

    return a_mass;
  }

  double primary_particle::get_beta () const
  {
    return momentum.mag () / get_total_energy ();
  }

  double primary_particle::get_kinetic_energy () const
  {
    double the_mass = get_mass ();
    double kinetic_energy
      = sqrt (momentum.mag () * momentum.mag () + the_mass * the_mass) 
      - the_mass;
    return kinetic_energy;
  }

  double primary_particle::get_total_energy () const
  {
    return get_kinetic_energy () + get_mass ();
  }

  void primary_particle::dump (ostream & out_, const string & indent_) const
  {
    tree_dump( out_, "genbb::primary_particle : ", indent_);
    return;
  }

  void primary_particle::tree_dump (std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << std::endl;
      }
    double mass = get_mass ();
    double energy = get_kinetic_energy ();

    out_ << indent << du::i_tree_dumpable::tag << "Type: " << type;
    if (type != UNDEF)
      {
        out_ << " (" << get_particle_label_from_type (type) << ')';
      }
    out_ << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Particle label: '" << particle_label << "'"
         << endl;
    ostringstream time_oss;
    time_oss.precision (15);
    time_oss << time / CLHEP::ns;
    out_ << indent << du::i_tree_dumpable::tag << "Time: " << time_oss.str ()
         << " ns" << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Kinetic energy: " << energy / CLHEP::MeV
         << " MeV" << endl;
    out_ << indent << du::i_tree_dumpable::tag << "Momentum: "
         << momentum / CLHEP::MeV
         << " MeV" << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Vertex: ";
    if (has_vertex ())
      {
        out_ << vertex / CLHEP::mm
             << " mm" << endl;
      }
    else
      {
        out_ << "<no vertex>" << endl;
      }
    
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
