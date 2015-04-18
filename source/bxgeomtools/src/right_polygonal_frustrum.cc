// right_polygonal_frustrum.cc

// Ourselves:
#include <geomtools/right_polygonal_frustrum.h>

// Standard library:
#include <cmath>
#include <iostream>

// Third-party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/polynomial.h>

// This project:
#include <geomtools/line_3d.h>
#include <geomtools/quadrangle.h>
#include <geomtools/regular_polygon.h>
#include <geomtools/triangle.h>
#include <geomtools/composite_surface.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(right_polygonal_frustrum,
                                             "geomtools::right_polygonal_frustrum");

  const std::string & right_polygonal_frustrum::rpf_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "right_polygonal_frustrum";
    }
    return label;
  }

  std::string right_polygonal_frustrum::get_shape_name() const
  {
    return right_polygonal_frustrum::rpf_label();
  }

  bool right_polygonal_frustrum::is_valid() const
  {
    if (!datatools::is_valid(_outer_bottom_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_outer_top_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_z_)) {
      return false;
    }
    return true;
  }

  bool right_polygonal_frustrum::has_top_face() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polygonal frustrum!");
    if (datatools::is_valid(_outer_top_radius_)) {
      if (_outer_top_radius_ == 0.0) {
        return false;
      } else if (datatools::is_valid(_inner_top_radius_)) {
        if (_outer_top_radius_ == _inner_top_radius_) {
          return false;
        }
      }
    }
    return true;
  }

  bool right_polygonal_frustrum::has_bottom_face() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polygonal frustrum!");
    if (datatools::is_valid(_outer_bottom_radius_)) {
      if (_outer_bottom_radius_ == 0.0) {
        return false;
      } else if (datatools::is_valid(_inner_bottom_radius_)) {
        if (_outer_bottom_radius_ == _inner_bottom_radius_) {
          return false;
        }
      }
    }
    return true;
  }

  bool right_polygonal_frustrum::has_inner_face() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polygonal frustrum!");
    if (has_inner_bottom_radius() || has_inner_top_radius()) {
      return true;
    }
    return false;
  }

  double right_polygonal_frustrum::get_xmin() const
  {
    return -std::max(_outer_bottom_radius_, _outer_top_radius_);
  }

  double right_polygonal_frustrum::get_xmax() const
  {
    return +std::max(_outer_bottom_radius_, _outer_top_radius_);
  }

  double right_polygonal_frustrum::get_ymin() const
  {
    return -std::max(_outer_bottom_radius_, _outer_top_radius_);
  }

  double right_polygonal_frustrum::get_ymax() const
  {
    return +std::max(_outer_bottom_radius_, _outer_top_radius_);
  }

  double right_polygonal_frustrum::get_zmin() const
  {
    return -0.5 *_z_;
  }

  double right_polygonal_frustrum::get_zmax() const
  {
    return +0.5 *_z_;
  }

  uint32_t right_polygonal_frustrum::get_n_sides() const
  {
    return _n_sides_;
  }

  double right_polygonal_frustrum::get_sector_angle() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    return 2 * M_PI / _n_sides_;
  }

  double right_polygonal_frustrum::get_start_angle(unsigned int sector_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    DT_THROW_IF(sector_ < _sector_start_ || sector_ > _sector_stop_,
                std::range_error, "Invalid sector indedx [" << sector_ << "]!");
    double alpha = get_sector_angle();
    int sector = sector_;
    double start_theta = sector * alpha;
    // std::cerr << "DEVEL: right_polygonal_frustrum::get_start_angle: "
    //           << "sector= " << sector_
    //           << " : start_theta=" << start_theta / CLHEP::degree
    //           << " degree"
    //           << std::endl;
    return start_theta;
  }


  void right_polygonal_frustrum::set_n_sides(uint32_t ns_)
  {
    DT_THROW_IF(ns_ < MIN_NUMBER_OF_SIDES, std::logic_error, "Invalid number of sides [" << ns_ << "]!");
    _n_sides_ = ns_;
    _sector_start_ = 0;
    _sector_stop_ = _n_sides_ - 1;
    return;
  }

  uint32_t right_polygonal_frustrum::get_sector_start() const
  {
    return _sector_start_;
  }

  uint32_t right_polygonal_frustrum::get_sector_stop() const
  {
    return _sector_stop_;
  }

  double right_polygonal_frustrum::get_z() const
  {
    return _z_;
  }

  void right_polygonal_frustrum::set_z(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error ,
                 "Invalid '" << new_value_ << "' Z value !");
    _z_ = new_value_;
    return;
  }

  bool right_polygonal_frustrum::has_inner_bottom_radius() const
  {
    return datatools::is_valid(_inner_bottom_radius_) && _inner_bottom_radius_ > 0;
  }

  void right_polygonal_frustrum::set_inner_bottom_radius(double inner_r_)
  {
    DT_THROW_IF (inner_r_ < 0.0, std::domain_error, "Invalid inner bottom radius '" << inner_r_ << "' !");
    DT_THROW_IF (_outer_bottom_radius_ < inner_r_,
                 std::domain_error,
                 "Outer bottom radius '"
                 << _outer_bottom_radius_ << "' <= inner bottom radius '"
                 << inner_r_ << "'!");
    _inner_bottom_radius_ = inner_r_;
    if (!datatools::is_valid(_inner_top_radius_) && datatools::is_valid(_inner_bottom_radius_)) {
      _inner_top_radius_ = 0.0;
    }
    return;
  }

  double right_polygonal_frustrum::get_inner_bottom_radius() const
  {
    return _inner_bottom_radius_;
  }

  void right_polygonal_frustrum::set_outer_bottom_radius(double outer_r_)
  {
    DT_THROW_IF(outer_r_ < 0.0, std::domain_error, "Invalid outer bottom radius '" << outer_r_ << "' !");
    DT_THROW_IF (outer_r_ < _inner_bottom_radius_,
                 std::domain_error,
                 "Outer bottom radius '"
                 << outer_r_ << "' <= inner bottom radius '"
                 << _inner_bottom_radius_ << "'!");
    _outer_bottom_radius_ = outer_r_;
    return;
  }

  double right_polygonal_frustrum::get_outer_bottom_radius() const
  {
    return _outer_bottom_radius_;
  }

  bool right_polygonal_frustrum::has_inner_top_radius() const
  {
    return datatools::is_valid(_inner_top_radius_) && _inner_top_radius_ > 0;
  }

  void right_polygonal_frustrum::set_inner_top_radius(double inner_r_)
  {
    DT_THROW_IF (inner_r_ < 0.0, std::domain_error, "Invalid inner top radius '" << inner_r_ << "' !");
    DT_THROW_IF (_outer_top_radius_ < inner_r_,
                 std::domain_error,
                 "Outer top radius '"
                 << _outer_top_radius_ << "' <= inner top radius '"
                 << inner_r_ << "'!");
    _inner_top_radius_ = inner_r_;
    if (!datatools::is_valid(_inner_bottom_radius_) && datatools::is_valid(_inner_top_radius_)) {
      _inner_bottom_radius_ = 0.0;
    }
    return;
  }

  double right_polygonal_frustrum::get_inner_top_radius() const
  {
    return _inner_top_radius_;
  }

  void right_polygonal_frustrum::set_outer_top_radius(double outer_r_)
  {
    DT_THROW_IF(outer_r_ < 0.0, std::domain_error, "Invalid outer top radius '" << outer_r_ << "' !");
    DT_THROW_IF (outer_r_ < _inner_top_radius_,
                 std::domain_error,
                 "Outer top radius '"
                 << outer_r_ << "' <= inner top radius '"
                 << _inner_top_radius_ << "'!");
    _outer_top_radius_ = outer_r_;
    return;
  }

  double right_polygonal_frustrum::get_outer_top_radius() const
  {
    return _outer_top_radius_;
  }

  right_polygonal_frustrum::right_polygonal_frustrum()
  {
    _set_defaults();
    return;
  }

  right_polygonal_frustrum::right_polygonal_frustrum(uint32_t n_sides_,
                                                     double inner_bottom_radius_,
                                                     double outer_bottom_radius_,
                                                     double inner_top_radius_,
                                                     double outer_top_radius_,
                                                     double z_)
  {
    _set_defaults();
    set_n_sides(n_sides_);
    set_inner_bottom_radius(inner_bottom_radius_);
    set_outer_bottom_radius(outer_bottom_radius_);
    set_inner_top_radius(inner_top_radius_);
    set_outer_top_radius(outer_top_radius_);
    set_z(z_);
    return;
  }

  right_polygonal_frustrum::~right_polygonal_frustrum()
  {
    return;
  }

  void right_polygonal_frustrum::_set_defaults()
  {
    _n_sides_ = 0;
    _sector_start_ = 0;
    _sector_stop_  = 0;
    datatools::invalidate(_inner_bottom_radius_);
    datatools::invalidate(_outer_bottom_radius_);
    datatools::invalidate(_inner_top_radius_);
    datatools::invalidate(_outer_top_radius_);
    datatools::invalidate(_z_);
    return;
  }

  void right_polygonal_frustrum::reset()
  {
    unlock();
    _set_defaults();
    this->i_shape_3d::reset();
    return;
  }

  void right_polygonal_frustrum::tree_dump(std::ostream & out_,
                                           const std::string & title_,
                                           const std::string & indent_,
                                           bool inherit_) const
  {
    i_shape_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "N sides : ";
    if (_n_sides_ > 0) {
      out_ << _n_sides_ ;
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

   out_ << indent_ << datatools::i_tree_dumpable::tag
        << "Start sector : " << _sector_start_ << std::endl;

   out_ << indent_ << datatools::i_tree_dumpable::tag
        << "Stop sector  : " << _sector_stop_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Inner bottom radius : ";
    if (datatools::is_valid(_inner_bottom_radius_)) {
      out_ << _inner_bottom_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Outer bottom radius : ";
    if (datatools::is_valid(_outer_bottom_radius_)) {
      out_ << _outer_bottom_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Inner top radius : ";
    if (datatools::is_valid(_inner_top_radius_)) {
      out_ << _inner_top_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Outer top radius : ";
    if (datatools::is_valid(_outer_top_radius_)) {
      out_ << _outer_top_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Z : ";
    if (datatools::is_valid(_z_)) {
      out_ << _z_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    return;
  }

  void right_polygonal_frustrum::compute_outer_face(composite_surface & face_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    face_.reset();
    double obr = get_outer_bottom_radius();
    double otr = get_outer_top_radius();
    double z   = get_z();
    for (unsigned int isector = _sector_start_;
         isector <= _sector_stop_;
         isector++) {
      double start_angle = get_start_angle(isector);
      double stop_angle = start_angle + get_sector_angle();
      if (obr == 0) {
        geomtools::face_info & finfo = face_.add();
        geomtools::triangle & t = finfo.add_face<geomtools::triangle>();
        geomtools::vector_3d A(otr * std::cos(start_angle),
                               otr * std::sin(start_angle),
                               +0.5 * z);
        geomtools::vector_3d B(otr * std::cos(stop_angle),
                               otr * std::sin(stop_angle),
                               +0.5 * z);
        geomtools::vector_3d C(0.0,
                               0.0,
                               -0.5 * z);
        t.set_vertexes(B, A, C);
        finfo.set_identity_positioning();
      } else if (otr == 0) {
        geomtools::face_info & finfo = face_.add();
        geomtools::triangle & t = finfo.add_face<geomtools::triangle>();
        geomtools::vector_3d A(obr * std::cos(start_angle),
                               obr * std::sin(start_angle),
                               -0.5 * z);
        geomtools::vector_3d B(obr * std::cos(stop_angle),
                               obr * std::sin(stop_angle),
                               -0.5 * z);
        geomtools::vector_3d C(0.0,
                               0.0,
                               +0.5 * z);
        t.set_vertexes(A, B, C);
        finfo.set_identity_positioning();
      } else {
        geomtools::face_info & finfo = face_.add();
        geomtools::quadrangle & q = finfo.add_face<geomtools::quadrangle>();
        geomtools::vector_3d A(obr * std::cos(start_angle),
                               obr * std::sin(start_angle),
                               -0.5 * z);
        geomtools::vector_3d B(obr * std::cos(stop_angle),
                               obr * std::sin(stop_angle),
                               -0.5 * z);
        geomtools::vector_3d C(otr * std::cos(stop_angle),
                               otr * std::sin(stop_angle),
                               +0.5 * z);
        geomtools::vector_3d D(otr * std::cos(start_angle),
                               otr * std::sin(start_angle),
                               +0.5 * z);
        q.set_vertexes(A, B, C, D);
        finfo.set_identity_positioning();
      }
    }
    // std::cerr << "DEVEL: outer face #surf=" << face_.get_number_of_surfaces() << std::endl;
    return;
  }

  void right_polygonal_frustrum::compute_inner_face(composite_surface & face_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    face_.reset();
    if (has_inner_face()) {
      double ibr = 0.0;
      if (has_inner_bottom_radius()) {
        ibr = get_inner_bottom_radius();
      }
      double itr = 0.0;
      if (has_inner_top_radius()) {
        itr = get_inner_top_radius();
      }
      double z = get_z();
      for (unsigned int isector = _sector_start_;
           isector <= _sector_stop_;
           isector++) {
        double start_angle = get_start_angle(isector);
        double stop_angle = start_angle + get_sector_angle();
        if (ibr == 0) {
           geomtools::face_info & finfo = face_.add();
           geomtools::triangle & t = finfo.add_face<geomtools::triangle>();
           geomtools::vector_3d A(itr * std::cos(start_angle),
                                  itr * std::sin(start_angle),
                                  +0.5 * z);
           geomtools::vector_3d B(itr * std::cos(stop_angle),
                                  itr * std::sin(stop_angle),
                                  +0.5 * z);
           geomtools::vector_3d C(0.0,
                                  0.0,
                                  -0.5 * z);
           t.set_vertexes(A, B, C);
           finfo.set_identity_positioning();
        } else if (itr == 0) {
           geomtools::face_info & finfo = face_.add();
           geomtools::triangle & t = finfo.add_face<geomtools::triangle>();
           geomtools::vector_3d A(ibr * std::cos(start_angle),
                                  ibr * std::sin(start_angle),
                                  -0.5 * z);
           geomtools::vector_3d B(ibr * std::cos(stop_angle),
                                  ibr * std::sin(stop_angle),
                                  -0.5 * z);
           geomtools::vector_3d C(0.0,
                                  0.0,
                                  +0.5 * z);
           t.set_vertexes(B, A, C);
           finfo.set_identity_positioning();
        } else {
          geomtools::face_info & finfo = face_.add();
          geomtools::quadrangle & q = finfo.add_face<geomtools::quadrangle>();
          geomtools::vector_3d A(ibr * std::cos(start_angle),
                                 ibr * std::sin(start_angle),
                                 -0.5 * z);
          geomtools::vector_3d B(ibr * std::cos(stop_angle),
                                 ibr * std::sin(stop_angle),
                                 -0.5 * z);
          geomtools::vector_3d C(itr * std::cos(stop_angle),
                                 itr * std::sin(stop_angle),
                                 +0.5 * z);
          geomtools::vector_3d D(itr * std::cos(start_angle),
                                 itr * std::sin(start_angle),
                                 +0.5 * z);
          q.set_vertexes(B, A, D, C);
          finfo.set_identity_positioning();
        }
      }
    }
    return;
  }

  void right_polygonal_frustrum::compute_top_face(composite_surface & face_,
                                                  placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    face_.reset();
    if (has_top_face()) {
      double z = get_z();
      double itr = 0.0;
      if (has_inner_top_radius()) {
        itr = get_inner_top_radius();
      }
      double otr = get_outer_top_radius();
      for (unsigned int isector = _sector_start_;
           isector <= _sector_stop_;
           isector++) {
        // std::cerr << "DEVEL: right_polygonal_frustrum::compute_top_face: "
        //           << "isector = " << isector
        //           << std::endl;
        double start_angle = get_start_angle(isector);
        double stop_angle = start_angle + get_sector_angle();
        if (itr == 0) {
          // std::cerr << "DEVEL: right_polygonal_frustrum::compute_top_face: "
          //           << "  add triangle."
          //           << std::endl;
          geomtools::face_info & finfo = face_.add();
          geomtools::triangle & t = finfo.add_face<geomtools::triangle>();
          geomtools::vector_3d A(otr * std::cos(start_angle),
                                 otr * std::sin(start_angle),
                                 0.0);
          geomtools::vector_3d B(otr * std::cos(stop_angle),
                                 otr * std::sin(stop_angle),
                                 0.0);
          geomtools::vector_3d C(0.0,
                                 0.0,
                                 0.0);
          t.set_vertexes(C, A, B);
          finfo.set_identity_positioning();
        } else {
          // std::cerr << "DEVEL: right_polygonal_frustrum::compute_top_face: "
          //           << "  add quadrangle."
          //           << std::endl;
          geomtools::face_info & finfo = face_.add();
          geomtools::quadrangle & q = finfo.add_face<geomtools::quadrangle>();
          geomtools::vector_3d A(otr * std::cos(start_angle),
                                 otr * std::sin(start_angle),
                                 0.0);
          geomtools::vector_3d B(otr * std::cos(stop_angle),
                                 otr * std::sin(stop_angle),
                                 0.0);
          geomtools::vector_3d C(itr * std::cos(stop_angle),
                                 itr * std::sin(stop_angle),
                                 0.0);
          geomtools::vector_3d D(itr * std::cos(start_angle),
                                 itr * std::sin(start_angle),
                                 0.0);
          q.set_vertexes(A, B, C, D);
          finfo.set_identity_positioning();
        }
      }
      face_placement_.set(0.0, 0.0, +0.5 * _z_, 0.0, 0.0, 0.0);
    }
    // std::cerr << "DEVEL: top face #surf=" << face_.get_number_of_surfaces() << std::endl;
    return;
  }

  void right_polygonal_frustrum::compute_bottom_face(composite_surface & face_,
                                                     placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    face_.reset();
    if (has_bottom_face()) {
      double z = get_z();
      double ibr = 0.0;
      if (has_inner_bottom_radius()) {
        ibr = get_inner_bottom_radius();
      }
      double obr = get_outer_bottom_radius();
      for (unsigned int isector = _sector_start_;
           isector <= _sector_stop_;
           isector++) {
        double start_angle = get_start_angle(isector);
        double stop_angle = start_angle + get_sector_angle();
        if (ibr == 0) {
          geomtools::face_info & finfo = face_.add();
          geomtools::triangle & t = finfo.add_face<geomtools::triangle>();
          geomtools::vector_3d A(obr * std::cos(start_angle),
                                 obr * std::sin(start_angle),
                                 0.0);
          geomtools::vector_3d B(obr * std::cos(stop_angle),
                                 obr * std::sin(stop_angle),
                                 0.0);
          geomtools::vector_3d C(0.0,
                                 0.0,
                                 0.0);
          t.set_vertexes(C, B, A);
          finfo.set_identity_positioning();
        } else {
          geomtools::face_info & finfo = face_.add();
          geomtools::quadrangle & q = finfo.add_face<geomtools::quadrangle>();
          geomtools::vector_3d A(obr * std::cos(start_angle),
                                 obr * std::sin(start_angle),
                                 0.0);
          geomtools::vector_3d B(obr * std::cos(stop_angle),
                                 obr * std::sin(stop_angle),
                                 0.0);
          geomtools::vector_3d C(ibr * std::cos(stop_angle),
                                 ibr * std::sin(stop_angle),
                                 0.0);
          geomtools::vector_3d D(ibr * std::cos(start_angle),
                                 ibr * std::sin(start_angle),
                                 0.0);
          q.set_vertexes(B, A, D, C);
          finfo.set_identity_positioning();
        }
      }
      face_placement_.set(0.0, 0.0, -0.5 * _z_, 0.0, 0.0, 0.0);
    }
    // std::cerr << "DEVEL: bottom face #surf=" << face_.get_number_of_surfaces() << std::endl;
    return;
  }

  /*
    right_polygonal_frustrum::ssaf_type
    right_polygonal_frustrum::get_start_stop_angle_face_type() const
    {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    ssaf_type ssaf = SSAF_INVALID;
    if (has_partial_angle()) {
    if (has_bottom_face() && ! has_top_face()) {
    ssaf = SSAF_TRIANGLE_BOTTOM_BASE;
    } else if (!has_bottom_face() && has_top_face()) {
    ssaf = SSAF_TRIANGLE_TOP_BASE;
    } else if (has_bottom_face() && has_top_face()) {
    ssaf = SSAF_QUADRANGLE;
    }
    }
    return ssaf;
    }
  */

  /*
    bool right_polygonal_frustrum::start_stop_angle_face_is_triangle_top() const
    {
    return get_start_stop_angle_face_type() == SSAF_TRIANGLE_TOP_BASE;
    }

    bool right_polygonal_frustrum::start_stop_angle_face_is_triangle_bottom() const
    {
    return get_start_stop_angle_face_type() == SSAF_TRIANGLE_BOTTOM_BASE;
    }

    bool right_polygonal_frustrum::start_stop_angle_face_is_triangle() const
    {
    return start_stop_angle_face_is_triangle_top() || start_stop_angle_face_is_triangle_bottom();
    }

    bool right_polygonal_frustrum::start_stop_angle_face_is_quadrangle() const
    {
    return get_start_stop_angle_face_type() == SSAF_QUADRANGLE;
    }

    void right_polygonal_frustrum::compute_start_stop_angle_face(quadrangle & qface_,
    triangle & tface_) const
    {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    qface_.reset();
    tface_.reset();
    if (has_partial_angle()) {
    double rob = _outer_bottom_radius_;
    double rot = _outer_top_radius_;
    double rib = 0.0;
    if (has_inner_bottom_radius()) {
    rib = _inner_bottom_radius_;
    }
    double rit = 0.0;
    if (has_inner_top_radius()) {
    rit = _inner_top_radius_;
    }
    if (start_stop_angle_face_is_quadrangle()) {
    vector_3d q0(rib, 0.0, -0.5 * _z_);
    vector_3d q1(rob, 0.0, -0.5 * _z_);
    vector_3d q2(rot, 0.0, +0.5 * _z_);
    vector_3d q3(rit, 0.0, +0.5 * _z_);
    qface_.set_vertexes(q0, q1, q2, q3);
    } else if (start_stop_angle_face_is_triangle_bottom()) {
    vector_3d q0(rib, 0.0, -0.5 * _z_);
    vector_3d q1(rob, 0.0, -0.5 * _z_);
    vector_3d q2(rot, 0.0, +0.5 * _z_);
    tface_.set_vertexes(q0, q1, q2);
    } else if (start_stop_angle_face_is_triangle_top()) {
    vector_3d q0(rit, 0.0, -0.5 * _z_);
    vector_3d q1(rot, 0.0, -0.5 * _z_);
    vector_3d q2(rob, 0.0, +0.5 * _z_);
    tface_.set_vertexes(q0, q1, q2);
    }
    }
    return;
    }

    void right_polygonal_frustrum::compute_start_angle_face(quadrangle & qface_,
    triangle & tface_,
    placement & face_placement_) const
    {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    compute_start_stop_angle_face(qface_, tface_);
    if (has_partial_angle()) {
    face_placement_.set(0.0, 0.0, 0.0, AXIS_Z, _start_angle_);
    }
    return;
    }

    void right_polygonal_frustrum::compute_stop_angle_face(quadrangle & qface_,
    triangle & tface_,
    placement & face_placement_) const
    {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    compute_start_stop_angle_face(qface_, tface_);
    if (has_partial_angle()) {
    face_placement_.set(0.0, 0.0, 0.0, AXIS_Z, _start_angle_ + _delta_angle_);
    }
    return;
    }
  */

  // virtual
  unsigned int right_polygonal_frustrum::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Polygonal frustrum is not valid !");
    unsigned int nfaces = 0;

    {
      // Outer side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_outer_face(cs);
      // std::cerr << "DEVEL: right_polygonal_frustrum::compute_faces: outer cs #faces=" << cs.get_number_of_surfaces() << std::endl;

      finfo.set_identity_positioning();
      face_identifier fid;
      fid.set_face_bit(FACE_OUTER_SIDE);
      finfo.set_identifier(fid);
      nfaces++;
    }

    if (has_inner_face()) {
      // Inner side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_inner_face(cs);
      // std::cerr << "DEVEL: right_polygonal_frustrum::compute_faces: inner cs #faces=" << cs.get_number_of_surfaces() << std::endl;
      finfo.set_identity_positioning();
      face_identifier fid;
      fid.set_face_bit(FACE_INNER_SIDE);
      finfo.set_identifier(fid);
      nfaces++;
    }

    if (has_bottom_face()) {
      // Bottom face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_bottom_face(cs, finfo.grab_positioning());
      // std::cerr << "DEVEL: right_polygonal_frustrum::compute_faces: bottom cs #faces=" << cs.get_number_of_surfaces() << std::endl;
      face_identifier fid;
      fid.set_face_bit(FACE_BOTTOM);
      finfo.set_identifier(fid);
      nfaces++;
    }

    if (has_top_face()) {
      // Top face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_top_face(cs, finfo.grab_positioning());
      // std::cerr << "DEVEL: right_polygonal_frustrum::compute_faces: top cs #faces=" << cs.get_number_of_surfaces() << std::endl;
      face_identifier fid;
      fid.set_face_bit(FACE_TOP);
      finfo.set_identifier(fid);
      nfaces++;
    }

    /*
      if (has_partial_angle()) {

      {
      // Start angle face:
      {
      face_info dummy;
      faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      if (start_stop_angle_face_is_quadrangle()) {
      finfo.set_face(new quadrangle);
      triangle dummy;
      compute_start_angle_face(finfo.grab_face<quadrangle>(),
      dummy,
      finfo.grab_positioning());
      } else if (start_stop_angle_face_is_triangle()) {
      finfo.set_face(new triangle);
      quadrangle dummy;
      compute_start_angle_face(dummy,
      finfo.grab_face<triangle>(),
      finfo.grab_positioning());
      }
      nfaces++;
      }

      {
      // Stop angle face:
      {
      face_info dummy;
      faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      if (start_stop_angle_face_is_quadrangle()) {
      finfo.set_face(new quadrangle);
      triangle dummy;
      compute_stop_angle_face(finfo.grab_face<quadrangle>(),
      dummy,
      finfo.grab_positioning());
      } else if (start_stop_angle_face_is_triangle()) {
      finfo.set_face(new triangle);
      quadrangle dummy;
      compute_stop_angle_face(dummy,
      finfo.grab_face<triangle>(),
      finfo.grab_positioning());
      }
      nfaces++;
      }
    */

    // std::cerr << "DEVEL: right_polygonal_frustrum::compute_faces:  Total #faces=" << faces_.size() << std::endl;

    return nfaces;
  }

  double right_polygonal_frustrum::get_surface(uint32_t flags_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    double s;
    datatools::invalidate(s);

    s = 0.0;

    if (flags_ & FACE_OUTER_SIDE) {
      composite_surface face;
      compute_outer_face(face);
      // face.tree_dump(std::cerr, "Outer face: ", "DEVEL: ");
      double s_out = face.get_surface();
      s += s_out;
    }

    if (has_inner_face()){
      if (flags_ & FACE_INNER_SIDE) {
        composite_surface face;
        compute_inner_face(face);
        // face.tree_dump(std::cerr, "Inner face: ", "DEVEL: ");
        double s_in = face.get_surface();
        s += s_in;
      }
    }

    if (has_bottom_face()){
      if (flags_ & FACE_BOTTOM) {
        composite_surface face;
        placement dummy;
        // face.tree_dump(std::cerr, "Bottom face: ", "DEVEL: ");
        compute_bottom_face(face, dummy);
        s += face.get_surface();
      }
    }

    if (has_top_face()){
      if (flags_ & FACE_TOP) {
        composite_surface face;
        placement dummy;
        // face.tree_dump(std::cerr, "Top face: ", "DEVEL: ");
        compute_top_face(face, dummy);
        s += face.get_surface();
      }
    }

    /*
      if (has_partial_angle()) {
      quadrangle angle_qface;
      triangle   angle_tface;
      compute_start_stop_angle_face(angle_qface, angle_tface);
      if (flags_ & FACE_START_ANGLE) {
      if (angle_qface.is_valid()) {
      s += angle_qface.get_surface();
      } else if (angle_tface.is_valid()) {
      s += angle_tface.get_surface();
      }
      }
      if (flags_ & FACE_STOP_ANGLE) {
      if (angle_qface.is_valid()) {
      s += angle_qface.get_surface();
      } else if (angle_tface.is_valid()) {
      s += angle_tface.get_surface();
      }
      }
      }
    */

    return s;
  }

  double right_polygonal_frustrum::get_volume(uint32_t flags_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    double v;
    datatools::invalidate(v);
    if (! is_valid()) {
      return v;
    }
    double ibr = 0.0;
    double itr = 0.0;
    if (has_inner_bottom_radius()) {
      ibr = _inner_bottom_radius_;
    }
    if (has_inner_top_radius()) {
      itr = _inner_top_radius_;
    }
    double vin = 0.0;
    double vout = 0.0;
    // XXX
    /*
    // XXX
    double vin = cone::compute_frustrum_volume(-0.5 * _z_,
    +0.5 * _z_,
    ibr,
    itr);
    double vout = cone::compute_frustrum_volume(-0.5 * _z_,
    +0.5 * _z_,
    _outer_bottom_radius_,
    _outer_top_radius_);
    */
    v = 0.0;
    if (flags_ & VOLUME_BULK) {
      v += (vout - vin);
    }
    if (has_inner_face()) {
      if (flags_ & VOLUME_CAVITY) {
        v += vin;
      }
    }
    v *= (1 + _sector_stop_ - _sector_stop_) / _n_sides_;
    return v;
  }

  void right_polygonal_frustrum::initialize(const datatools::properties & config_,
                                            const handle_dict_type * objects_)
  {
    reset();
    this->i_shape_3d::initialize(config_, objects_);

    double lunit = CLHEP::mm;
    if (config_.has_key("length_unit")) {
      const std::string lunit_str = config_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    double aunit = CLHEP::degree;
    if (config_.has_key("angle_unit")) {
      const std::string aunit_str = config_.fetch_string("angle_unit");
      aunit = datatools::units::get_angle_unit_from(aunit_str);
    }


    DT_THROW_IF (! config_.has_key("n_sides"), std::logic_error,
                 "Missing frustrum 'n_sides' property !");
    unsigned int n_sides = 0;
    if (config_.has_key("n_sides")) {
      int ns = config_.fetch_integer("n_sides");
      DT_THROW_IF (ns < MIN_NUMBER_OF_SIDES, std::logic_error,
                   "Invalid number of sides !");
      n_sides = (unsigned int) ns;
    }

    double inner_top_r;
    datatools::invalidate (inner_top_r);
    if (config_.has_key ("inner_top_r")) {
      inner_top_r = config_.fetch_real ("inner_top_r");
      if (! config_.has_explicit_unit ("inner_top_r")) {
        inner_top_r *= lunit;
      }
    }

    DT_THROW_IF (! config_.has_key("outer_top_r"), std::logic_error,
                 "Missing frustrum 'outer_top_r' property !");
    double outer_top_r;
    datatools::invalidate (outer_top_r);
    if (config_.has_key ("outer_top_r")) {
      outer_top_r = config_.fetch_real ("outer_top_r");
      if (! config_.has_explicit_unit ("outer_top_r")) {
        outer_top_r *= lunit;
      }
    }

    double inner_bottom_r;
    datatools::invalidate (inner_bottom_r);
    if (config_.has_key ("inner_bottom_r")) {
      inner_bottom_r = config_.fetch_real ("inner_bottom_r");
      if (! config_.has_explicit_unit ("inner_bottom_r")) {
        inner_bottom_r *= lunit;
      }
    }

    double outer_bottom_r;
    datatools::invalidate (outer_bottom_r);
    DT_THROW_IF (! config_.has_key("outer_bottom_r"), std::logic_error,
                 "Missing frustrum 'outer_bottom_r' property !");
    if (config_.has_key ("outer_bottom_r")) {
      outer_bottom_r = config_.fetch_real ("outer_bottom_r");
      if (! config_.has_explicit_unit ("outer_bottom_r")) {
        outer_bottom_r *= lunit;
      }
    }

    DT_THROW_IF (! config_.has_key("z"), std::logic_error,
                 "Missing frustrum 'z' property !");
    double z = config_.fetch_real("z");
    if (! config_.has_explicit_unit("z")) {
      z *= lunit;
    }

    // int start_sector = 0;
    // int stop_sector = 0;
    // bool not_full_angle = false;
    // if (config_.has_key ("start_sector")) {
    //   start_sector = config_.fetch_integer ("start_sector");
    // }
    // if (config_.has_key ("stop_sector")) {
    //   stop_sector = config_.fetch_real ("stop_sector");
    // }

    set_n_sides(n_sides);
    set_outer_bottom_radius(outer_bottom_r);
    set_outer_top_radius(outer_top_r);
    set_inner_bottom_radius(inner_bottom_r);
    set_inner_top_radius(inner_top_r);
    set_z(z);
    // if (not_full_angle) {
    //   set_sector_start(start_sector);
    //   set_sector_stop(stop_sector);
    // }

    lock();
    return;
  }

  double right_polygonal_frustrum::compute_outer_radius(double z_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid polygonal frustrum!");
    double r;
    datatools::invalidate(r);
    if (std::abs(z_) <= 0.5 * _z_) {
      double rob = _outer_bottom_radius_;
      double rot = _outer_top_radius_;
      r = 0.5 * (rob + rot) + z_ * (rot - rob) / _z_;
    }
    return r;
  }

  double right_polygonal_frustrum::compute_inner_radius(double z_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid polygonal frustrum!");
    double r;
    datatools::invalidate(r);
    if (std::abs(z_) <= 0.5 * _z_ && has_inner_face()) {
      double rib = _inner_bottom_radius_;
      if (!datatools::is_valid(rib)) {
        rib = 0.0;
      }
      double rit = _inner_top_radius_;
      if (!datatools::is_valid(rit)) {
        rit = 0.0;
      }
      r = 0.5 * (rib + rit) + z_ * (rit - rib) / _z_;
    }
    return r;
  }

  bool right_polygonal_frustrum::is_inside(const vector_3d & point_, double skin_) const
  {
    /*
     *         +-----------------------+
     *         | ..................... |
     *         | :                   : |
     *         | :                  >---< skin/2
     *         | :                   : |
     *         | :            ro     : |
     *         | :         +---------->|
     *         | :                   : |
     *         | :                   : |
     *         | :                   : |
     *         | :.................... |
     *         +-----------------------+
     *
     *         +-----------------------+
     *         | ..................... |
     *         | :                   : |
     *         | :                  >---< skin/2
     *         | :     .........     : |
     *         | :     : +---+ : ro  : |
     *         | :skin>---<+---------->|
     *         | :---- : +---+ :     : |
     *         | : 2   :.......:     : |
     *         | :                   : |
     *         | :.................... |
     *         +-----------------------+
     *
     */
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid polygonal frustrum!");
    double skin = compute_tolerance(skin_);
    double hskin = 0.5 * skin;
    if (std::abs(point_.z()) >= 0.5 * _z_ - hskin) {
      return false;
    }
    double roz = compute_outer_radius(point_.z());
    double apothem = roz * std::cos(M_PI / _n_sides_) - hskin;
    if (apothem > 0.0) {
      regular_polygon rpo(_n_sides_, apothem, regular_polygon::BUILD_BY_APOTHEM);
      vector_3d pos(point_.x(), point_.y(), 0.0);
      if (rpo.is_on_surface(pos, 0.0)) {
        if (has_inner_face()) {
          double riz = compute_inner_radius(point_.z());
          double apothemi = riz * std::cos(M_PI / _n_sides_) + hskin;
          regular_polygon rpi(_n_sides_, apothemi, regular_polygon::BUILD_BY_APOTHEM);
          if (! rpi.is_on_surface(pos, 0.0)) {
            return true;
          }
        } else {
          return true;
        }
      }
    }
    return false;
  }

  bool right_polygonal_frustrum::is_outside(const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid polygonal frustrum!");
    double skin = compute_tolerance(skin_);
    double hskin = 0.5 * skin;
    if (std::abs(point_.z()) >= 0.5 * _z_ + hskin) {
      return true;
    }
    // std::cerr << "DEVEL: right_polygonal_frustrum::is_outside: point_.z()=" << point_.z() << std::endl;
    double roz = compute_outer_radius(point_.z());
    // std::cerr << "DEVEL: right_polygonal_frustrum::is_outside: roz=" << roz << std::endl;
    // if (!datatools::is_valid(roz)) {
    //   std::cerr << "DEVEL: right_polygonal_frustrum::is_outside: roz is not valid" << std::endl;
    //   std::string word;
    //   std::cin >> word;
    // }
    if (datatools::is_valid(roz)) {
      double apothem = roz * std::cos(M_PI / _n_sides_) + hskin;
      // std::cerr << "DEVEL: right_polygonal_frustrum::is_outside: apothem=" << apothem << std::endl;
      regular_polygon rpo(_n_sides_, apothem, regular_polygon::BUILD_BY_APOTHEM);
      // rpo.tree_dump(std::cerr, "RPO: ", "DEVEL: ");
      vector_3d pos(point_.x(), point_.y(), 0.0);
      //std::cerr << "DEVEL: right_polygonal_frustrum::is_outside: TEST" << std::endl;
      if (! rpo.is_on_surface(pos, 0.0)) {
        //std::cerr << "DEVEL: right_polygonal_frustrum::is_outside:    ! rpo.is_on_surface" << std::endl;
        return true;
      }
      if (has_inner_face()) {
        //std::cerr << "DEVEL: right_polygonal_frustrum::is_outside: has_inner_face" << std::endl;
        double riz = compute_inner_radius(point_.z());
        double apothemi = riz * std::cos(M_PI / _n_sides_) - hskin;
        if (apothemi > 0.0) {
          regular_polygon rpi(_n_sides_, apothemi, regular_polygon::BUILD_BY_APOTHEM);
          if (rpi.is_on_surface(pos, 0.0)) {
            return true;
          }
        }
      }
    }
    return false;
  }

  face_identifier right_polygonal_frustrum::on_surface(const vector_3d & position_,
                                                       const face_identifier & surface_mask_,
                                                       double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    double skin = get_skin(skin_);

    face_identifier mask;
    if (surface_mask_.is_valid()) {
      DT_THROW_IF(! surface_mask_.is_face_bits_mode(), std::logic_error,
                  "Face mask uses no face bits!");
      mask = surface_mask_;
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
        finfo.get_positioning().mother_to_child(position_, position_c);
        if (finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
      }
    }

    return face_identifier::face_invalid();
  }


  vector_3d
  right_polygonal_frustrum::get_normal_on_surface (const vector_3d & position_,
                                                   const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    vector_3d normal;
    geomtools::invalidate(normal);
    switch(a_surface_bit.get_face_bits()) {
    case FACE_OUTER_SIDE:
      {
        composite_surface face;
        compute_outer_face(face);
        normal = face.get_normal_on_surface(position_, false);
      }
      break;
    case FACE_INNER_SIDE:
      if (has_inner_face()) {
        composite_surface face;
        compute_inner_face(face);
        normal = -face.get_normal_on_surface(position_, false);
      }
      break;
    case FACE_BOTTOM:
      if (has_bottom_face()) {
        composite_surface face;
        placement face_pl;
        compute_bottom_face(face, face_pl);
        vector_3d position_c;
        face_pl.mother_to_child(position_c);
        vector_3d normal_c = -face.get_normal_on_surface(position_c, false);
        normal = face_pl.child_to_mother_direction(normal_c);
      }
      break;
    case FACE_TOP:
      if (has_top_face()) {
        composite_surface face;
        placement face_pl;
        compute_top_face(face, face_pl);
        vector_3d position_c;
        face_pl.mother_to_child(position_c);
        vector_3d normal_c = face.get_normal_on_surface(position_c, false);
        normal = face_pl.child_to_mother_direction(normal_c);
      }
      break;
    }
    return normal;
  }

  bool right_polygonal_frustrum::find_intercept (const vector_3d & from_,
                                                 const vector_3d & direction_,
                                                 face_intercept_info & intercept_,
                                                 double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    intercept_.reset ();

    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 4;
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

    return intercept_.is_ok();
  }


  void right_polygonal_frustrum::generate_wires_self(wires_type & wires_,
                                                     uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    bool debug_explode    = options_ & WR_BASE_EXPLODE;
    bool draw_bottom      = !(options_ & WR_RPF_NO_BOTTOM_FACE);
    bool draw_top         = !(options_ & WR_RPF_NO_TOP_FACE);
    bool draw_inner       = !(options_ & WR_RPF_NO_INNER_FACE);
    bool draw_outer       = !(options_ & WR_RPF_NO_OUTER_FACE);
    // bool draw_start_angle = !(options_ & WR_RPF_NO_START_ANGLE_FACE);
    // bool draw_stop_angle  = !(options_ & WR_RPF_NO_STOP_ANGLE_FACE);
    bool draw_boundings   =  (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings && has_bounding_data()) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    double explode_factor = 1.0;
    if (debug_explode) {
      explode_factor = 1.25;
    }

    bool edge_top = false;
    bool edge_bottom = false;

    if (draw_bottom && has_bottom_face()) {
      composite_surface bottom_face;
      placement bottom_pl;
      compute_bottom_face(bottom_face, bottom_pl);
      uint32_t options = base_options;
      bottom_face.generate_wires(wires_, bottom_pl, options);
      edge_bottom = true;
    }

    if (draw_top && has_top_face()) {
      composite_surface top_face;
      placement top_pl;
      compute_top_face(top_face, top_pl);
      // std::cerr << "DEVEL: right_polygonal_frustrum::generate_wires_self: "
      //           << "Top face = " << top_face.get_number_of_surfaces() << std::endl;
      uint32_t options = base_options;
      top_face.generate_wires(wires_, top_pl, options);
      edge_top = true;
    }

    if (has_inner_face()) {
      if (draw_inner) {
        uint32_t options = base_options;
        composite_surface in_rccn_face;
        compute_inner_face(in_rccn_face);
        in_rccn_face.generate_wires_self(wires_, options);
      }
    }

    if (draw_outer) {
      uint32_t options = base_options;
      composite_surface out_rccn_face;
      compute_outer_face(out_rccn_face);
      out_rccn_face.generate_wires_self(wires_, options);
    }

    /*
      if (has_partial_angle()) {
      uint32_t options = base_options;
      if (draw_start_angle) {
      quadrangle angle_qface;
      triangle angle_tface;
      placement p;
      compute_start_angle_face(angle_qface, angle_tface, p);
      if (angle_qface.is_valid()) angle_qface.generate_wires(wires_, p, options);
      else if (angle_tface.is_valid()) angle_tface.generate_wires(wires_, p, options);
      }
      if (draw_stop_angle) {
      quadrangle angle_qface;
      triangle angle_tface;
      placement p;
      compute_stop_angle_face(angle_qface, angle_tface, p);
      if (angle_qface.is_valid()) angle_qface.generate_wires(wires_, p, options);
      else if (angle_tface.is_valid()) angle_tface.generate_wires(wires_, p, options);
      }
      }
    */

    return;
  }

  void right_polygonal_frustrum::_build_bounding_data()
  {
    _grab_bounding_data().make_cylinder(get_xmax(),
                                        -0.5 * get_z(),
                                        +0.5 * get_z());
    return;
  }

} // end of namespace geomtools
