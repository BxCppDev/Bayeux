// -*- Mode: c++; -*- 
// primary_event.cc
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
 *
 */

#include <genbb_help/primary_event.h>

#include <genbb_help/genbb_help_config.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <boost/cstdint.hpp>

#include <geomtools/utils.h>

// Special backward compatibility support for serialization :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(genbb::primary_event,"genbb::primary_event")
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(genbb::primary_event,"__genbb::primary_event__")

namespace genbb {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(primary_event,"genbb::primary_event")

  // //DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (primary_event_3d,"genbb::primary_event")

  // const std::string primary_event::SERIAL_TAG     = "genbb::primary_event";
  // const std::string primary_event::OLD_SERIAL_TAG = "__genbb::primary_event__";

  // const std::string & primary_event::get_serial_tag () const
  // {
  //   if (library_config::g_use_old_serialization_tag)
  //     {
  //       return primary_event::OLD_SERIAL_TAG;
  //     }
  //   return primary_event::SERIAL_TAG;
  // }

  bool primary_event::is_valid () const
  {
    return time >= 0.;
  }

  void primary_event::reset ()
  {
    reset_label ();
    time = -1.;
    genbb_weight = 1.0;
    particles.clear ();
    reset_classification ();
    return;
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
    genbb_weight = 1.0;
    return;
  }
  
  // dtor:
  primary_event::~primary_event ()
  {    
    return;
  }

  void primary_event::set_time (double t_)
  {
    time = t_;
    return;
  }

  double primary_event::get_time () const
  {
    return time;
  }

  void primary_event::set_genbb_weight (double genbb_weight_)
  {
    if (genbb_weight_ < 0.0)
      {
        std::ostringstream message;
        message << "genbb::primary_event::set_genbb_weight: Invalid 'genbb_weight' value  (" << genbb_weight_ << " <0) !";
        throw std::logic_error (message.str());
      }
    genbb_weight = genbb_weight_;
    return;
  }

  double primary_event::get_genbb_weight () const
  {
    return genbb_weight;
  }

  bool primary_event::is_genbb_weighted () const
  {
    return genbb_weight == 1.0;
  }

  const primary_event::particles_col_type & primary_event::get_particles () const
  {
    return particles;
  }

  primary_event::particles_col_type & primary_event::get_particles ()
  {
    return particles;
  }

  void primary_event::add_particle (const primary_particle & p_)
  {
    particles.push_back (p_);
    return;
  }

  const std::string & primary_event::get_label () const
  {
    return label;
  }

  void primary_event::set_label (const std::string & l_)
  {
    label = l_;
    return;
  }

  void primary_event::reset_label ()
  {
    label = "";
    return;
  }

  const std::string & primary_event::get_classification () const
  {
    return classification;
  }

  void primary_event::set_classification (const std::string & c_)
  {
    classification = c_;
    return;
  }

  void primary_event::reset_classification ()
  {
    classification = "";
    return;
  }

  void primary_event::compute_classification ()
  {
    size_t n_eminus = 0;
    size_t n_eplus = 0;
    size_t n_gamma = 0;
    size_t n_alpha = 0;
    size_t n_others = 0;
    for (particles_col_type::const_iterator i = particles.begin ();
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
    return;
  }

  void primary_event::tree_dump (std::ostream & out_,
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
    out_ << indent << du::i_tree_dumpable::tag << "Valid: " << is_valid () << std::endl;
    if (is_valid ())
      {
        out_ << indent << du::i_tree_dumpable::tag << "Label : '" << label << "'" << std::endl;
        out_ << indent << du::i_tree_dumpable::tag << "Time  : " << time / CLHEP::second << " s" << std::endl;
        out_ << indent << du::i_tree_dumpable::tag << "Particles: [" << particles.size () << "]" << std::endl;
        int particle_counter = 0;
        for (particles_col_type::const_iterator it = particles.begin ();
             it != particles.end ();
             it++)
          {
            std::ostringstream indent_oss;
            indent_oss << indent << du::i_tree_dumpable::skip_tag;
            particles_col_type::const_iterator jt = it;
            jt++;
            out_ << indent_ << du::i_tree_dumpable::skip_tag; 
            if (jt == particles.end ())
              {
                out_ << du::i_tree_dumpable::last_tag; 
                indent_oss << du::i_tree_dumpable::last_skip_tag;
              }
            else
              {
                out_ << du::i_tree_dumpable::tag; 
                indent_oss << du::i_tree_dumpable::skip_tag;
              }
            out_ << "Particle #" << particle_counter << " : " << std::endl;
            it->tree_dump (out_, "", indent_oss.str());
            particle_counter++;
          }
        out_ << indent << du::i_tree_dumpable::tag
             << "GENBB weight : " << get_genbb_weight () 
             << std::endl;
        out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
             << "Classification : '" << get_classification () 
             << "'" << std::endl;
      }
    else 
      {
        out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
             << "Status: " << "<invalid>" << std::endl;
      }
    return;
  }

  void primary_event::dump (std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_) const
  {
    tree_dump (out_, title_, indent_);
  }
  
  void primary_event::dump (std::ostream & out_,
                            const std::string & indent_) const
  {
    dump (out_, "genbb::primary_event:", indent_); 
    return;
  }

  void primary_event::rotate (double phi_, double theta_, double delta_)
  {
    geomtools::rotation_3d rot;
    geomtools::create_rotation_3d (rot, phi_, theta_, delta_);
    rot.invert ();
    for (particles_col_type::iterator i = particles.begin ();
         i != particles.end ();
         i++)
      {
        primary_particle & p = *i;
        geomtools::vector_3d & momentum = p.momentum;
        geomtools::vector_3d rotated_momentum = rot * momentum;
        momentum = rotated_momentum;
      }

    return;
  }
  
} // end of namespace genbb

// end of primary_event.cc
