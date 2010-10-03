// -*- mode: c++; -*- 
/* primary_particle.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-11
 * Last modified: 2010-04-11
 * 
 * License: 
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

#include <boost/cstdint.hpp>

#include <geomtools/utils.h>

#include <datatools/serialization/serialization.h>
#include <datatools/utils/utils.h>

namespace genbb {

  using namespace std;

  struct primary_particle 
    : public datatools::serialization::i_serializable 
  {
  public:
    
    static const string SERIAL_TAG;
    
    virtual const string & get_serial_tag () const;
    
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
    
    int32_t              type;
    string               particle_label;
    double               mass;     // CLHEP energy unit
    double               time;     // CLHEP time unit
    geomtools::vector_3d momentum; // CLHEP momentum unit

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

  private:
    friend class boost::serialization::access; 
    template<class Archive>
      void serialize (Archive            & ar_, 
		      const unsigned int   version_)
      {
	ar_ & boost::serialization::make_nvp ("type", type);
	if (type  == UNDEF)
	  {
	    ar_ & boost::serialization::make_nvp ("particle_label", particle_label);
	  }
	else
	  {
	    particle_label = get_particle_label_from_type (type);
	  }
	ar_ & boost::serialization::make_nvp ("time", time);
	ar_ & boost::serialization::make_nvp ("momentum", momentum);
      }

  public:
 
    static string get_label (int type_);

    static string get_particle_label_from_type (int type_);
   
    static int get_particle_type_from_label (const string & label_);

  };

} // end of namespace genbb

#endif // __genbb_help__primary_particle_h

// end of primary_particle.h
