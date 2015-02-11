// -*- mode: c++; -*-
/// \file geomtools/i_placement.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 *
 * License:
 *
 * Description:
 *  Utilities.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_PLACEMENT_H
#define GEOMTOOLS_I_PLACEMENT_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/reflection_macros.h>
#include <datatools/reflection_macros.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>

namespace geomtools {

  class placement;

  /// \brief Abstract interface for all placement objects
  class i_placement
    : DATATOOLS_SERIALIZABLE_CLASS ,
      public datatools::i_tree_dumpable
  {
  public:

    /// Constructor
    i_placement ();

    /// Destructor
    virtual ~i_placement ();

    bool is_multiple () const;

    placement get_placement (int item_) const;

    /// Same as get_placement(int) but workaround method overloading for CAMP
    placement get_placement_by_index (int item_) const;

    virtual bool has_only_one_rotation () const;

    virtual size_t get_dimension () const = 0;

    virtual size_t get_number_of_items () const = 0;

    virtual size_t compute_index_map (std::vector<uint32_t> & map_, int item_) const = 0;

    virtual bool is_replica () const = 0;

    virtual void get_placement (int item_, placement & p_) const = 0;

    void compute_placement (int item_, placement & p_) const;

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    DATATOOLS_SERIALIZATION_DECLARATION();

    //! Reflection interface
    DR_CLASS_RTTI();

  };

} // end of namespace geomtools

// Activate reflection layer for the geomtools::i_placement class :
DR_CLASS_INIT(::geomtools::i_placement);

/*
// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::i_placement, 0)
*/

#endif // GEOMTOOLS_I_PLACEMENT_H
