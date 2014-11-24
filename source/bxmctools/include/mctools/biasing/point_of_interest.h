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
 * History:
 *
 */

#ifndef MCTOOLS_BIASING_POINT_OF_INTEREST_H
#define MCTOOLS_BIASING_POINT_OF_INTEREST_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/utils.h>

namespace mctools {

  namespace biasing {

    /// \brief A point of interest (POI) geometry
    /**
     *
     *
     *                  ^\
     *                R : \
     *                  : |  Solid angle = 2 pi ( 1 - cos theta)
     *   vertex         : |
     *      + - - - - - - + POI
     *         . theta    |
     *            .       |
     *               .    /
     *                  ./
     *
     *
     *
     */
    class point_of_interest
    {
    public:

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

      /// Set the attractivity
      void set_attractivity(double);

      /// Return the attractivity
      double get_attractivity() const;

      /// Set the radius
      void set_radius(double);

      /// Return the radius
      double get_radius() const;

      /// Check validity
      bool is_valid() const;

      /// Invalidate
      void invalidate();

    protected:

      /// Set the default attributes
      void _set_default();

    private:

      std::string          _name_;         //!< Name of the point of interest
      geomtools::vector_3d _position_;     //!< Position of the point of interest in some reference frame (generally world volume)
      double               _attractivity_; //!< Attractivity of the point of interest
      double               _radius_;       //!< Radius of the attractivity influence

    };

  } // namespace biasing

} // namespace mctools

#endif // MCTOOLS_BIASING_POINT_OF_INTEREST_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
