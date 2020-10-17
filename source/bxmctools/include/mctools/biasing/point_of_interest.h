/// \file mctools/biasing/point_of_interest.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-21
 * Last modified: 2014-11-24
 *
 * License:
 *
 * Description:
 *
 *   A point of interest in the geometry
 *
 */

#ifndef MCTOOLS_BIASING_POINT_OF_INTEREST_H
#define MCTOOLS_BIASING_POINT_OF_INTEREST_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>
// #include <geomtools/placement.h>
// #include <geomtools/blur_spot.h>

namespace datatools {
  // Forward declaration:
  class properties;
}

namespace geomtools {
  // Forward declaration:
  class manager;
}

namespace mctools {

  namespace biasing {

    /// \brief A point of interest (POI) in the geometry
    /** A point of interest (POI) is defined by a special position
     *  of the geometry setup associated to a radius that defines
     *  a fiducial spherical region around that position.
     *  More, a POI is given an \a attractivity value that reflects
     *  that the POI is *attractive* (positive values)
     *  or *repulsive* (negative value) typically with respect to the tracking
     *  of simulated particles. A zero attractivity qualifies
     *  a *neutral* POI.
     *
     *  The concept of POI allows to define some favoured tracking direction
     *  while propagating particles within the geometry. It can be used to
     *  configure biasing algorithms.
     *
     *  \code
     *                                            incoming
     *                                              particles
     *             _.-"""""-._                 ....+  from a vertex
     *           .'           `.       ....""""     \
     *          /               \ L""""              \
     *         |         POI     | towards the POI    \
     *         |        +        |     region          \
     *         |         \ R     |                      \ far from the POI
     *          \         \     /                        \   region
     *           `._       \ _.'                         _\/
     *              `-.....-'
     *  \endcode
     *
     *  TO DO: Use a 2D/3D oriented blur_spot for better flexibility
     *
     */
    class point_of_interest
      : public datatools::i_tree_dumpable
    {
    public:

      enum attractive_shape_type {
        ATTRACTIVE_SHAPE_INVALID = 0,
        ATTRACTIVE_SHAPE_SPHERE  = 1,
        ATTRACTIVE_SHAPE_DISC    = 2
      };

      /// Default constructor
      point_of_interest();

      /// Set the name
      void set_name(const std::string &);

      /// Return the name
      const std::string & get_name() const;

      /// Set the position
      void set_position(const geomtools::vector_3d &);

      /// Return the position
      const geomtools::vector_3d & get_position() const;

      /// Return the position
      geomtools::vector_3d & grab_position();

      /// Set the orientation
      void set_orientation(const geomtools::vector_3d &);

      /// Return the orientation
      const geomtools::vector_3d & get_orientation() const;

      /// Set the attractivity
      void set_attractivity(double);

      /// Return the attractivity
      double get_attractivity() const;

      /// Check if the point of interest has a defined attractivity
      bool has_attractivity() const;

      /// Check if the point of interest is attractive
      bool is_attractive() const;

      /// Check if the point of interest is repulsive
      bool is_repulsive() const;

      /// Check if the point of interest has zero attractivity
      bool is_neutral() const;

      /// Set the radius
      void set_radius(double);

      /// Return the radius
      double get_radius() const;

      /// Check validity
      bool is_valid() const;

      /// Invalidate
      void invalidate();

      /// Initialization
      void initialize(const datatools::properties & config_, const geomtools::manager * geomgr_ = 0);

      /// Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const override;

      /// Check if a linear trajectory coming from a given source with a given direction traverse the sphere
      bool hit(const geomtools::vector_3d & source_, const geomtools::vector_3d & direction_) const;

    protected:

      /// Set the default attributes
      void _set_default();

    private:

      std::string          _name_;         //!< Name of the point of interest
      // geomtools::placement _placement_; //!< Placement of the spot
      // geomtools::blur_spot _spot_;      //!< Spot
      geomtools::vector_3d _position_;     //!< Position of the point of interest in some reference frame (generally the world volume)
      attractive_shape_type _attractive_shape_ = ATTRACTIVE_SHAPE_INVALID;  //!< Attractive shape 
      geomtools::vector_3d _orientation_;  //!< Orientation of the point of interest in some reference frame (generally the world volume)
      double               _radius_;       //!< Radius of the attractivity influence
      double               _attractivity_; //!< Attractivity of the point of interest
      bool                 _skip_check_inside_ = false;
      
    };

  } // namespace biasing

} // namespace mctools

#endif // MCTOOLS_BIASING_POINT_OF_INTEREST_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
