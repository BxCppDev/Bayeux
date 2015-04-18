// face_intercept_info.cc

// Ourselves:
#include <geomtools/face_intercept_info.h>

namespace geomtools {

  face_intercept_info::face_intercept_info()
  {
    _face_id_.invalidate();
    invalidate(_impact_);
    return;
  }

  void face_intercept_info::reset()
  {
    _face_id_.invalidate();
    invalidate(_impact_);
    return;
  }

  const face_identifier & face_intercept_info::get_face_id() const
  {
    return _face_id_;
  }

  face_identifier & face_intercept_info::grab_face_id()
  {
    return _face_id_;
  }

  void face_intercept_info::set_face_id(const face_identifier & fid_)
  {
    _face_id_ = fid_;
    return;
  }

  void face_intercept_info::set_impact(const vector_3d & point_)
  {
    _impact_ = point_;
    return;
  }

  const vector_3d & face_intercept_info::get_impact() const
  {
    return _impact_;
  }

  vector_3d & face_intercept_info::grab_impact()
  {
    return _impact_;
  }

  bool face_intercept_info::is_valid() const
  {
    if (!geomtools::is_valid(_impact_)) {
      // std::cerr << "DEVEL: face_intercept_info::is_valid: Invalid impact." << std::endl;
      return false;
    }
    if (!_face_id_.is_valid()) {
      // std::cerr << "DEVEL: face_intercept_info::is_valid: Invalid face identifier." << std::endl;
      return false;
    }
    if (!_face_id_.is_unique()) {
      // std::cerr << "DEVEL: face_intercept_info::is_valid: Not unique face identifier." << std::endl;
      return false;
    }
    return true;
  }

  bool face_intercept_info::is_ok() const
  {
    return is_valid();
  }

  void face_intercept_info::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "Face intercept information: " << std::endl;
    out_ << indent_ << "|-- Face ID : " << _face_id_.to_string() << std::endl;
    out_ << indent_ << "|-- Impact  : " << _impact_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent_ << "`-- Ok      : " << is_ok() << std::endl;
    return;
  }

} // end of face_intercept_info
