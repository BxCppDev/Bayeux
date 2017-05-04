// primary_particle.cc
/*
 * Copyright 2007-2015 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Ourselves:
#include <genbb_help/primary_particle.h>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <genbb_help/genbb_help_config.h>
#include <genbb_help/pdg_particle_tools.h>

// Special backward compatibility support for serialization :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(genbb::primary_particle,
                                                      "genbb::primary_particle")
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(genbb::primary_particle,
                                                               "__genbb::primary_particle__")

namespace genbb {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(primary_particle, "genbb::primary_particle")

  // static
  const int     primary_particle::INVALID_GENERATION_ID;
  const int32_t primary_particle::PDG_CODE_UNDEFINED;

  /// \brief Ion data for parser
  struct ion_data_type {
    /// Default constructor
    ion_data_type();
    /// Reset
    void reset();
    /// Check validity
    bool is_valid() const;
    int Z;        //!< Atomic number
    int A;        //!< Number of nucleons
    double Estar; //!< Excitation energy
    int Q;        //!< Ion charge
  };

  ion_data_type::ion_data_type()
  {
    reset();
    return;
  }

  void ion_data_type::reset()
  {
    Z = -1;
    A = 0;
    Estar = 0.0;
    Q = 0;
    return;
  }

  bool ion_data_type::is_valid() const
  {
    return Z > 1 && A >= Z && Estar >= 0.0;
  }

  /// \brief A parser for labels describing a nucleus or an ion
  class ion_parser {
  public:
    enum what_type {
      WHAT_NUCLEUS = 0,
      WHAT_ION     = 1
    };
    ion_parser(what_type what_, bool pm_ = false);
    bool parse(const std::string & label_, ion_data_type & data_) const;
  private:
    what_type _what_;           //!< Ion or nucleus
    bool      _print_messages_; //!< Flag to print message
  };

  ion_parser::ion_parser(what_type what_, bool pm_) : _what_(what_), _print_messages_(pm_)
  {}

  bool ion_parser::parse(const std::string & label_, ion_data_type & data_) const
  {
    static std::string what_nucleus("nucleus");
    static std::string what_ion("ion");
    const std::string * what_string_ptr = 0;
    // Select the header identifier:
    if (_what_ == WHAT_NUCLEUS) {
      what_string_ptr = &what_nucleus;
    } else {
      what_string_ptr = &what_ion;
    }
    const std::string & what_string = *what_string_ptr;
    bool messages = _print_messages_;
    int z = -1;
    int a =  0;
    double estar = -1.0 * CLHEP::keV;
    double energy_unit = CLHEP::keV;
    int q = 0;
    std::string word = label_;
    boost::trim(word);
    if (!boost::starts_with(word, what_string + "(") || !boost::ends_with(word, ")")) {
      if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Missing '" << what_string << "(...)' format!");
      return false;
    }
    std::string params = word.substr(what_string.length()+1, word.length() - (what_string.length() + 2));

    std::vector<std::string> param_tokens;
    boost::split(param_tokens,params,boost::is_any_of(";"));
    for (int i = 0; i < (int) param_tokens.size(); i++) {
      const std::string & partok = param_tokens[i];

      if (boost::starts_with(partok, "Z=")) {
        // Parse atomic number:
        std::string z_str = partok.substr(2);
        std::istringstream z_iss(z_str);
        z_iss >> z;
        if (! z_iss) {
          if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid '" << partok << "' Z-token!");
          return false;
        }
        if (z < 0) {
          if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid Z='" << z << "' value!");
          return false;
        }
      } else if (boost::starts_with(partok, "A=")) {
        // Parse number of nucleons:
        std::string a_str = partok.substr(2);
        std::istringstream a_iss(a_str);
        a_iss >> a;
        if (! a_iss) {
          if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid '" << partok << "' A-token!");
          return false;
        }
        if (a < 0) {
          if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid A='" << a << "' value!");
          return false;
        }
      } else if (boost::starts_with(partok, "E*=")) {
        // Parse excitation energy:
        std::string estar_str = partok.substr(3);
        std::istringstream estar_iss(estar_str);
        estar_iss >> estar;
        if (! estar_iss) {
          if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid '" << partok << "' E*-token!");
          return false;
        }
        if (estar < 0.0) {
          if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid E*='" << estar << "' value!");
          estar = -1.0 * CLHEP::keV;
          return false;
        }
        estar *= energy_unit;
      } else if (_what_ == WHAT_ION && boost::starts_with(partok, "Q=")) {
        // Parse ion charge:
        std::string q_str = partok.substr(2);
        std::istringstream q_iss(q_str);
        q_iss >> q;
        if (! q_iss) {
          if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid '" << partok << "' Q-token!");
          return false;
        }
      } else {
        if (messages) DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid '" << partok << "' token!");
        return false;
      }
    }

    if (messages) DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Parsed ion from '" << label_ << "'!");
    data_.Z = z;
    data_.A = a;
    data_.Estar = estar;
    data_.Q = q;
    if (!data_.is_valid()) {
      data_.reset();
    }
    return data_.is_valid();
  }

  // static
  std::string primary_particle::nucleus_to_label(int z_, int a_, double excitation_energy_)
  {
    DT_THROW_IF(z_ < 1, std::range_error, "Invalid Z value (Z=" << z_ << ") !");
    DT_THROW_IF(a_ < z_, std::range_error, "Invalid A value (A=" << a_ << ") !");
    DT_THROW_IF(excitation_energy_ < 0.0, std::range_error, "Invalid negative excitation energy !");
    std::ostringstream oss;
    oss.precision(15);
    oss << "nucleus("
        << "Z=" << z_
        << ";A=" << a_
        << ";E*=" << excitation_energy_/CLHEP::keV << " keV"
        << ')';
    return oss.str();
  }

  // static
  std::string primary_particle::ion_to_label(int z_, int a_, double excitation_energy_, int charge_)
  {
    DT_THROW_IF(z_ < 1, std::range_error, "Invalid Z value (Z=" << z_ << ") !");
    DT_THROW_IF(a_ < z_, std::range_error, "Invalid A value (A=" << a_ << ") !");
    DT_THROW_IF(excitation_energy_ < 0.0, std::range_error, "Invalid negative excitation energy !");
    std::ostringstream oss;
    oss.precision(15);
    oss << "ion("
        << "Z=" << z_
        << ";A=" << a_
        << ";E*=" << excitation_energy_ / CLHEP::keV << " keV"
        << ";Q=" << charge_
        << ')';
    return oss.str();
  }

  // static
  bool primary_particle::label_to_ion(const std::string label_,
                                      int & z_,
                                      int & a_,
                                     double & excitation_energy_,
                                      int & charge_)
  {
    static boost::scoped_ptr<ion_parser> _ion_parser;
    if (! _ion_parser) {
      _ion_parser.reset(new ion_parser(ion_parser::WHAT_ION, false));
    }
    ion_data_type ion_data;
    bool success = _ion_parser->parse(label_, ion_data);
    if (success) {
      z_ = ion_data.Z;
      a_ = ion_data.A;
      excitation_energy_ = ion_data.Estar;
      charge_ = ion_data.Q;
    }
    return success;
  }

  // static
  bool primary_particle::label_to_nucleus(const std::string label_,
                                          int & z_,
                                          int & a_,
                                          double & excitation_energy_)
  {
    static boost::scoped_ptr<ion_parser> _nucleus_parser;
    if (! _nucleus_parser) {
      _nucleus_parser.reset(new ion_parser(ion_parser::WHAT_NUCLEUS, false));
    }
    ion_data_type ion_data;
    bool success = _nucleus_parser->parse(label_, ion_data);
    if (success) {
      z_ = ion_data.Z;
      a_ = ion_data.A;
      excitation_energy_ = ion_data.Estar;
    }
    return success;
  }

  bool primary_particle::is_valid() const
  {
    // Check the type:
    if (! has_pdg_code()) {
      if (_type_ == PARTICLE_UNDEFINED) {
        return false;
      }
    }

    // Check for a non-empty label for particle types that need one:
    if (needs_particle_label() && !has_particle_label()) {
      return false;
    }

    // Check the time:
    if (! datatools::is_valid(_time_)) {
      return false;
    }

    // Check the momentum:
    if (! geomtools::is_valid(_momentum_)) {
      return false;
    }
    return true;
  }

  bool primary_particle::needs_particle_label() const
  {
    if (has_pdg_code()) {
      // A PDG code should be enough to fully qualify the particle
      // type  so that no additional information stored in the
      // particle label is needed:
      return false;
    }
    if (has_type()) {
      // Particles with a valid type:
      if (is_unknown() || is_nucleus() || is_ion() || is_neutrino()) {
        // Special case for unregistered/unknown particles,
        // nuclei and ions needs additional informations
        // stored in the particle label:
        return true;
      }
      // Other particles are fully qualified by their types:
      return false;
    }
    // Particles with no type (+ optional label), nor PDG code
    // do need a particle label that contains some definition
    // in some arbitrary format:
    return true;
  }

  bool primary_particle::has_particle_label() const
  {
    return ! _particle_label_.empty();
  }

  const std::string & primary_particle::get_particle_label() const
  {
    return _particle_label_;
  }

  void primary_particle::set_particle_label(const std::string & pl_)
  {
    /*
      if (_type_ == PARTICLE_UNDEFINED) {
      _particle_label_
      }
    */
    _particle_label_ = pl_;
    return;
  }

  bool primary_particle::has_pdg_code() const
  {
    return _pdg_code_ != PDG_CODE_UNDEFINED;
  }

  int primary_particle::get_pdg_code() const
  {
    return _pdg_code_;
  }

  void primary_particle::set_pdg_code(int pdg_code_)
  {
    _pdg_code_ = pdg_code_;
    return;
  }

  int primary_particle::fetch_pdg_code() const
  {
    switch (_type_) {
    case GAMMA : return pdg::particle::GAMMA;
    case POSITRON : return pdg::particle::POSITRON;
    case ELECTRON : return pdg::particle::ELECTRON;
    case NEUTRINO :
      {
        bool antineutrino = false;
        if (_auxiliaries_.has_key("neutrino.type")) {
          std::string nt = _auxiliaries_.fetch_string("neutrino.type");
          if (nt == "neutrino") {
            antineutrino = false;
          } else if (nt == "antineutrino") {
            antineutrino = true;
          } else {
            DT_THROW(std::logic_error, "Unknown neutrino type '" << nt << "'!");
          }
        }
        if (_auxiliaries_.has_key("neutrino.flavour")) {
          std::string f = _auxiliaries_.fetch_string("neutrino.flavour");
          if (f == "electron") {
            if (antineutrino) return pdg::particle::ANTI_NEUTRINO_ELECTRON;
            else  return pdg::particle::NEUTRINO_ELECTRON;
          } else if (f == "muon") {
            if (antineutrino) return pdg::particle::ANTI_NEUTRINO_MUON;
            else  return pdg::particle::NEUTRINO_MUON;
          } else if (f == "tau") {
            if (antineutrino) return pdg::particle::ANTI_NEUTRINO_TAU;
            else  return pdg::particle::NEUTRINO_TAU;
          } else {
            DT_THROW(std::logic_error, "Unknown neutrino flavour '" << f << "'!");
          }
        }
        // Default:
        return pdg::particle::ANTI_NEUTRINO_ELECTRON;
      }
    case MUON_PLUS : return pdg::particle::MUON_PLUS;
    case MUON_MINUS : return pdg::particle::MUON_MINUS;
    case PION_0 : return pdg::particle::PION_ZERO;
    case PION_PLUS : return pdg::particle::PION_PLUS;
    case PION_MINUS : return pdg::particle::PION_MINUS;
    case KAON_0_LONG : return pdg::particle::KAON_ZERO_LONG;
    case KAON_PLUS : return pdg::particle::KAON_PLUS;
    case KAON_MINUS : return pdg::particle::KAON_MINUS;
    case NEUTRON : return pdg::particle::NEUTRON;
    case PROTON : return pdg::particle::PROTON;
    case ANTI_PROTON : return pdg::particle::ANTI_PROTON;
    case KAON_0_SHORT : return pdg::particle::KAON_ZERO_SHORT;
    case ETA : return pdg::particle::ETA;
    case LAMBDA : return pdg::particle::LAMBDA;
    case SIGMA_PLUS : return pdg::particle::SIGMA_PLUS;
    case SIGMA_0 : return pdg::particle::SIGMA_ZERO;
    case SIGMA_MINUS : return pdg::particle::SIGMA_MINUS;
    case XI_0 : return pdg::particle::XI_ZERO;
    case XI_MINUS : return pdg::particle::XI_MINUS;
    case OMEGA_MINUS : return pdg::particle::OMEGA_MINUS;
    case ANTI_NEUTRON : return pdg::particle::ANTI_NEUTRON;
    case ANTI_LAMBDA : return pdg::particle::ANTI_LAMBDA;
    case ANTI_SIGMA_MINUS : return pdg::particle::ANTI_SIGMA_PLUS;
    case ANTI_SIGMA_0 : return pdg::particle::ANTI_SIGMA_ZERO;
    case ANTI_SIGMA_PLUS : return pdg::particle::ANTI_SIGMA_MINUS;
    case ANTI_XI_0 : return pdg::particle::ANTI_XI_ZERO;
    case ANTI_XI_PLUS : return pdg::particle::ANTI_XI_MINUS;
    case ANTI_OMEGA_PLUS : return pdg::particle::ANTI_OMEGA_MINUS;
    case DEUTERON : return pdg::particle::DEUTERON;
    case TRITON : return pdg::particle::TRITON;
    case ALPHA : return pdg::particle::ALPHA;
    case GEANTINO : return pdg::particle::MC_GEANTINO;
    case HE3 : return pdg::particle::HE3;
    case CERENKOV : return pdg::particle::MC_OPTICAL_PHOTON;
    case NUCLEUS :
    case ION :
      {
        DT_THROW_IF(! _auxiliaries_.has_key("ion.Z"), std::logic_error, "Missing 'ion.Z' value!");
        const int z = _auxiliaries_.fetch_integer("ion.Z");
        DT_THROW_IF(! _auxiliaries_.has_key("ion.A"), std::logic_error, "Missing 'ion.A' value!");
        const int a = _auxiliaries_.fetch_integer("ion.A");
        DT_THROW_IF(! _auxiliaries_.has_key("ion.I"), std::logic_error, "Missing 'ion.I' value!");
        const int i = _auxiliaries_.fetch_integer("ion.I");
        // Beware: no atomic charge state is recorded here
        return pdg::particle::build_nuclear_code(z, a, i);
      }
    case PARTICLE_UNKNOWN:
      {
        // We could check here the identifier label to guess some exotic particle...
        // but we have no scheme for now.
        return pdg::particle::MC_UNSPECIFIED;
      }
    default:
      {
        return pdg::particle::INVALID_CODE;
      }
    }
    return pdg::particle::INVALID_CODE;
  }

  bool primary_particle::has_type() const
  {
    return _type_ != PARTICLE_UNDEFINED;
  }

  int primary_particle::get_type() const
  {
    return _type_;
  }

  void primary_particle::set_type(int type_)
  {
    _type_ = type_;
    if (_particle_label_.empty()) {
      if (_type_ != PARTICLE_UNDEFINED) {
        _particle_label_ = particle_label_from_type(_type_);
      } else {
        _particle_label_.clear();
      }
    }
    return;
  }

  void primary_particle::set_nucleus(int z_, int a_, double excitation_energy_, bool pdg_)
  {
    {
      DT_THROW_IF(pdg_, std::logic_error, "Definition of a nucleus through its PDG code is not supported yet!");
    }
    set_type(NUCLEUS);
    set_particle_label(nucleus_to_label(z_, a_, excitation_energy_));
    _auxiliaries_.store_integer("ion.Z", z_);
    _auxiliaries_.store_integer("ion.A", a_);
    int i = 0;
    if (excitation_energy_ > 0.0) {
      i = 1;
    }
    _auxiliaries_.store_integer("ion.I", i);
    _auxiliaries_.store_real("ion.excitation_energy", excitation_energy_);
    _auxiliaries_.store_integer("ion.Q", z_);
    return;
  }

  void primary_particle::set_ion(int z_, int a_, double excitation_energy_, int charge_, bool pdg_)
  {
    {
      DT_THROW_IF(pdg_, std::logic_error, "Definition of an ion through its PDG code is not supported yet!");
    }
    int nelectrons = z_ - charge_;
    DT_THROW_IF(nelectrons < 0, std::range_error, "Invalid ionization!");
    set_particle_label(ion_to_label(z_, a_, excitation_energy_, charge_));
    set_type(ION);
    _auxiliaries_.store_integer("ion.Z", z_);
    _auxiliaries_.store_integer("ion.A", a_);
    _auxiliaries_.store_real("ion.excitation_energy", excitation_energy_);
    _auxiliaries_.store_integer("ion.Q", charge_);
    return;
  }

  void primary_particle::set_neutrino(const std::string & label_)
  {
    set_type(NEUTRINO);
    set_particle_label(label_);
    return;
  }

  void primary_particle::set_neutrino(const std::string & flavour_, bool antineutrino_)
  {
    set_type(NEUTRINO);
    _auxiliaries_.store_string("neutrino.flavour", flavour_);
    if (antineutrino_) {
      _auxiliaries_.store_string("neutrino.type", "antineutrino");
    } else {
      _auxiliaries_.store_string("neutrino.type", "neutrino");
    }
    return;
  }

  bool primary_particle::is_unknown() const
  {
    return _type_ == PARTICLE_UNKNOWN;
  }

  bool primary_particle::is_gamma() const
  {
    return _type_ == GAMMA;
  }

  bool primary_particle::is_positron() const
  {
    return _type_ == POSITRON;
  }

  bool primary_particle::is_electron() const
  {
    return _type_ == ELECTRON;
  }

  bool primary_particle::is_alpha() const
  {
    return _type_ == ALPHA;
  }

  bool primary_particle::is_deuteron() const
  {
    return _type_ == DEUTERON;
  }

  bool primary_particle::is_tritium() const
  {
    return _type_ == TRITIUM;
  }

  bool primary_particle::is_neutron() const
  {
    return _type_ == NEUTRON;
  }

  bool primary_particle::is_proton() const
  {
    return _type_ == PROTON;
  }

  bool primary_particle::is_muon_plus() const
  {
    return _type_ == MUON_PLUS;
  }

  bool primary_particle::is_muon_minus() const
  {
    return _type_ == MUON_MINUS;
  }

  bool primary_particle::is_neutrino() const
  {
    return _type_ == NEUTRINO;
  }

  bool primary_particle::is_ion() const
  {
    return _type_ == ION;
  }

  bool primary_particle::is_nucleus() const
  {
    return _type_ == NUCLEUS;
  }

  bool primary_particle::is_pion_plus() const
  {
    return _type_ == PION_PLUS;
  }

  bool primary_particle::is_pion_minus() const
  {
    return _type_ == PION_MINUS;
  }

  bool primary_particle::is_pion_zero() const
  {
    return _type_ == PION_0;
  }

  bool primary_particle::has_time() const
  {
    return datatools::is_valid(_time_);
  }

  double primary_particle::get_time() const
  {
    return _time_;
  }

  double & primary_particle::grab_time()
  {
    return _time_;
  }

  void  primary_particle::set_time(double time_)
  {
    _time_ = time_;
    return;
  }

  void primary_particle::shift_time(double delta_time_)
  {
    _time_ += delta_time_;
    return;
  }

  void primary_particle::set_momentum(const geomtools::vector_3d & v_)
  {
    _momentum_ = v_;
    return;
  }

  const geomtools::vector_3d & primary_particle::get_momentum() const
  {
    return _momentum_;
  }

  geomtools::vector_3d & primary_particle::grab_momentum()
  {
    return _momentum_;
  }

  void primary_particle::compute_four_momentum(CLHEP::HepLorentzVector & q_) const
  {
    q_.setT(get_total_energy());
    q_.setX(_momentum_.x());
    q_.setY(_momentum_.y());
    q_.setZ(_momentum_.z());
    return;
  }

  void primary_particle::set_vertex(const geomtools::vector_3d & m_)
  {
    _vertex_ = m_;
    return;
  }

  const geomtools::vector_3d & primary_particle::get_vertex() const
  {
    return _vertex_;
  }

  geomtools::vector_3d & primary_particle::grab_vertex()
  {
    return _vertex_;
  }

  bool primary_particle::has_vertex() const
  {
    return geomtools::is_valid(_vertex_);
  }

  void primary_particle::invalidate_vertex()
  {
    geomtools::invalidate_vector_3d(_vertex_);
    return;
  }

  void primary_particle::_set_defaults()
  {
    _generation_id_ = INVALID_GENERATION_ID; // No unique Id is set
    _type_ = PARTICLE_UNDEFINED;             // No Geant3 particule type is defined
    _pdg_code_ = PDG_CODE_UNDEFINED;         // No PDG particule code is defined
    datatools::invalidate(_mass_);           // Mass is not known
    _time_ = 0.0 * CLHEP::ns;                // Creation time is 0 by convention
    geomtools::invalidate(_momentum_);       // No momentum is defined
    this->invalidate_vertex();               // No vertex is defined
    return;
  }

  void primary_particle::reset()
  {
    _set_defaults();
    _particle_label_.clear();
    _auxiliaries_.clear();
    return;
  }

  primary_particle::primary_particle()
  {
    _set_defaults();
    return;
  }

  primary_particle::primary_particle(int32_t type_,
                                     double time_,
                                     const geomtools::vector_3d & momentum_)
  {
    _set_defaults();
    set_type(type_);
    set_time(time_);
    set_momentum(momentum_);
    return;
  }

  primary_particle::~primary_particle()
  {
    return;
  }

  bool primary_particle::charge_is_known() const
  {
    return datatools::is_valid(get_charge());
  }

  double primary_particle::get_charge() const
  {
    double q;
    datatools::invalidate(q);
    switch (_type_) {
    case GAMMA      : q =  0.0; break;
    case POSITRON   : q =  1.0; break;
    case ELECTRON   : q = -1.0; break;
    case NEUTRINO   : q =  0.0; break;
    case MUON_PLUS  : q =  1.0; break;
    case MUON_MINUS : q = -1.0; break;
    case PION_0     : q =  0.0; break;
    case PION_PLUS  : q =  1.0; break;
    case PION_MINUS : q = -1.0; break;
    case KAON_0_LONG  : q =  0.0; break;
    case KAON_PLUS    : q =  1.0; break;
    case KAON_MINUS   : q = -1.0; break;
    case NEUTRON      : q =  0.0; break;
    case PROTON       : q =  1.0; break;
    case ANTI_PROTON  : q = -1.0; break;
    case KAON_0_SHORT : q =  0.0; break;
    case ETA          : q =  0.0; break;
    case LAMBDA       : q =  0.0; break;
    case SIGMA_PLUS   : q =  1.0; break;
    case SIGMA_0      : q =  0.0; break;
    case SIGMA_MINUS  : q = -1.0; break;
    case XI_0         : q =  0.0; break;
    case XI_MINUS     : q = -1.0; break;
    case OMEGA_MINUS  : q = -1.0; break;
    case ANTI_NEUTRON      : q =  0.0; break;
    case ANTI_LAMBDA       : q =  0.0; break;
    case ANTI_SIGMA_MINUS  : q = -1.0; break;
    case ANTI_SIGMA_0      : q =  0.0; break;
    case ANTI_SIGMA_PLUS   : q =  1.0; break;
    case ANTI_XI_0         : q =  0.0; break;
    case ANTI_XI_PLUS      : q =  1.0; break;
    case ANTI_OMEGA_PLUS   : q =  1.0; break;
    case DEUTERON   : q =  1.0; break;
    case TRITON     : q =  1.0; break;
    case ALPHA      : q =  2.0; break;
    case GEANTINO   : q =  1.0; break;
    case HE3        : q =  2.0; break;
    case CERENKOV   : q =  0.0; break;
    case NUCLEUS    :
      {
        if (_auxiliaries_.has_key("ion.Q")) {
          q = 1.0 * _auxiliaries_.fetch_integer("ion.Q");
        }
      }
      break;
    case ION        :
      {
        if (_auxiliaries_.has_key("ion.Q")) {
          q = 1.0 * _auxiliaries_.fetch_integer("ion.Q");
        }
      }
      break;
    default: break;
    }
    return q;
  }

  bool primary_particle::has_generation_id() const
  {
    return _generation_id_ > INVALID_GENERATION_ID;
  }

  void primary_particle::reset_generation_id()
  {
    _generation_id_ = INVALID_GENERATION_ID;
    return;
  }

  void primary_particle::set_generation_id(int gen_id_)
  {
    if (gen_id_ < 0) {
      _generation_id_ = INVALID_GENERATION_ID;
    } else {
      _generation_id_ = gen_id_;
    }
    return;
  }

  int primary_particle::get_generation_id() const
  {
    return _generation_id_;
  }

  bool primary_particle::mass_is_known() const
  {
    return datatools::is_valid(get_mass());
  }

  void primary_particle::reset_mass()
  {
    datatools::invalidate(_mass_);
    return;
  }

  void primary_particle::set_mass(double value_)
  {
    DT_THROW_IF(value_ < 0.0, std::domain_error, "Invalid negative mass!");
    _mass_ = value_;
    return;
  }

  double primary_particle::get_mass() const
  {
    if (datatools::is_valid(_mass_)) {
      return _mass_;
    }
    double a_mass;
    datatools::invalidate(a_mass);

    if (is_gamma()) {
      a_mass = 0.0;
    }

    if (is_positron() || is_electron()) {
      a_mass = CLHEP::electron_mass_c2;
    }

    if (is_alpha()) {
      a_mass = 3.727417 * CLHEP::GeV;
    }

    if (_type_ == NEUTRON) {
      a_mass = 939.565560 * CLHEP::MeV;
    }

    if (_type_ == PROTON) {
      a_mass = 938.272013 * CLHEP::MeV;
    }

    if (_type_ == MUON_PLUS || _type_ == MUON_MINUS) {
      a_mass = 105.658369 * CLHEP::MeV;
    }

    if (_type_ == NEUTRINO) {
      a_mass = 0.0 * CLHEP::MeV;
    }

    return a_mass;
  }

  double primary_particle::get_beta() const
  {
    return _momentum_.mag() / get_total_energy();
  }

  double primary_particle::get_kinetic_energy() const
  {
    double m = get_mass();
    double p = _momentum_.mag();
    double kinetic_energy = std::sqrt(p * p + m * m) - m;
    return kinetic_energy;
  }

  double primary_particle::get_total_energy() const
  {
    return get_kinetic_energy() + get_mass();
  }

  void primary_particle::dump(std::ostream & out_, const std::string & indent_) const
  {
    tree_dump( out_, "genbb::primary_particle : ", indent_);
    return;
  }

  void primary_particle::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Generation Id  : ";
    if (has_generation_id()) {
      out_ << _generation_id_;
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Type           : ";
    if (_type_ == PARTICLE_UNDEFINED) {
      out_ << "<none>";
    } else {
      out_ << _type_;
      if (has_particle_label()) {
        out_ << " (label='" << get_particle_label() << "')";
      }
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "PDG code       : ";
    if (has_pdg_code()) {
      out_ << _pdg_code_;
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    /*
      out_ << indent << datatools::i_tree_dumpable::tag
      << "Particle label : '" << _particle_label_ << "'"
      << std::endl;
    */

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mass           : ";
    if (mass_is_known()) {
      out_ << get_mass() / CLHEP::MeV << " MeV";
    } else {
      out_ << "<unknown>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Charge         : ";
    if (charge_is_known()) {
      out_ << get_charge() << " e";
    } else {
      out_ << "<unknown>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Time           : ";
    double time = _time_;
    if (has_time()) {
      std::ostringstream time_oss;
      time_oss.precision(15);
      time_oss << time / CLHEP::ns;
      out_ << time_oss.str() << " ns";
    } else {
      out_ << "<unknown>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Kinetic energy : ";
    double kenergy = get_kinetic_energy();
    if (datatools::is_valid(kenergy)) {
      out_ << kenergy / CLHEP::MeV << " MeV";
    } else {
      out_ << "<unknown>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Momentum       : ";
    if (geomtools::is_valid(_momentum_)) {
      out_ << _momentum_ / CLHEP::MeV << " MeV";
    } else {
      out_ << "<unknown>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Vertex         : ";
    if (has_vertex()) {
      out_ << _vertex_ / CLHEP::mm
           << " mm" << std::endl;
    } else {
      out_ << "<no vertex>" << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Auxiliary properties: ";
    if (_auxiliaries_.size() == 0) {
      out_ << "<none>" << std::endl;
    } else {
      out_ << '[' << _auxiliaries_.size() << ']' << std::endl;
      std::ostringstream indent_oss;
      indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
      _auxiliaries_.tree_dump(out_, "", indent_oss.str());
    }

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Valid          : " << is_valid() << std::endl;

    return;
  }

  const datatools::properties &
  primary_particle::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties &
  primary_particle::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  std::string primary_particle::particle_label_from_type(int type_)
  {
    switch (type_) {
    case 0: return "unknown";
    case 1: return "gamma";
    case 2: return "e+";
    case 3: return "e-";
    case 4: return "neutrino";
    case 5: return "mu+";
    case 6: return "mu-";
    case 7: return "pi0";
    case 8: return "pi+";
    case 9: return "pi-";
    case 10: return "kaon0_long";
    case 11: return "kaon+";
    case 12: return "kaon-";
    case 13: return "neutron";
    case 14: return "proton";
    case 15: return "anti_proton";
    case 16: return "kaon0_short";
    case 17: return "eta";
    case 18: return "lambda";
    case 19: return "sigma+";
    case 20: return "sigma0";
    case 21: return "sigma-";
    case 22: return "xi0";
    case 23: return "xi-";
    case 24: return "omega-";
    case 25: return "anti_neutron";
    case 26: return "anti_lambda";
    case 27: return "anti_sigma-";
    case 28: return "anti_sigma0";
    case 29: return "anti_sigma+";
    case 30: return "anti_xi0";
    case 31: return "anti_xi+";
    case 32: return "anti_omega+";
    case 45: return "deuteron";
    case 46: return "triton"; // was tritium
    case 47: return "alpha";
    case 48: return "geantino";
    case 49: return "he3";
    case 50: return "cerenkov";
    case 100000000: return "nucleus";
    case 110000000: return "ion";
    }
    return "";
  }

  int primary_particle::particle_type_from_label(const std::string & label_)
  {
    if (label_ == "unknown") {
      return primary_particle::PARTICLE_UNKNOWN;
    }
    else if (label_ == "gamma") {
      return primary_particle::GAMMA;
    }
    else if (label_ == "positron" || label_ == "e+") {
      return primary_particle::POSITRON;
    }
    else if (label_ == "electron" || label_ == "e-") {
      return primary_particle::ELECTRON;
    }
    else if (label_ == "neutrino" || label_ == "nu") {
      return primary_particle::NEUTRINO;
    }
    else if (label_ == "mu+" || label_ == "muon+") {
      return primary_particle::MUON_PLUS;
    }
    else if (label_ == "mu-" || label_ == "muon-") {
      return primary_particle::MUON_MINUS;
    }
    else if (label_ == "pi0") {
      return primary_particle::PION_0;
    }
    else if (label_ == "pi+") {
      return primary_particle::PION_PLUS;
    }
    else if (label_ == "pi-") {
      return primary_particle::PION_MINUS;
    }
    else if (label_ == "kaon0_long") {
      return primary_particle::KAON_0_LONG;
    }
    else if (label_ == "kaon+") {
      return primary_particle::KAON_PLUS;
    }
    else if (label_ == "kaon-") {
      return primary_particle::KAON_MINUS;
    }
    else if (label_ == "neutron") {
      return primary_particle::NEUTRON;
    }
    else if (label_ == "proton") {
      return primary_particle::PROTON;
    }
    else if (label_ == "anti_proton") {
      return primary_particle::ANTI_PROTON;
    }
    else if (label_ == "kaon0_short") {
      return primary_particle::KAON_0_SHORT;
    }
    else if (label_ == "eta") {
      return primary_particle::ETA;
    }
    else if (label_ == "lambda") {
      return primary_particle::LAMBDA;
    }
    else if (label_ == "sigma+") {
      return primary_particle::SIGMA_PLUS;
    }
    else if (label_ == "sigma0") {
      return primary_particle::SIGMA_0;
    }
    else if (label_ == "sigma-") {
      return primary_particle::SIGMA_MINUS;
    }
    else if (label_ == "xi0") {
      return primary_particle::XI_0;
    }
    else if (label_ == "xi-") {
      return primary_particle::XI_MINUS;
    }
    else if (label_ == "omega-") {
      return primary_particle::OMEGA_MINUS;
    }
    else if (label_ == "anti_neutron") {
      return primary_particle::ANTI_NEUTRON;
    }
    else if (label_ == "anti_lambda") {
      return primary_particle::ANTI_LAMBDA;
    }
    else if (label_ == "anti_sigma-") {
      return primary_particle::ANTI_SIGMA_MINUS;
    }
    else if (label_ == "anti_sigma0") {
      return primary_particle::ANTI_SIGMA_0;
    }
    else if (label_ == "anti_sigma+") {
      return primary_particle::ANTI_SIGMA_PLUS;
    }
    else if (label_ == "anti_xi0") {
      return primary_particle::ANTI_XI_0;
    }
    else if (label_ == "anti_xi+") {
      return primary_particle::ANTI_XI_PLUS;
    }
    else if (label_ == "anti_omega+") {
      return primary_particle::ANTI_OMEGA_PLUS;
    }
    else if (label_ == "deuteron") {
      return primary_particle::DEUTERON;
    }
    else if (label_ == "tritium" || label_ == "triton") {
      return primary_particle::TRITIUM;
    }
    else if (label_ == "alpha") {
      return primary_particle::ALPHA;
    }
    else if (label_ == "geantino") {
      return primary_particle::GEANTINO;
    }
    else if (label_ == "he3") {
      return primary_particle::HE3;
    }
    else if (label_ == "cerenkov") {
      return primary_particle::CERENKOV;
    }
    else if (label_ == "nucleus") {
      return primary_particle::NUCLEUS;
    }
    else if (label_ == "ion") {
      return primary_particle::ION;
    }
    else return primary_particle::PARTICLE_UNDEFINED;
  }

} // end of namespace genbb
