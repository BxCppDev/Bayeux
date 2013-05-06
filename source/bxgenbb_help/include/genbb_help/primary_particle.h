// -*- mode: c++; -*-
/* primary_particle.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-11
 * Last modified: 2010-04-11
 *
 * License:
 * Copyright 2007-2011 F. Mauger
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Description:
 *   Primary particle for GENBB generator
 *
 * History:
 *
 */

#ifndef GENBB_HELP_PRIMARY_PARTICLE_H_
#define GENBB_HELP_PRIMARY_PARTICLE_H_ 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>

// Portable integral types (mandatory):
#include <boost/cstdint.hpp>

#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/i_tree_dump.h>

// Interface base class from datatools to support serialization tools:
#include <datatools/i_serializable.h>

#include <geomtools/utils.h>

namespace genbb {

  struct primary_particle
    : DATATOOLS_SERIALIZABLE_CLASS,
      public datatools::i_tree_dumpable
  {
  public:

    enum particle_type
    {
      // Using GEANT3 definition from:
      // http://wwwasdoc.web.cern.ch/wwwasdoc/geant_html3/node72.html#SECTION024000000000000000000000
      UNDEF      = -1,
      PARTICLE_UNDEFINED = -1,
      GAMMA      = 1,
      POSITRON   = 2,
      ELECTRON   = 3,
      NEUTRINO   = 4,
      MUON_PLUS  = 5,
      MUON_MINUS = 6,
      PION_0     = 7,
      PION_PLUS  = 8,
      PION_MINUS = 9,
      NEUTRON    = 13,
      PROTON     = 14,
      DEUTERON   = 45,
      TRITIUM    = 46,
      ALPHA      = 47,
      HE3        = 49
    };

  public:

    void reset ();

    int get_type () const;

    void set_type (int type_);

    double get_time () const;

    double & grab_time ();

    const std::string & get_particle_label () const;

    // Only if particle type == UNDEF:
    void set_particle_label (const std::string &);

    void set_time (double time_);

    bool is_valid () const;

    bool is_gamma () const;

    bool is_positron () const;

    bool is_electron () const;

    bool is_alpha () const;

    bool is_neutron () const;

    bool is_muon_plus () const;

    bool is_muon_minus () const;

    void set_momentum (const geomtools::vector_3d & m_);

    const geomtools::vector_3d & get_momentum () const;

    geomtools::vector_3d & grab_momentum ();

    void set_vertex (const geomtools::vector_3d & v_);

    const geomtools::vector_3d & get_vertex () const;

    geomtools::vector_3d & grab_vertex ();

    bool has_vertex () const;

    void invalidate_vertex ();

    double get_charge () const; // in unit of e

    double get_mass () const; // in unit of energy

    bool mass_is_known () const;

    double get_total_energy () const;

    double get_kinetic_energy () const;

    double get_beta () const;

    /// Constructor
    primary_particle ();

    /// Constructor
    primary_particle (int32_t a_type,
                      double a_time,
                      const geomtools::vector_3d &);

    /// Destructor
    virtual ~primary_particle ();

    virtual void
    tree_dump (std::ostream      & out_    = std::clog,
               const std::string & title_  = "",
               const std::string & indent_ = "",
               bool inherit_               = false) const;

    void dump (std::ostream & out_ = std::clog,
               const std::string & a_indent = "") const;

    static std::string get_label (int a_type);

    static std::string get_particle_label_from_type (int a_type);

    static int get_particle_type_from_label (const std::string & a_label);

  private:

    int32_t              _type_;
    std::string          _particle_label_;
    double               _mass_;     // CLHEP energy unit
    double               _time_;     // CLHEP time unit
    geomtools::vector_3d _momentum_; // CLHEP momentum unit
    geomtools::vector_3d _vertex_;   // CLHEP position unit

    //! Support for Boost-based serialization
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(primary_particle)

    //! Support for backward compatibility serialization tag
    DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

  };

} // end of namespace genbb

DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::genbb::primary_particle)

#endif // GENBB_HELP_PRIMARY_PARTICLE_H_

// end of primary_particle.h
