// triangle.cc

// Ourselves:
#include <geomtools/triangle.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// - GSL:
// #include <gsl/gsl_poly.h>
#include <gsl/gsl_linalg.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/utils.h>
#include <geomtools/plane.h>
#include <geomtools/line_3d.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(triangle, "geomtools::triangle")

  const std::string & triangle::triangle_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "triangle";
    }
    return label;
  }

  std::string triangle::get_shape_name() const
  {
    return triangle_label();
  }

  bool triangle::is_valid() const
  {
    if (!::geomtools::is_valid(get_vertex_0())) {
      std::cerr << "DEVEL: triangle::is_valid: invalid V0=" << get_vertex_0() << std::endl;
      return false;
    }
    if (!::geomtools::is_valid(get_vertex_1())) {
      std::cerr << "DEVEL: triangle::is_valid: invalid V1=" << get_vertex_1() << std::endl;
      return false;
    }
    if (!::geomtools::is_valid(get_vertex_2())) {
      std::cerr << "DEVEL: triangle::is_valid: invalid V2=" << get_vertex_2() << std::endl;
      return false;
    }
    return true;
    /*
    return ::geomtools::is_valid(get_vertex_0())
      && ::geomtools::is_valid(get_vertex_1())
      && ::geomtools::is_valid(get_vertex_2());
    */
  }

  // virtual
  unsigned int triangle::compute_vertexes(vertex_col_type & vertexes_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    vertexes_.clear();
    vertexes_.reserve(3);
    vertexes_.push_back(get_vertex_0());
    vertexes_.push_back(get_vertex_1());
    vertexes_.push_back(get_vertex_2());
    return vertexes_.size();
  }

  // virtual
  double triangle::get_perimeter(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    double p = (get_vertex_1() - get_vertex_0()).mag()
      + (get_vertex_2() - get_vertex_1()).mag()
      + (get_vertex_0() - get_vertex_2()).mag();
    return p;
  }

  // virtual
  double triangle::get_surface(uint32_t /*flags_*/) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    const geomtools::vector_3d u01 = (get_vertex_1() - get_vertex_0());
    const geomtools::vector_3d u02 = (get_vertex_2() - get_vertex_0());
    double s = (u01.cross(u02).mag());
    return s;
  }

  // static
  bool triangle::check_alignement(const geomtools::vector_3d & v0_,
                                  const geomtools::vector_3d & v1_,
                                  const geomtools::vector_3d & v2_,
                                  double tolerance_)
  {
    // Check that all 3 vertices are different :
    const geomtools::vector_3d u01 = (v1_ - v0_);
    if (u01.mag2() == 0) {
      return false;
    }
    const geomtools::vector_3d u12 = (v2_ - v1_);
    if (u12.mag2() == 0) {
      return false;
    }
    const geomtools::vector_3d u20 = (v0_ - v2_);
    if (u20.mag2() == 0) {
      return false;
    }

    // Check non-alignment of vertices :

    double normal_tolerance = tolerance_;
    if (tolerance_ <= 0.0 || ! datatools::is_valid (tolerance_)) {
      // use default (rather strict) tolerance :
      normal_tolerance = constants::get_default_tolerance();
    }

    // Vertices 0, 1, 2 :
    const geomtools::vector_3d n012 = u01.cross(u12);
    const double m012 = n012.mag();
    if (m012 < normal_tolerance) {
      // DT_LOG_WARNING (datatools::logger::PRIO_WARNING, "Vertices 0, 1, 2 are aligned !");
      return false;
    }
    return true;
  }

  void triangle::set_vertexes(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_)
  {
    double tolerance = get_tolerance();
    DT_THROW_IF(!check_alignement(p0_, p1_, p2_, tolerance),
                std::logic_error,
                "Vertexes are aligned!");
    _vertex_0_ext_ = 0;
    _vertex_1_ext_ = 0;
    _vertex_2_ext_ = 0;
    _vertex_0_ = p0_;
    _vertex_1_ = p1_;
    _vertex_2_ = p2_;
    return;
  }

  void triangle::set_vertexes_ext(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_)
  {
    double tolerance = get_tolerance();
    DT_THROW_IF(!check_alignement(p0_, p1_, p2_, tolerance),
                std::logic_error,
                "Vertexes are aligned!");
    invalidate(_vertex_0_);
    invalidate(_vertex_1_);
    invalidate(_vertex_2_);
    _vertex_0_ext_ = &p0_;
    _vertex_1_ext_ = &p1_;
    _vertex_2_ext_ = &p2_;
    return;
  }

  bool triangle::has_vertex_0_ext() const
  {
    return _vertex_0_ext_ != 0;
  }

  bool triangle::has_vertex_1_ext() const
  {
    return _vertex_1_ext_ != 0;
  }

  bool triangle::has_vertex_2_ext() const
  {
    return _vertex_2_ext_ != 0;
  }

  const vector_3d & triangle::get_vertex_0() const
  {
    if (has_vertex_0_ext()) {
      return *_vertex_0_ext_;
    }
    return _vertex_0_;
  }

  const vector_3d & triangle::get_vertex_1() const
  {
    if (has_vertex_1_ext()) {
      return *_vertex_1_ext_;
    }
    return _vertex_1_;
  }

  const vector_3d & triangle::get_vertex_2() const
  {
    if (has_vertex_2_ext()) {
      return *_vertex_2_ext_;
    }
    return _vertex_2_;
  }

  const vector_3d & triangle::get_vertex(int index_) const
  {
    DT_THROW_IF(index_ < 0 || index_ > 2,
                std::range_error,
                "Invalid vertex index [" << index_ << "]!");
    if (index_ == 0) return get_vertex_0();
    if (index_ == 1) return get_vertex_1();
    return get_vertex_2();
  }

  void triangle::_set_defaults()
  {
    _vertex_0_ext_ = 0;
    _vertex_1_ext_ = 0;
    _vertex_2_ext_ = 0;
    invalidate(_vertex_0_);
    invalidate(_vertex_1_);
    invalidate(_vertex_2_);
    return;
  }

  triangle::triangle()
  {
    _set_defaults();
    return;
  }

  triangle::triangle(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_)
  {
    _set_defaults();
    set_vertexes(p0_, p1_, p2_);
    return;
  }

  triangle::~triangle()
  {
    return;
  }

  void triangle::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      vector_3d vtx0;
      vector_3d vtx1;
      vector_3d vtx2;
      invalidate_vector_3d(vtx0);
      invalidate_vector_3d(vtx1);
      invalidate_vector_3d(vtx2);

      if (config_.has_key("vertex_0")) {
        std::vector<double> coord0;
        config_.fetch("vertex_0", coord0);
        DT_THROW_IF(coord0.size() != 3, std::logic_error, "Invalid number of coordinates for vertex #0!");
        set_vector_3d(coord0, vtx0);
        DT_THROW_IF(! is_valid_vector_3d(vtx0), std::logic_error, "Invalid vertex #0!");
        if ( config_.has_explicit_unit("vertex_0")) {
          vtx0 *= lunit;
        }
      }

      if (config_.has_key("vertex_1")) {
        std::vector<double> coord1;
        config_.fetch("vertex_1", coord1);
        DT_THROW_IF(coord1.size() != 3, std::logic_error, "Invalid number of coordinates for vertex #1!");
        set_vector_3d(coord1, vtx1);
        DT_THROW_IF(! is_valid_vector_3d(vtx1), std::logic_error, "Invalid vertex #1!");
        if ( config_.has_explicit_unit("vertex_1")) {
          vtx1 *= lunit;
        }
      }

      if (config_.has_key("vertex_2")) {
        std::vector<double> coord2;
        config_.fetch("vertex_2", coord2);
        DT_THROW_IF(coord2.size() != 3, std::logic_error, "Invalid number of coordinates for vertex #2!");
        set_vector_3d(coord2, vtx2);
        DT_THROW_IF(! is_valid_vector_3d(vtx2), std::logic_error, "Invalid vertex #2!");
        if ( config_.has_explicit_unit("vertex_2")) {
          vtx2 *= lunit;
        }
      }

      set_vertexes(vtx0, vtx1, vtx2);
    }

    return;
  }

  void triangle::reset()
  {
    _set_defaults();
    return;
  }

  void triangle::compute_proper_frame(vector_3d & u0_, vector_3d & u1_, vector_3d & u2_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    u0_ = get_vertex_1() - get_vertex_0();
    u1_ = get_vertex_2() - get_vertex_0();
    u2_ = u0_.cross(u1_).unit();
    return;
  }

  bool triangle::compute_proper_frame_coordinates(const vector_3d & position_,
                                                  double & xp_, double & yp_, double & zp_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    datatools::invalidate(xp_);
    datatools::invalidate(yp_);
    datatools::invalidate(zp_);
    vector_3d u0;
    vector_3d u1;
    vector_3d u2;
    compute_proper_frame(u0, u1, u2);

    double a_data[9];
    a_data[0] = u0.x();
    a_data[1] = u1.x();
    a_data[2] = u2.x();

    a_data[3] = u0.y();
    a_data[4] = u1.y();
    a_data[5] = u2.y();

    a_data[6] = u0.z();
    a_data[7] = u1.z();
    a_data[8] = u2.z();

    double b_data[3];
    b_data[0] = position_.x() - get_vertex_0().x();
    b_data[1] = position_.y() - get_vertex_0().y();
    b_data[2] = position_.z() - get_vertex_0().z();

    double x_data[3];

    gsl_matrix_view m = gsl_matrix_view_array(a_data, 3, 3);
    gsl_vector_view b = gsl_vector_view_array(b_data, 3);
    gsl_vector_view x = gsl_vector_view_array(x_data, 3);
    int s;
    gsl_permutation * p = gsl_permutation_alloc(3);
    int errcode = gsl_linalg_LU_decomp(&m.matrix, p, &s);
    if (errcode != GSL_SUCCESS) {
      gsl_permutation_free(p);
      return false;
    }
    errcode = gsl_linalg_LU_solve(&m.matrix, p, &b.vector, &x.vector);
    if (errcode != GSL_SUCCESS) {
      gsl_permutation_free(p);
      return false;
    }
    gsl_permutation_free(p);
    xp_ = x_data[0];
    yp_ = x_data[1];
    zp_ = x_data[2];
    return true;
  }

  bool triangle::is_on_surface(const vector_3d & position_,
                               double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    bool devel = false;
    // devel = true;
    double normal_tolerance = compute_tolerance(tolerance_);
    double half_normal_tolerance = 0.5 * normal_tolerance;
    double xM;
    double yM;
    double zM;
    if (!compute_proper_frame_coordinates(position_, xM, yM, zM)) {
      if (devel) std::cerr << "DEVEL: triangle::is_on_surface: no poper frame coordinates" << std::endl;
      return false;
    }
      if (devel) std::cerr << "DEVEL: triangle::is_on_surface: invalid half_normal_tolerance=" << half_normal_tolerance << std::endl;
    if (std::abs(zM) > half_normal_tolerance) {
      if (devel) std::cerr << "DEVEL: triangle::is_on_surface: invalid zM=" << zM << std::endl;
      return false;
    }
    if (xM < 0.0) {
      if (devel) std::cerr << "DEVEL: triangle::is_on_surface: invalid negative xM=" << xM << std::endl;
       return false;
    }
    if (yM < 0.0) {
      if (devel) std::cerr << "DEVEL: triangle::is_on_surface: invalid negative yM=" << yM << std::endl;
      return false;
    }
    if ((xM + yM) > 1.0) {
      if (devel) std::cerr << "DEVEL: triangle::is_on_surface: invalid too large xM+yM=" << xM + yM << std::endl;
      return false;
    }
    return true;
  }

  vector_3d triangle::get_normal_on_surface(const vector_3d & position_,
                                            bool check_,
                                            double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    vector_3d u0;
    vector_3d u1;
    compute_proper_frame(u0, u1, normal);
    return normal;
  }

  bool triangle::find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    intercept_.reset();
    plane p(get_vertex_0(), get_vertex_1() - get_vertex_0(),  get_vertex_2() - get_vertex_0());
    double tolerance = compute_tolerance(tolerance_);
    bool res = p.find_intercept(from_, direction_, intercept_, tolerance);
    if (res) {
      if (!is_on_surface(intercept_.get_impact(), tolerance)) {
        intercept_.reset();
      }
    }
    return intercept_.is_ok();
  }

  void triangle::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Vertex 0 : " << get_vertex_0();
    if (has_vertex_0_ext()) {
      out_ << " (external)";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Vertex 1 : " << get_vertex_1();
    if (has_vertex_1_ext()) {
      out_ << " (external)";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Vertex 2 : " << get_vertex_2();
    if (has_vertex_2_ext()) {
      out_ << " (external)";
    }
    out_ << std::endl;

    return;
  }

  void triangle::generate_wires_self(wires_type & wires_,
                                     uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid triangle!");
    bool no_first_edge  = (options_ & WR_TRIANGLE_NO_FIRST_EDGE);
    bool no_second_edge = (options_ & WR_TRIANGLE_NO_SECOND_EDGE);
    bool no_third_edge  = (options_ & WR_TRIANGLE_NO_THIRD_EDGE);

    uint32_t base_options = options_ & WR_BASE_MASK;

    if (!no_first_edge) {
      {
        polyline_type dummy;
        wires_.push_back(dummy);
      }
      polyline_type & segment = wires_.back();
      segment.push_back(get_vertex_0());
      segment.push_back(get_vertex_1());
    }

    if (!no_second_edge) {
      {
        polyline_type dummy;
        wires_.push_back(dummy);
      }
      polyline_type & segment = wires_.back();
      segment.push_back(get_vertex_1());
      segment.push_back(get_vertex_2());
    }

    if (!no_third_edge) {
      {
        polyline_type dummy;
        wires_.push_back(dummy);
      }
      polyline_type & segment = wires_.back();
      segment.push_back(get_vertex_2());
      segment.push_back(get_vertex_0());
    }

    if (base_options & i_wires_3d_rendering::WR_BASE_GRID) {
      vector_3d u0 = get_vertex_1() - get_vertex_0();
      double l0 = u0 .mag();
      vector_3d u1 = get_vertex_2() - get_vertex_0();
      double l1 = u1 .mag();
      u0 = u0.unit();
      u1 = u1.unit();
      uint32_t nsamples_12 = linear_sampling_from_options(base_options);
      double dl0 = l0 / (nsamples_12 - 1);
      double dl1 = l1 / (nsamples_12 - 1);
      for (size_t i = 1; i < nsamples_12 - 1; i++) {
        vector_3d p0 = get_vertex_0() + i * dl0 * u0;
        vector_3d p1 = get_vertex_0() + i * dl1 * u1;
        line_3d l01(p0, p1);
        l01.generate_wires_self(wires_);
      }

      vector_3d u01 = get_vertex_2() - get_vertex_1();
      double l01 = u01 .mag();
      double dl01 = l01 / (nsamples_12 - 1);
      u01 = u01.unit();
      for (size_t i = 1; i < nsamples_12 - 1; i++) {
        vector_3d p0 = get_vertex_0();
        vector_3d p1 = get_vertex_1() + i * dl01 * u01;
        line_3d line_l01(p0, p1);
        line_l01.generate_wires_self(wires_);
      }

    }

    return;
  }

} // end of namespace geomtools
