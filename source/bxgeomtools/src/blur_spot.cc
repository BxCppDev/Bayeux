// -*- mode: c++; -*-
/* blur_spot.cc
 */

#include <geomtools/blur_spot.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <geomtools/i_object_3d.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/exception.h>

namespace geomtools {

  using namespace std;

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(blur_spot, "geomtools::blur_spot")

  const double blur_spot::DEFAULT_VALUE  = 0.0;
  const double blur_spot::DEFAULT_NSIGMA = 2.0;

  bool blur_spot::is_valid () const
  {
    if ( _blur_dimension_ == DIMENSION_INVALID) return false;
    if ( ! _placement_.is_valid ()) return false;
    if (is_dimension_one ())
      {
        return datatools::is_valid (_z_error_);
      }
    else if (is_dimension_two ())
      {
        return datatools::is_valid (_x_error_)
          && datatools::is_valid (_y_error_);
      }
    else //if (is_dimension_three ())
      {
        return datatools::is_valid (_x_error_)
          && datatools::is_valid (_y_error_)
          && datatools::is_valid (_z_error_);
      }
    // return true;
  }

  void blur_spot::reset ()
  {
    _placement_.reset ();
    _auxiliaries_.clear ();
    datatools::invalidate (_x_error_);
    datatools::invalidate (_y_error_);
    datatools::invalidate (_z_error_);
    return;
  }

  void blur_spot::invalidate ()
  {
    reset ();
    _blur_dimension_ = DIMENSION_INVALID;
    return;
  }

  double blur_spot::get_tolerance () const
  {
    return _tolerance_;
  }

  void blur_spot::set_tolerance (double tolerance_)
  {
    if (tolerance_ > DEFAULT_VALUE)
      {
        _tolerance_ = tolerance_;
      }
    else
      {
        _tolerance_ = constants::DEFAULT_TOLERANCE;
      }
    return;
  }

  bool blur_spot::is_dimension_zero () const
  {
    return _blur_dimension_ == DIMENSION_ZERO;
  }

  bool blur_spot::is_dimension_one () const
  {
    return _blur_dimension_ == DIMENSION_ONE;
  }

  bool blur_spot::is_dimension_two () const
  {
    return _blur_dimension_ == DIMENSION_TWO;
  }

  bool blur_spot::is_dimension_three () const
  {
    return _blur_dimension_ == DIMENSION_THREE;
  }

  // Set the intrinsic dimension of the object :
  void blur_spot::set_blur_dimension (int blur_dimension_)
  {
    if (blur_dimension_ == DIMENSION_INVALID)
      {
        reset ();
        _blur_dimension_ = DIMENSION_INVALID;
      }
    else
      {
        DT_THROW_IF (blur_dimension_ < DIMENSION_ZERO || blur_dimension_ > DIMENSION_THREE,
                     std::range_error,
                     "Invalid blur dimension (" << blur_dimension_ << ") !");
        _blur_dimension_ = blur_dimension_;
      }
    return;
  }

  int blur_spot::get_blur_dimension () const
  {
    return _blur_dimension_;
  }

  // Default Constructor :
  blur_spot::blur_spot ()
  {
    _blur_dimension_ = DIMENSION_INVALID;
    _tolerance_ = constants::DEFAULT_TOLERANCE;
    datatools::invalidate (_x_error_);
    datatools::invalidate (_y_error_);
    datatools::invalidate (_z_error_);
    return;
  }

  // Constructor :
  blur_spot::blur_spot (int8_t blur_dimension_,
                        double tolerance_)
  {
    _blur_dimension_ = DIMENSION_INVALID;
    if (blur_dimension_ != DIMENSION_INVALID)
      {
        set_blur_dimension (blur_dimension_);
      }
    _tolerance_ = constants::DEFAULT_TOLERANCE;
    set_tolerance (tolerance_);
    datatools::invalidate (_x_error_);
    datatools::invalidate (_y_error_);
    datatools::invalidate (_z_error_);
    return;
  }

  blur_spot::~blur_spot ()
  {
    return;
  }

  void blur_spot::set_position (const vector_3d & position_)
  {
    _placement_.set_translation (position_);
    return;
  }

  void blur_spot::set_placement (const placement & placement_)
  {
    _placement_ = placement_;
  }

  placement & blur_spot::grab_placement ()
  {
    return _placement_;
  }

  const placement & blur_spot::get_placement () const
  {
    return _placement_;
  }

  const vector_3d & blur_spot::get_position () const
  {
    return _placement_.get_translation ();
  }

  const rotation_3d & blur_spot::get_rotation () const
  {
    return _placement_.get_rotation ();
  }

  const rotation_3d & blur_spot::get_inverse_rotation () const
  {
    return _placement_.get_inverse_rotation ();
  }

  void blur_spot::set_auxiliaries (const datatools::properties & aux_)
  {
    _auxiliaries_ = aux_;
  }

  datatools::properties & blur_spot::grab_auxiliaries ()
  {
    return _auxiliaries_;
  }

  const datatools::properties & blur_spot::get_auxiliaries () const
  {
    return _auxiliaries_;
  }

  bool blur_spot::_match_0d (const vector_3d & position_,
                             double tolerance_) const
  {
    // Default tolerance :
    double tolerance = _tolerance_;
    if (tolerance_ > DEFAULT_VALUE)
      {
        tolerance = tolerance_;
      }
    vector_3d distance = position_ - _placement_.get_translation ();
    //clog << "DEVEL: geomtools: blur_spot::_match_0d: distance=" << distance.mag () / << endl;
    bool near = distance.mag () < tolerance;
    //if (near) clog << "DEVEL: geomtools: blur_spot::_match_0d: NEAR distance=" << distance.mag () / CLHEP::mm << endl;
    return near;
    // return position_.isNear (_placement_.get_translation (), tolerance);
  }

  bool blur_spot::_match_1d (const vector_3d & position_,
                             int mode_,
                             double nsigma1_or_tolerance_,
                             double nsigma2_or_tolerance_) const
  {
    vector_3d local_pos;
    _placement_.mother_to_child (position_, local_pos);
    if (mode_ == MODE_INTERVAL)
      {
        // clog << "DEVEL: geomtools: blur_spot::_match_1d: MODE_INTERVAL"<< endl;
        // Default tolerance :
        double tolerance_rho = _tolerance_;
        if (nsigma1_or_tolerance_ > DEFAULT_VALUE)
          {
            tolerance_rho = nsigma1_or_tolerance_;
          }
        if (local_pos.perp() > tolerance_rho)
          {
            return false;
          }
        if (std::abs(local_pos.z ()) > _z_error_)
          {
            return false;
          }
        return true;
      }
    else if (mode_ == MODE_GAUSSIAN)
      {
        //clog << "DEVEL: geomtools: blur_spot::_match_1d: MODE_GAUSSIAN"<< endl;
        // Default N sigma along Z axis :
        double nsigma_z = DEFAULT_NSIGMA;
        if (nsigma1_or_tolerance_ > DEFAULT_VALUE)
          {
            nsigma_z = nsigma1_or_tolerance_;
          }
        // Default tolerance :
        double tolerance_rho = _tolerance_;
        if (nsigma2_or_tolerance_ > DEFAULT_VALUE)
          {
            tolerance_rho = nsigma2_or_tolerance_;
          }
        if (local_pos.perp() > tolerance_rho)
          {
            return false;
          }
        if (std::abs(local_pos.z ()) > nsigma_z * _z_error_)
          {
            return false;
          }
        return true;
      }
    return false;
  }

  bool blur_spot::_match_2d (const vector_3d & position_,
                             int mode_,
                             double nsigma1_or_tolerance_,
                             double nsigma2_or_tolerance_,
                             double nsigma3_or_tolerance_) const
  {
    vector_3d local_pos;
    _placement_.mother_to_child (position_, local_pos);
    if (mode_ == MODE_INTERVAL)
      {
        // Default tolerance :
        double tolerance_z = _tolerance_;
        if (nsigma1_or_tolerance_ > DEFAULT_VALUE)
          {
            tolerance_z = nsigma1_or_tolerance_;
          }
        if (std::abs(local_pos.z ()) > tolerance_z)
          {
            return false;
          }
        if (std::abs(local_pos.x ()) > _x_error_)
          {
            return false;
          }
        if (std::abs(local_pos.y ()) > _y_error_)
          {
            return false;
          }
        return true;
      }
    else if (mode_ == MODE_GAUSSIAN)
      {
        // Default N sigma along X axis :
        double nsigma_x = DEFAULT_NSIGMA;
        if (nsigma1_or_tolerance_ > DEFAULT_VALUE)
          {
            nsigma_x = nsigma1_or_tolerance_;
          }
        // Default N sigma along Y axis :
        double nsigma_y = DEFAULT_NSIGMA;
        if (nsigma2_or_tolerance_ > DEFAULT_VALUE)
          {
            nsigma_y = nsigma2_or_tolerance_;
          }
        // Default tolerance along Z axis :
        double tolerance_z = _tolerance_;
        if (nsigma3_or_tolerance_ > DEFAULT_VALUE)
          {
            tolerance_z = nsigma3_or_tolerance_;
          }
        if (std::abs(local_pos.z ()) > tolerance_z)
          {
            return false;
          }
        if (std::abs(local_pos.x ()) > nsigma_x * _x_error_)
          {
            return false;
          }
        if (std::abs(local_pos.y ()) > nsigma_y * _y_error_)
          {
            return false;
          }
        return true;
      }
    return false;
  }

  bool blur_spot::_match_3d (const vector_3d & position_,
                             int mode_,
                             double nsigma1_or_tolerance_,
                             double nsigma2_or_tolerance_,
                             double nsigma3_or_tolerance_) const
  {
    vector_3d local_pos;
    _placement_.mother_to_child (position_, local_pos);
    if (mode_ == MODE_INTERVAL)
      {
        if (std::abs(local_pos.x ()) > (_x_error_))
          {
            return false;
          }
        if (std::abs(local_pos.y ()) > (_y_error_))
          {
            return false;
          }
        if (std::abs(local_pos.z ()) > (_z_error_))
          {
            return false;
          }
        return true;
      }
    else if (mode_ == MODE_GAUSSIAN)
      {
        // Default N sigma along X axis :
        double nsigma_x = DEFAULT_NSIGMA;
        if (nsigma1_or_tolerance_ > DEFAULT_VALUE)
          {
            nsigma_x = nsigma1_or_tolerance_;
          }
        // Default N sigma along Y axis :
        double nsigma_y = DEFAULT_NSIGMA;
        if (nsigma2_or_tolerance_ > DEFAULT_VALUE)
          {
            nsigma_y = nsigma2_or_tolerance_;
          }
        // Default N sigma along Z axis :
        double nsigma_z = DEFAULT_NSIGMA;
        if (nsigma3_or_tolerance_ > DEFAULT_VALUE)
          {
            nsigma_z = nsigma3_or_tolerance_;
          }
        if (std::abs(local_pos.x ()) > nsigma_x * _x_error_)
          {
            return false;
          }
        if (std::abs(local_pos.y ()) > nsigma_y * _y_error_)
          {
            return false;
          }
        if (std::abs(local_pos.z ()) > nsigma_z * _z_error_)
          {
            return false;
          }
        return true;
      }
    return false;
  }

  bool blur_spot::match (const vector_3d & position_,
                         int mode_,
                         double nsigma1_or_tolerance_,
                         double nsigma2_or_tolerance_,
                         double nsigma3_or_tolerance_) const
  {
    if (is_dimension_zero ())
      {
        return _match_0d (position_,
                          nsigma1_or_tolerance_);
      }
    else if (is_dimension_one ())
      {
        return _match_1d (position_,
                          mode_,
                          nsigma1_or_tolerance_,
                          nsigma2_or_tolerance_);
      }
    else if (is_dimension_two ())
      {
        return _match_2d (position_,
                          mode_,
                          nsigma1_or_tolerance_,
                          nsigma2_or_tolerance_,
                          nsigma3_or_tolerance_);
      }
    else if (is_dimension_three ())
      {
        return _match_3d (position_,
                          mode_,
                          nsigma1_or_tolerance_,
                          nsigma2_or_tolerance_,
                          nsigma3_or_tolerance_);
      }

    return false;
  }

  double blur_spot::get_x_error () const
  {
    return _x_error_;
  }

  double blur_spot::get_y_error () const
  {
    return _y_error_;
  }

  double blur_spot::get_z_error () const
  {
    return _z_error_;
  }

  void blur_spot::set_x_error (double x_error_)
  {
    if (is_dimension_two() || is_dimension_three())
      {
        if (x_error_ > 0.0)
          {
            _x_error_ = x_error_;
          }
        else
          {
            datatools::invalidate (_x_error_);
          }
      }
    else
      {
        DT_THROW_IF(true, std::logic_error, "Cannot set the X error in dimension 0 or 1");
      }
    return;
  }

  void blur_spot::set_y_error (double y_error_)
  {
    if (is_dimension_two() || is_dimension_three())
      {
        if (y_error_ > 0.0)
          {
            _y_error_ = y_error_;
          }
        else
          {
            datatools::invalidate (_y_error_);
          }
      }
    else
      {
        DT_THROW_IF(true, std::logic_error, "geomtools::blur_spot::set_y_error: Cannot set the Y error in dimension 0 or 1");
      }
    return;
  }

  void blur_spot::set_z_error (double z_error_)
  {
    if (is_dimension_one() || is_dimension_three())
      {
        if (z_error_ > 0.0)
          {
            _z_error_ = z_error_;
          }
        else
          {
            datatools::invalidate (_z_error_);
          }
      }
    else
      {
        DT_THROW_IF(true, std::logic_error,"Cannot set the Z error in dimension 0 or 2");
      }
    return;
  }

  /// Set all errors of the spot
  void blur_spot::set_errors (double err_1_,
                              double err_2_,
                              double err_3_)
  {
    if (is_dimension_one())
      {
        set_z_error(err_1_);
        DT_THROW_IF (datatools::is_valid(err_2_),
                     std::logic_error, "Cannot set dimension 2 error in dimension 1");
        DT_THROW_IF (datatools::is_valid(err_3_),
                     std::logic_error, "Cannot set dimension 3 error in dimension 1");
      }
    else if (is_dimension_two())
      {
        set_x_error(err_1_);
        set_y_error(err_2_);
        DT_THROW_IF (datatools::is_valid(err_3_),
                     std::logic_error, "Cannot set dimension 3 error in dimension 2");
      }
    else if (is_dimension_three())
      {
        set_x_error(err_1_);
        set_y_error(err_2_);
        set_z_error(err_3_);
      }
    else {
      DT_THROW_IF(true, std::logic_error, "Cannot set errors in dimension 0");
    }
    return;
  }

  void blur_spot::randomize_mygsl (mygsl::rng & ran_,
                                   vector_3d & random_point_,
                                   int mode_) const
  {
    if (is_dimension_zero ())
      {
        random_point_ = _placement_.get_translation ();
        return;
      }
    vector_3d local_pos (0.0, 0.0, 0.0);
    if (is_dimension_one ())
      {
        if (mode_ == MODE_INTERVAL)
          {
            local_pos.setZ (_z_error_ * ran_.flat(-1., +1.));
          }
        else
          {
            local_pos.setZ (ran_.gaussian (0.0, _z_error_));
          }
      }
    else if (is_dimension_two ())
      {
        if (mode_ == MODE_INTERVAL)
          {
            local_pos.setX (_x_error_ * ran_.flat(-1., +1.));
            local_pos.setY (_y_error_ * ran_.flat(-1., +1.));
          }
        else
          {
            local_pos.setX (ran_.gaussian (0.0, _x_error_));
            local_pos.setY (ran_.gaussian (0.0, _y_error_));
          }
      }
    else if (is_dimension_three ())
      {
        if (mode_ == MODE_INTERVAL)
          {
            local_pos.setX (_x_error_ * ran_.flat(-1., +1.));
            local_pos.setY (_y_error_ * ran_.flat(-1., +1.));
            local_pos.setZ (_z_error_ * ran_.flat(-1., +1.));
          }
        else
          {
            local_pos.setX (ran_.gaussian (0.0, _x_error_));
            local_pos.setY (ran_.gaussian (0.0, _y_error_));
            local_pos.setZ (ran_.gaussian (0.0, _z_error_));
          }
      }
    _placement_.child_to_mother (local_pos, random_point_);
    return;
  }

  void blur_spot::print() const
  {
    tree_dump(std::clog,"Blur spot:", "");
  }

  void blur_spot::tree_dump (ostream & out_,
                             const string & title_,
                             const string & indent_,
                             bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << std::endl;
      }

    out_ << indent << i_tree_dumpable::tag << "Dimension : "
         << (int) _blur_dimension_  << endl;

    out_ << indent << i_tree_dumpable::tag << "Tolerance : "
         << _tolerance_ / CLHEP::mm  << " mm" << endl;

    out_ << indent << i_tree_dumpable::tag << "Placement : "  << endl;
    {
      ostringstream indent_oss;
      indent_oss << i_tree_dumpable::skip_tag;
      _placement_.tree_dump (out_, "", indent_oss.str ());
    }

    if (is_dimension_two () || is_dimension_three ())
      {
        out_ << indent << i_tree_dumpable::tag << "X-error : "
             << _x_error_ / CLHEP::mm << " mm" << endl;
        out_ << indent << i_tree_dumpable::tag << "Y-error : "
             << _y_error_  / CLHEP::mm << " mm" << endl;
      }

    if (is_dimension_one () || is_dimension_three ())
      {
        out_ << indent << i_tree_dumpable::tag << "Z-error : "
             << _z_error_  / CLHEP::mm << " mm"  << endl;
      }

    out_ << indent << i_tree_dumpable::inherit_tag (inherit_)
         << "Auxiliary properties : "  << endl;
    {
      ostringstream indent_oss;
      indent_oss << i_tree_dumpable::last_skip_tag;
      _auxiliaries_.tree_dump (out_, "", indent_oss.str ());
    }

    return;
  }

  void blur_spot::generate_wires (std::list<polyline_3d> & lpl_,
                                  const placement & p_,
                                  uint32_t /*options_*/) const
  {
    double dx, dy, dz;
    dx = _tolerance_;
    dy = _tolerance_;
    dz = _tolerance_;

    /*
      std::cerr << "geomtools::blur_spot::generate_wires: "
      << "tolerance=" << _tolerance_/CLHEP::mm << " mm"
      << std::endl;
    */
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
    for (int i = 0; i < 3; i++) {
        vertexes[i][0].set((i == 0) ? -dx : 0,
                           (i == 1) ? -dy : 0,
                           (i == 2) ? -dz : 0);
        vertexes[i][1].set((i == 0) ? +dx : 0,
                           (i == 1) ? +dy : 0,
                           (i == 2) ? +dz : 0);
      }

    {
      for (int i = 0; i < 3; i++) {
        {
          polyline_3d xpl;
          lpl_.push_back (xpl);
        }
        polyline_3d & pl = lpl_.back ();
        pl.set_closed (false);
        vector_3d v1, v;
        _placement_.child_to_mother (vertexes[i][0], v1);
        p_.child_to_mother (v1, v);
        pl.add (v);
        if (i == 0) {
            // Workaround to avoid automatic Gnuplot data mesh display mode :
            // add the middle point of the X segment.
            vector_3d v0 = 0.5*(vertexes[i][0]+vertexes[i][1]);
            _placement_.child_to_mother (v0, v1);
            p_.child_to_mother (v1, v);
            pl.add (v);
          }
        _placement_.child_to_mother (vertexes[i][1], v1);
        p_.child_to_mother (v1, v);
        pl.add (v);
      }
    }

    if (is_dimension_two() || is_dimension_three()) {
        {
          polyline_3d xpl;
          lpl_.push_back (xpl);
        }
        polyline_3d & pl = lpl_.back ();
        pl.set_closed (true);
        vector_3d v1, v;
        _placement_.child_to_mother (vertexes[0][0], v1);
        p_.child_to_mother (v1, v);
        pl.add (v);
        _placement_.child_to_mother (vertexes[1][0], v1);
        p_.child_to_mother (v1, v);
        pl.add (v);
        _placement_.child_to_mother (vertexes[0][1], v1);
        p_.child_to_mother (v1, v);
        pl.add (v);
        _placement_.child_to_mother (vertexes[1][1], v1);
        p_.child_to_mother (v1, v);
        pl.add (v);
      }
    if (is_dimension_three()) {
        for (int j = 0; j < 2; j++) {
          {
            polyline_3d xpl;
            lpl_.push_back (xpl);
          }
          polyline_3d & pl = lpl_.back ();
          pl.set_closed (true);
          vector_3d v1, v;
          _placement_.child_to_mother (vertexes[j][0], v1);
          p_.child_to_mother (v1, v);
          pl.add (v);
          _placement_.child_to_mother (vertexes[2][0], v1);
          p_.child_to_mother (v1, v);
          pl.add (v);
          _placement_.child_to_mother (vertexes[j][1], v1);
          p_.child_to_mother (v1, v);
          pl.add (v);
          _placement_.child_to_mother (vertexes[2][1], v1);
          p_.child_to_mother (v1, v);
          pl.add (v);
        }
      }
    return;
  }

} // end of namespace geomtools

// end of blur_spot.cc
