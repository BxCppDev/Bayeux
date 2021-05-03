//! \file  geomtools/foot_point_on_quadric_finder.h
//! \brief Find a point on a quadric surface
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of geomtools.
//
// geomtools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// geomtools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with geomtools. If not, see <http://www.gnu.org/licenses/>.

#ifndef GEOMTOOLS_FOOT_POINT_ON_QUADRIC_FINDER_H
#define GEOMTOOLS_FOOT_POINT_ON_QUADRIC_FINDER_H

// This project:
#include <geomtools/quadric.h>

namespace geomtools {

  //! \brief Marching method to find the foot point of a point on the surface of a quadric
  struct foot_point_on_quadric_finder
  {
  public:

    //! Return default alpha max
    static double default_alpha_max();

    //! Return default max number of iterations
    static unsigned int default_max_iter();

    //! Set the absolute tolerance (length unit)
    void set_epsilon(double);

    //! Set the maximum value for alpha (dimensionless, default: 20)
    void set_alpha_max(double);

    //! Constructor
    foot_point_on_quadric_finder(const quadric & q_,
                                 double epsilon_,
                                 double alpha_max_ = -1.0,
                                 unsigned int max_iter_ = 20,
                                 const std::string & dbg_filename_ = "");

    //! Main foot point finder algorithm
    int find(const geomtools::vector_3d & start_,
             geomtools::vector_3d & foot_point_);

  protected:

    void _init();

  private:

    const quadric * _quadric_ = nullptr; //!< Referenced quadric
    double _epsilon_; //!< Tolerance (expected dimension of a length or the dimension of the traversed space)
    double _alpha_max_; //!< Maximum (dimensionless)
    unsigned int _max_iter_; //!< Maximum number of iterations
    bool _debug_ = false; //!< Debug flag
    std::string _debug_filename_; //!< Debug filename

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_POINT_ON_QUADRIC_FINDER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
