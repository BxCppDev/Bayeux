// -*- mode: c++; -*- 
/* primary_event.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-11
 * Last modified: 2013-02-25
 * 
 * License: 
 * Copyright 2007-2013 F. Mauger
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
 * Description: 
 *
 *   Primary event from the GENBB generator.
 *
 * 
 * History: 
 *
 * 2012-06-21 FM : 
 *  - support 'weighted' MC event in case 
 *    of user energy range for DBD events
 *  - serialization version 2 supports new 'weight' attributes
 *
 */

#ifndef GENBB_HELP_PRIMARY_EVENT_H_
#define GENBB_HELP_PRIMARY_EVENT_H_ 1

#include <string>
#include <list>

// Interface base class from datatools to support serialization tools:
#include <datatools/i_serializable.h>

#include <datatools/units.h>
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>

#include <genbb_help/primary_particle.h>

namespace genbb {

  struct primary_event
    : DATATOOLS_SERIALIZABLE_CLASS,
      public datatools::i_tree_dumpable
  {
    
  public:
    typedef std::list<primary_particle> particles_col_type;
    typedef particles_col_type particles_col_t; // Backward comp.

  public:

    bool is_valid () const;

    void reset ();

    void set_time (double);

    double get_time () const;

    void set_genbb_weight (double genbb_weight_);

    double get_genbb_weight () const;

    bool is_genbb_weighted () const;

    const particles_col_type & get_particles () const;

    particles_col_type & grab_particles ();

    particles_col_type & get_particles ();

    void add_particle (const primary_particle &);

    const std::string & get_label () const;

    void set_label (const std::string & l_);

    void reset_label ();

    const std::string & get_classification () const;

    void set_classification (const std::string & c_);

    void reset_classification ();

    void compute_classification ();

    void rotate (double a_phi, double a_theta, double a_delta);

    double get_total_kinetic_energy () const;

    const datatools::properties & get_auxiliaries () const;

    datatools::properties & grab_auxiliaries ();

    /// Constructor 
    primary_event ();
    
    /// Destructor
    virtual ~primary_event ();

    virtual void
    tree_dump (std::ostream      & out_    = std::clog,
               const std::string & title_  = "",
               const std::string & indent_ = "",
               bool inherit_               = false) const;
    
    void dump (std::ostream & a_out = std::clog,
               const std::string & a_indent = "") const;

    void dump (std::ostream & a_out,
               const std::string & a_title,
               const std::string & a_indent) const;
    
  private:

    std::string           _label_;
    double                _time_;
    particles_col_type    _particles_;
    std::string           _classification_;
    double                _genbb_weight_;
    datatools::properties _auxiliaries_;

  //! Support for Boost-based serialization
  DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(primary_event)

  //! Support for backward compatibility serialization tag
  DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

  };

} // end of namespace genbb

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(genbb::primary_event, "genbb::primary_event")

DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::genbb::primary_event)

#endif // GENBB_HELP_PRIMARY_EVENT_H_

// end of primary_event.h
