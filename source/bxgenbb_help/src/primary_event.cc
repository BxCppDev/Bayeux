// -*- mode: c++; -*-
// primary_event.cc
/*
 * Copyright 2007-2014 F. Mauger
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
 *
 */

// Ourselves:
#include <genbb_help/primary_event.h>

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <limits>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/genbb_help_config.h>

// Special backward compatibility support for serialization :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(genbb::primary_event,"genbb::primary_event")
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(genbb::primary_event,"__genbb::primary_event__")

namespace genbb {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(primary_event,"genbb::primary_event")

  bool primary_event::is_valid() const
  {
    if (!datatools::is_valid(_time_)) {
      return false;
    }
    for (particles_col_type::const_iterator it = _particles_.begin();
         it != _particles_.end();
         it++) {
      if (! it->is_valid()) {
        return false;
      }
    }
    return true;
  }

  void primary_event::_set_defaults()
  {
    datatools::invalidate(_time_);
    _genbb_weight_ = 1.0;
    return;
  }

  void primary_event::reset()
  {
    _set_defaults();
    reset_label();
    _particles_.clear();
    reset_classification();
    _auxiliaries_.clear();
    return;
  }

  double primary_event::get_total_kinetic_energy() const
  {
    double tke = 0.;
    for (particles_col_type::const_iterator i = _particles_.begin();
         i != _particles_.end();
         i++) {
        const primary_particle & p = *i;
        tke += p.get_kinetic_energy();
      }
    return tke;
  }

  primary_event::primary_event()
  {
    _set_defaults();
    return;
  }

  primary_event::~primary_event()
  {
    return;
  }

  void primary_event::set_time(double t_)
  {
    _time_ = t_;
    return;
  }

  double primary_event::get_time() const
  {
    return _time_;
  }

  void primary_event::set_genbb_weight(double genbb_weight_)
  {
    DT_THROW_IF (genbb_weight_ < 0.0, std::logic_error,
                 "Invalid 'genbb_weight' value  (" << genbb_weight_ << " < 0) !");
    _genbb_weight_ = genbb_weight_;
    return;
  }

  double primary_event::get_genbb_weight() const
  {
    return _genbb_weight_;
  }

  bool primary_event::is_genbb_weighted() const
  {
    return _genbb_weight_ == 1.0;
  }

  const primary_event::particles_col_type & primary_event::get_particles() const
  {
    return _particles_;
  }

  primary_event::particles_col_type & primary_event::grab_particles()
  {
    return _particles_;
  }

  void primary_event::add_particle(const primary_particle & p_)
  {
    _particles_.push_back(p_);
    return;
  }

  primary_particle & primary_event::add_particle()
  {
    primary_particle dummy;
    _particles_.push_back(dummy);
    return _particles_.back();
  }

  unsigned int primary_event::get_number_of_particles() const
  {
    return _particles_.size();
  }

  const primary_particle & primary_event::get_particle(int index_) const
  {
    DT_THROW_IF(index_ < 0 || index_ >= (int)_particles_.size(),
                std::range_error,
                "Invalid particle index '" << index_ << "' !");
    int count = 0;
    particles_col_type::const_iterator i = _particles_.begin();
    for (; i != _particles_.end(); i++) {
      if (count == index_) {
        break;
      }
      count++;
    }
    return *i;
 }

  primary_particle & primary_event::grab_particle(int index_)
  {
    DT_THROW_IF(index_ < 0 || index_ >= (int)_particles_.size(),
                std::range_error,
                "Invalid particle index '" << index_ << "' !");
    int count = 0;
    particles_col_type::iterator i = _particles_.begin();
    for (; i != _particles_.end(); i++) {
      if (count == index_) {
        break;
      }
      count++;
    }
    return *i;
  }

  const std::string & primary_event::get_label() const
  {
    return _label_;
  }

  void primary_event::set_label(const std::string & l_)
  {
    _label_ = l_;
    return;
  }

  void primary_event::reset_label()
  {
    _label_ = "";
    return;
  }

  const std::string & primary_event::get_classification() const
  {
    return _classification_;
  }

  void primary_event::set_classification(const std::string & c_)
  {
    _classification_ = c_;
    return;
  }

  void primary_event::reset_classification()
  {
    _classification_ = "";
    return;
  }

  void primary_event::compute_classification()
  {
    size_t n_eminus = 0;
    size_t n_eplus = 0;
    size_t n_gamma = 0;
    size_t n_alpha = 0;
    size_t n_others = 0;
    for (particles_col_type::const_iterator i = _particles_.begin();
         i != _particles_.end();
         i++) {
      const primary_particle & p = *i;
      if (p.is_electron()) n_eminus++;
      else if (p.is_positron()) n_eplus++;
      else if (p.is_gamma()) n_gamma++;
      else if (p.is_alpha()) n_alpha++;
      else n_others++;
    }
    std::ostringstream cl_ss;

    cl_ss << n_eminus << 'e'
          << n_eplus << 'p'
          << n_gamma << 'g'
          << n_alpha << 'a'
          << n_others << 'X';
    set_classification(cl_ss.str());
    return;
  }

  void primary_event::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag << "Auxiliary properties: ";
    if (_auxiliaries_.size() == 0) {
      out_ << "<none>" << std::endl;
    } else {
      out_ << '[' << _auxiliaries_.size() << ']' << std::endl;
      std::ostringstream indent_oss;
      indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
      _auxiliaries_.tree_dump(out_, "", indent_oss.str());
    }

    out_ << indent << datatools::i_tree_dumpable::tag << "Label : '" << _label_ << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Time  : " << _time_ / CLHEP::second << " s" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Particles: [" << _particles_.size() << "]" << std::endl;

    int particle_counter = 0;
    for (particles_col_type::const_iterator it = _particles_.begin();
         it != _particles_.end();
         it++) {
      std::ostringstream indent_oss;
      indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
      particles_col_type::const_iterator jt = it;
      jt++;
      out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
      if (jt == _particles_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
        indent_oss << datatools::i_tree_dumpable::last_skip_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
      }
      out_ << "Particle #" << particle_counter << " : " << std::endl;
      it->tree_dump(out_, "", indent_oss.str());
      particle_counter++;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "GENBB weight : " << get_genbb_weight()
         << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Classification : '" << get_classification()
         << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "Valid: " << is_valid() << std::endl;

    return;
  }

  void primary_event::dump(std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_) const
  {
    tree_dump(out_, title_, indent_);
  }

  void primary_event::dump(std::ostream & out_,
                            const std::string & indent_) const
  {
    dump(out_, "genbb::primary_event:", indent_);
    return;
  }

  void primary_event::rotate(double phi_, double theta_, double delta_)
  {
    geomtools::rotation_3d rot;
    geomtools::create_rotation_3d(rot, phi_, theta_, delta_);
    rot.invert();
    for (particles_col_type::iterator i = _particles_.begin();
         i != _particles_.end();
         i++) {
        primary_particle & p = *i;
        geomtools::vector_3d & momentum = p.grab_momentum();
        geomtools::vector_3d rotated_momentum = rot * momentum;
        momentum = rotated_momentum;
      }
    return;
  }

  const datatools::properties &
  primary_event::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties &
  primary_event::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  void primary_event::set_auxiliaries(const datatools::properties & aux_)
  {
    _auxiliaries_ = aux_;
    return;
  }

  void primary_event::shift_particles_time(double delta_time_, int from_)
  {
    int count = 0;
    for (particles_col_type::iterator i = _particles_.begin();
         i != _particles_.end();
         i++) {
      primary_particle & p = *i;
      if (count >= from_) {
        p.shift_time(delta_time_);
      }
      count++;
    }
    return;
  }

} // end of namespace genbb
