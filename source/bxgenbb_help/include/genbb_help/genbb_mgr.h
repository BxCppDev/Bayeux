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
#include <datatools/utils/properties.h>
#include <datatools/utils/utils.h>

#include <geomtools/utils.h>

#include <genbb_help/i_genbb.h>

namespace genbb {

  using namespace std;

  class genbb_mgr : public i_genbb
  {
  public:

    enum format_t
      {
	FORMAT_GENBB = 0,
	FORMAT_BOOST = 1
      };

    static const string FORMAT_GENBB_LABEL;
    static const string FORMAT_BOOST_LABEL;

  private:
    bool __debug;
    bool __initialized;
    list<string> __filenames;
    string      __current_filename;
    int         __format;
    istream * __in;
    ifstream  __fin;
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

    void set_format (const string & format_ = FORMAT_GENBB_LABEL);

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
  
    void set (const string & filename_);

    void init ();

    void initialize ();

    void initialize (const datatools::utils::properties & config_);

    void reset ();

    virtual bool has_next ();

    virtual void load_next (primary_event & event_, 
			    bool compute_classification_ = true);

    void dump (ostream & out_ = clog) const;

  private:

    void __at_init ();

    void __at_reset ();

    void __load_next ();

    void __load_next_genbb ();

    void __load_next_boost ();

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_mgr_h

// end of genbb_mgr.h
