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

  /// \brief A selector of geometry IDs using a specific geometry category
  ///
  /// The selector is given a textual rule that describes:
  ///  - the geometry category of objects to be selected
  ///  - some additional filtering informations to select a subset of objects
  ///    in the requested category through some specific subaddresses
  class id_selector
  {

  public:

    /// Default constructor
    id_selector();

    /// Constructor
    id_selector(const id_mgr & id_mgr_);

    /// Destructor
    ~id_selector();

    /// Check initialization status
    bool is_initialized() const;

    /// Initialize the selector frop a textual rule
    void initialize(const std::string & rules_);

    /// Initialize the selector frop a textual rule
    void initialize(const datatools::properties & config_);

    /// Reset the selector
    void reset();

    /// Set the GID manager
    void set_id_mgr(const id_mgr & id_mgr_);

    /// Get the information associated to the requested geometry category
    const id_mgr::category_info & get_category_info() const;

    /// Check if a geometry ID match the selection
    bool match(const geom_id & gid_) const;

    /// Print
    void dump(std::ostream & out_ = std::clog, const std::string & title_ = "") const;

    /// Print
    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_ = "",
                   const std::string & indent_ = "") const;

  private:

    const id_mgr                * _id_mgr_;    //!< Handle to a geometry ID manager
    const id_mgr::category_info * _cat_info_;  //!< Handle to some 'category info' object
    std::map<int, address_set>    _addr_sets_; //!< Collection of address sets

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
