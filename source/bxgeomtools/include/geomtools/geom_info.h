/// \file geomtools/geom_info.h
/* Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-30
 * Last modified: 2010-04-30
 *
 * License:
 *
 * Description:
 *
 *   Geometry information about an object from a geometry setup.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_GEOM_INFO_H
#define GEOMTOOLS_GEOM_INFO_H 1

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_predicate.h>

// This project:
#include <geomtools/geom_id.h>
#include <geomtools/placement.h>
#include <geomtools/logical_volume.h>

namespace geomtools {

  // Forward declaration:
  class logical_volume;

  /// \brief A record for geometry information about a physical volume in a virtual geometry
  class geom_info
  {

  public:

    /// Default constructor
    geom_info ();

    /// Constructor
    geom_info (const geom_id &);

    /// Constructor
    geom_info (const geom_id &,
               const placement &,
               const logical_volume &);

    /// Check if a logical volume is defined
    bool has_logical () const;

    /// Return the placement of the physical volume in the world coordinate system
    const placement & get_world_placement () const;

    /// Return the logical volume
    const logical_volume & get_logical () const;

    /// Return the geometry Id
    const geom_id & get_id () const;

    /// Return the geometry Id
    const geom_id & get_gid () const;

    /// Return the geometry Id
    const geom_id & get_geom_id () const;

    /// Return a reference to the non mutable container of auxiliary properties
    const datatools::properties & get_properties () const;

    /// Return a reference to the mutable container of auxiliary properties
    datatools::properties & get_properties ();

    /// Basic print
    friend std::ostream & operator<< (std::ostream &, const geom_info &);

  private:

    datatools::properties  _properties_;      //!< Container of auxiliary properties
    geom_id                _gid_;             //!< Geometry Id
    placement              _world_placement_; //!< Placement of the physical volume in the world coordinate system
    const logical_volume * _logical_;         //!< Handle to the logical volume

  };

  /// \brief Predicate that tests if a geom_info object has a given geometry Id
  struct has_geom_type_predicate : public datatools::i_predicate<geom_info>
  {
    has_geom_type_predicate (uint32_t geom_type_) : _geom_type_ (geom_type_)
    {
      return;
    }
    bool operator () (const geom_info & ginfo_) const override
    {
      return (ginfo_.get_geom_id ().get_type () == _geom_type_);
    }
    uint32_t _geom_type_;
  };

  /// Dictionary of geometry information objects (key type is: geomtools::geom_id)
  typedef std::map<geom_id, geom_info> geom_info_dict_type;

} // end of namespace geomtools

#endif // GEOMTOOLS_GEOM_INFO_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
