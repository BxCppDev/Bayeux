// -*- mode: c++; -*- 
/* genbb_mgr.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2009-01-19
 * 
 * License: 
 * 
 * Description: 
 *   Manager for GENBB input data
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__genbb_mgr_h
#define __genbb_help__genbb_mgr_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Vector/ThreeVector.h>

namespace genbb {

  struct primary_particle
  {
    int        type;
    double     time;
    Hep3Vector momentum;
    bool is_gamma () const
    {
      return type == 1;
    }
    bool is_electron () const
    {
      return type == 3;
    }
    bool is_positron () const
    {
      return type == 2;
    }
    bool is_alpha () const
    {
      return type == 47;
    }

    static std::string get_label (int type_);

    void dump (std::ostream & out_ = std::clog) const;

  };
  
  struct primary_event
  {
    typedef std::list<primary_particle> particles_col_t;

    double          time;
    particles_col_t particles;

    primary_event ()
    {
      time = -1.;
    }

    bool is_valid () const
    {
      return time >= 0.;
    }

    void reset ()
    {
      time = -1.;
      particles.clear ();
    }

    void dump (std::ostream & out_ = std::clog) const;

  };

  class genbb_mgr
  {
    bool __debug;
    bool __initialized;
    std::list<std::string> __filenames;
    std::istream * __in;
    std::ifstream  __fin;
    primary_event  __current;

  // ctor/dtor:
  public: 

    bool is_debug () const
    {
      return __debug;
    }

    void set_debug (bool d_)
    {
      __debug = d_;
    }

    genbb_mgr ();

    virtual ~genbb_mgr ();
  
    void set (const std::string & filename_);

    void init ();

    void reset ();

    bool has_next ();

    void load_next (primary_event & event_);

    void dump (std::ostream & out_ = std::clog) const;

  private:

    void __load_next ();

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_mgr_h

// end of genbb_mgr.h
