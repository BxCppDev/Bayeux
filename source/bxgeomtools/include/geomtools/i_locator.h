/// \file geomtools/i_locator.h
/* Author(s):  Francois Mauger <mauger@lpccaen.in2p3.fr>
 *             Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2010-02-09
 * Last modified: 2013-07-16
 *
 * License:
 *
 * Description:
 *
 *   Geometry locator abstract interface
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_LOCATOR_H
#define GEOMTOOLS_I_LOCATOR_H 1

// This project:
#include <geomtools/clhep.h>
#include <geomtools/geomtools_config.h>
#include <geomtools/geom_id.h>

namespace geomtools {

  class geom_info;

  /// An abstract interface for simple locators that build the geometry ID
  /// associated to some position for some geometry type.
  class i_base_locator
  {
    virtual bool find_geom_id (const vector_3d & position_,
                               int type_,
                               geom_id & gid_,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

  };

  /// An abstract interface for locators based on some internal lookup table.
  /// Its methods return some reference to some stored geometrical meta data.
  class i_locator : public i_base_locator
  {

  public:

    /// Given a geometry ID, check if it is valid (known within some internal table).
    virtual bool validate_id (const geom_id & id_) const = 0;

    /// Given a geometry ID, returns the associated geom_info instance (from some internal table).
    virtual const geom_info & get_geom_info (const geom_id & id_) const = 0;


    /// Given a position and the type of some geometry volume, returns the
    /// associated geom_id instance (from some internal table).
    virtual const geom_id & get_geom_id (const vector_3d & position_,
                                         int type_ = geom_id::INVALID_TYPE,
                                         double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Given a position and the type of some geometry volume, returns if
    /// position is inside and the associated geom_id
    bool find_geom_id (const vector_3d & position_,
                               int type_,
                               geom_id & gid_,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

  };

  class manager;

  class base_locator : public i_base_locator
  {
  public:

    /// Constructor
    base_locator ();

    /// Destructor
    virtual ~base_locator ();

    /// Main initialization method from a list of properties
    virtual void initialize (const datatools::properties & config_) = 0;

    /// Check if there is a geometry manager
    bool has_geo_manager() const;

    /// Set the reference to the geometry manager
    void set_geo_manager (const manager & mgr_);

    /// Get a non-mutable reference to the geometry manager
    const manager & get_geo_manager () const;

    /// Main reset method
    virtual void reset () = 0;

    /// Check if locator is initialized
    virtual bool is_initialized () const = 0;

    /// Get the logging priority threshold
    datatools::logger::priority get_logging_priority () const;

    /// Set the logging priority threshold
    void set_logging_priority (datatools::logger::priority);

    /// Given a position and the type of some geometry volume, returns if
    /// position is inside and the associated geom_id
    bool find_geom_id (const vector_3d & position_,
                               int type_,
                               geom_id & gid_,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override = 0;

  protected:

    void _basic_initialize (const datatools::properties & config_);

    datatools::logger::priority _logging_priority;

  private:

    const geomtools::manager * _geo_manager_;
  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_LOCATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
