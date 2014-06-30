// -*- mode: c++; -*-
/// \file geomtools/multiple_placement.h
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

#ifndef GEOMTOOLS_MULTIPLE_PLACEMENT_H
#define GEOMTOOLS_MULTIPLE_PLACEMENT_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// This project:
#include <geomtools/placement.h>

namespace geomtools {

  class multiple_placement
    : public i_placement
  {
  public:

    typedef std::vector<placement> placement_col_type;

  public:

    bool is_valid () const;

    void invalidate ();

    void add (const placement & p_);

    const placement & get_placement (int index_) const;

    placement & get_placement (int index_);

  public:

    virtual size_t get_dimension () const;

    virtual bool is_replica () const;

    virtual size_t get_number_of_items () const;

    virtual void get_placement (int item_, placement & p_) const;

    virtual size_t compute_index_map (std::vector<uint32_t> & map_,
                                      int item_) const;

  public:

    /// Default constructor
    multiple_placement ();

    /// Destructor
    virtual ~multiple_placement ();

    virtual void reset ();

    /// Smart print
    virtual void tree_dump (std::ostream & out_ = std::clog,
                            const std::string & title_ = "geomutils::multiple_placement",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const;

  private:

    placement_col_type _placements_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_MULTIPLE_PLACEMENT_H
