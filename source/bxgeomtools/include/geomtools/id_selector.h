// -*- mode: c++ ; -*- 
/* is_selector.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 * 
 * License: 
 * 
 * Description: 
 *   A selector for geometry ID
 * 
 */
 
#ifndef __geomtools__is_selector_h
#define __geomtools__id_selector_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <geomtools/address_set.h>
#include <geomtools/id_mgr.h>

namespace geomtools {

  using namespace std;


  class id_selector
  {
  private:

    const id_mgr                * __id_mgr; 
    const id_mgr::category_info * __cat_info;
    map<int, address_set> __addr_sets;

  public:

    bool is_initialized () const;

    id_selector ();

    id_selector (const id_mgr & id_mgr_);

    ~id_selector ();

    void initialize (const string & rules_);

    void reset ();

    bool match (const geom_id & gid_) const;

    void dump (ostream & out_ = clog, const string & title_ = "") const;

  };

} // end of namespace geomtools


#endif // __geomtools__id_selector_h


// end of id_selector.h



