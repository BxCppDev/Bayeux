/// \file geomtools/id_selector.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 *
 * License:
 *
 * Description:
 *   A selector for geometry ID
 *
 */

#ifndef GEOMTOOLS_ID_SELECTOR_H
#define GEOMTOOLS_ID_SELECTOR_H 1

// Standard library:
#include <string>
#include <map>

// This project:
#include <geomtools/id_mgr.h>
#include <geomtools/address_set.h>

namespace geomtools {

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

    void initialize (const std::string & rules_);

    void reset ();

    bool match (const geom_id & gid_) const;

    void dump (std::ostream & out_ =std:: clog, const std::string & title_ = "") const;

  private:

    const id_mgr                * _id_mgr_; //!< Handle to a geometry ID manager
    const id_mgr::category_info * _cat_info_; //!< Handle to some 'category info' object
    std::map<int, address_set> _addr_sets_; //!< Collection of address sets

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_ID_SELECTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
