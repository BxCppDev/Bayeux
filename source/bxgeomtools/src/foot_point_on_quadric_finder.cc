// geomtools/foot_point_on_quadric_finder.cc
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
#include <geomtools/foot_point_on_quadric_finder.h>

// This project:
#include <geomtools/point_on_quadric_finder.h>

namespace geomtools {

  // static
  double foot_point_on_quadric_finder::default_alpha_max()
  {
    static const double _am(20.0);
    return _am;
  }

  // static
  unsigned int foot_point_on_quadric_finder::default_max_iter()
  {
    static const unsigned int _mi(25);
    return _mi;
  }

  void foot_point_on_quadric_finder::set_epsilon(double eps_)
  {
    DT_THROW_IF(!datatools::is_valid(eps_), std::domain_error, "Invalid tolerance value!");
    DT_THROW_IF(eps_ <= 0.0, std::domain_error, "Invalid tolerance value!");
    _epsilon_ = eps_;
    return;
  }

  void foot_point_on_quadric_finder::set_alpha_max(double am_)
  {
    DT_THROW_IF(!datatools::is_valid(am_), std::logic_error, "Invalid alpha max value!");
    DT_THROW_IF(am_ <= 0.0, std::domain_error, "Invalid alpha max value!");
    _alpha_max_ = am_;
    return;
  }

  foot_point_on_quadric_finder::foot_point_on_quadric_finder(const quadric & q_,
                                                             double epsilon_,
                                                             double alpha_max_,
                                                             unsigned int max_iter_,
                                                             const std::string & dbg_filename_)
  {
    _quadric_ = &q_;
    set_epsilon(epsilon_);
    if (alpha_max_ <= 0.0) {
      set_alpha_max(default_alpha_max());
    } else {
      set_alpha_max(alpha_max_);
    }
    if (max_iter_ == 0) {
      _max_iter_ = default_max_iter();
    } else {
      _max_iter_ = max_iter_;
    }
    _debug_filename_ = dbg_filename_;
    _debug_ = false;
    _init();
    return;
  }

  void foot_point_on_quadric_finder::_init()
  {
    if (!_debug_filename_.empty()) {
      _debug_ = true;
    }
    return;
  }

  int foot_point_on_quadric_finder::find(const geomtools::vector_3d & start_,
                                         geomtools::vector_3d & on_surface_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    if (_debug_) logging = datatools::logger::PRIO_DEBUG;
    int error_code = EXIT_SUCCESS;
    geomtools::vector_3d p = start_;
    bool go_on = true;
    unsigned int niter = 0;
    double eps2 = _epsilon_ * _epsilon_;
    geomtools::vector_3d p0;
    point_on_quadric_finder poqf(*_quadric_, _epsilon_, _max_iter_, "");
    if (poqf.find(p, p0) != EXIT_SUCCESS) {
     error_code = EXIT_FAILURE;
    } else {
      boost::scoped_ptr<std::ofstream> fdebug;
      if (_debug_) {
        fdebug.reset(new std::ofstream(_debug_filename_.c_str()));
        fdebug.get()->precision(12);
        *fdebug.get() << p0.x() << ' ' << p0.y() << ' ' << p0.z() << std::endl;
      }
      do {
        niter++;
        if (niter > _max_iter_) {
          error_code = EXIT_FAILURE;
          go_on = false;
          break;
        }
        // P 61
        geomtools::vector_3d dfp0;
        _quadric_->eval_gradient(p0, dfp0);
        DT_LOG_DEBUG(logging, "gradient f(p0) = " << dfp0);
        double dfp0_sqr = dfp0.mag2();
        DT_LOG_DEBUG(logging, "Squared{gradient f(p0)} = " << dfp0_sqr);
        double k = (p - p0).dot(dfp0) / dfp0_sqr;
        geomtools::vector_3d q0 = p - k * dfp0;
        geomtools::vector_3d p0q0 = (q0 - p0);
        if (p0q0.mag2() < eps2) {
          on_surface_ = q0;
          go_on = false;
          break;
        } else {
          geomtools::vector_3d p1;
          if (poqf.find(q0, p1) != EXIT_SUCCESS) {
            error_code = EXIT_FAILURE;
            go_on = false;
            break;
          }
          const geomtools::vector_3d & f1 = p0q0;
          geomtools::vector_3d f2 = p1 - q0;
          geomtools::vector_3d pp0 = p - p0;
          double a0 = pp0.dot(f1);
          double a1 =  2 * f2.dot(pp0) - f1.mag2();
          double a2 = -3 * f1.dot(f2);
          double a3 = -2 * f2.mag2();
          double alpha = 1.0 - (a0 + a1 + a2 + a3) / (a1 + 2 * a2 + 3 * a3);
          if (alpha > 0.0 && alpha < _alpha_max_) {
            geomtools::vector_3d q00 = p0 + alpha * f1 + alpha * alpha * f2;
            poqf.find(q00, p1);
            geomtools::vector_3d p1p0 = p0 - p1;
            if (p1p0.mag2() < eps2) {
              go_on = false;
            }
            p0 = p1;
          } else {
            DT_LOG_DEBUG(logging, "alpha = " << alpha);
            error_code = EXIT_FAILURE;
            go_on = false;
            break;
          }
        }
        if (fdebug) {
          *fdebug.get() << p0.x() << ' ' << p0.y() << ' ' << p0.z() << std::endl;
        }
      } while (go_on);
      on_surface_ = p0;
    }
    if (error_code != EXIT_SUCCESS) {
      geomtools::invalidate(on_surface_);
    }
    return error_code;
  }

} // end of namespace geomtools
