// -*- mode: c++; -*- 
/* genbb_mgr.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2010-04-11
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

#include <boost/cstdint.hpp>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Vector/ThreeVector.h>

#include <datatools/serialization/serialization.h>
#include <datatools/serialization/io_factory.h>

#include <geomtools/utils.h>

#include <genbb_help/i_genbb.h>

namespace genbb {

  class genbb_mgr : public i_genbb
  {
  public:

    enum
      {
	FORMAT_GENBB = 0,
	FORMAT_BOOST = 1
      };

    static const std::string FORMAT_GENBB_LABEL;
    static const std::string FORMAT_BOOST_LABEL;

  private:
    bool __debug;
    bool __initialized;
    std::list<std::string> __filenames;
    int            __format;
    std::istream * __in;
    std::ifstream  __fin;
    datatools::serialization::data_reader __reader;
    primary_event  __current;

  public: 

    bool is_initialized () const;

    bool is_debug () const
    {
      return __debug;
    }

    void set_debug (bool d_)
    {
      __debug = d_;
    }

    int get_format () const
    {
      return __format;
    }

    void set_format (int format_ = FORMAT_GENBB);

    void set_format (const std::string & format_ = FORMAT_GENBB_LABEL);

    bool is_format_genbb () const
    {
      return __format == FORMAT_GENBB;
    }

    bool is_format_boost () const
    {
      return __format == FORMAT_BOOST;
    }

    // ctor:
    genbb_mgr (int format_ = FORMAT_GENBB);
    
    // dtor:
    virtual ~genbb_mgr ();
  
    void set (const std::string & filename_);

    void init ();

    void initialize ();

    void reset ();

    virtual bool has_next ();

    virtual void load_next (primary_event & event_, 
			    bool compute_classification_ = true);

    void dump (std::ostream & out_ = std::clog) const;

  private:

    void __load_next ();

    void __load_next_genbb ();

    void __load_next_boost ();

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_mgr_h

// end of genbb_mgr.h
