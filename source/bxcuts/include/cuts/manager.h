// -*- mode: c++ ; -*- 
/* manager.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-19
 * Last modified: 2010-09-19
 * 
 * License: 
 * 
 * Description: 
 *   Cut manager: 
 *
 * History: 
 * 
 */

#ifndef __cuts__manager_h
#define __cuts__manager_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <cuts/factory.h>
#include <datatools/utils/multi_properties.h>

namespace cuts {

  using namespace std;

  class manager
  {
  public:

  private:
    bool       __debug;
    factory *  __factory;
    cut_dict_t __cuts;

  public:

    bool is_debug () const;

    void set_debug (bool);

    manager ();

    virtual ~manager ();

    void initialize (const datatools::utils::multi_properties & config_);

    void reset ();

    bool has_cut (const string & cut_name_) const;

    i_cut * get_cut (const string & cut_name_);

  };

} // end of namespace cuts

#endif // __cuts__manager_h

// end of manager.h
