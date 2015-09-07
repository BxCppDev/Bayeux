// pdg_particle_tools.cc - Implementation of PDG particle codes
//
// Copyright (c) 2015 by Francois Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of genbb_help.
//
// genbb_help is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// genbb_help is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with genbb_help.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <genbb_help/pdg_particle_tools.h>

// Standard library:
#include <sstream>

namespace genbb {

  namespace pdg {

    particle::particle()
    {
      _code_ = INVALID_CODE;
      return;
    }

    particle::particle(int32_t pdg_code_,
                       const std::string & name_,
                       const std::string & latex_repr_)
    {
      set_code(pdg_code_);
      set_name(name_);
      set_latex_repr(latex_repr_);
      return;
    }

    particle::particle(int32_t pdg_code_,
                       const std::string & name_,
                       const std::string & alt_name_,
                       const std::string & latex_repr_)
    {
      set_code(pdg_code_);
      set_name(name_);
      set_alt_name(alt_name_);
      set_latex_repr(latex_repr_);
      return;
    }

    particle::~particle()
    {
      return;
    }

    bool particle::is_valid() const
    {
      return _code_ != INVALID_CODE && !_name_.empty() && !_latex_repr_.empty();
    }

    void particle::reset()
    {
      _code_ = INVALID_CODE;
      _name_.clear();
      _alt_name_.clear();
      _latex_repr_.clear();
      return;
    }

    int32_t particle::get_code() const
    {
      return _code_;
    }

    particle & particle::set_code(int32_t pdg_code_)
    {
      _code_ = pdg_code_;
      return *this;
    }

    const std::string & particle::get_name() const
    {
      return _name_;
    }

    particle & particle::set_name(const std::string & name_)
    {
      _name_ = name_;
      return *this;
    }

    bool particle::has_alt_name() const
    {
      return !_alt_name_.empty();
    }

    const std::string & particle::get_alt_name() const
    {
      return _alt_name_;
    }

    particle & particle::set_alt_name(const std::string & alt_name_)
    {
      _alt_name_ = alt_name_;
      return *this;
    }

    const std::string & particle::get_latex_repr() const
    {
      return _latex_repr_;
    }

    particle & particle::set_latex_repr(const std::string & latex_repr_)
    {
      _latex_repr_ = latex_repr_;
      return *this;
    }

    const datatools::properties & particle::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    datatools::properties & particle::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    void particle::tree_dump(std::ostream & out_,
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Code : " << _code_
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Name : '" << _name_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Alternative name : '" << _alt_name_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "LaTeX representation : '" << _latex_repr_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Auxiliary properties : ";
      if ( _auxiliaries_.size() == 0) {
        out_ << "<none>";
      }
      out_ << std::endl;
      if ( _auxiliaries_.size() > 0) {
        std::ostringstream indent2;
        indent2 << indent_ << i_tree_dumpable::skip_tag;
        _auxiliaries_.tree_dump(out_, "", indent2.str());
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Valid : " << is_valid()
           << std::endl;

      return;
    }

    // static
    int32_t particle::build_nuclear_code(int z_, int a_, int i_)
    {
      return 1000000000 + z_ * 10000 + a_ * 10 + i_;
    }

    particle_registry::particle_registry(uint32_t /* flags_ */)
    {
      bool do_register_standard_particles = true;

      if (do_register_standard_particles) {
        register_standard_particles();
      }

      return;
    }

    particle_registry::~particle_registry()
    {
      return;
    }

    void particle_registry::clear()
    {
      _records_.clear();
      return;
    }

    particle & particle_registry::registration(const particle & part_)
    {
      DT_THROW_IF(!part_.is_valid(), std::logic_error,
                  "Particle '" << part_.get_name() << "' is not valid!");
      DT_THROW_IF(has_particle(part_.get_code()), std::logic_error,
                  "Particle with PDG code '" << part_.get_code()
                  << "' is already registered!");
      int32_t pdg_code = part_.get_code();
      {
        _records_[pdg_code] = part_;
      }
      return _records_.find(pdg_code)->second;
    }

    void particle_registry::unregistration(const std::string & name_)
    {
      particle_dict_type::iterator found = _records_.end();
      for (particle_dict_type::iterator i = _records_.begin();
           i != _records_.end();
           i++) {
        const particle & part = i->second;
        if (part.get_name() == name_ || (part.has_alt_name() && part.get_alt_name() == name_)) {
          found = i;
          break;
        }
      }
      DT_THROW_IF(found == _records_.end(), std::logic_error,
                  "Particle registry has no registered particle with name '" << name_ << "'!");
      _records_.erase(found);
      return;
    }

    void particle_registry::unregistration(int32_t pdg_code_)
    {
      particle_dict_type::iterator found = _records_.find(pdg_code_);
      DT_THROW_IF(found == _records_.end(), std::logic_error,
                  "Particle registry has no registered particle with PDG code '" << pdg_code_ << "'!");
      _records_.erase(found);
      return;
    }

    bool particle_registry::has_particle(int32_t pdg_code_) const
    {
      return _records_.find(pdg_code_) != _records_.end();
    }

    const particle & particle_registry::get_particle(int32_t pdg_code_) const
    {
      DT_THROW_IF(!has_particle(pdg_code_), std::logic_error,
                  "No particle with PDG code '" << pdg_code_ << "' is registered!");
      return _records_.find(pdg_code_)->second;
    }

    bool particle_registry::has_particle_by_name(const std::string & name_) const
    {
      for (particle_dict_type::const_iterator i = _records_.begin();
           i != _records_.end();
           i++) {
        const particle & part = i->second;
        if (part.get_name() == name_ || (part.has_alt_name() && part.get_alt_name() == name_)) {
          return true;
        }
      }
      return false;
    }

    const particle & particle_registry::get_particle_by_name(const std::string & name_) const
    {
      for (particle_dict_type::const_iterator i = _records_.begin();
           i != _records_.end();
           i++) {
        const particle & part = i->second;
        if (part.get_name() == name_ || (part.has_alt_name() && part.get_alt_name() == name_)) {
          return i->second;
        }
      }
      DT_THROW(std::logic_error, "No particle with name '" << name_ << "' is registered!");
    }

    void particle_registry::tree_dump(std::ostream & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Particles: [" << _records_.size() << ']' << std::endl;
      for (particle_dict_type::const_iterator i = _records_.begin();
           i != _records_.end();
           i++) {
        particle_dict_type::const_iterator j = i;
        out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
        if (++j == _records_.end()) {
          out_ << indent_ << i_tree_dumpable::last_tag;
        } else {
          out_ << indent_ << i_tree_dumpable::tag;
        }
        out_ << "PDG code '" << i->first << "' for '" << i->second.get_name() << "' particle";
        if (i->second.has_alt_name()) {
          out_ << " (or '" << i->second.get_alt_name() << "')";
        }
        out_ << std::endl;
      }

      return;
    }

    // static
    particle_registry & particle_registry::system_registry()
    {
      return _access_system_registry_();
    }

    // static
    const particle_registry & particle_registry::const_system_registry()
    {
      return const_cast<particle_registry &>(system_registry());
    }

    // static
    particle_registry & particle_registry::_access_system_registry_()
    {
      static boost::scoped_ptr<particle_registry> _reg_singleton;
      if (! _reg_singleton) {
        _reg_singleton.reset(new particle_registry);
      }
      return *_reg_singleton;
    }

    void particle_registry::register_standard_particles()
    {
      // Leptons:
      registration(particle(particle::ELECTRON,               "e-", "electron",  "{e^-}"));
      registration(particle(particle::POSITRON,               "e+", "positron",  "{e^+}"));
      registration(particle(particle::NEUTRINO_ELECTRON,      "nu_e",            "{\\nu_e}"));
      registration(particle(particle::ANTI_NEUTRINO_ELECTRON, "anti_nu_e",       "{\\bar\\nu_e}"));
      registration(particle(particle::MUON_MINUS,             "mu-",             "{\\mu^-}"));
      registration(particle(particle::MUON_PLUS,              "mu+",             "{\\mu^+}"));
      registration(particle(particle::NEUTRINO_MUON,          "nu_mu",           "{\\nu_\\mu}"));
      registration(particle(particle::ANTI_NEUTRINO_MUON,     "anti_nu_mu",      "{\\bar\\nu_\\mu}"));
      registration(particle(particle::TAU_MINUS,              "tau-",            "{\\tau^-}"));
      registration(particle(particle::TAU_PLUS,               "tau+",            "{\\tau^+}"));
      registration(particle(particle::NEUTRINO_TAU,           "nu_tau",          "{\\nu_\\tau}"));
      registration(particle(particle::ANTI_NEUTRINO_TAU,      "anti_nu_tau",     "{\\bar\\nu_\\tau}"));

      // Bosons:
      registration(particle(particle::GAMMA,             "gamma",                     "{\\gamma}"));
      registration(particle(particle::MC_GEANTINO,       "geantino",                  "{g^0}"));
      registration(particle(particle::MC_OPTICAL_PHOTON, "opticalphoton", "cerenkov", "{\\gamma_{opt}}"));

      // Mesons:
      registration(particle(particle::PION_ZERO,       "pi0",        "{\\pi^0}"));
      registration(particle(particle::PION_PLUS,       "pi+",        "{\\pi^+}"));
      registration(particle(particle::PION_MINUS,      "pi-",        "{\\pi^-}"));
      registration(particle(particle::KAON_ZERO_LONG,  "kaon0L",     "{K_L^0}"));
      registration(particle(particle::KAON_ZERO_SHORT, "kaon0S",     "{K_S^0}"));
      registration(particle(particle::KAON_ZERO,       "kaon0",      "{K^0}"));
      registration(particle(particle::ANTI_KAON_ZERO,  "anti_kaon0", "{\\bar{K}^0}"));
      registration(particle(particle::KAON_PLUS,       "kaon+",      "{K^+}"));
      registration(particle(particle::KAON_MINUS,      "kaon-",      "{K^-}"));
      registration(particle(particle::ETA,             "eta",        "{\\eta}"));

      // Hadrons:
      // /data/sw/Cadfael/Binary/Cadfael-trunk/Build-Linux-x86_64/ports/geant4/geant4-prefix/src/geant4/source/particles/hadrons/barions/src
      registration(particle(particle::PROTON,           "proton",       "p",      "{p}"));
      registration(particle(particle::ANTI_PROTON,      "anti_proton",  "anti_p", "{\\bar{p}}"));
      registration(particle(particle::NEUTRON,          "neutron",      "n",      "{n}"));
      registration(particle(particle::ANTI_NEUTRON,     "anti_neutron", "anti_n", "{\\bar{n}}"));
      registration(particle(particle::LAMBDA,           "lambda",       "{\\Lambda^0}"));
      registration(particle(particle::ANTI_LAMBDA,      "anti_lambda",  "{\\bar\\Lambda^0}"));
      registration(particle(particle::SIGMA_PLUS,       "sigma+",       "{\\Sigma^+}"));
      registration(particle(particle::ANTI_SIGMA_PLUS,  "anti_sigma+",  "{\\bar\\Sigma^-}"));
      registration(particle(particle::SIGMA_ZERO,       "sigma0",       "{\\Sigma^0}"));
      registration(particle(particle::ANTI_SIGMA_ZERO,  "anti_sigma0",  "{\\bar\\Sigma^0}"));
      registration(particle(particle::SIGMA_MINUS,      "sigma-",       "{\\Sigma^-}"));
      registration(particle(particle::ANTI_SIGMA_MINUS, "anti_sigma-",  "{\\bar\\Sigma^+}"));
      registration(particle(particle::XI_ZERO,          "xi0",          "{\\Xi^0}"));
      registration(particle(particle::ANTI_XI_ZERO,     "anti_xi0",     "{\\bar\\Xi^0}"));
      registration(particle(particle::XI_MINUS,         "xi-",          "{\\Xi^-}"));
      registration(particle(particle::ANTI_XI_MINUS,    "anti_xi-",     "{\\bar\\Xi^+}"));
      registration(particle(particle::OMEGA_MINUS,      "omega-",       "{\\Omega^-}"));
      registration(particle(particle::ANTI_OMEGA_MINUS, "anti_omega-",  "{\\bar\\Omega^+}"));

      // Light ions:
      registration(particle(particle::DEUTERON,      "deuteron", "D",  "{D}"));
      registration(particle(particle::ANTI_DEUTERON, "anti_deuteron",  "{\\bar{D}}"));
      registration(particle(particle::TRITON,        "triton",   "T",  "{T}"));
      registration(particle(particle::ANTI_TRITON,   "anti_triton",    "{\\bar{T}}"));
      registration(particle(particle::HE3,           "He3",            "{^3\\mathrm{He}}"));
      registration(particle(particle::ANTI_HE3,      "anti_He3",       "{\\bar{^3\\mathrm{He}}}"));
      registration(particle(particle::ALPHA,         "alpha",          "{\\alpha}"));
      registration(particle(particle::ANTI_ALPHA,    "anti_alpha",     "{\\bar\\alpha}"));

      return;
    }

  } // end of namespace pdg

} // end of namespace genbb
