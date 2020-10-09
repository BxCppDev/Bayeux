/// \file geomtools/multiple_placement.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 *
 * License:
 *
 * Description:
 *  Multiple placement.
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

  //! \brief Multiple placement with an arbitrary collection of single placements addressed by index
  class multiple_placement : public i_placement
  {
  public:

    //! \brief Type alias for a collection of single placements
    typedef std::vector<placement> placement_col_type;

    bool is_valid() const;

    void invalidate();

    void add(const placement & p_);

    const placement & get_placement(int index_) const;

    placement & get_placement(int index_);

    size_t get_dimension() const override;

    bool is_replica() const override;

    size_t get_number_of_items() const override;

    void get_placement(int item_, placement & p_) const override;

    size_t compute_index_map(std::vector<uint32_t> & map_,
                                     int item_) const override;

    /// Default constructor
    multiple_placement();

    /// Destructor
    ~multiple_placement() override;

    /// Reset
    virtual void reset();

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "geomutils::multiple_placement",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  private:

    placement_col_type _placements_; //!< Collection of single placements

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_MULTIPLE_PLACEMENT_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
