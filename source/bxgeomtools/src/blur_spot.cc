// blur_spot.cc

// Ourselves:
#include <geomtools/blur_spot.h>

// Standard library:
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/exception.h>

// This project:
#include <geomtools/i_object_3d.h>
#include <geomtools/polyline_3d.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(blur_spot, "geomtools::blur_spot")

  const double blur_spot::DEFAULT_VALUE  = 0.0;
  const double blur_spot::DEFAULT_NSIGMA = 2.0;

  bool blur_spot::is_valid() const
  {
    if (! this->base_hit::is_valid()) return false;
    if ( _blur_dimension_ == DIMENSION_INVALID) return false;
    if ( ! _placement_.is_valid()) return false;
    if (is_dimension_one()) {
      return datatools::is_valid(_z_error_);
    } else if (is_dimension_two()) {
      return datatools::is_valid(_x_error_)
        && datatools::is_valid(_y_error_);
    }
    return datatools::is_valid(_x_error_)
      && datatools::is_valid(_y_error_)
      && datatools::is_valid(_z_error_);
  }

  void blur_spot::reset()
  {
    this->blur_spot::invalidate();
    return;
  }

  void blur_spot::invalidate()
  {
    invalidate_placement();
    datatools::invalidate(_x_error_);
    datatools::invalidate(_y_error_);
    datatools::invalidate(_z_error_);
    _blur_dimension_ = DIMENSION_INVALID;
    this->base_hit::invalidate();
    return;
  }

  double blur_spot::get_tolerance() const
  {
    return _tolerance_;
  }

  void blur_spot::set_tolerance(double tolerance_)
  {
    if (tolerance_ > DEFAULT_VALUE) {
      _tolerance_ = tolerance_;
    } else {
      _tolerance_ = constants::DEFAULT_TOLERANCE;
    }
    return;
  }

  bool blur_spot::is_dimension_zero() const
  {
    return _blur_dimension_ == DIMENSION_ZERO;
  }

  bool blur_spot::is_dimension_one() const
  {
    return _blur_dimension_ == DIMENSION_ONE;
  }

  bool blur_spot::is_dimension_two() const
  {
    return _blur_dimension_ == DIMENSION_TWO;
  }

  bool blur_spot::is_dimension_three() const
  {
    return _blur_dimension_ == DIMENSION_THREE;
  }

  // Set the intrinsic dimension of the object :
  void blur_spot::set_blur_dimension(int blur_dimension_)
  {
    if (blur_dimension_ == DIMENSION_INVALID) {
      reset();
      _blur_dimension_ = DIMENSION_INVALID;
    } else {
      DT_THROW_IF(blur_dimension_ < DIMENSION_ZERO || blur_dimension_ > DIMENSION_THREE,
                  std::range_error,
                  "Invalid blur dimension (" << blur_dimension_ << ") !");
      _blur_dimension_ = blur_dimension_;
    }
    return;
  }

  int blur_spot::get_blur_dimension() const
  {
    return _blur_dimension_;
  }

  // Default Constructor :
  blur_spot::blur_spot()
  {
    this->blur_spot::invalidate();
    _tolerance_ = constants::DEFAULT_TOLERANCE;
    return;
  }

  // Constructor :
  blur_spot::blur_spot(int8_t blur_dimension_,
                       double tolerance_)
  {
    this->blur_spot::invalidate();
    if (blur_dimension_ != DIMENSION_INVALID) {
      set_blur_dimension(blur_dimension_);
    }
    _tolerance_ = constants::DEFAULT_TOLERANCE;
    set_tolerance(tolerance_);
    return;
  }

  blur_spot::~blur_spot()
  {
    return;
  }

  void blur_spot::set_position(const vector_3d & position_)
  {
    _store_set(STORE_PLACEMENT);
    _placement_.set_translation(position_);
    return;
  }

  void blur_spot::set_placement(const placement & placement_)
  {
    _store_set(STORE_PLACEMENT);
    _placement_ = placement_;
  }

  void blur_spot::invalidate_placement()
  {
    _placement_.reset();
    _store_unset(STORE_PLACEMENT);
    return;
  }

  placement & blur_spot::grab_placement()
  {
    _store_set(STORE_PLACEMENT);
    return _placement_;
  }

  const placement & blur_spot::get_placement() const
  {
    return _placement_;
  }

  const vector_3d & blur_spot::get_position() const
  {
    return _placement_.get_translation();
  }

  const rotation_3d & blur_spot::get_rotation() const
  {
    return _placement_.get_rotation();
  }

  const rotation_3d & blur_spot::get_inverse_rotation() const
  {
    return _placement_.get_inverse_rotation();
  }

  bool blur_spot::_match_0d(const vector_3d & position_,
                            double tolerance_) const
  {
    // Default tolerance :
    double tolerance = _tolerance_;
    if (tolerance_ > DEFAULT_VALUE) {
      tolerance = tolerance_;
    }
    vector_3d distance = position_ - _placement_.get_translation();
    //clog << "DEVEL: geomtools: blur_spot::_match_0d: distance=" << distance.mag() / << endl;
    bool near = distance.mag() < tolerance;
    //if (near) clog << "DEVEL: geomtools: blur_spot::_match_0d: NEAR distance=" << distance.mag() / CLHEP::mm << endl;
    return near;
    // return position_.isNear(_placement_.get_translation(), tolerance);
  }

  bool blur_spot::_match_1d(const vector_3d & position_,
                            int mode_,
                            double nsigma1_or_tolerance_,
                            double nsigma2_or_tolerance_) const
  {
    vector_3d local_pos;
    _placement_.mother_to_child(position_, local_pos);
    if (mode_ == MODE_INTERVAL) {
      // clog << "DEVEL: geomtools: blur_spot::_match_1d: MODE_INTERVAL"<< endl;
      // Default tolerance :
      double tolerance_rho = _tolerance_;
      if (nsigma1_or_tolerance_ > DEFAULT_VALUE) {
        tolerance_rho = nsigma1_or_tolerance_;
      }
      if (local_pos.perp() > tolerance_rho) {
        return false;
      }
      if (std::abs(local_pos.z()) > _z_error_) {
        return false;
      }
      return true;
    } else if (mode_ == MODE_GAUSSIAN) {
      //clog << "DEVEL: geomtools: blur_spot::_match_1d: MODE_GAUSSIAN"<< endl;
      // Default N sigma along Z axis :
      double nsigma_z = DEFAULT_NSIGMA;
      if (nsigma1_or_tolerance_ > DEFAULT_VALUE) {
        nsigma_z = nsigma1_or_tolerance_;
      }
      // Default tolerance :
      double tolerance_rho = _tolerance_;
      if (nsigma2_or_tolerance_ > DEFAULT_VALUE) {
        tolerance_rho = nsigma2_or_tolerance_;
      }
      if (local_pos.perp() > tolerance_rho) {
        return false;
      }
      if (std::abs(local_pos.z()) > nsigma_z * _z_error_) {
        return false;
      }
      return true;
    }
    return false;
  }

  bool blur_spot::_match_2d(const vector_3d & position_,
                            int mode_,
                            double nsigma1_or_tolerance_,
                            double nsigma2_or_tolerance_,
                            double nsigma3_or_tolerance_) const
  {
    vector_3d local_pos;
    _placement_.mother_to_child(position_, local_pos);
    if (mode_ == MODE_INTERVAL) {
      // Default tolerance :
      double tolerance_z = _tolerance_;
      if (nsigma1_or_tolerance_ > DEFAULT_VALUE) {
        tolerance_z = nsigma1_or_tolerance_;
      }
      if (std::abs(local_pos.z()) > tolerance_z) {
        return false;
      }
      if (std::abs(local_pos.x()) > _x_error_) {
        return false;
      }
      if (std::abs(local_pos.y()) > _y_error_) {
        return false;
      }
      return true;
    } else if (mode_ == MODE_GAUSSIAN) {
      // Default N sigma along X axis :
      double nsigma_x = DEFAULT_NSIGMA;
      if (nsigma1_or_tolerance_ > DEFAULT_VALUE) {
        nsigma_x = nsigma1_or_tolerance_;
      }
      // Default N sigma along Y axis :
      double nsigma_y = DEFAULT_NSIGMA;
      if (nsigma2_or_tolerance_ > DEFAULT_VALUE) {
        nsigma_y = nsigma2_or_tolerance_;
      }
      // Default tolerance along Z axis :
      double tolerance_z = _tolerance_;
      if (nsigma3_or_tolerance_ > DEFAULT_VALUE) {
        tolerance_z = nsigma3_or_tolerance_;
      }
      if (std::abs(local_pos.z()) > tolerance_z) {
        return false;
      }
      if (std::abs(local_pos.x()) > nsigma_x * _x_error_) {
        return false;
      }
      if (std::abs(local_pos.y()) > nsigma_y * _y_error_) {
        return false;
      }
      return true;
    }
    return false;
  }

  bool blur_spot::_match_3d(const vector_3d & position_,
                            int mode_,
                            double nsigma1_or_tolerance_,
                            double nsigma2_or_tolerance_,
                            double nsigma3_or_tolerance_) const
  {
    vector_3d local_pos;
    _placement_.mother_to_child(position_, local_pos);
    if (mode_ == MODE_INTERVAL) {
      if (std::abs(local_pos.x()) > (_x_error_)) {
        return false;
      }
      if (std::abs(local_pos.y()) > (_y_error_)) {
        return false;
      }
      if (std::abs(local_pos.z()) > (_z_error_)) {
        return false;
      }
      return true;
    } else if (mode_ == MODE_GAUSSIAN) {
      // Default N sigma along X axis :
      double nsigma_x = DEFAULT_NSIGMA;
      if (nsigma1_or_tolerance_ > DEFAULT_VALUE) {
        nsigma_x = nsigma1_or_tolerance_;
      }
      // Default N sigma along Y axis :
      double nsigma_y = DEFAULT_NSIGMA;
      if (nsigma2_or_tolerance_ > DEFAULT_VALUE) {
        nsigma_y = nsigma2_or_tolerance_;
      }
      // Default N sigma along Z axis :
      double nsigma_z = DEFAULT_NSIGMA;
      if (nsigma3_or_tolerance_ > DEFAULT_VALUE) {
        nsigma_z = nsigma3_or_tolerance_;
      }
      if (std::abs(local_pos.x()) > nsigma_x * _x_error_) {
        return false;
      }
      if (std::abs(local_pos.y()) > nsigma_y * _y_error_) {
        return false;
      }
      if (std::abs(local_pos.z()) > nsigma_z * _z_error_) {
        return false;
      }
      return true;
    }
    return false;
  }

  bool blur_spot::match(const vector_3d & position_,
                        int mode_,
                        double nsigma1_or_tolerance_,
                        double nsigma2_or_tolerance_,
                        double nsigma3_or_tolerance_) const
  {
    if (is_dimension_zero()) {
      return _match_0d(position_,
                       nsigma1_or_tolerance_);
    } else if (is_dimension_one()) {
      return _match_1d(position_,
                       mode_,
                       nsigma1_or_tolerance_,
                       nsigma2_or_tolerance_);
    } else if (is_dimension_two()) {
      return _match_2d(position_,
                       mode_,
                       nsigma1_or_tolerance_,
                       nsigma2_or_tolerance_,
                       nsigma3_or_tolerance_);
    } else if (is_dimension_three()) {
      return _match_3d(position_,
                       mode_,
                       nsigma1_or_tolerance_,
                       nsigma2_or_tolerance_,
                       nsigma3_or_tolerance_);
    }
    return false;
  }

  double blur_spot::get_x_error() const
  {
    return _x_error_;
  }

  double blur_spot::get_y_error() const
  {
    return _y_error_;
  }

  double blur_spot::get_z_error() const
  {
    return _z_error_;
  }

  void blur_spot::set_x_error(double x_error_)
  {
    if (is_dimension_two() || is_dimension_three()) {
      if (x_error_ > 0.0) {
        _x_error_ = x_error_;
      } else {
        datatools::invalidate(_x_error_);
      }
    } else {
      DT_THROW_IF(true, std::logic_error, "Cannot set the X error in dimension 0 or 1");
    }
    return;
  }

  void blur_spot::set_y_error(double y_error_)
  {
    if (is_dimension_two() || is_dimension_three()) {
      if (y_error_ > 0.0) {
        _y_error_ = y_error_;
      } else {
        datatools::invalidate(_y_error_);
      }
    } else {
      DT_THROW_IF(true, std::logic_error, "Cannot set the Y error in dimension 0 or 1");
    }
    return;
  }

  void blur_spot::set_z_error(double z_error_)
  {
    if (is_dimension_one() || is_dimension_three()) {
      if (z_error_ > 0.0) {
        _z_error_ = z_error_;
      } else {
        datatools::invalidate(_z_error_);
      }
    } else {
      DT_THROW_IF(true, std::logic_error,"Cannot set the Z error in dimension 0 or 2");
    }
    return;
  }

  /// Set all errors of the spot
  void blur_spot::set_errors(double err_1_,
                             double err_2_,
                             double err_3_)
  {
    if (is_dimension_one()) {
      set_z_error(err_1_);
      DT_THROW_IF(datatools::is_valid(err_2_),
                  std::logic_error, "Cannot set dimension 2 error in dimension 1");
      DT_THROW_IF(datatools::is_valid(err_3_),
                  std::logic_error, "Cannot set dimension 3 error in dimension 1");
    } else if (is_dimension_two()) {
      set_x_error(err_1_);
      set_y_error(err_2_);
      DT_THROW_IF(datatools::is_valid(err_3_),
                  std::logic_error, "Cannot set dimension 3 error in dimension 2");
    } else if (is_dimension_three()) {
      set_x_error(err_1_);
      set_y_error(err_2_);
      set_z_error(err_3_);
    } else {
      DT_THROW_IF(true, std::logic_error, "Cannot set errors in dimension 0");
    }
    return;
  }

  void blur_spot::randomize_mygsl(mygsl::rng & ran_,
                                  vector_3d & random_point_,
                                  int mode_) const
  {
    if (is_dimension_zero()) {
      random_point_ = _placement_.get_translation();
      return;
    }
    vector_3d local_pos(0.0, 0.0, 0.0);
    if (is_dimension_one()) {
      if (mode_ == MODE_INTERVAL) {
        local_pos.setZ(_z_error_ * ran_.flat(-1., +1.));
      } else {
        local_pos.setZ(ran_.gaussian(0.0, _z_error_));
      }
    } else if (is_dimension_two()) {
      if (mode_ == MODE_INTERVAL) {
        local_pos.setX(_x_error_ * ran_.flat(-1., +1.));
        local_pos.setY(_y_error_ * ran_.flat(-1., +1.));
      } else {
        local_pos.setX(ran_.gaussian (0.0, _x_error_));
        local_pos.setY(ran_.gaussian (0.0, _y_error_));
      }
    } else if (is_dimension_three()) {
      if (mode_ == MODE_INTERVAL) {
        local_pos.setX(_x_error_ * ran_.flat(-1., +1.));
        local_pos.setY(_y_error_ * ran_.flat(-1., +1.));
        local_pos.setZ(_z_error_ * ran_.flat(-1., +1.));
      } else {
        local_pos.setX(ran_.gaussian(0.0, _x_error_));
        local_pos.setY(ran_.gaussian(0.0, _y_error_));
        local_pos.setZ(ran_.gaussian(0.0, _z_error_));
      }
    }
    _placement_.child_to_mother(local_pos, random_point_);
    return;
  }

  void blur_spot::print() const
  {
    tree_dump(std::clog,"Blur spot:", "");
  }

  void blur_spot::tree_dump(std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_,
                            bool inherit_) const
  {
    this->base_hit::tree_dump(out_, title_, indent_, true);
    std::string indent = indent_;

    out_ << indent << i_tree_dumpable::tag << "Dimension : "
         << (int) _blur_dimension_  << std::endl;

    out_ << indent << i_tree_dumpable::tag << "Placement : "  << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent << i_tree_dumpable::skip_tag;
      _placement_.tree_dump(out_, "", indent_oss.str());
    }

    if (is_dimension_two() || is_dimension_three()) {
      out_ << indent << i_tree_dumpable::tag << "X-error : "
           << _x_error_ / CLHEP::mm << " mm" << std::endl;
      out_ << indent << i_tree_dumpable::tag << "Y-error : "
           << _y_error_  / CLHEP::mm << " mm" << std::endl;
    }

    if (is_dimension_one() || is_dimension_three()) {
      out_ << indent << i_tree_dumpable::tag << "Z-error : "
           << _z_error_  / CLHEP::mm << " mm"  << std::endl;
    }

    out_ << indent << i_tree_dumpable::inherit_tag(inherit_) << "Tolerance : "
         << _tolerance_ / CLHEP::mm  << " mm" << std::endl;

    return;
  }

  void blur_spot::generate_wires_self(wires_type & wires_,
                                      uint32_t /*options_*/) const
  {
    double dx, dy, dz;
    dx = _tolerance_;
    dy = _tolerance_;
    dz = _tolerance_;
    if (is_dimension_zero()) {
    }
    if (is_dimension_one()) {
      dz = _z_error_;
    }
    if (is_dimension_two())  {
      dx = _x_error_;
      dy = _x_error_;
    }
    if (is_dimension_three()) {
      dx = _x_error_;
      dy = _x_error_;
      dz = _z_error_;
    }

    vector_3d vertexes[3][2];
    for (size_t i = 0; i < 3; i++) {
      vertexes[i][0].set((i == 0) ? -dx : 0,
                         (i == 1) ? -dy : 0,
                         (i == 2) ? -dz : 0);
      vertexes[i][1].set((i == 0) ? +dx : 0,
                         (i == 1) ? +dy : 0,
                         (i == 2) ? +dz : 0);
    }

    {
      for (size_t i = 0; i < 3; i++) {
        {
          polyline_type xpl;
          wires_.push_back(xpl);
        }
        polyline_type & pl = wires_.back();
        vector_3d v1;
        _placement_.child_to_mother(vertexes[i][0], v1);
        pl.push_back(v1);
        /*
          if (i == 0) {
          // Workaround to avoid automatic Gnuplot data mesh display mode :
          // add the middle point of the X segment.
          vector_3d v0 = 0.5 *(vertexes[i][0]+vertexes[i][1]);
          _placement_.child_to_mother (v0, v1);
          pl.add(v1);
          }
        */
        _placement_.child_to_mother(vertexes[i][1], v1);
        pl.push_back(v1);
      }
    }

    if (is_dimension_two() || is_dimension_three()) {
      {
        polyline_type xpl;
        wires_.push_back(xpl);
      }
      polyline_type & wire = wires_.back();
      vector_3d v1;
      _placement_.child_to_mother(vertexes[0][0], v1);
      wire.push_back(v1);
      _placement_.child_to_mother(vertexes[1][0], v1);
      wire.push_back(v1);
      _placement_.child_to_mother(vertexes[0][1], v1);
      wire.push_back(v1);
      _placement_.child_to_mother(vertexes[1][1], v1);
      wire.push_back(v1);
      _placement_.child_to_mother(vertexes[0][0], v1);
      wire.push_back(v1);
    }
    if (is_dimension_three()) {
      for (size_t j = 0; j < 2; j++) {
        {
          polyline_type xpl;
          wires_.push_back(xpl);
        }
        polyline_type & wire = wires_.back();
        vector_3d v1;
        _placement_.child_to_mother(vertexes[j][0], v1);
        wire.push_back(v1);
        _placement_.child_to_mother(vertexes[2][0], v1);
        wire.push_back(v1);
        _placement_.child_to_mother(vertexes[j][1], v1);
        wire.push_back(v1);
        _placement_.child_to_mother(vertexes[2][1], v1);
        wire.push_back(v1);
        _placement_.child_to_mother(vertexes[j][0], v1);
        wire.push_back(v1);
      }
    }
    return;
  }

} // end of namespace geomtools
