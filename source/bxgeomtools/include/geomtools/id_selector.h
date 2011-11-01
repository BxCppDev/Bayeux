// -*- mode: c++ ; -*- 
/* id_selector.h
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
 
#ifndef __geomtools__id_selector_h
#define __geomtools__id_selector_h 1

#include <string>
#include <map>

#include <geomtools/id_mgr.h>
#include <geomtools/address_set.h>

namespace geomtools {

  using namespace std;

  class geom_id;

  class id_selector
  {

  public:

    bool is_initialized () const;

    void set_id_mgr (const id_mgr & id_mgr_);

    const id_mgr::category_info & get_category_info () const;

    id_selector ();

    id_selector (const id_mgr & id_mgr_);

    ~id_selector ();

    void initialize (const string & rules_);

    void reset ();

    bool match (const geom_id & gid_) const;

    void dump (ostream & out_ = clog, const string & title_ = "") const;

  private:

    const id_mgr                * _id_mgr_; //!< Handle to a geometry ID manager
    const id_mgr::category_info * _cat_info_; //!< Handle to some 'category info' object
    map<int, address_set> _addr_sets_; //!< Collection of address sets

  };

} // end of namespace geomtools


#endif // __geomtools__id_selector_h


// end of id_selector.h



