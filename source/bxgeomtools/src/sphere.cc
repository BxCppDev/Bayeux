// geomtools/sphere.cc

// Ourselves:
#include <geomtools/sphere.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/properties.h>
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/circle.h>
#include <geomtools/disk.h>
#include <geomtools/cone.h>
#include <geomtools/spherical_sector.h>
#include <geomtools/right_circular_conical_nappe.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(sphere, "geomtools::sphere");

  using namespace std;

  const std::string & sphere::sphere_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "sphere";
    }
    return label;
  }

  void sphere::_build_bounding_data()
  {
    if (! has_partial_phi() && ! has_partial_theta()) {
      _grab_bounding_data().make_cylinder(_r_, get_zmin(), get_zmax());
    } else {
      mygsl::min_max xrange;
      mygsl::min_max yrange;
      mygsl::min_max zrange;
      if (!has_r_min()) {
        xrange.add(0.0);
        yrange.add(0.0);
        zrange.add(0.0);
      }
      double theta_start = 0.0;
      double theta_stop  = M_PI;
      double phi_start = 0.0;
      double phi_stop  = 2 * M_PI;
      if (has_partial_theta()) {
        theta_start = _start_theta_;
        theta_stop = theta_start + _delta_theta_;
      }
      if (has_partial_phi()) {
        phi_start = _start_phi_;
        phi_stop = phi_start + _delta_phi_;
      }
      double dtheta = std::min(0.5 * CLHEP::degree, _delta_theta_ / 100);
      double dphi   = std::min(0.5 * CLHEP::degree, _delta_phi_ / 100);
      for (double theta = theta_start;
           theta < theta_stop;
           theta += dtheta) {
        double ct = std::cos(theta);
        double st = std::sin(theta);
        for (double phi = phi_start;
             phi < phi_stop;
             phi += dphi) {
          double cp = std::cos(phi);
          double sp = std::sin(phi);
          double xo = get_r() * st * cp;
          double yo = get_r() * st * sp;
          double zo = get_r() * ct;
          if (xo > get_r() * 0.99) {
            xo = get_r();
          }
          if (xo < - get_r() * 0.99) {
            xo = -get_r();
          }
          if (yo > get_r() * 0.99) {
            yo = get_r();
          }
          if (yo < - get_r() * 0.99) {
            yo = -get_r();
          }
          xrange.add(xo);
          yrange.add(yo);
          zrange.add(zo);
          if (has_r_min()) {
            double xi = get_r_min() * st * cp;
            double yi = get_r_min() * st * sp;
            double zi = get_r_min() * ct;
            xrange.add(xi);
            yrange.add(yi);
            zrange.add(zi);
          }
        }
      }
      _grab_bounding_data().make_box(xrange.get_min(), xrange.get_max(),
                                     yrange.get_min(), yrange.get_max(),
                                     zrange.get_min(), zrange.get_max());
    }
    return;
  }

  double sphere::get_xmin () const
  {
    return -_r_;
  }

  double sphere::get_xmax () const
  {
    return +_r_;
  }

  double sphere::get_ymin () const
  {
    return -_r_;
  }

  double sphere::get_ymax () const
  {
    return +_r_;
  }

  double sphere::get_zmin () const
  {
    if (has_partial_theta()) {
      return std::min(_r_ * std::cos(_start_theta_ + _delta_theta_),
                      _r_min_ * std::cos(_start_theta_ + _delta_theta_)
                      );
    } else {
      return -_r_;
    }
  }

  double sphere::get_zmax () const
  {
    if (has_partial_theta()) {
      return std::max(_r_ * std::cos(_start_theta_),
                      _r_min_ * std::cos(_start_theta_)
                      );
    } else {
      return +_r_;
    }
  }

  double
  sphere::get_r() const
  {
    return _r_;
  }

  double
  sphere::get_r_max() const
  {
    return _r_;
  }

  bool sphere::has_r_min() const
  {
    return _r_min_ > 0.0;
  }

  double
  sphere::get_r_min () const
  {
    return _r_min_;
  }

  double
  sphere::get_radius () const
  {
    return get_r_max();
  }

  void
  sphere::set_r (double new_value_)
  {
    DT_THROW_IF(new_value_ <= 0.0, std::domain_error,
                "Invalid radius (" << new_value_ << ") !");
    _r_ = new_value_;
    if (_r_ < _r_min_) {
      reset_r_min ();
    }
  }

  void
  sphere::set_r_max (double new_value_)
  {
    set_r(new_value_);
  }

  void
  sphere::set_r_min (double new_value_)
  {
    _r_min_ = new_value_;
    DT_THROW_IF(new_value_ < 0.0, std::domain_error,
                "Invalid inner radius (" << new_value_ << ") !");
    if (_r_min_ > _r_) {
      set_r_max(_r_min_);
    }
  }

  void
  sphere::reset_r_max ()
  {
    datatools::invalidate(_r_);
  }

  void
  sphere::reset_r_min ()
  {
    _r_min_ = 0.0;
  }

  void
  sphere::set_radius (double new_value_)
  {
    set_r (new_value_);
    return;
  }

  void
  sphere::set (double r_)
  {
    reset_r_min();
    set_r_max(r_);
    return;
  }

  void
  sphere::set (double r_min_, double r_max_)
  {
    set_r_min(r_min_);
    set_r_max(r_max_);
    return;
  }

  void sphere::set_phi(double start_phi_, double delta_phi_)
  {
    DT_THROW_IF(start_phi_ < 0.0,
                std::domain_error,
                "Start phi is negative !");
    DT_THROW_IF(start_phi_ >= 2 * M_PI,
                std::domain_error,
                "Start phi is too large (> 2 pi) !");
    DT_THROW_IF(delta_phi_ > 2 * M_PI,
                std::domain_error,
                "Delta phi is too large (> 2 pi)!");
    DT_THROW_IF(delta_phi_ < 0,
                std::domain_error,
                "Delta phi is negative (< 0)!");
    _start_phi_ = start_phi_;
    _delta_phi_ = delta_phi_;
    return;
  }

  double sphere::get_start_phi() const
  {
    return _start_phi_;
  }

  double sphere::get_delta_phi() const
  {
    return _delta_phi_;
  }

  void sphere::set_theta(double start_theta_, double delta_theta_)
  {
    DT_THROW_IF(start_theta_ < 0.0,
                std::domain_error,
                "Start theta is too small (< 0)!");
    DT_THROW_IF(start_theta_ > M_PI,
                std::domain_error,
                "Start theta is too large (> pi)!");
    DT_THROW_IF(delta_theta_ < 0,
                std::domain_error,
                "Delta theta is negative (< 0)!");
    // DT_THROW_IF(delta_theta_ > M_PI,
    //             std::domain_error,
    //             "Delta theta is too large (> pi)!");
    DT_THROW_IF((start_theta_ + delta_theta_) > M_PI,
                std::domain_error,
                "Theta bound is too large (> pi)!");
    _start_theta_ = start_theta_;
    _delta_theta_ = delta_theta_;
    return;
  }

  double sphere::get_start_theta() const
  {
    return _start_theta_;
  }

  double sphere::get_delta_theta() const
  {
    return _delta_theta_;
  }

  void sphere::_set_default()
  {
    datatools::invalidate(_r_);
    datatools::invalidate(_r_min_);
    datatools::invalidate(_start_phi_);
    datatools::invalidate(_delta_phi_);
    datatools::invalidate(_start_theta_);
    datatools::invalidate(_delta_theta_);
    return;
  }

  sphere::sphere ()
  {
    _set_default();
  }

  sphere::sphere (double r_)
  {
    _set_default();
    set_r_max(r_);
  }

  sphere::sphere (double radius_min_, double radius_max_)
  {
    _set_default();
    set_r_min(radius_min_);
    set_r_max(radius_max_);
  }

  sphere::~sphere ()
  {
  }

  void sphere::initialize(const datatools::properties & config_,
                          const handle_dict_type * objects_)
  {
    reset();
    this->i_shape_3d::initialize(config_, objects_);

    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    double aunit = CLHEP::degree;
    if (config_.has_key ("angle_unit")) {
      const std::string aunit_str = config_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    double rmin, rmax;
    rmin = 0.0;
    if (config_.has_key ("r_max")) {
      rmax = config_.fetch_real ("r_max");
      if (! config_.has_explicit_unit ("r_max")) {
        rmax *= lunit;
      }
    } else {
      DT_THROW_IF (! config_.has_key ("r"), std::logic_error,
                   "Missing sphere 'r' property !");
      rmax = config_.fetch_real ("r");
      if (! config_.has_explicit_unit ("r")) {
        rmax *= lunit;
      }
    }
    if (config_.has_key ("r_min")) {
      rmin = config_.fetch_real ("r_min");
      if (! config_.has_explicit_unit ("r_min")) {
        rmin *= lunit;
      }
    }

    double start_theta = 0.0;
    double delta_theta = M_PI * CLHEP::radian;
    bool not_full_theta = false;
    if (config_.has_key ("start_theta")) {
      start_theta = config_.fetch_real ("start_theta");
      if (! config_.has_explicit_unit ("start_theta")) {
        start_theta *= aunit;
      }
      not_full_theta = true;
    }
    if (config_.has_key ("delta_theta")) {
      delta_theta = config_.fetch_real ("delta_theta");
      if (! config_.has_explicit_unit ("delta_theta")) {
        delta_theta *= aunit;
      }
      not_full_theta = true;
    }

    double start_phi = 0.0;
    double delta_phi = 2 * M_PI * CLHEP::radian;
    bool not_full_phi = false;
    if (config_.has_key ("start_phi")) {
      start_phi = config_.fetch_real ("start_phi");
      if (! config_.has_explicit_unit ("start_phi")) {
        start_phi *= aunit;
      }
      not_full_phi = true;
    }
    if (config_.has_key ("delta_phi")) {
      delta_phi = config_.fetch_real ("delta_phi");
      if (! config_.has_explicit_unit ("delta_phi")) {
        delta_phi *= aunit;
      }
      not_full_phi = true;
    }
    if (rmin > 0.0) {
      set_r_min(rmin);
    }
    set_r_max(rmax);
    if (not_full_theta) {
      set_theta(start_theta, delta_theta);
    }
    if (not_full_phi) {
      set_phi(start_phi, delta_phi);
    }

    lock();
    return;
  }

  double
  sphere::get_surface (uint32_t mask_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    double delta_phi = 2 * M_PI;
    if (has_partial_phi()) {
      delta_phi = _delta_phi_;
    }
    double start_theta = 0.0;
    double delta_theta = M_PI;
    if (has_partial_theta()) {
      start_theta = _start_theta_;
      delta_theta = _delta_theta_;
    }
    double stop_theta = start_theta + delta_theta;
    double r_min = 0.0;
    if (has_r_min()) {
      r_min = _r_min_;
    }
    double s = 0.0;
    if  (mask_ & FACE_OUTER_SIDE) {
      s += delta_phi
        * (std::cos(start_theta) - std::cos(stop_theta))
        * _r_ * _r_;
    }
    if  (mask_ & FACE_INNER_SIDE) {
      s += delta_phi
        * (std::cos(start_theta) - std::cos(stop_theta))
        * r_min * r_min;
    }
    if  (mask_ & FACE_START_PHI_SIDE) {
      s += 0.5 * ( _r_ * _r_ -  r_min * r_min)
        * delta_theta;
    }
    if  (mask_ & FACE_STOP_PHI_SIDE) {
      s += 0.5 * ( _r_ * _r_ -  r_min * r_min)
        * delta_theta;
    }
    if  (mask_ & FACE_START_THETA_SIDE) {
      s += 0.5 * delta_phi * ( _r_ * _r_ - r_min * r_min ) * std::sin(start_theta);
    }
    if  (mask_ & FACE_STOP_THETA_SIDE) {
      s += 0.5 * delta_phi * ( _r_ * _r_ - r_min * r_min ) * std::sin(stop_theta);
    }
    return s;
  }

  double
  sphere::get_volume (uint32_t /*flags*/) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    double delta_phi = 2 * M_PI;
    if (has_partial_phi()) {
      delta_phi = _delta_phi_;
    }
    double start_theta = 0.0;
    double delta_theta = M_PI;
    if (has_partial_theta()) {
      start_theta = _start_theta_;
      delta_theta = _delta_theta_;
    }
    double stop_theta = start_theta + delta_theta;
    double r_min = 0.0;
    if (has_r_min()) {
      r_min = _r_min_;
    }
    double v =
      delta_phi
      * (std::cos(start_theta) - std::cos(stop_theta))
      * (gsl_pow_3(_r_) - gsl_pow_3(r_min)) / 3.0;
    return v;
  }

  bool
  sphere::is_valid () const
  {
    if (datatools::is_valid(_r_min_)) {
      if (_r_min_ < 0.0) return false;
    }
    return datatools::is_valid(_r_)
      && (_r_ > 0.0);
  }

  void
  sphere::reset ()
  {
    unlock();

    _set_default();

    this->i_shape_3d::reset();
    return;
  }

  bool sphere::has_partial_phi() const
  {
    return datatools::is_valid(_start_phi_) && datatools::is_valid(_delta_phi_);
  }

  bool sphere::has_partial_theta() const
  {
    return datatools::is_valid(_start_theta_) && datatools::is_valid(_delta_theta_);
  }

  bool sphere::is_orb() const
  {
    if (! datatools::is_valid(_r_)) return false;
    if (has_r_min()) return false;
    if (has_partial_phi()) return false;
    if (has_partial_theta()) return false;
    return true;
  }

  std::string
  sphere::get_shape_name () const
  {
    return sphere_label();
  }

  void sphere::compute_side_face(faces_mask_type type_,
                                 spherical_sector & face_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    if (type_ == FACE_INNER_SIDE) {
      compute_inner_side_face(face_);
    } else if (type_ == FACE_OUTER_SIDE) {
      compute_outer_side_face(face_);
    } else {
      DT_THROW(std::logic_error, "Invalid face type [" << type_<< "]!");
    }
    return;
  }

  void sphere::compute_inner_side_face(spherical_sector & inner_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    inner_.reset();
    if (has_r_min()) {
      inner_.set_radius(_r_min_);
      if (has_partial_theta()) {
        inner_.set_start_theta(_start_theta_);
        inner_.set_delta_theta(_delta_theta_);
      }
      if (has_partial_phi()) {
        inner_.set_start_phi(_start_phi_);
        inner_.set_delta_phi(_delta_phi_);
      }
    }
    return;
  }

  void sphere::compute_outer_side_face(spherical_sector & outer_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    outer_.reset();
    outer_.set_radius(_r_);
    if (has_partial_theta()) {
      outer_.set_start_theta(_start_theta_);
      outer_.set_delta_theta(_delta_theta_);
    }
    if (has_partial_phi()) {
      outer_.set_start_phi(_start_phi_);
      outer_.set_delta_phi(_delta_phi_);
    }
    return;
  }

  void sphere::compute_start_theta_face(right_circular_conical_nappe & face_,
                                        disk & dface_,
                                        placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    face_.reset();
    dface_.reset();
    face_placement_.reset();
    if (has_partial_theta()) {
      double theta = _start_theta_;
      if (std::abs(theta - 0.5 * M_PI) < get_angular_tolerance()) {
        dface_.set_outer_r(_r_);
        if (has_r_min()) {
          dface_.set_inner_r(_r_min_);
        }
        if (has_partial_phi()) {
          dface_.set_start_angle(_start_phi_);
          dface_.set_delta_angle(_delta_phi_);
        }
        face_placement_.set(0.0, 0.0, 0.0, 0, 0, 0);
     } else {
        double ztop = _r_ * std::cos(theta);
        double r_min = 0.0;
        if (has_r_min()) {
          r_min = _r_min_;
        }
        double zbottom = r_min * std::cos(theta);
        double rtop = _r_ * std::sin(theta);
        double rbottom = r_min * std::sin(theta);
        face_.set_top_radius(ztop > zbottom ? rtop : rbottom);
        face_.set_bottom_radius(ztop > zbottom ? rbottom : rtop);
        face_.set_z(std::abs(ztop - zbottom));
        if (has_partial_phi()) {
          face_.set_start_angle(_start_phi_);
          face_.set_delta_angle(_delta_phi_);
        }
        face_placement_.set(0.0, 0.0, 0.5 * (zbottom + ztop), 0, 0, 0);
      }
    }
    return;
  }

  void sphere::compute_stop_theta_face(right_circular_conical_nappe & face_,
                                       disk & dface_,
                                       placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    face_.reset();
    dface_.reset();
    face_placement_.reset();
    if (has_partial_theta()) {
      double theta = _start_theta_ + _delta_theta_;
      if (std::abs(theta - 0.5 * M_PI) < get_angular_tolerance()) {
        dface_.set_outer_r(_r_);
        if (has_r_min()) {
          dface_.set_inner_r(_r_min_);
        }
        if (has_partial_phi()) {
          dface_.set_start_angle(_start_phi_);
          dface_.set_delta_angle(_delta_phi_);
        }
        face_placement_.set(0.0, 0.0, 0.0, 0, 0, 0);
      } else {
        double ztop = _r_ * std::cos(theta);
        double zbottom = 0.0;
        double r_min = 0.0;
        if (has_r_min()) {
          r_min = _r_min_;
        }
        zbottom = r_min * std::cos(theta);
        double rtop = _r_ * std::sin(theta);
        double rbottom = r_min * std::sin(theta);
        face_.set_top_radius(ztop > zbottom ? rtop : rbottom);
        face_.set_bottom_radius(ztop > zbottom ? rbottom : rtop);
        face_.set_z(std::abs(ztop - zbottom));
        if (has_partial_phi()) {
          face_.set_start_angle(_start_phi_);
          face_.set_delta_angle(_delta_phi_);
        }
        face_placement_.set(0.0, 0.0, 0.5 * (zbottom + ztop), 0, 0, 0);
      }
    }
    return;
  }

  void sphere::compute_start_stop_theta_face(faces_mask_type type_,
                                             right_circular_conical_nappe & face_,
                                             disk & dface_,
                                             placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    if (type_ == FACE_START_THETA_SIDE) {
      compute_start_theta_face(face_, dface_, face_placement_);
    } else if (type_ == FACE_STOP_THETA_SIDE) {
      compute_stop_theta_face(face_, dface_, face_placement_);
    } else {
      DT_THROW(std::logic_error, "Invalid face type [" << type_<< "]!");
    }
    return;
  }


  void sphere::compute_start_phi_face(disk & face_,
                                      placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    face_.reset();
    face_placement_.reset();
    if (has_partial_phi()) {
      double start_phi = _start_phi_;
      face_.set_r(_r_);
      if (has_r_min()) {
        face_.set_inner_r(_r_min_);
      }
      double start_theta = 0.0;
      double delta_theta = M_PI;
      if (has_partial_theta()) {
        start_theta = _start_theta_;
        delta_theta = _delta_theta_;
      }
      face_.set_start_angle(start_theta);
      face_.set_delta_angle(delta_theta);
      face_placement_.set_translation(0.0, 0.0, 0.0);
      face_placement_.set_orientation(get_special_rotation_angle(ROTATION_ANGLE_270),
                                      get_special_rotation_angle(ROTATION_ANGLE_270) + start_phi,
                                      0.0,
                                      EULER_ANGLES_YXY);

      // face_placement_.set_orientation(start_phi + get_special_rotation_angle(ROTATION_ANGLE_90),
      //                                      get_special_rotation_angle(ROTATION_ANGLE_90),
      //                                      0.0
      //                                      );
    }
    return;
  }

  void sphere::compute_stop_phi_face(disk & face_,
                                       placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    face_.reset();
    face_placement_.reset();
    if (has_partial_phi()) {
      double start_phi = _start_phi_;
      double delta_phi = _delta_phi_;
      double stop_phi = start_phi + delta_phi;
      face_.set_r(_r_);
      if (has_r_min()) {
        face_.set_inner_r(_r_min_);
      }
      double start_theta = 0.0;
      double delta_theta = M_PI;
      if (has_partial_theta()) {
        start_theta = _start_theta_;
        delta_theta = _delta_theta_;
      }
      face_.set_start_angle(start_theta);
      face_.set_delta_angle(delta_theta);
      face_placement_.set_translation(0.0, 0.0, 0.0);
      face_placement_.set_orientation(get_special_rotation_angle(ROTATION_ANGLE_270),
                                      get_special_rotation_angle(ROTATION_ANGLE_270) + stop_phi,
                                      0.0,
                                      EULER_ANGLES_YXY);
      // face_placement_.set_orientation(-ROTATION_ANGLE_90,
      //                                 0.0, //-ROTATION_ANGLE_90 + stop_phi,
      //                                 0.0,
      //                                 EULER_ANGLES_YXY);
    }
    return;
  }

  void sphere::compute_start_stop_phi_face(faces_mask_type type_,
                                           disk & face_,
                                           placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    if (type_ == FACE_START_PHI_SIDE) {
      compute_start_phi_face(face_, face_placement_);
    } else if (type_ == FACE_STOP_PHI_SIDE) {
      compute_stop_phi_face(face_, face_placement_);
    } else {
      DT_THROW(std::logic_error, "Invalid face type [" << type_<< "]!");
    }
    return;
  }

  // virtual
  unsigned int sphere::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    unsigned int nfaces = 0;

    {
      // Outer side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      spherical_sector & ss = finfo.add_face<spherical_sector>();
      finfo.grab_positioning().set_identity();
      compute_side_face(FACE_OUTER_SIDE, ss);
      face_identifier fid;
      fid.set_face_bit(FACE_OUTER_SIDE);
      finfo.set_identifier(fid);
      finfo.set_label("outer_side");
      nfaces++;
    }

    if (has_r_min()) {
      // Inner side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      spherical_sector & ss = finfo.add_face<spherical_sector>();
      finfo.grab_positioning().set_identity();
      compute_side_face(FACE_INNER_SIDE, ss);
      face_identifier fid;
      fid.set_face_bit(FACE_INNER_SIDE);
      finfo.set_identifier(fid);
      finfo.set_label("inner_side");
      nfaces++;
    }

    if (has_partial_theta()) {

      {
        // Start theta face:
        // std::cerr << "DEVEL: sphere::compute_faces: start theta=" << _start_theta_ / M_PI << " pi" << std::endl;
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        if (std::abs(_start_theta_ - 0.5 * M_PI) < get_angular_tolerance()) {
          // std::cerr << "DEVEL: sphere::compute_faces: DISK!" << std::endl;
          right_circular_conical_nappe rccn;
          disk & d =  finfo.add_face<disk>();
          compute_start_theta_face(rccn, d, finfo.grab_positioning());
        } else {
          // std::cerr << "DEVEL: sphere::compute_faces: RCCN!" << std::endl;
          right_circular_conical_nappe & rccn = finfo.add_face<right_circular_conical_nappe>();
          disk d;
          compute_start_theta_face(rccn, d, finfo.grab_positioning());
        }
        face_identifier fid;
        fid.set_face_bit(FACE_START_THETA_SIDE);
        finfo.set_identifier(fid);
        finfo.set_label("start_theta");
        nfaces++;
      }

      {
        // Stop theta face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        double stop_theta = _start_theta_ + _delta_theta_;
        // std::cerr << "DEVEL: sphere::compute_faces: stop theta=" << stop_theta / M_PI << " pi" << std::endl;
        if (std::abs(stop_theta - 0.5 * M_PI) < get_angular_tolerance()) {
          // std::cerr << "DEVEL: sphere::compute_faces: DISK!" << std::endl;
          right_circular_conical_nappe rccn;
          disk & d =  finfo.add_face<disk>();
          compute_stop_theta_face(rccn, d, finfo.grab_positioning());
        } else {
          // std::cerr << "DEVEL: sphere::compute_faces: RCCN!" << std::endl;
          right_circular_conical_nappe & rccn = finfo.add_face<right_circular_conical_nappe>();
          disk d;
          compute_stop_theta_face(rccn, d, finfo.grab_positioning());
        }
        face_identifier fid;
        fid.set_face_bit(FACE_STOP_THETA_SIDE);
        finfo.set_identifier(fid);
        finfo.set_label("stop_theta");
        nfaces++;
      }

    }

    if (has_partial_phi()) {

      {
        // Start phi face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        disk & d = finfo.add_face<disk>();
        compute_start_phi_face(d, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_START_PHI_SIDE);
        finfo.set_identifier(fid);
        finfo.set_label("start_phi");
        nfaces++;
      }

      {
        // Stop phi face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        disk & d = finfo.add_face<disk>();
        compute_stop_phi_face(d, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_STOP_PHI_SIDE);
        finfo.set_identifier(fid);
        finfo.set_label("stop_phi");
        nfaces++;
      }

    }

    return nfaces;
  }

  bool
  sphere::is_inside (const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;
    double r = point_.mag();
    if ( r > (_r_ - hskin) ) {
      return false;
    }
    if (has_r_min()) {
      if ( r < (_r_min_ - hskin) ) {
        return false;
      }
    }
    double theta = point_.theta();
    double rho   = r * std::sin(theta);
    double phi = point_.phi();
    double eps_phi = std::atan2(hskin, rho);
    if (has_partial_phi()) {
      if (! angle_is_in(phi, _start_phi_ + eps_phi , _delta_phi_ - 2 * eps_phi, 0.0)) {
        return false;
      }
    }
    if (has_partial_theta()) {
      double eps_theta = std::atan2(hskin, r);
      if (!angle_is_in(theta, _start_theta_ + eps_theta, _delta_theta_ - 2 * eps_theta, 0.0)) {
        return false;
      }
    }
    return true;
  }

  bool
  sphere::is_outside (const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;
    double r = point_.mag();
    if ( r > (_r_ + hskin) ) {
      return true;
    }
    if (has_r_min()) {
      if ( r < (_r_min_ - hskin) ) {
        return true;
      }
    }
    double theta = point_.theta();
    double rho   = r * std::sin(theta);
    double phi = point_.phi();
    double eps_phi = std::atan2(hskin, rho);
    if (has_partial_phi()) {
      if (! angle_is_in(phi, _start_phi_ - eps_phi , _delta_phi_ + 2 * eps_phi, 0.0)) {
        return true;
      }
    }
    if (has_partial_theta()) {
      double eps_theta = std::atan2(hskin, r);
      if (!angle_is_in(theta, _start_theta_ - eps_theta, _delta_theta_ + 2 * eps_theta, 0.0)) {
        return true;
      }
    }
    return false;
  }

  double
  sphere::get_parameter (const std::string & flag_) const
  {
    if  (flag_ == "r") return get_r ();
    if  (flag_ == "radius") return get_r ();
    if  (flag_ == "outer_radius") return get_r_max();
    if  (flag_ == "inner_radius") return get_r_min();
    if  (flag_ == "outer_diameter") return 2 * get_r_max();
    if  (flag_ == "diameter") return 2 * get_r();
    if  (flag_ == "inner_diameter") return 2 * get_r_min();
    if  (flag_ == "volume") return get_volume();
    if  (flag_ == "surface.side") return get_surface(FACE_SIDE);
    if  (flag_ == "surface.outer_side") return get_surface(FACE_SIDE);
    if  (flag_ == "surface.inner_side") return get_surface(FACE_INNER_SIDE);
    if  (flag_ == "surface.start_theta_side") return get_surface(FACE_START_THETA_SIDE);
    if  (flag_ == "surface.stop_theta_side") return get_surface(FACE_STOP_THETA_SIDE);
    if  (flag_ == "surface.start_phi_side") return get_surface(FACE_START_PHI_SIDE);
    if  (flag_ == "surface.stop_phi_side") return get_surface(FACE_STOP_PHI_SIDE);
    if  (flag_ == "surface") return get_surface (FACE_ALL);
    DT_THROW_IF(true, std::logic_error,"Unknown flag '" << flag_ << "' !");
  }

  vector_3d
  sphere::get_normal_on_surface (const vector_3d & a_position,
                                 const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid sphere!");
    vector_3d normal;
    invalidate(normal);
    switch(a_surface_bit.get_face_bits()) {
    case FACE_OUTER_SIDE :
      normal = a_position.unit();
      break;
    case FACE_INNER_SIDE :
      if (has_r_min()) {
        normal = -a_position.unit();
      }
      break;
    case FACE_START_THETA_SIDE :
      if (has_partial_theta()) {
        right_circular_conical_nappe face;
        disk dface;
        placement face_placement;
        compute_start_stop_theta_face(FACE_START_THETA_SIDE, face, dface, face_placement);
        vector_3d p_face;
        face_placement.mother_to_child(a_position, p_face);
        vector_3d p_normal;
        if (face.is_valid()) {
          p_normal = face.get_normal_on_surface(p_face);
        }
        if (dface.is_valid()) {
          p_normal = dface.get_normal_on_surface(p_face);
        }
        face_placement.child_to_mother_direction(p_normal, normal);
        normal *= -1.0;
      }
      break;
    case FACE_STOP_THETA_SIDE :
      if (has_partial_theta()) {
        right_circular_conical_nappe face;
        disk dface;
        placement face_placement;
        compute_start_stop_theta_face(FACE_STOP_THETA_SIDE, face, dface, face_placement);
        vector_3d p_face;
        face_placement.mother_to_child(a_position, p_face);
        vector_3d p_normal;
        if (face.is_valid()) {
          p_normal = face.get_normal_on_surface(p_face);
        }
        if (dface.is_valid()) {
          p_normal = dface.get_normal_on_surface(p_face);
        }
       face_placement.child_to_mother_direction(p_normal, normal);
      }
      break;
    case FACE_START_PHI_SIDE :
      if (has_partial_phi()) {
        disk face;
        placement face_placement;
        compute_start_stop_phi_face(FACE_START_PHI_SIDE, face, face_placement);
        vector_3d p_face;
        face_placement.mother_to_child(a_position, p_face);
        vector_3d p_normal = face.get_normal_on_surface(p_face);
        face_placement.child_to_mother_direction(p_normal, normal);
        normal *= -1.0;
      }
      break;
    case FACE_STOP_PHI_SIDE :
      if (has_partial_phi()) {
        disk face;
        placement face_placement;
        compute_start_stop_phi_face(FACE_STOP_PHI_SIDE, face, face_placement);
        vector_3d p_face;
        face_placement.mother_to_child(a_position, p_face);
        vector_3d p_normal = face.get_normal_on_surface(p_face);
        face_placement.child_to_mother_direction(p_normal, normal);
      }
      break;
    }
    return normal;
  }


  face_identifier sphere::on_surface(const vector_3d & a_position,
                                     const face_identifier & a_surface_mask,
                                     double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Sphere is not valid!");
    double skin = compute_tolerance(a_skin);

    face_identifier mask;
    if (a_surface_mask.is_valid()) {
      DT_THROW_IF(! a_surface_mask.is_face_bits_mode(), std::logic_error,
                  "Face mask uses no face bits!");
      mask = a_surface_mask;
    } else {
      mask.set_face_bits_any();
    }

    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (finfo.is_valid() && mask.has_face_bit(finfo.get_identifier().get_face_bits())) {
        vector_3d position_c;
        finfo.get_positioning().mother_to_child(a_position, position_c);
        if (finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
      }
    }

    return face_identifier(FACE_NONE);
  }

  bool
  sphere::find_intercept(const vector_3d & from_,
                         const vector_3d & direction_,
                         face_intercept_info & intercept_,
                         double skin_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");
    intercept_.reset ();
    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 6;
    face_intercept_info intercepts[NFACES];
    unsigned int candidate_impact_counter = 0;

    int face_counter = 0;
    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (!finfo.is_valid()) {
        continue;
      }
      const i_shape_2d & face = finfo.get_face_ref();
      const placement & face_placement = finfo.get_positioning();
      const face_identifier & face_id = finfo.get_identifier();
      if (face.i_find_intercept::find_intercept(from_,
                                                direction_,
                                                face_placement,
                                                intercepts[face_counter],
                                                skin)) {
        intercepts[face_counter].set_face_id(face_id);
        candidate_impact_counter++;
      }
      face_counter++;
    }

    if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
        if (intercepts[iface].is_ok()) {
          double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
          if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            intercept_.set_face_id(intercepts[iface].get_face_id());
            intercept_.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    return intercept_.is_ok ();
  }

  std::ostream &
  operator<< (std::ostream & out_, const sphere & s_)
  {
    out_ << '{' << sphere::sphere_label() << ' '
         << s_._r_ << ' '
         << s_._r_min_ << ' '
         << s_._start_phi_ << ' '
         << s_._delta_phi_ << ' '
         << s_._start_theta_ << ' '
         << s_._delta_theta_ << '}';
    return out_;
  }

  std::istream &
  operator>> (std::istream & in_, sphere & s_)
  {
    s_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    std::string name;
    in_ >> name;
    if (name != sphere::sphere_label()) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    double r, rmin;
    rmin = 0.0;
    in_ >> r >> rmin;
    if (! in_) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    double start_phi, delta_phi;
    in_ >> start_phi >> delta_phi;
    if (! in_)
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    double start_theta, delta_theta;
    in_ >> start_theta >> delta_theta;
    if (! in_)
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    c = 0;
    in_.get (c);
    if (c != '}')  {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    try {
      s_.set(r);
      s_.set_r_min(rmin);
    }
    catch (...) {
      s_.reset ();
      in_.clear (std::ios_base::failbit);
    }
    return in_;
  }

  void sphere::tree_dump (std::ostream & out_,
                          const std::string & title_,
                          const std::string & indent_,
                          bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "R : " << _r_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "R(min) : " << _r_min_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Start phi : " << _start_phi_ / CLHEP::degree << " degree" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Delta phi : " << _delta_phi_ / CLHEP::degree << " degree" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Start theta : " << _start_theta_ / CLHEP::degree << " degree" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Delta theta : " << _delta_theta_ / CLHEP::degree << " degree" << std::endl;

    return;
  }


  void sphere::generate_wires_self(wires_type & wires_,
                                   uint32_t options_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Sphere is not valid !");

    bool draw_inner_face       = !(options_ & WR_SPHERE_NO_INNER_FACE);
    bool draw_outer_face       = !(options_ & WR_SPHERE_NO_OUTER_FACE);
    bool draw_start_theta_face = !(options_ & WR_SPHERE_NO_START_THETA_FACE);
    bool draw_stop_theta_face  = !(options_ & WR_SPHERE_NO_STOP_THETA_FACE);
    bool draw_start_phi_face   = !(options_ & WR_SPHERE_NO_START_PHI_FACE);
    bool draw_stop_phi_face    = !(options_ & WR_SPHERE_NO_STOP_PHI_FACE);
    bool draw_boundings        =  (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    // uint32_t nsampling_theta = i_wires_3d_rendering::angular_sampling_from_options(base_options);
    // uint32_t nsampling_phi   = i_wires_3d_rendering::angular_sampling_from_options(base_options);

    if (has_r_min()) {

      if (draw_inner_face) {
        spherical_sector face;
        placement face_placement;
        face_placement.set_identity();
        compute_inner_side_face(face);
        uint32_t options = base_options;
        face.generate_wires(wires_, face_placement, options);
      }

    }

    if (draw_outer_face) {
      spherical_sector face;
      placement face_placement;
      face_placement.set_identity();
      compute_outer_side_face(face);
      uint32_t options = base_options;
      face.generate_wires(wires_, face_placement, options);
    }

    if (has_partial_theta()) {

      if (draw_start_theta_face) {
        right_circular_conical_nappe face;
        disk dface;
        placement face_placement;
        compute_start_theta_face(face, dface, face_placement);
        uint32_t options = base_options;
        if (face.is_valid()) face.generate_wires(wires_, face_placement, options);
        else if (dface.is_valid()) dface.generate_wires(wires_, face_placement, options);
     }

      if (draw_stop_theta_face) {
        right_circular_conical_nappe face;
        disk dface;
        placement face_placement;
        compute_stop_theta_face(face, dface, face_placement);
        uint32_t options = base_options;
        if (face.is_valid()) face.generate_wires(wires_, face_placement, options);
        else if (dface.is_valid()) dface.generate_wires(wires_, face_placement, options);
      }
    }

    if (has_partial_phi()) {

      if (draw_start_phi_face) {
        disk face;
        placement face_placement;
        compute_start_phi_face(face, face_placement);
        uint32_t options = base_options;
        face.generate_wires(wires_, face_placement, options);
      }

      if (draw_stop_phi_face) {
        disk face;
        placement face_placement;
        compute_stop_phi_face(face, face_placement);
        uint32_t options = base_options;
        face.generate_wires(wires_, face_placement, options);
      }

    }

    return;
  }

  // static
  void sphere::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::sphere")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default \n"
                              "length unit for length parameters of the box.  \n"
                              )
        .add_example("Set the default length unit::       \n"
                     "                                    \n"
                     "  length_unit : string = \"mm\"     \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("angle_unit")
        .set_from("geomtools::sphere")
        .set_terse_description("The unit symbol used for angle unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("degree")
        .set_long_description("A character string that represents the default \n"
                              "angle unit for angle parameters of the sphere.  \n"
                              )
        .add_example("Set the default angle unit::        \n"
                     "                                    \n"
                     "  angle_unit : string = \"mm\"      \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("r_max")
        .set_from("geomtools::sphere")
        .set_terse_description("The outer radius of the sphere")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the outer radius::             \n"
                     "                                   \n"
                     "  r_max : real as length = 5.0 cm  \n"
                     "                                   \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("r_min")
        .set_from("geomtools::sphere")
        .set_terse_description("The inner radius of the sphere")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the inner radius::             \n"
                     "                                   \n"
                     "  r_min : real as length = 1.0 cm  \n"
                     "                                   \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("start_phi")
        .set_from("geomtools::sphere")
        .set_terse_description("The starting phi angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the starting phi angle::              \n"
                     "                                          \n"
                     "  start_phi : real as angle = 30.0 degree \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("delta_phi")
        .set_from("geomtools::sphere")
        .set_terse_description("The delta phi angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(2 * M_PI * CLHEP::radian)
        .add_example("Set the delta phi angle::                 \n"
                     "                                          \n"
                     "  delta_phi : real as angle = 60.0 degree \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("start_theta")
        .set_from("geomtools::sphere")
        .set_terse_description("The starting theta angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the starting theta angle::              \n"
                     "                                            \n"
                     "  start_theta : real as angle = 0.0 degree  \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("delta_theta")
        .set_from("geomtools::sphere")
        .set_terse_description("The delta theta angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(M_PI * CLHEP::radian)
        .add_example("Set the delta theta angle::                 \n"
                     "                                            \n"
                     "  delta_theta : real as angle = 90.0 degree \n"
                     "                                            \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::sphere the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::sphere, ocd_)
{
  // The class name :
  ocd_.set_class_name ("geomtools::sphere");

  // The class terse description :
  ocd_.set_class_description ("A class representing a 3D sphere solid shape");

  // The library the class belongs to :
  ocd_.set_class_library ("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The sphere solid 3D-shape");

  // Populate the specific OCD :
  geomtools::sphere::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the       \n"
                               "``datatools::properties`` ASCII format::          \n"
                               "                                                  \n"
                               "  tolerance.length : real as length = 1.0e-6 um   \n"
                               "  tolerance.angle  : real as angle = 1.0e-6 radian\n"
                               "  length_unit : string = \"mm\"                   \n"
                               "  angle_unit : string = \"degree\"                \n"
                               "  r_max : real as length = 15.0 mm                \n"
                               "  r_min : real as length = 10.0 mm                \n"
                               "                                                  \n"
                               );

  /** Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides (see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'geomtools::sphere' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::sphere, "geomtools::sphere")
