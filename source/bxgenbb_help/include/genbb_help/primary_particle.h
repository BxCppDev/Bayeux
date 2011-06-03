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

#ifndef __genbb_help__primary_particle_h
#define __genbb_help__primary_particle_h 1

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

#include <datatools/utils/utils.h>

// Interface base class from datatools to support serialization tools:
#include <datatools/serialization/i_serializable.h>

#include <geomtools/utils.h>

namespace genbb {

  using namespace std;

  struct primary_particle 
    : public datatools::serialization::i_serializable 
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

    const string & get_particle_label () const;

    // Only if particle type == UNDEF:
    void set_particle_label (const string &);

    void set_time (double time_);

    bool is_valid () const;

    bool is_gamma () const
    {
      return get_type () == GAMMA;
    }

    bool is_positron () const
    {
      return get_type () == POSITRON;
    }

    bool is_electron () const
    {
      return get_type () == ELECTRON;
    }

    bool is_alpha () const
    {
      return get_type () == ALPHA;
    }

    bool is_neutron () const
    {
      return get_type () == NEUTRON;
    }

    void set_momentum (const geomtools::vector_3d & m_);

    const geomtools::vector_3d & get_momentum () const;

    double get_charge () const; // in unit of e

    double get_mass () const; // in unit of energy

    double get_total_energy () const;

    double get_kinetic_energy () const;

    double get_beta () const;

    // ctor:
    primary_particle ();

    // ctor:
    primary_particle (int32_t type_, 
		      double time_, 
		      const geomtools::vector_3d &);

    // dtor:
    virtual ~primary_particle ();

    void dump (ostream & out_ = clog, 
	       const string & indent_ = "") const;
 
    static string get_label (int type_);

    static string get_particle_label_from_type (int type_);
   
    static int get_particle_type_from_label (const string & label_);

  public:
    
    //! Serialization tag.
    DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

    static const std::string OLD_SERIAL_TAG;

  private:

    friend class boost::serialization::access; 
    BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

  public:
    
    int32_t              type;
    string               particle_label;
    double               mass;     // CLHEP energy unit
    double               time;     // CLHEP time unit
    geomtools::vector_3d momentum; // CLHEP momentum unit

  };

} // end of namespace genbb

#endif // __genbb_help__primary_particle_h

// end of primary_particle.h
