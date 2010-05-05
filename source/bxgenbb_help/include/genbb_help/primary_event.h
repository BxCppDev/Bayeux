// -*- mode: c++; -*- 
/* primary_event.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-11
 * Last modified: 2010-04-11
 * 
 * License: 
 * 
 * Description: 
 *   Primary event from the GENBB generator
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__primary_event_h
#define __genbb_help__primary_event_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>

#include <boost/cstdint.hpp>

#include <geomtools/utils.h>

#include <datatools/serialization/serialization.h>
#include <datatools/utils/units.h>

#include <genbb_help/primary_particle.h>

namespace genbb {

  struct primary_event
    : public datatools::serialization::i_serializable 
  {
  public:
    
    static const std::string SERIAL_TAG;
    
    virtual const std::string & get_serial_tag () const;
    
  public:
    typedef std::list<primary_particle> particles_col_t;

    double          time;
    particles_col_t particles;
    std::string     classification;

  public:

    bool is_valid () const;

    void reset ();

    void set_time (double);

    double get_time () const;

    const particles_col_t & get_particles () const;

    particles_col_t & get_particles ();

    void add_particle (const primary_particle &);

    const std::string & get_classification () const;

    void set_classification (const std::string & c_);

    void reset_classification ();

    void compute_classification ();

    double get_total_kinetic_energy () const;

    // ctor:
    primary_event ();
    
    // dtor:
    virtual ~primary_event ();

    void dump (std::ostream & out_ = std::clog,
	       const std::string & indent_ = "") const;

    void dump (std::ostream & out_,
	       const std::string & title_,
	       const std::string & indent_) const;

  private:

    friend class boost::serialization::access; 
    template<class Archive>
    void serialize (Archive            & ar_, 
                    const unsigned int   version_)
    {
      ar_ & boost::serialization::make_nvp ("time", time);
      ar_ & boost::serialization::make_nvp ("particles", particles);
      ar_ & boost::serialization::make_nvp ("classification", classification);
    }

  };

} // end of namespace genbb

#endif // __genbb_help__primary_event_h

// end of primary_event.h
