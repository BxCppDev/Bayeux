/// \file geomtools/polycone.cc

// Ourselves:
#include <geomtools/polycone.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/tabulated_function.h>
#include <mygsl/numerical_differentiation.h>
#include <mygsl/interval.h>
#include <mygsl/min_max.h>

// This project:
#include <geomtools/disk.h>
#include <geomtools/cone.h>
#include <geomtools/box.h>
#include <geomtools/triangle.h>
#include <geomtools/quadrangle.h>
#include <geomtools/composite_surface.h>
#include <geomtools/right_circular_conical_nappe.h>
#include <geomtools/right_circular_conical_frustrum.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(polycone, "geomtools::polycone")

  const std::string & polycone::polycone_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "polycone";
    }
    return label;
  }

  std::string polycone::get_shape_name () const
  {
    return polycone_label();
  }

  void polycone::_build_bounding_data()
  {
    if (! has_partial_angle()) {
      _grab_bounding_data().make_cylinder(_r_max_, get_zmin(), get_zmax());
    } else {
      mygsl::min_max xrange;
      mygsl::min_max yrange;
      mygsl::min_max zrange;
      for (size_t ifrustrum = 0; ifrustrum < number_of_frustra(); ifrustrum++) {
        right_circular_conical_frustrum fi;
        placement fi_placement;
        compute_frustrum(fi, fi_placement, ifrustrum);
        const bounding_data & bbi = fi.get_bounding_data();
        box bb;
        placement bb_pl;
        bbi.compute_bounding_box(bb, bb_pl);
        std::vector<vector_3d> corners;
        bb.compute_vertexes(corners);
        for (int i = 0; i < (int) corners.size(); i++) {
          vector_3d vtx;
          bb_pl.child_to_mother(corners[i], vtx);
          vector_3d vtx2;
          fi_placement.child_to_mother(vtx, vtx2);
          xrange.add(vtx2.x());
          yrange.add(vtx2.y());
          zrange.add(vtx2.z());
        }
      }
      _grab_bounding_data().make_box(xrange.get_min(), xrange.get_max(),
                                     yrange.get_min(), yrange.get_max(),
                                     zrange.get_min(), zrange.get_max());
    }
    return;
  }

  void polycone::_set_defaults()
  {
    _computed_ = false;
    _has_top_face_    = boost::logic::indeterminate;
    _has_bottom_face_ = boost::logic::indeterminate;
    _has_inner_face_  = boost::logic::indeterminate;
    datatools::invalidate(_top_surface_);
    datatools::invalidate(_bottom_surface_);
    datatools::invalidate(_outer_side_surface_);
    datatools::invalidate(_inner_side_surface_);
    datatools::invalidate(_start_angle_surface_);
    datatools::invalidate(_stop_angle_surface_);
    datatools::invalidate(_outer_volume_);
    datatools::invalidate(_inner_volume_);
    datatools::invalidate(_volume_);
    datatools::invalidate(_z_min_);
    datatools::invalidate(_z_max_);
    datatools::invalidate(_r_max_);
    _extruded_ = false;
    datatools::invalidate(_start_angle_);
    datatools::invalidate(_delta_angle_);
    return;
  }

  void polycone::compute_frustrum(right_circular_conical_frustrum & f_,
                                  placement & positioning_,
                                  int index_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Polycone is not valid !");
    f_.reset();
    positioning_.reset();
    DT_THROW_IF(index_ < 0 || index_ >= (int)number_of_frustra(),
                std::range_error,
                "Invalid frustrum index [" << index_ << "]!");
    frustrum_data fd;
    get_frustrum_data(index_, fd);
    double z = fd.z2 - fd.z1;
    double zpos = 0.5 * (fd.z2 + fd.z1);
    f_.set_z(z);
    f_.set_outer_bottom_radius(fd.b1);
    f_.set_outer_top_radius(fd.b2);
    if (datatools::is_valid(fd.a1) && fd.a1 >= 0.0) {
      f_.set_inner_bottom_radius(fd.a1);
    }
    if (datatools::is_valid(fd.a2) && fd.a2 >= 0.0) {
      f_.set_inner_top_radius(fd.a2);
    }
    positioning_.set_identity();
    positioning_.set_translation(0.0, 0.0, zpos);
    if (has_partial_angle()) {
      f_.set_start_angle(get_start_angle());
      f_.set_delta_angle(get_delta_angle());
    }
    return;
  }

  bool polycone::has_top_face() const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    if (_has_top_face_) {
      return true;
    } else if (!_has_top_face_) {
      return false;
    } else {
      DT_THROW(std::logic_error, "Polycone is not properly initialized !");
    }
  }

  bool polycone::has_bottom_face() const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    if (_has_bottom_face_) {
      return true;
    } else if (!_has_bottom_face_) {
      return false;
    } else {
      DT_THROW(std::logic_error, "Polycone is not properly initialized !");
    }
  }

  bool polycone::has_inner_face() const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    if (_has_inner_face_) {
      return true;
    } else if (!_has_inner_face_) {
      return false;
    } else {
      DT_THROW(std::logic_error, "Polycone is not properly initialized !");
    }
  }

  bool polycone::has_angle_faces() const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    return has_partial_angle();
  }

  void polycone::compute_top_face(disk & disk_, placement & positioning_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    disk_.reset();
    positioning_.reset();
    if (has_top_face()) {
      // std::cerr << "DEVEL: polycone::compute_top_face: " << std::endl;
      right_circular_conical_frustrum cf;
      placement cf_placement;
      compute_frustrum(cf, cf_placement, number_of_frustra() - 1);
      if (cf.is_valid()) {
        placement disk_placement;
        cf.compute_top_face(disk_, disk_placement);
        double z = cf_placement.get_translation().z();
        z += disk_placement.get_translation().z();
        cf_placement.grab_translation().setZ(z);
        positioning_ = cf_placement;
      }
    }
    return;
  }

  void polycone::compute_bottom_face(disk & disk_, placement & positioning_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    disk_.reset();
    positioning_.reset();
    if (has_bottom_face()) {
      // std::cerr << "DEVEL: polycone::compute_bottom_face: " << std::endl;
      right_circular_conical_frustrum cf;
      placement cf_placement;
      compute_frustrum(cf, cf_placement, 0);
      if (cf.is_valid()) {
        placement disk_placement;
        cf.compute_bottom_face(disk_, disk_placement);
        double z = cf_placement.get_translation().z();
        z += disk_placement.get_translation().z();
        cf_placement.grab_translation().setZ(z);
        positioning_ = cf_placement;
      }
    }
    return;
  }

  void polycone::compute_inner_face(right_circular_conical_nappe & face_,
                                    placement & positioning_,
                                    int index_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    face_.reset();
    positioning_.reset();
    if (has_inner_face()) {
      right_circular_conical_frustrum rccf;
      placement rccf_placement;
      compute_frustrum(rccf, rccf_placement, index_);
      if (rccf.has_inner_face()) {
        rccf.compute_inner_face(face_);
        positioning_ = rccf_placement;
      }
    }
    return;
  }

  right_circular_conical_frustrum::ssaf_type
  polycone::get_start_stop_angle_face_type(int index_) const
  {
    right_circular_conical_frustrum::ssaf_type ssaf = right_circular_conical_frustrum::SSAF_INVALID;
    if (has_partial_angle()) {
      right_circular_conical_frustrum rccf;
      placement rccf_placement;
      compute_frustrum(rccf, rccf_placement, index_);
      ssaf = rccf.get_start_stop_angle_face_type();
    }
    return ssaf;
  }

  void polycone::compute_start_angle_face(quadrangle & qface_,
                                          triangle & tface_,
                                          placement & positioning_,
                                          int index_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    qface_.reset();
    tface_.reset();
    positioning_.reset();
    if (has_partial_angle()) {
      right_circular_conical_frustrum rccf;
      placement rccf_placement;
      compute_frustrum(rccf, rccf_placement, index_);
      if (rccf.is_valid()) {
        placement start_angle_placement;
        rccf.compute_start_angle_face(qface_, tface_, start_angle_placement);
        double z = start_angle_placement.get_translation().z();
        z += rccf_placement.get_translation().z();
        start_angle_placement.grab_translation().setZ(z);
        positioning_ = start_angle_placement;
      }
    }
    return;
  }

  void polycone::compute_stop_angle_face(quadrangle & qface_,
                                         triangle & tface_,
                                         placement & positioning_,
                                         int index_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    qface_.reset();
    tface_.reset();
    positioning_.reset();
    if (has_partial_angle()) {
      right_circular_conical_frustrum rccf;
      placement rccf_placement;
      compute_frustrum(rccf, rccf_placement, index_);
      if (rccf.is_valid()) {
        placement stop_angle_placement;
        rccf.compute_stop_angle_face(qface_, tface_, stop_angle_placement);
        double z = stop_angle_placement.get_translation().z();
        z += rccf_placement.get_translation().z();
        stop_angle_placement.grab_translation().setZ(z);
        positioning_ = stop_angle_placement;
      }
    }
    return;
  }

  void polycone::compute_outer_face(right_circular_conical_nappe & face_,
                                    placement & positioning_,
                                    int index_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    face_.reset();
    positioning_.reset();
    right_circular_conical_frustrum rccf;
    placement rccf_placement;
    compute_frustrum(rccf, rccf_placement, index_);
    rccf.compute_outer_face(face_);
    positioning_ = rccf_placement;
    return;
  }

  void polycone::compute_outer_face(composite_surface & face_,
                                    placement & positioning_) const
  {
    face_.reset();
    positioning_.reset();
    // Outer side face:
    for (size_t i = 0; i < number_of_frustra(); i++) {
      geomtools::face_info & finfo = face_.add();
      right_circular_conical_nappe & rccn = finfo.add_face<right_circular_conical_nappe>();
      compute_outer_face(rccn, finfo.grab_positioning(), i);
      if (! rccn.is_valid()) {
        face_.remove_last();
      }
    }
    return;
  }

  void polycone::compute_inner_face(composite_surface & face_,
                                    placement & positioning_) const
  {
    face_.reset();
    positioning_.reset();
    if (has_inner_face()) {
      // Inner side face:
      for (size_t i = 0; i < number_of_frustra(); i++) {
        geomtools::face_info & finfo = face_.add();
        right_circular_conical_nappe & rccn = finfo.add_face<right_circular_conical_nappe>();
        compute_inner_face(rccn, finfo.grab_positioning(), i);
        if (! rccn.is_valid()) {
          face_.remove_last();
        }
      }
    }
    return;
  }

  void polycone::compute_start_angle_face(composite_surface & face_,
                                          placement & positioning_) const
  {
    face_.reset();
    positioning_.reset();
    if (has_partial_angle()) {
      // Inner side face:
      for (size_t i = 0; i < number_of_frustra(); i++) {
        geomtools::face_info & finfo = face_.add();
        if (get_start_stop_angle_face_type(i) == right_circular_conical_frustrum::SSAF_QUADRANGLE) {
          quadrangle & q = finfo.add_face<quadrangle>();
          triangle dummy;
          compute_start_angle_face(q, dummy, finfo.grab_positioning(), i);
          if (! q.is_valid()) {
            face_.remove_last();
          }
        } else if (get_start_stop_angle_face_type(i) != right_circular_conical_frustrum::SSAF_INVALID) {
          quadrangle dummy;
          triangle & t = finfo.add_face<triangle>();
          compute_start_angle_face(dummy, t, finfo.grab_positioning(), i);
          if (! t.is_valid()) {
            face_.remove_last();
          }
        }
      }
    }
    return;
  }

  void polycone::compute_stop_angle_face(composite_surface & face_,
                                         placement & positioning_) const
  {
    face_.reset();
    positioning_.reset();
    if (has_partial_angle()) {
      // Inner side face:
      for (size_t i = 0; i < number_of_frustra(); i++) {
        geomtools::face_info & finfo = face_.add();
        if (get_start_stop_angle_face_type(i) == right_circular_conical_frustrum::SSAF_QUADRANGLE) {
          quadrangle & q = finfo.add_face<quadrangle>();
          triangle dummy;
          compute_stop_angle_face(q, dummy, finfo.grab_positioning(), i);
          if (! q.is_valid()) {
            face_.remove_last();
          }
        } else if (get_start_stop_angle_face_type(i) != right_circular_conical_frustrum::SSAF_INVALID) {
          quadrangle dummy;
          triangle & t = finfo.add_face<triangle>();
          compute_stop_angle_face(dummy, t, finfo.grab_positioning(), i);
          if (! t.is_valid()) {
            face_.remove_last();
          }
        }
      }
    }
    return;
  }

  // virtual
  unsigned int polycone::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Polycone is not valid !");
    unsigned int nfaces = 0;

    if (has_bottom_face()) {
      // Bottom face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      disk & d = finfo.add_face<disk>();
      compute_bottom_face(d, finfo.grab_positioning());
      if (d.is_valid()) {
        face_identifier fid;
        fid.set_face_bit(FACE_BOTTOM);
        finfo.set_identifier(fid);
        finfo.set_label("bottom");
        nfaces++;
      } else {
        faces_.pop_back();
      }
    }

    if (has_top_face()) {
      // Top face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      disk & d = finfo.add_face<disk>();
      if (d.is_valid()) {
        compute_top_face(d, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_TOP);
        finfo.set_identifier(fid);
        finfo.set_label("top");
        nfaces++;
      } else {
        faces_.pop_back();
      }
    }

    {
      // Outer side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_outer_face(cs, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_OUTER_SIDE);
      finfo.set_identifier(fid);
      finfo.set_label("outer_side");
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
      compute_inner_face(cs, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_INNER_SIDE);
      finfo.set_identifier(fid);
      finfo.set_label("inner_side");
      nfaces++;
    }

    if (has_partial_angle()) {

      {
        // Start angle face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        composite_surface & cs = finfo.add_face<composite_surface>();
        compute_start_angle_face(cs, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_START_ANGLE);
        finfo.set_identifier(fid);
        finfo.set_label("start_angle");
        nfaces++;
      }

      {
        // Stop angle face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        composite_surface & cs = finfo.add_face<composite_surface>();
        compute_stop_angle_face(cs, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_STOP_ANGLE);
        finfo.set_identifier(fid);
        finfo.set_label("stop_angle");
        nfaces++;
      }

    }

    return nfaces;
  }

  double polycone::get_xmin () const
  {
    return -_r_max_;
  }

  double polycone::get_xmax () const
  {
    return +_r_max_;
  }

  double polycone::get_ymin () const
  {
    return -_r_max_;
  }

  double polycone::get_ymax () const
  {
    return +_r_max_;
  }

  double polycone::get_zmin () const
  {
    return _z_min_;
  }

  double polycone::get_zmax () const
  {
    return _z_max_;
  }

  double polycone::get_z () const
  {
    return _z_max_ - _z_min_;
  }

  double polycone::get_z_min () const
  {
    return _z_min_;
  }

  double polycone::get_z_max () const
  {
    return _z_max_;
  }

  double polycone::get_r_max () const
  {
    return _r_max_;
  }

  const polycone::rz_col_type & polycone::points () const
  {
    return _points_;
  }

  polycone::polycone ()
  {
    _set_defaults ();
    return;
  }

  polycone::~polycone ()
  {
    return;
  }

  void polycone::compute_inner_polycone (polycone & inner_)
  {
    inner_.reset ();
    for (polycone::rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      bool add_it = true;
      if (add_it) {
        inner_.add (z, rmin, false);
      }
    }
    inner_._start_angle_ = this->_start_angle_;
    inner_._delta_angle_ = this->_delta_angle_;
    inner_._compute_all_ ();
    inner_.lock();
    return;
  }

  void polycone::compute_outer_polycone (polycone & outer_)
  {
    outer_.reset ();
    for (polycone::rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      double z = i->first;
      double rmax = i->second.rmax;
      outer_.add (z, 0.0, rmax, false);
    }
    outer_._start_angle_ = this->_start_angle_;
    outer_._delta_angle_ = this->_delta_angle_;
    outer_._compute_all_();
    outer_.lock();
    return;
  }

  void polycone::compute_inflated(polycone & inflated_,
                                  double by_r_,
                                  double by_z_,
                                  double by_angle_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    bool apply_inflated_z = false;
    if (z_eps > 0.0) {
      apply_inflated_z = true;
    }
    inflated_.reset();
    size_t sz = _points_.size();
    std::size_t counter = 0;
    double maxR = -1.0;
    for (polycone::rz_col_type::const_iterator i = _points_.begin();
         i != _points_.end();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      double rmax = i->second.rmax;
      rmax += r_eps;
      if (datatools::is_valid(rmin) and rmin >= 0.0) {
        rmin -= r_eps;
      } else {
        rmin = 0.0;
      }
      if (maxR < 0.0 or rmax > maxR) {
        maxR = rmax;
      }
      if (rmax < 0.0) rmax = 0.0;
      if (rmin < 0.0) rmin = 0.0;
      if (apply_inflated_z) {
        if (counter == 0) {
          if (rmin < rmax) {
            inflated_.add(z - z_eps, rmin, rmax, false);
          } else {
            inflated_.add(z - z_eps, i->second.rmin, i->second.rmax, false);
          }
        }
      }
      inflated_.add(z, rmin, rmax, false);
      if (apply_inflated_z) {
        if (counter + 1 == sz) {
          if (rmin < rmax) {
            inflated_.add(z + z_eps, rmin, rmax, false);
          }else {
            inflated_.add(z + z_eps, i->second.rmin, i->second.rmax, false);
          }
        }
      }
      counter++;
    }
    double eps_angle = by_angle_;
    if (eps_angle < 0.0) {
      eps_angle = r_eps / maxR;
    }
    if (has_partial_angle()) {
      double start_angle = get_start_angle();
      double delta_angle = get_delta_angle();
      if (eps_angle > 0.0) {
        start_angle += eps_angle;
        delta_angle -= 2 * eps_angle;
        if (delta_angle < 0.0)  {   
          start_angle = get_start_angle() + 0.5 * get_delta_angle() - 0.25 * eps_angle;
          delta_angle = 0.5 * eps_angle;
        }
      }
      inflated_.set_start_angle(start_angle);
      inflated_.set_delta_angle(delta_angle);
    }
    inflated_._compute_all_();
    inflated_.lock();
    return;
  }

  void polycone::compute_deflated(polycone & deflated_,
                                  double by_r_,
                                  double by_z_,
                                  double by_angle_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    bool apply_tolerance_z = false;
    if (z_eps > 0.0) {
      apply_tolerance_z = true;
    }
    deflated_.reset();
    size_t sz = _points_.size();
    std::size_t counter = 0;
    bool skip_next = true;
    double maxR = -1.0;
    for (polycone::rz_col_type::const_iterator i = _points_.begin();
         i != _points_.end();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      double rmax = i->second.rmax;
      rmax -= r_eps;
      if (datatools::is_valid(rmin) and rmin > 0.0) {
        rmin += r_eps;
      } else {
        rmin = 0.0;
      }
      if (maxR < 0.0 or rmax > maxR) {
        maxR = rmax;
      }
      if (rmax < 0.0) rmax = 0.0;
      if (rmin < 0.0) rmin = 0.0;
      if (rmax < rmin) {
        double rmed = 0.5 * (rmax + rmin);
        rmax = rmed;
        rmin = rmed;
      }
      double z0 = z;
      if (apply_tolerance_z) {
        if (skip_next) {
          z0 += z_eps;
          auto j = i;
          j++;
          double z1 = j->first;
          if (z0 <= z1) skip_next = false;
        }
      }
      deflated_.add(z0, rmin, rmax, false);
      counter++;
      if (counter == sz / 2) {
        break;
      }
    }
    counter = 0;
    skip_next = true;
    for (polycone::rz_col_type::const_reverse_iterator i = _points_.rbegin ();
         i != _points_.rend ();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      double rmax = i->second.rmax;
      rmax -= r_eps;
      if (datatools::is_valid(rmin)) {
        rmin -= r_eps;
      } else {
        rmin = 0.0;
      }
      if (maxR < 0.0 or rmax > maxR) {
        maxR = rmax;
      }
      if (rmax < 0.0) rmax = 0.0;
      if (rmin < 0.0) rmin = 0.0;
      double z0 = z;
      if (apply_tolerance_z) {
        if (skip_next) {
          z0 -= z_eps;
          auto j = i;
          j++;
          double z1 = j->first;
          if (z0 <= z1) skip_next = false;
        }
      }
      deflated_.add(z0, rmin, rmax, false);
      counter++;
      if (counter > sz / 2) {
        break;
      }
    }
    double eps_angle = by_angle_;
    if (eps_angle < 0.0) {
      eps_angle = r_eps / maxR;
    }
    if (has_partial_angle()) {
      double start_angle = get_start_angle();
      double delta_angle = get_delta_angle();
      if (eps_angle > 0.0) {
        start_angle += eps_angle;
        delta_angle -= 2 * eps_angle;
        if (delta_angle < 0.0)  {   
          start_angle = get_start_angle() + 0.5 * get_delta_angle() - 0.25 * eps_angle;
          delta_angle = 0.5 * eps_angle;
        }
      }
      deflated_.set_start_angle(start_angle);
      deflated_.set_delta_angle(delta_angle);
    }
    deflated_._compute_all_();
    deflated_.lock();
    return;
  }

  void polycone::compute_envelope(polycone & envelope_,
                                  double r_tolerance_,
                                  double z_tolerance_,
                                  double angle_tolerance_)
  {
    compute_inflated(envelope_, r_tolerance_, z_tolerance_, angle_tolerance_);
    return;
  }

  unsigned int polycone::number_of_frustra() const
  {
    return _points_.size() - 1;
  }

  void polycone::get_frustrum_data(size_t index_, frustrum_data & fd_) const
  {
    DT_THROW_IF (index_ >= number_of_frustra(),
                 std::range_error,
                 "Invalid frustrum index [" << index_ << "] !");
    size_t count = 0;
    for (rz_col_type::const_iterator it = _points_.begin ();
         it != _points_.end ();
         it++) {
      rz_col_type::const_iterator jt = it;
      jt++;
      if (count == index_) {
        fd_.z1 = it->first;
        fd_.a1 = it->second.rmin;
        fd_.b1 = it->second.rmax;
        fd_.z2 = jt->first;
        fd_.a2 = jt->second.rmin;
        fd_.b2 = jt->second.rmax;
        break;
      }
      count++;
      if (count == _points_.size()) break;
    }

    return;
  }

  void polycone::_build_from_envelope_and_skin_ (double skin_thickness_,
                                                 double skin_step_)
  {
    _build_from_envelope_and_skin_ (skin_thickness_,
                                    skin_step_,
                                    std::numeric_limits<double>::quiet_NaN (),
                                    std::numeric_limits<double>::quiet_NaN ());
    return;
  }

  void polycone::_build_from_envelope_and_skin_ (double skin_thickness_,
                                                 double skin_step_,
                                                 double zmin_,
                                                 double zmax_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_TRACE;
    DT_LOG_TRACE (local_priority, "Entering...");

    DT_THROW_IF (skin_thickness_ < 0.0, std::logic_error, "Invalid skin thickness !");
    DT_THROW_IF (_points_.size () < 2, std::logic_error, "Not enough points !");

    DT_LOG_TRACE (local_priority, "Building interpolated primary outer envelope...");

    const std::string interpolation_mode = "linear";
    //interpolation_mode = "akima";
    mygsl::tabulated_function tf (interpolation_mode);
    for (rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      double z = i->first;
      double rmax = i->second.rmax;
      tf.add_point (z, rmax);
    }
    tf.lock_table ();

    // thickness of the polycone's skin:
    const double skin = skin_thickness_;

    // compute the recommended sampling step along the envelope:
    const double dx0 = 5 * skin; // default
    double dx = dx0;
    if (skin_step_ > 0.0) {
      dx = skin_step_;
    }
    DT_LOG_TRACE (local_priority, "Building interpolated inner envelope...");
    //mygsl::tabulated_function tf2 (interpolation_mode);
    mygsl::tabulated_function tf3 (interpolation_mode);
    for (mygsl::tabulated_function::points_map_type::const_iterator
           i = tf.points ().begin ();
         i != tf.points ().end ();
         i++) {
      mygsl::tabulated_function::points_map_type::const_iterator j = i;
      const double xi = i->first;
      const double yi = i->second;
      j++;
      mygsl::interval domain (tf.x_min (), tf.x_max (), 0.01 * skin);
      const double drdz = mygsl::derivative (tf, xi, domain);
      const double uy = drdz;
      const double dyij = skin / sqrt (1 + uy * uy);
      const double dxij = - dyij * uy;
      const double xk = xi - dxij;
      const double yk = yi - dyij;
      //tf2.add_point (xj, yj, false);
      tf3.add_point (xk, yk, false);
    }
    tf3.lock_table ();

    // Manage bounds:
    DT_LOG_TRACE (local_priority, "Building properly bounded interpolated inner envelope...");
    mygsl::tabulated_function tf3bis (interpolation_mode);
    for (mygsl::tabulated_function::points_map_type::const_iterator
           i = tf.points ().begin ();
         i != tf.points ().end ();
         i++) {
      const double x = i->first;
      double y;
      if (tf3.is_valid (x)) {
        y = tf3 (x);
      } else {
        if (x < tf3.x_min ()) {
          const double df3dx = mygsl::derivative_forward (tf3, tf3.x_min (), 0.05 * dx);
          y = tf3 (tf3.x_min ()) + (x - tf3.x_min ()) * df3dx;
        } else { //if (x > tf3.x_max ())

          const double df3dx = mygsl::derivative_backward (tf3, tf3.x_max (), 0.05 * dx);
          y = tf3 (tf3.x_max ()) + (x - tf3.x_max ()) * df3dx;
        }
      }
      tf3bis.add_point (x, y, false);
    }
    tf3bis.lock_table ();

    DT_LOG_TRACE (local_priority, "Building final interpolated outer and inner envelopes with requested Z sampling...");

    mygsl::tabulated_function tf_outer ("linear");
    mygsl::tabulated_function tf_inner ("linear");
    double z1 = tf.x_min ();
    double z2 = tf.x_max ();
    DT_LOG_TRACE (local_priority, "z1=" << z1);
    DT_LOG_TRACE (local_priority, "z2=" << z2);
    const double zmin = zmin_;
    if (datatools::is_valid (zmin)) {
      DT_LOG_TRACE (local_priority, "Z(min)=" << zmin);
      if (zmin > tf.x_min ()) {
        z1 = zmin;
      }
    }
    const double zmax = zmax_;
    if (datatools::is_valid (zmax)) {
      DT_LOG_TRACE (local_priority, "Z(max)=" << zmax);
      if (zmax < tf.x_max ()) {
        z2 = zmax;
      }
    }
    double dz = dx;
    double za, ra;
    datatools::invalidate (za);
    datatools::invalidate (ra);
    double z = z1;
    bool stop = false;
    do {
      if (z >= z2) {
        DT_LOG_TRACE (local_priority, "z2 stop");
        z = z2;
        stop = true;
      }
      //double safe = 0.05 * skin;
      const double safe = 0.1 * dz;
      mygsl::interval domain (tf.x_min (), tf.x_max (), safe);
      const double drdz = mygsl::derivative (tf, z, domain);
      dz = dx / sqrt (1.0 + drdz * drdz);
      if (dz > dx) {
        dz = dx;
      }
      double r_outer = tf (z);
      if (r_outer < 0.0) { // should not occur

        r_outer = 0.0;
      }
      double r_inner = tf3bis (z);
      if (datatools::is_valid (za) && datatools::is_valid (ra)) {
        const double zb = z;
        const double rb = r_inner;
        if ((ra * rb) < 0.0) {
          const double a = (rb - ra) / (zb - za);
          const double b = rb - a * zb;
          const double z_r = -b / a;
          const double rmin_r = 0.0;
          const double rmax_r = tf (z_r);
          tf_outer.add_point (z_r, rmax_r, false);
          tf_inner.add_point (z_r, rmin_r, false);
        }
      }
      za = z;
      ra = r_inner;
      if (r_inner < 0.0) {
        r_inner = 0.0;
      }
      tf_outer.add_point (z, r_outer, false);
      tf_inner.add_point (z, r_inner, false);
      z += dz;
      if (stop) break;
    } while (! stop);
    // lock interpolators:
    tf_outer.lock_table ();
    tf_inner.lock_table ();
    DT_LOG_TRACE (local_priority, "Locked !");
    _points_.clear ();

    mygsl::tabulated_function::points_map_type::const_iterator i = tf_outer.points ().begin ();
    mygsl::tabulated_function::points_map_type::const_iterator j = tf_inner.points ().begin ();
    for (size_t k = 0; k < tf_outer.points ().size (); k++) {
      double local_z = i->first;
      double rmin = j->second;
      double rmax = i->second;
      if (rmin > rmax) rmin = rmax;
      if (rmin < 0.0) { // should not occur
        rmin = 0.0;
      }
      if (rmin < 1e-300) {
        rmin = 0.0;
      }
      this->add (local_z, rmin, rmax, false);
      i++;
      j++;
    }

    _compute_all_ ();
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  void polycone::initialize (const datatools::properties & setup_,
                             const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(setup_, objects_);
    if (!is_valid()) {

    double lunit = CLHEP::mm;
    double aunit = CLHEP::degree;
    if (setup_.has_key ("length_unit")) {
      const std::string lunit_str = setup_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }
    if (setup_.has_key ("angle_unit")) {
      const std::string aunit_str = setup_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    std::string build_mode_label;
    if (setup_.has_key ("build_mode")) {
      build_mode_label = setup_.fetch_string ("build_mode");
    }

    if (build_mode_label == "points") {
      DT_THROW_IF (! setup_.has_key ("list_of_z"), std::logic_error, "Missing 'list_of_z' property !");
      std::vector<double> zs;
      setup_.fetch ("list_of_z", zs);
      DT_THROW_IF (zs.size () < 2, std::logic_error, "'list_of_z' has not enough points !");

      DT_THROW_IF (! setup_.has_key ("list_of_rmax"), std::logic_error, "Missing 'list_of_rmax' property !");
      std::vector<double> rmaxs;
      setup_.fetch ("list_of_rmax", rmaxs);
      DT_THROW_IF (rmaxs.size () != zs.size (), std::logic_error,
                   "'list_of_z' and 'list_of_rmax' have not the same size !");

      std::vector<double> rmins;
      double rmin;
      datatools::invalidate (rmin);
      if (setup_.has_key ("list_of_rmin")) {
        setup_.fetch ("list_of_rmin", rmins);
        DT_THROW_IF (rmins.size () != zs.size (), std::logic_error,
                     "'list_of_z' and 'list_of_rmin' have not the same size !");
      } else if (setup_.has_key ("rmin")) {
        rmin = setup_.fetch_real ("rmin");
        if (! setup_.has_explicit_unit ("rmin")) {
          rmin *= lunit;
        }
      } else {
        rmin = 0.0 * lunit;
      }

      for (size_t i = 0; i < zs.size (); i++) {
        const double a_z = zs[i];
        double a_rmin;
        if (datatools::is_valid (rmin)) {
          a_rmin = rmin;
        } else {
          a_rmin = rmins[i];
        }
        const double a_rmax = rmaxs[i];
        this->add (a_z, a_rmin, a_rmax, false);
      }
      this->_compute_all_ ();
    } else if (build_mode_label == "datafile") {
      DT_THROW_IF (! setup_.has_key ("datafile"), std::logic_error,"Missing 'datafile' property !");
      std::string datafile = setup_.fetch_string ("datafile");
      datatools::fetch_path_with_env (datafile);
      double zmin;
      datatools::invalidate (zmin);
      if (setup_.has_key ("zmin")) {
        zmin = setup_.fetch_real ("zmin");
        if (! setup_.has_explicit_unit ("zmin")) {
          zmin *= lunit;
        }
      }
      double zmax;
      datatools::invalidate (zmax);
      if (setup_.has_key ("zmax")) {
        zmax = setup_.fetch_real ("zmax");
        if (! setup_.has_explicit_unit ("zmax")) {
          zmax *= lunit;
        }
      }
      int dc_mode = RMIN_RMAX;
      if (setup_.has_key ("datafile.columns")) {
        std::string dc = setup_.fetch_string ("datafile.columns");
        if (dc == "rmin_as_rmax") {
          dc_mode = RMIN_AS_RMAX;
        } else if (dc == "ignore_rmin") {
          dc_mode = IGNORE_RMIN;
        } else if (dc == "rmin_rmax") {
          dc_mode = RMIN_RMAX;
        } else {
          DT_THROW_IF (true,
                       std::logic_error,
                       "Invalid 'datafile.columns' mode ('"
                       << dc << "' !");
        }
      }
      this->initialize (datafile, zmin, zmax, dc_mode);
    } else {
      DT_THROW_IF (true, std::logic_error, "Invalid build mode '" << build_mode_label << "' !");
    }

    if (!has_partial_angle()) {
      if (setup_.has_key ("start_angle")) {
        double sangle = setup_.fetch_real("start_angle");
        if (! setup_.has_explicit_unit("start_angle")) {
          sangle *= aunit;
        }
        DT_THROW_IF(!setup_.has_key ("delta_angle"), std::logic_error, "Missing ''  parameter!");
        double dangle = setup_.fetch_real("delta_angle");
        if (! setup_.has_explicit_unit("delta_angle")) {
          dangle *= aunit;
        }
        set_start_angle(sangle);
        set_delta_angle(dangle);
      }
    }
    }
    lock();
    return;
  }

  void polycone::_at_lock()
  {
    this->i_shape_3d::_at_lock();
    if (!_computed_) {
      _compute_all_();
    }
    return;
  }

  void polycone::initialize (const std::string & filename_)
  {
    polycone::initialize (filename_,
                          std::numeric_limits<double>::quiet_NaN (),
                          std::numeric_limits<double>::quiet_NaN ());
    return;
  }

  void polycone::initialize (const std::string & filename_,
                             double zmin_, double zmax_, int dc_mode_)
  {
    const std::string filename = filename_;
    std::ifstream ifs;
    ifs.open (filename.c_str ());
    DT_THROW_IF (! ifs, std::logic_error, "Cannot open data file '" << filename << "' !");

    size_t count = 0;
    double length_unit = CLHEP::mm;
    double z_factor = 1.0;
    double r_factor = 1.0;
    double skin_thickness;
    datatools::invalidate (skin_thickness);
    double skin_step;
    datatools::invalidate (skin_step);
    bool ignore_rmin = false;
    if (dc_mode_ == RMIN_RMAX) {
      //std::cerr << "DEVEL: ****** polycone::initialize: RMIN_RMAX\n";
    }
    if (dc_mode_ == IGNORE_RMIN) {
      ignore_rmin = true;
      //std::cerr << "DEVEL: ****** polycone::initialize: IGNORE_RMIN\n";
    }
    bool rmin_as_rmax = false;
    if (dc_mode_ == RMIN_AS_RMAX) {
      rmin_as_rmax = true;
      //std::cerr << "DEVEL: ****** polycone::initialize: RMIN_AS_RMAX\n";
    }
    while (! ifs.eof ()) {
      std::string line;
      std::getline (ifs, line);
      count++;
      {
        std::istringstream iss (line);
        std::string  word;
        iss >> word;
        if (word.empty ()) continue;
        if (word[0] == '#') {
          if (word.size () >= 2) {
            if (word == "#@length_unit") {
              std::string unit_str;
              iss >> unit_str;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the length unit directive in data file '"
                           << filename << "' at line " << count << " !");
              length_unit = datatools::units::get_length_unit_from (unit_str);
            } else if (word == "#@ignore_rmin") {
              ignore_rmin = true;
              //std::cerr << "DEVEL: ****** polycone::initialize: form file: IGNORE_RMIN\n";
            } else if (word == "#@z_factor") {
              iss >> z_factor;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the Z-factor directive in data file '"
                           << filename << "' at line " << count << " !");
            } else if (word == "#@r_factor") {
              iss >> r_factor;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the R-factor directive in data file '"
                           << filename << "' at line " << count << " !");
            } else if (word == "#@skin_thickness") {
              iss >> skin_thickness;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the skin_thickness directive in data file '"
                           << filename << "' at line " << count << " !");
            } else if (word == "#@skin_step") {
              iss >> skin_step;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the skin_thickness directive in data file '"
                           << filename << "' at line " << count << " !");
            }
          }
          continue;
        }
      }
      // parse (z,r1) or (z,r1,r2) formated lines:
      {
        std::istringstream iss (line);
        double z, r1, r2;
        datatools::invalidate (z);
        datatools::invalidate (r1);
        datatools::invalidate (r2);
        iss >> z;
        DT_THROW_IF (! iss, std::logic_error,
                     "Format error for 'z' in data file '"
                     << filename << "' at line " << count << " !");
        iss >> r1;
        DT_THROW_IF (! iss, std::logic_error,
                     "Format error for 'r1' in data file '"
                     << filename << "' at line " << count << " !");
        // try to read a third column:
        std::string token;
        iss >> token;
        if (token.length () == 0) {
          // two columns format:
          r2 = r1;
          datatools::invalidate (r1);
          DT_THROW_IF(rmin_as_rmax,
                      std::logic_error,
                      "Polycone build mode 'rmin_as_rmax' cannot be used "
                      << "with the two-columns data file '"
                      << filename << "' !");
        } else {
          if (token[0] == '#') {
            // if line ends with a comment: this is two columns format !
            r2 = r1;
            datatools::invalidate (r1);
          } else {
            // try three columns format:
            std::istringstream iss2 (token);
            iss2 >> r2;
            DT_THROW_IF (! iss2, std::logic_error,
                         "Format error for 'r2' in data file '"
                         << filename << "' at line " << count << " !");
            if (ignore_rmin) {
              datatools::invalidate (r1);
            } else if (datatools::is_valid (skin_thickness)) {
              DT_THROW_IF (true, std::logic_error,
                           "Invalid format for 'z r2' pair "
                           << "in 'skin_thickness' mode from data file '"
                           << filename << "' at line " << count << " !");
            }
          }
        }
        DT_THROW_IF (datatools::is_valid (r2) && (r2 < 0.0), std::logic_error,
                     "Invalid value '" << r2 << "' for '2' in data file '"
                     << filename << "' at line " << count << " !");
        double tz, tr1, tr2;
        tz  = z  * z_factor * length_unit;
        tr1 = r1 * r_factor * length_unit;
        tr2 = r2 * r_factor * length_unit;
        if (datatools::is_valid (r1)) {
          if (rmin_as_rmax) {
            this->add (tz, 0.0, tr1, false);
          } else {
            this->add (tz, tr1, tr2, false);
          }
        } else {
          this->add (tz, tr2, false);
        }
      }
    }
    this->_compute_all_ ();

    if (datatools::is_valid (skin_thickness)) {
      if (! datatools::is_valid (skin_step)) {
        skin_step = std::abs (zmax_ - zmin_) / 20.0;
      }
      _build_from_envelope_and_skin_ (skin_thickness, skin_step, zmin_, zmax_);
    }
    return;
  }

  void polycone::_compute_misc_()
  {
    DT_THROW_IF (number_of_frustra() < 1, std::logic_error, "No frustrum !");
    DT_THROW_IF (_points_.size() < 2, std::logic_error, "No polycone sections !");

    _has_top_face_    = boost::logic::indeterminate;
    _has_bottom_face_ = boost::logic::indeterminate;
    _has_inner_face_  = boost::logic::indeterminate;

    {
      // Check for top face:
      _has_top_face_ = false;
      right_circular_conical_frustrum first;
      placement dummy;
      compute_frustrum(first, dummy, number_of_frustra() - 1);
      if (first.has_top_face()) {
        _has_top_face_ = true;
      }
    }

    {
      // Check for bottom face:
      _has_bottom_face_ = false;
      right_circular_conical_frustrum last;
      placement dummy;
      compute_frustrum(last, dummy, 0);
      if (last.has_bottom_face()) {
        _has_bottom_face_ = true;
      }
    }

    {
      // Check for inner face:
      _has_inner_face_ = false;
      for (size_t ifrustrum = 0; ifrustrum < number_of_frustra(); ifrustrum++) {
        // Check for inner face:
        right_circular_conical_frustrum fi;
        placement dummy;
        compute_frustrum(fi, dummy, ifrustrum);
        if (fi.has_inner_face()) {
          _has_inner_face_ = true;
          break;
        }
      }
    }

    return;
  }

  void polycone::_compute_all_()
  {
    _compute_misc_();
    _compute_surfaces_();
    _compute_volume_();
    _compute_limits_();
    _computed_ = true;
    return;
  }

  void polycone::add (double z_, double rmax_, bool compute_)
  {
    DT_THROW_IF (rmax_ < 0.0, std::domain_error, "Invalid negative 'rmax' !");
    r_min_max RMM;
    RMM.rmin = 0.0;
    RMM.rmax = rmax_;
    _points_[z_] = RMM;
    if (_points_.size () > 1) {
      if (compute_) _compute_all_ ();
    }
    return;
  }

  bool polycone::has_start_angle() const
  {
    return datatools::is_valid(_start_angle_);
  }

  void polycone::set_start_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ >= 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' start angle value !");
    _start_angle_ = new_value_;
    return;
  }

  double polycone::get_start_angle() const
  {
    return _start_angle_;
  }

  bool polycone::has_delta_angle() const
  {
    return datatools::is_valid(_delta_angle_);
  }

  void polycone::set_delta_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ > 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' delta angle value !");
    _delta_angle_ = new_value_;
    return;
  }

  double polycone::get_delta_angle() const
  {
    return _delta_angle_;
  }

  bool polycone::has_partial_angle() const
  {
    if (_delta_angle_ == 2 * M_PI) return false;
    if (_start_angle_ > 0.0) return true;
    return false;
  }

  bool polycone::is_extruded () const
  {
    return _extruded_;
  }

  void polycone::add(double z_, double rmin_,  double rmax_, bool compute_)
  {
    DT_THROW_IF (rmin_ < 0.0, std::domain_error, "Invalid negative 'rmin' !");
    DT_THROW_IF (rmax_ < rmin_, std::domain_error,
                 "Invalid value for 'rmax==" << rmax_ << "' ! ('rmin==" << rmin_ << "')");
    r_min_max RMM;
    if (rmin_ > 0.0) _extruded_ = true;
    RMM.rmin = rmin_;
    RMM.rmax = rmax_;
    _points_[z_] = RMM;
    if (_points_.size () > 1) {
      if (compute_) _compute_all_();
    }
    return;
  }

  bool polycone::is_valid () const
  {
    return _points_.size () > 1;
  }

  void polycone::reset ()
  {
    unlock();

    _points_.clear ();
    _set_defaults();

    this->i_shape_3d::_reset();
    return;
  }

  void polycone::initialize ()
  {
    _compute_all_ ();
    return;
  }

  void polycone::_compute_limits_ ()
  {
    if (! is_valid ()) return;
    _z_min_ = _z_max_ = _r_max_ = std::numeric_limits<double>::quiet_NaN();
    for (rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      const double z = i->first;
      const double rmax = i->second.rmax;
      if (! datatools::is_valid (_z_min_)) {
        _z_min_ = z;
      } else if (z < _z_min_) {
        _z_min_ = z;
      }
      if (! datatools::is_valid (_z_max_)) {
        _z_max_ = z;
      } else if (z > _z_max_) {
        _z_max_ = z;
      }
      if (! datatools::is_valid (_r_max_)) {
        _r_max_ = rmax;
      } else if (rmax > _r_max_) {
        _r_max_ = rmax;
      }
    }
    return;
  }

  void polycone::_compute_surfaces_ ()
  {
    if (! is_valid ()) return;

    // Bottom surface:
    if (has_bottom_face()) {
      rz_col_type::const_iterator i = _points_.begin ();
      const double rmin0 = i->second.rmin;
      const double rmax0 = i->second.rmax;
      _bottom_surface_ = M_PI * (rmax0 * rmax0 - rmin0 * rmin0);
    }

    // Outer side surface:
    {
      rz_col_type::const_iterator i = _points_.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      double rmax0 = i->second.rmax;
      double s = 0.0;
      rz_col_type::const_iterator j = _points_.begin ();
      j++;
      while (j != _points_.end ()) {
        const double z1 = j->first;
        const double rmin1 = j->second.rmin;
        const double rmax1 = j->second.rmax;
        const double R1 = rmax0;
        const double R2 = rmax1;
        const double A = cone::compute_frustrum_lateral_surface(z0, z1, R1, R2);
        s += A;
        // increment:
        j++;
        z0 = z1;
        rmin0 = rmin1;
        rmax0 = rmax1;
      }
      _outer_side_surface_ = s;

      // Top surface:
      if (has_bottom_face()) {
        _top_surface_ = M_PI * (rmax0 * rmax0 - rmin0 * rmin0);
      }
    }

    if (has_inner_face()) {
      // Inner side surface:
      rz_col_type::const_iterator i = _points_.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      double s = 0.0;
      rz_col_type::const_iterator j = i;
      j++;
      while (j != _points_.end ()) {
        const double z1 = j->first;
        const double rmin1 = j->second.rmin;
        // See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
        const double R1 = rmin0;
        const double R2 = rmin1;
        const double A = cone::compute_frustrum_lateral_surface(z0, z1, R1, R2);
        s += A;
        // Increment:
        j++;
        z0 = z1;
        rmin0 = rmin1;
      }
      _inner_side_surface_ = s;
    }
    return;
  }

  void polycone::_compute_volume_()
  {
    if (! is_valid ()) return;
    double vext = 0.0;
    double vint = 0.0;
    // Outer envelope:
    {
      rz_col_type::const_iterator i = _points_.begin();
      double z0 = i->first;
      double rmax0 = i->second.rmax;
      rz_col_type::const_iterator j = i;
      j++;
      while (j != _points_.end()) {
        const double z1 = j->first;
        const double r1 = j->second.rmax;
        const double R1 = rmax0;
        const double R2 = r1;
        const double V = cone::compute_frustrum_volume(z0,z1,R1,R2);
        vext += V;
        // Increment:
        j++;
        z0 = z1;
        rmax0 = r1;
      }
    }
    // Inner envelope:
    if (has_inner_face()) {
      rz_col_type::const_iterator i = _points_.begin();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      rz_col_type::const_iterator j = i;
      j++;
      while (j != _points_.end()) {
        const double z1 = j->first;
        const double rmin1 = j->second.rmin;
        const double R1 = rmin0;
        const double R2 = rmin1;
        const double V = cone::compute_frustrum_volume(z0,z1,R1,R2);
        vint += V;
        // Increment:
        j++;
        z0 = z1;
        rmin0 = rmin1;
      }
    }

    if (has_partial_angle()) {
      vext *= (_delta_angle_ / (2 * M_PI));
      vint *= (_delta_angle_ / (2 * M_PI));
    }

    _outer_volume_ = vext;
    _inner_volume_ = vint;
    _volume_ = _outer_volume_ - _inner_volume_;
    return;
  }

  double polycone::get_surface(uint32_t mask_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    double s = 0.0;
    int mask = mask_;

    if (has_inner_face() && mask & FACE_INNER_SIDE) {
      s += _inner_side_surface_;
    }

    if (mask & FACE_OUTER_SIDE) {
      s += _outer_side_surface_;
    }

    if (has_bottom_face() && mask & FACE_BOTTOM) {
      s += _bottom_surface_;
    }

    if (has_top_face() && mask & FACE_TOP) {
      s += _top_surface_;
    }

    if (has_partial_angle()) {
      s *= (_delta_angle_ / (2 * M_PI));
    }

    if (has_angle_faces()) {
      if (mask & FACE_START_ANGLE) {
        s += _start_angle_surface_;
      }
      if (mask & FACE_STOP_ANGLE) {
        s += _stop_angle_surface_;
      }
    }

    return s;
  }

  double polycone::get_volume (uint32_t /*flags*/) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    return _volume_;
  }

  double polycone::get_parameter (const std::string & flag_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    if ( flag_ == "r_max" )              return get_r_max ();
    if ( flag_ == "z_min" )              return get_z_min ();
    if ( flag_ == "z_max" )              return get_z_max ();
    if ( flag_ == "volume" )             return get_volume ();
    if ( flag_ == "volume.inner" )       return _inner_volume_;
    if ( flag_ == "volume.outer" )       return _outer_volume_;
    if ( flag_ == "surface.top" )        return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" )     return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.inner_side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer_side" ) return get_surface (FACE_OUTER_SIDE);
    if ( flag_ == "surface" )            return get_surface (FACE_ALL);

    DT_THROW_IF (true, std::logic_error, "Unknown parameter '" << flag_ << "' flag !");
  }

  bool polycone::is_outside(const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    unsigned int noutside = 0;
    double skin = get_skin(skin_);
    // Scan the conical frustra:
    for (unsigned int i = 0; i < number_of_frustra(); i++) {
      right_circular_conical_frustrum rccf;
      placement rccf_placement;
      compute_frustrum(rccf, rccf_placement, i);
      vector_3d pos_c;
      rccf_placement.mother_to_child(point_, pos_c);
      if (rccf.is_outside(pos_c, skin)) {
        noutside++;
      }
    }
    return noutside == number_of_frustra();
  }

  bool polycone::is_inside (const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;

    const double z = point_.z();
    // Exclude points outside the bounding cylinder:
    if (z > get_z_max() - hskin) return false;
    if (z < get_z_min() + hskin) return false;
    const double r = hypot(point_.x(), point_.y());
    if (r > get_r_max() - hskin) return false;

    bool inside = false;
    // Scan the conical frustra:
    for (unsigned int i = 0; i < number_of_frustra(); i++) {
      right_circular_conical_frustrum rccf;
      placement rccf_placement;
      compute_frustrum(rccf, rccf_placement, i);
      vector_3d pos_c;
      rccf_placement.mother_to_child(point_, pos_c);
      if (rccf.is_inside(pos_c, 0.0)) {
        // std::cerr << "DEVEL: polyhedra::is_inside: " << "in frustrum #" << i << std::endl;
        face_identifier mask_id;
        uint32_t face_bits = 0;
        face_bits |= right_circular_conical_frustrum::FACE_OUTER_SIDE;
        if (rccf.has_inner_face()) {
          face_bits |= right_circular_conical_frustrum::FACE_INNER_SIDE;
        }
        if (i == 0 && rccf.has_top_face()) {
          face_bits |= right_circular_conical_frustrum::FACE_TOP;
        }
        if (i == number_of_frustra() - 1 && rccf.has_bottom_face()) {
          face_bits |= right_circular_conical_frustrum::FACE_BOTTOM;
        }
        mask_id.set_face_bits(face_bits);
        face_identifier surf_id = rccf.on_surface(point_, mask_id, skin);
        if (! surf_id.is_valid()) {
          inside = true;
        }
        break;
      }
    }

    // std::cerr << "DEVEL: polycone::is_inside: " << "inside=" << inside << std::endl;
    return inside;
  }

  vector_3d polycone::get_normal_on_surface (const vector_3d & position_,
                                             const face_identifier & surface_bit_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    vector_3d normal;
    invalidate(normal);
    switch(surface_bit_.get_face_bits()) {
    case FACE_BOTTOM:
      if (has_bottom_face()) {
        normal.set(0.0, 0.0, -1.0);
      }
      break;
    case FACE_TOP:
      if (has_top_face()) {
        normal.set(0.0, 0.0, +1.0);
      }
      break;
    case FACE_OUTER_SIDE:
      for (size_t i = 0; i < number_of_frustra(); i++) {
        right_circular_conical_nappe outer_nappe;
        placement pl_nappe;
        compute_outer_face(outer_nappe, pl_nappe, i);
        vector_3d position_c;
        pl_nappe.mother_to_child(position_, position_c);
        vector_3d normal_c;
        normal_c = outer_nappe.get_normal_on_surface(position_c, false);
        pl_nappe.child_to_mother_direction(normal_c, normal);
      }
      break;
    case FACE_INNER_SIDE:
      if (has_inner_face()) {
        for (size_t i = 0; i < number_of_frustra(); i++) {
          right_circular_conical_nappe inner_nappe;
          placement pl_nappe;
          compute_inner_face(inner_nappe, pl_nappe, i);
          if (inner_nappe.is_valid()) {
            vector_3d position_c;
            pl_nappe.mother_to_child(position_, position_c);
            vector_3d normal_c;
            normal_c = -inner_nappe.get_normal_on_surface(position_c, false);
            pl_nappe.child_to_mother_direction(normal_c, normal);
          }
        }
      }
      break;
    case FACE_START_ANGLE:
      if (has_partial_angle()) {
        for (size_t i = 0; i < number_of_frustra(); i++) {
          quadrangle qface;
          triangle tface;
          placement pl_face;
          compute_start_angle_face(qface, tface, pl_face, i);
          vector_3d position_c;
          pl_face.mother_to_child(position_, position_c);
          if (qface.is_valid()) { // && qface.is_on_surface(position_c, skin)) {
            vector_3d normal_c = qface.get_normal_on_surface(position_c, false);
            pl_face.child_to_mother_direction(normal_c, normal);
          } else if (tface.is_valid()) { // && tface.is_on_surface(position_c, skin)) {
            vector_3d normal_c = tface.get_normal_on_surface(position_c, false);
            pl_face.child_to_mother_direction(normal_c, normal);
          }
        }
      }
      break;
    case FACE_STOP_ANGLE:
      if (has_partial_angle()) {
        for (size_t i = 0; i < number_of_frustra(); i++) {
          quadrangle qface;
          triangle tface;
          placement pl_face;
          compute_stop_angle_face(qface, tface, pl_face, i);
          vector_3d position_c;
          pl_face.mother_to_child(position_, position_c);
          if (qface.is_valid()) { // && qface.is_on_surface(position_c, skin)) {
            vector_3d normal_c = qface.get_normal_on_surface(position_c, false);
            pl_face.child_to_mother_direction(normal_c, normal);
          } else if (tface.is_valid()) { // && tface.is_on_surface(position_c, skin)) {
            vector_3d normal_c = tface.get_normal_on_surface(position_c, false);
            pl_face.child_to_mother_direction(normal_c, normal);
          }
        }
      }
      break;
    }
    return normal;
  }

  face_identifier polycone::on_surface(const vector_3d & position_,
                                       const face_identifier & surface_mask_,
                                       double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
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

  bool polycone::find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 face_intercept_info & intercept_,
                                 double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    intercept_.reset();

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

    return intercept_.is_ok();
  }

  std::ostream & operator<< (std::ostream & out_, const polycone & p_)
  {
    out_ << '{' << polycone::polycone_label();
    out_ << ' ' << p_._points_.size ();
    for (polycone::rz_col_type::const_iterator i = p_._points_.begin ();
         i != p_._points_.end ();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      double rmax = i->second.rmax;
      out_ << ' ' << z << ' ' << rmin << ' ' << rmax;
    }
    out_ << '}';
    return out_;
  }

  std::istream & operator>> (std::istream & in_, polycone & p_)
  {
    p_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    std::string name;
    in_ >> name;
    if (name != polycone::polycone_label()) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    size_t n;
    in_ >> n;
    for (size_t i = 0; i < n; i++) {
      double rmin, rmax, z;
      in_ >> z >> rmin >> rmax;
      if (! in_) {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
      try {
        p_.add (z, rmin, rmax, false);
      }
      catch (...)
        {
          p_.reset ();
          in_.clear (std::ios_base::failbit);
        }
    }
    c = 0;
    in_.get (c);
    if (c != '}') {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    p_._compute_all_ ();
    return in_;
  }

  void polycone::tree_dump (std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_,
                            bool inherit_) const
  {
    i_shape_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z(min) : " << get_z_min () / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z(max) : " << get_z_max () / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "R(max) : " << get_r_max () / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Number of points : " << _points_.size () << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Number of furstra : " << number_of_frustra() << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Volume : " << get_volume () / CLHEP::cm3 << " cm3" << std::endl;

    if (has_bottom_face()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Bottom surface : "
           << get_surface (FACE_BOTTOM) / CLHEP::cm2 << " cm2" << std::endl;
    }

    if (has_top_face()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Top surface : "
           << get_surface (FACE_TOP) / CLHEP::cm2 << " cm2" << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Outer side surface : "
         << get_surface (FACE_OUTER_SIDE) / CLHEP::cm2 << " cm2" << std::endl;

    if (has_inner_face()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Inner side surface : "
           << get_surface (FACE_INNER_SIDE) / CLHEP::cm2 << " cm2" << std::endl;
    }

    if (has_partial_angle()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Start angle surface : "
           << get_surface (FACE_START_ANGLE) / CLHEP::cm2 << " cm2" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Stop angle surface : "
           << get_surface (FACE_STOP_ANGLE) / CLHEP::cm2 << " cm2" << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Total surface : " << get_surface (FACE_ALL) / CLHEP::cm2 << " cm2" << std::endl;

    return;
  }

  void polycone::generate_wires_self(wires_type & wires_,
                                     uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polycone!");
    bool devel = false;
    // devel = true;
    if (devel) std::cerr << "polycone::generate_wires_self: Entering..." << std::endl;

    // bool debug_explode    = options_ & WR_BASE_EXPLODE;
    bool draw_bottom      = !(options_ & WR_POLYCONE_NO_BOTTOM_FACE);
    bool draw_top         = !(options_ & WR_POLYCONE_NO_TOP_FACE);
    bool draw_inner       = !(options_ & WR_POLYCONE_NO_INNER_FACE);
    bool draw_outer       = !(options_ & WR_POLYCONE_NO_OUTER_FACE);
    bool draw_start_angle = !(options_ & WR_POLYCONE_NO_START_ANGLE_FACE);
    bool draw_stop_angle  = !(options_ & WR_POLYCONE_NO_STOP_ANGLE_FACE);
    bool draw_boundings    =  (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings && has_bounding_data()) {
      if (devel) std::cerr << "polycone::generate_wires_self: draw boundings..." << std::endl;
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    if (has_bottom_face() && draw_bottom) {
      if (devel) std::cerr << "polycone::generate_wires_self: draw bottom..." << std::endl;
      disk bottom_face;
      placement bottom_pl;
      compute_bottom_face(bottom_face, bottom_pl);
      if (bottom_face.is_valid()) {
        uint32_t options = base_options;
        // options |= WR_BASE_GRID;
        bottom_face.generate_wires(wires_, bottom_pl, options);
      }
    }

    if (has_top_face() && draw_top) {
      if (devel) std::cerr << "polycone::generate_wires_self: draw top..." << std::endl;
      disk top_face;
      placement top_pl;
      compute_top_face(top_face, top_pl);
      if (top_face.is_valid()) {
        uint32_t options = base_options;
        // options |= WR_BASE_GRID;
        top_face.generate_wires(wires_, top_pl, options);
      }
    }

    if (has_inner_face() && draw_inner) {
      if (devel) std::cerr << "polycone::generate_wires_self: draw inner..." << std::endl;
      composite_surface inner;
      placement pl_inner;
      compute_inner_face(inner, pl_inner);
      if (inner.is_valid()) {
        uint32_t options = base_options;
        inner.generate_wires(wires_, pl_inner, options);
      }
    }

    if (draw_outer) {
      if (devel) std::cerr << "polycone::generate_wires_self: draw outer..." << std::endl;
      composite_surface outer;
      placement pl_outer;
      compute_outer_face(outer, pl_outer);
      if (outer.is_valid()) {
        uint32_t options = base_options;
        outer.generate_wires(wires_, pl_outer, options);
      }
    }

    if (has_partial_angle()) {

      if (draw_start_angle) {
        if (devel) std::cerr << "polycone::generate_wires_self: draw start angle..." << std::endl;
        for (size_t i = 0; i < number_of_frustra(); i++) {
          quadrangle qface;
          triangle tface;
          placement pl_face;
          compute_start_angle_face(qface, tface, pl_face, i);
          uint32_t options = base_options;
          if (qface.is_valid()) qface.generate_wires(wires_, pl_face, options);
          else if (tface.is_valid()) tface.generate_wires(wires_, pl_face, options);
        }
      }

      if (draw_stop_angle) {
        if (devel) std::cerr << "polycone::generate_wires_self: draw stop angle..." << std::endl;
        for (size_t i = 0; i < number_of_frustra(); i++) {
          quadrangle qface;
          triangle tface;
          placement pl_face;
          compute_stop_angle_face(qface, tface, pl_face, i);
          uint32_t options = base_options;
          if (qface.is_valid()) qface.generate_wires(wires_, pl_face, options);
          else if (tface.is_valid()) tface.generate_wires(wires_, pl_face, options);
        }
      }

    }

    if (devel) std::cerr << "polycone::generate_wires_self: Exiting." << std::endl;
    return;
  }

} // end of namespace geomtools
