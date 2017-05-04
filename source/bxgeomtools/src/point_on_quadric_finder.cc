// geomtools/point_on_quadric_finder.cc
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

// Ourselves:
#include <geomtools/point_on_quadric_finder.h>

// Standard library:
#include <fstream>
#include <memory>

namespace geomtools {

  point_on_quadric_finder::point_on_quadric_finder(const quadric & q_,
                                                   double epsilon_,
                                                   unsigned int max_iter_,
                                                   const std::string & dbg_filename_)
  {
    _quadric_ = &q_;
    _epsilon_ = epsilon_;
    _max_iter_ = max_iter_;
    _debug_filename_ = dbg_filename_;
    _debug_ = false;
    if (!_debug_filename_.empty()) {
      _debug_ = true;
    }
    return;
  }

  int point_on_quadric_finder::find(const geomtools::vector_3d & start_,
                                    geomtools::vector_3d & on_surface_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    if (_debug_) logging = datatools::logger::PRIO_DEBUG;
    int error_code = EXIT_SUCCESS;
    geomtools::vector_3d q0 = start_;
    bool go_on = true;
    unsigned int niter = 0;
    double eps2 = _epsilon_ * _epsilon_;
    geomtools::vector_3d q1;
    geomtools::invalidate(q1);
    std::unique_ptr<std::ofstream> fdebug;
    if (_debug_) {
      fdebug.reset(new std::ofstream(_debug_filename_.c_str()));
      fdebug.get()->precision(12);
      *fdebug.get() << q0.x() << ' ' << q0.y() << ' ' << q0.z() << std::endl;
    }
    do {
      niter++;
      if (niter > _max_iter_) {
        error_code = EXIT_FAILURE;
        go_on = false;
        break;
      }
      DT_LOG_DEBUG(logging, "niter = " << niter);
      double f0 = _quadric_->eval_f(q0);
      DT_LOG_DEBUG(logging, "f0 = " << f0);
      geomtools::vector_3d dfq0;
      _quadric_->eval_gradient(q0, dfq0);
      DT_LOG_DEBUG(logging, "gradient f(q0) = " << dfq0);
      double dfq0_sqr = dfq0.mag2();
      DT_LOG_DEBUG(logging, "Squared{gradient f(q0)} = " << dfq0_sqr);
      q1 = q0 - f0 * dfq0 / dfq0_sqr;
      DT_LOG_DEBUG(logging, "q1 = " << q1);
      geomtools::vector_3d q1q0 = (q1 - q0);
      if (q1q0.mag2() < eps2) {
        go_on = false;
      }
      q0 = q1;
      if (fdebug) {
        *fdebug.get() << q0.x() << ' ' << q0.y() << ' ' << q0.z() << std::endl;
      }
    } while (go_on);
    on_surface_ = q1;
    return error_code;
  }

} // end of namespace geomtools
