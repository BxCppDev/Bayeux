// -*- mode: c++; -*- 
/* wgenbb.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 
 * 
 * License: 
 * 
 * Description: 
 *   C++ wrapper GENBB generator
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__wgenbb_h
#define __genbb_help__wgenbb_h 1

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>

#include <genbb_help/i_genbb.h>
#include <genbb_help/genbb_commons.h>

#include <mygsl/rng.h>

#include <datatools/utils/properties.h>
#include <datatools/utils/utils.h>
#include <datatools/utils/units.h>
#include <boost/filesystem.hpp>

#include <cfortran/cfortran.h> 

extern "C" 
{ 
  void set_random_seed (int *); 

  void genbbparprint (void); 

  void genbbsub (int *, 
		 const char *,
		 int *,
		 int *,
		 int *,
		 int *); 

  void grndm (float *, int *); 

  void genbbinit (void); 

} 

typedef GENEVENT_t GENEVENT_DEF;
typedef ENRANGE_t ENRANGE_DEF;

namespace genbb {

  using namespace std;

  class wgenbb
  {
  public:

    enum decay_type_t
    {
      DECAY_TYPE_UNDEFINED = 0,
      DECAY_TYPE_DBD = 1,
      DECAY_TYPE_BACKGROUND = 2
    };

  private:

    static size_t __g_counter;

  private:
    bool   __debug;
    bool   __initialized;

    int    __decay_type;
    string __decay_isotope;  
    char   __c_decay_isotope[32];
    int    __decay_dbd_level;  
    int    __decay_dbd_mode;  
    size_t __event_count;

    double __energy_min;
    double __energy_max;

    unsigned long __seed;
    mygsl::rng    __random;

  private:

    void __set_decay_isotope (const string & di_);

  public:

    bool is_debug () const;

    size_t get_event_count () const;

    double get_to_all_events () const;

    wgenbb ();

    virtual ~wgenbb ();

    void initialize (const datatools::utils::properties & config_);

    void reset ();

    virtual bool has_next ();

    virtual void load_next (primary_event & event_, 
			    bool compute_classification_ = true);

    void dump (ostream & = clog) const;

  private:

    void __init ();

    void __clean ();

  };

} // end of namespace genbb

#endif // __genbb_help__wgenbb_h

// end of wgenbb.h
