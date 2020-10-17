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
#include <datatools/reflection_interface.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>

namespace geomtools {

  // Forward class declaration
  class placement;

  //! \brief Abstract interface for all placement objects
  class i_placement
    : public datatools::i_serializable,
      public datatools::i_tree_dumpable
  {
  public:

    //! Default constructor
    i_placement();

    //! Destructor
    ~i_placement() override;

    //! Check if the placement handles multiple positions
    bool is_multiple() const;

    //! Return the placement at given item rank/index
    placement get_placement(int item_) const;

    //! Same as get_placement(int) but workaround method overloading for CAMP
    placement get_placement_by_index(int item_) const;

    //! Check if the placement is based on one unique rotation
    virtual bool has_only_one_rotation() const;

    //! Return the dimension of the placement object
    //!
    //! A single unique placement is of dimension 0
    //! A multiple placement replicated along a given axis is of dimension 0
    //! A multiple placement replicated along a 2D mesh addresses by X-Y grid
    //! coordinates is of dimension 2
    virtual size_t get_dimension() const = 0;

    //! Return the number of placement items
    virtual size_t get_number_of_items() const = 0;

    //! Compute an array of indexes in a multidimensional frame from the item rank/index
    virtual size_t compute_index_map(std::vector<uint32_t> & map_, int item_) const = 0;

    //! Check if the placement is a replica (GDML/Geant4 concept for multiple placement objects)
    virtual bool is_replica() const = 0;

    //! Compute the placement at given item rank/index
    virtual void get_placement(int item_, placement & p_) const = 0;

    //! Compute the placement at given item rank/index
    void compute_placement(int item_, placement & p_) const;

    //! Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION()

    //! Reflection interface
    DR_CLASS_RTTI()

  };

} // end of namespace geomtools

// Activate reflection layer for the geomtools::i_placement class :
DR_CLASS_INIT(::geomtools::i_placement)

/*
// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::i_placement, 0)
*/

#endif // GEOMTOOLS_I_PLACEMENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
