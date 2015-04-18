/// quadrangle.cc

// Ourselves:
#include <geomtools/quadrangle.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// - GSL:
#include <gsl/gsl_linalg.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/utils.h>
#include <geomtools/plane.h>
#include <geomtools/line_3d.h>
#include <geomtools/triangle.h>

namespace geomtools {

  const std::string & quadrangle::quadrangle_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "quadrangle";
    }
    return label;
  }

  std::string quadrangle::get_shape_name() const
  {
    return quadrangle_label();
  }

  bool quadrangle::is_valid() const
  {
    return ::geomtools::is_valid(get_vertex_0())
      && ::geomtools::is_valid(get_vertex_1())
      && ::geomtools::is_valid(get_vertex_2())
      && ::geomtools::is_valid(get_vertex_3());
  }

  // virtual
  unsigned int quadrangle::compute_vertexes(vertex_col_type & vertexes_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    vertexes_.clear();
    vertexes_.reserve(4);
    vertexes_.push_back(get_vertex_0());
    vertexes_.push_back(get_vertex_1());
    vertexes_.push_back(get_vertex_2());
    vertexes_.push_back(get_vertex_3());
    return vertexes_.size();
  }

  // virtual
  double quadrangle::get_perimeter(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    double p = (get_vertex_1() - get_vertex_0()).mag()
      + (get_vertex_2() - get_vertex_1()).mag()
      + (get_vertex_3() - get_vertex_2()).mag()
      + (get_vertex_0() - get_vertex_3()).mag();
    return p;
  }

  // virtual
  double quadrangle::get_surface(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    const geomtools::vector_3d u01 = (get_vertex_1() - get_vertex_0());
    const geomtools::vector_3d u02 = (get_vertex_2() - get_vertex_0());
    const geomtools::vector_3d u03 = (get_vertex_3() - get_vertex_0());
    double s= (u01.cross(u02).mag());
    s += (u02.cross(u03).mag());
    return s;
  }

  // static
  bool quadrangle::check_vertexes(const geomtools::vector_3d & v0_,
                                  const geomtools::vector_3d & v1_,
                                  const geomtools::vector_3d & v2_,
                                  const geomtools::vector_3d & v3_,
                                  double tolerance_)
  {
    double normal_tolerance = tolerance_;
    if (tolerance_ <= 0.0 || ! datatools::is_valid(tolerance_)) {
      // use default (rather strict) tolerance :
      normal_tolerance = constants::get_default_tolerance();
    }
    double half_normal_tolerance = 0.5 * normal_tolerance;

    if (!triangle::check_alignement(v0_, v1_, v2_, normal_tolerance)) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Vertex 0, 1 and 2 are aligned!");
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "|-- Vertex 0 = " << v0_);
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "|-- Vertex 1 = " << v1_);
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "`-- Vertex 2 = " << v2_);
      return false;
    }
    plane p(v0_, v1_ - v0_,  v2_ - v0_);
    if (!p.is_on_surface(v3_, normal_tolerance)) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Vertex 3 is not in the plane defined by vertexs 0, 1 and 2!");
      return false;
    }
    triangle t(v0_, v1_,  v2_);
    if (t.is_on_surface(v3_, normal_tolerance)) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Vertexes do not form a convex polygon!");
      return false;
    }
    double x3, y3, z3;
    t.compute_proper_frame_coordinates(v3_, x3, y3, z3);
    if (x3 >= 0.0) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Vertex 3 is misplaced!");
      return false;
    }
    if (y3 <= 0.0) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Vertex 3 is misplaced!");
      return false;
    }
    if (x3 + y3 >= 1.0) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Vertex 3 is misplaced!");
    }
    if (std::abs(z3) > half_normal_tolerance) {
      return false;
    }
    return true;
  }

  void quadrangle::set_vertexes(const vector_3d & p0_,
                                const vector_3d & p1_,
                                const vector_3d & p2_,
                                const vector_3d & p3_)
  {
    double tolerance = get_tolerance();
    DT_THROW_IF(!check_vertexes(p0_, p1_, p2_, p3_, tolerance),
                std::logic_error,
                "Vertexes are aligned!");
    _vertex_0_ext_ = 0;
    _vertex_1_ext_ = 0;
    _vertex_2_ext_ = 0;
    _vertex_3_ext_ = 0;
    _vertex_0_ = p0_;
    _vertex_1_ = p1_;
    _vertex_2_ = p2_;
    _vertex_3_ = p3_;
    return;
  }

  void quadrangle::set_vertexes_ext(const vector_3d & p0_,
                                    const vector_3d & p1_,
                                    const vector_3d & p2_,
                                    const vector_3d & p3_)
  {
    double tolerance = get_tolerance();
    DT_THROW_IF(!check_vertexes(p0_, p1_, p2_, p3_, tolerance),
                std::logic_error,
                "Vertexes are aligned!");
    invalidate(_vertex_0_);
    invalidate(_vertex_1_);
    invalidate(_vertex_2_);
    invalidate(_vertex_3_);
    _vertex_0_ext_ = &p0_;
    _vertex_1_ext_ = &p1_;
    _vertex_2_ext_ = &p2_;
    _vertex_3_ext_ = &p3_;
    return;
  }

  bool quadrangle::has_vertex_0_ext() const
  {
    return _vertex_0_ext_ != 0;
  }

  bool quadrangle::has_vertex_1_ext() const
  {
    return _vertex_1_ext_ != 0;
  }

  bool quadrangle::has_vertex_2_ext() const
  {
    return _vertex_2_ext_ != 0;
  }

  bool quadrangle::has_vertex_3_ext() const
  {
    return _vertex_3_ext_ != 0;
  }

  const vector_3d & quadrangle::get_vertex_0() const
  {
    if (has_vertex_0_ext()) {
      return *_vertex_0_ext_;
    }
    return _vertex_0_;
  }

  const vector_3d & quadrangle::get_vertex_1() const
  {
    if (has_vertex_1_ext()) {
      return *_vertex_1_ext_;
    }
    return _vertex_1_;
  }

  const vector_3d & quadrangle::get_vertex_2() const
  {
    if (has_vertex_2_ext()) {
      return *_vertex_2_ext_;
    }
    return _vertex_2_;
  }

  const vector_3d & quadrangle::get_vertex_3() const
  {
    if (has_vertex_3_ext()) {
      return *_vertex_3_ext_;
    }
    return _vertex_3_;
  }

  const vector_3d & quadrangle::get_vertex(int index_) const
  {
    DT_THROW_IF(index_ < 0 || index_ > 3,
                std::range_error,
                "Invalid vertex index [" << index_ << "]!");
    if (index_ == 0) return get_vertex_0();
    if (index_ == 1) return get_vertex_1();
    if (index_ == 2) return get_vertex_2();
    return get_vertex_3();
  }

  void quadrangle::_set_defaults()
  {
    _vertex_0_ext_ = 0;
    _vertex_1_ext_ = 0;
    _vertex_2_ext_ = 0;
    _vertex_3_ext_ = 0;
    invalidate(_vertex_0_);
    invalidate(_vertex_1_);
    invalidate(_vertex_2_);
    invalidate(_vertex_3_);
    return;
  }

  quadrangle::quadrangle()
  {
    _set_defaults();
    return;
  }

  quadrangle::quadrangle(const vector_3d & p0_,
                         const vector_3d & p1_,
                         const vector_3d & p2_,
                         const vector_3d & p3_)
  {
    _set_defaults();
    set_vertexes(p0_, p1_, p2_, p3_);
    return;
  }

  quadrangle::~quadrangle()
  {
    return;
  }

  void quadrangle::reset()
  {
    _t012_.reset();
    _t023_.reset();
    _set_defaults();
    return;
  }

  const triangle & quadrangle::get_triangle(inner_triangle_index_type i_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    DT_THROW_IF(i_ == IT_NONE, std::logic_error, "Invalid inner triangle index!");
    if (_t012_.get() == 0 || _t023_.get() == 0) {
      quadrangle * mutable_this = const_cast<quadrangle *>(this);
      mutable_this->_t012_.reset(new triangle);
      mutable_this->_t023_.reset(new triangle);
      mutable_this->make_pair_of_triangles(*_t012_, *_t023_);
    }
    if (i_ == IT_FIRST) return *_t012_.get();
    else return *_t023_.get();
  }

  void quadrangle::make_pair_of_triangles(triangle & ta_, triangle & tb_, int mode_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    ta_.reset();
    tb_.reset();
    ta_.set_tolerance(get_tolerance());
    tb_.set_tolerance(get_tolerance());
    ta_.set_angular_tolerance(get_angular_tolerance());
    tb_.set_angular_tolerance(get_angular_tolerance());
    if (mode_ == 0) {
      ta_.set_vertexes_ext(get_vertex_0(), get_vertex_1(), get_vertex_2());
      tb_.set_vertexes_ext(get_vertex_0(), get_vertex_2(), get_vertex_3());
    } else {
      ta_.set_vertexes_ext(get_vertex_1(), get_vertex_2(), get_vertex_3());
      tb_.set_vertexes_ext(get_vertex_1(), get_vertex_3(), get_vertex_0());
    }
    return;
  }

  void quadrangle::compute_proper_frame(vector_3d & u0_, vector_3d & u1_, vector_3d & u2_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    u0_ = get_vertex_1() - get_vertex_0();
    u1_ = get_vertex_2() - get_vertex_0();
    u2_ = u0_.cross(u1_).unit();
    return;
  }

  bool quadrangle::compute_proper_frame_coordinates(const vector_3d & position_,
                                                    double & xp_, double & yp_, double & zp_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
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

  bool quadrangle::is_on_surface(const vector_3d & position_,
                                 double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    double tolerance = compute_tolerance(tolerance_);

    const triangle & t012 = get_triangle(IT_FIRST);
    const triangle & t023 = get_triangle(IT_SECOND);
    if (t012.is_on_surface(position_, tolerance)
        || t023.is_on_surface(position_, tolerance)) {
      return true;
    }
    return false;
  }

  vector_3d quadrangle::get_normal_on_surface(const vector_3d & position_,
                                              bool check_,
                                              double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    vector_3d u0;
    vector_3d u1;
    compute_proper_frame(u0, u1, normal);
    return normal;
  }

  bool quadrangle::find_intercept(const vector_3d & from_,
                                  const vector_3d & direction_,
                                  face_intercept_info & intercept_,
                                  double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    intercept_.reset();
    plane p(get_vertex_0(), get_vertex_1() - get_vertex_0(),  get_vertex_2() - get_vertex_0());
    bool res = p.find_intercept(from_, direction_, intercept_, tolerance_);
    if (res) {
      double xI, yI, zI;
      if (!is_on_surface(intercept_.get_impact(), tolerance_)) {
        intercept_.reset();
      }
    }
    return intercept_.is_ok();
  }


  void quadrangle::tree_dump(std::ostream & out_,
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

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Vertex 2 : " << get_vertex_2();
    if (has_vertex_2_ext()) {
      out_ << " (external)";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Vertex 3 : " << get_vertex_3();
    if (has_vertex_3_ext()) {
      out_ << " (external)";
    }
    out_ << std::endl;

    return;
  }

  void quadrangle::generate_wires_self(wires_type & wires_,
                                       uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadrangle!");
    bool no_first_edge = (options_ & WR_QUADRANGLE_NO_FIRST_EDGE);
    bool no_second_edge = (options_ & WR_QUADRANGLE_NO_SECOND_EDGE);
    bool no_third_edge = (options_ & WR_QUADRANGLE_NO_THIRD_EDGE);
    bool no_fourth_edge = (options_ & WR_QUADRANGLE_NO_FOURTH_EDGE);

    uint32_t base_options = options_ & WR_BASE_MASK;

    bool do_it = false;

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
      segment.push_back(get_vertex_3());
    }

    if (!no_fourth_edge) {
      {
        polyline_type dummy;
        wires_.push_back(dummy);
      }
      polyline_type & segment = wires_.back();
      segment.push_back(get_vertex_3());
      segment.push_back(get_vertex_0());
    }

    uint32_t nsamples_r = 1;
    if (base_options & i_wires_3d_rendering::WR_BASE_GRID) {

      vector_3d u03 = get_vertex_3() - get_vertex_0();
      double l03 = u03.mag();
      vector_3d u12 = get_vertex_2() - get_vertex_1();
      double l12 = u12.mag();
      u03 = u03.unit();
      u12 = u12.unit();
      uint32_t nsamples_03_12  = linear_sampling_from_options(base_options);
      double dl03 = l03 / (nsamples_03_12 - 1);
      double dl12 = l12 / (nsamples_03_12 - 1);
      for (int i = 1; i < nsamples_03_12 - 1; i++) {
        vector_3d p03 =  get_vertex_0() + i * dl03 * u03;
        vector_3d p12 =  get_vertex_1() + i * dl12 * u12;
        line_3d segment(p03, p12);
        segment.generate_wires_self(wires_);
      }

      vector_3d u01 = get_vertex_1() - get_vertex_0();
      double l01 = u01.mag();
      vector_3d u32 = get_vertex_2() - get_vertex_3();
      double l32 = u32.mag();
      u01 = u01.unit();
      u32 = u32.unit();
      uint32_t nsamples_01_32  = linear_sampling_from_options(base_options);
      double dl01 = l01 / (nsamples_01_32 - 1);
      double dl32 = l32 / (nsamples_01_32 - 1);
      for (int i = 1; i < nsamples_01_32 - 1; i++) {
        vector_3d p01 =  get_vertex_0() + i * dl01 * u01;
        vector_3d p32 =  get_vertex_3() + i * dl32 * u32;
        line_3d segment(p01, p32);
        segment.generate_wires_self(wires_);
      }

    }

    return;
  }

} // end of namespace geomtools
