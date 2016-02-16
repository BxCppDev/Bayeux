/// regular_polygon.cc

// Ourselves:
#include <geomtools/regular_polygon.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>


// Third party:
// - GSL:
#include <gsl/gsl_poly.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/utils.h>
#include <geomtools/triangle.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(regular_polygon, "geomtools::regular_polygon");

  const std::string & regular_polygon::regular_polygon_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "regular_polygon";
    }
    return label;
  }

  std::string regular_polygon::get_shape_name() const
  {
    return regular_polygon_label();
  }

  uint32_t regular_polygon::get_n_sides() const
  {
    return _n_sides_;
  }

  void regular_polygon::set_n_sides(uint32_t ns_)
  {
    DT_THROW_IF(ns_ < MIN_NUMBER_OF_SIDES, std::logic_error, "Invalid number of sides [" << ns_ << "]!");
    _n_sides_ = ns_;
    return;
  }

  double regular_polygon::get_r() const
  {
    return _r_;
  }

  double regular_polygon::get_radius() const
  {
    return get_r ();
  }

  void regular_polygon::set_diameter(double new_value_)
  {
    set_r (new_value_ * 0.5);
    return;
  }

  double regular_polygon::get_diameter() const
  {
    return (_r_ + _r_);
  }

  void regular_polygon::set_r(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::logic_error, "Invalid '" << new_value_ << "' R value!");
    _r_ = new_value_;
    return;
  }

  double regular_polygon::get_apothem() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    double a = _r_ * std::cos(M_PI / _n_sides_);
    return a;
  }

  double regular_polygon::get_side_length() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    double sl = 2 * _r_ * std::sin(M_PI / _n_sides_);
    return sl;
  }

  // virtual
  unsigned int regular_polygon::compute_vertexes(vertex_col_type & vertexes_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    vertexes_.clear();
    vertexes_.reserve(_n_sides_);
    for (unsigned int i = 0; i < _n_sides_; i++) {
      double x, y;
      get_vertex(i, x, y);
      vertexes_.push_back(vector_3d(x, y, 0.0));
    }
    return vertexes_.size();
  }

  double regular_polygon::get_perimeter(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    return get_side_length() * _n_sides_;
  }

  double regular_polygon::get_surface(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    return get_apothem() * get_side_length() * _n_sides_ * 0.5;
  }

  bool regular_polygon::is_valid() const
  {
    return _n_sides_ >= MIN_NUMBER_OF_SIDES && datatools::is_valid(_r_);
  }

  void regular_polygon::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("r"), std::logic_error, "Missing regular polygon 'r' property !");
      double r = config_.fetch_real("r");
      if (! config_.has_explicit_unit("r")) {
        r *= lunit;
      }

      DT_THROW_IF(! config_.has_key("n_sides"), std::logic_error, "Missing regular polygon 'n_sides' property !");
      int n_sides = config_.fetch_integer("n_sides");
      DT_THROW_IF(n_sides < 3, std::logic_error, "Invalid number of sides!");

      set_r(r);
      set_n_sides((uint32_t) n_sides);

      // double aunit = CLHEP::degree;
      // if (config_.has_key("angle_unit")) {
      //   const std::string aunit_str = config_.fetch_string("angle_unit");
      //   aunit = datatools::units::get_angle_unit_from(aunit_str);
      // }

      // double start_angle = 0.0;
      // double delta_angle = 2 * M_PI * CLHEP::radian;
      // bool not_full_angle = false;
      // if (config_.has_key ("start_angle")) {
      //   start_angle = config_.fetch_real ("start_angle");
      //   if (! config_.has_explicit_unit ("start_angle")) {
      //     start_angle *= aunit;
      //   }
      //   not_full_angle = true;
      // }
      // if (config_.has_key ("delta_angle")) {
      //   delta_angle = config_.fetch_real ("delta_angle");
      //   if (! config_.has_explicit_unit ("delta_angle")) {
      //     delta_angle *= aunit;
      //   }
      //   not_full_angle = true;
      // }
      // if (not_full_angle) {
      //   set_start_angle(start_angle);
      //   set_delta_angle(delta_angle);
      // }

    }

    return;
  }

  void regular_polygon::reset()
  {
     _n_sides_ = 0;
     datatools::invalidate(_r_);
     this->i_object_3d::reset();
     return;
  }

  regular_polygon::regular_polygon()
  {
    _n_sides_ = 0;
    datatools::invalidate(_r_);
    return;
  }

  regular_polygon::regular_polygon(uint32_t n_sides_,
                                   double r_,
                                   int mode_)
  {
    double r = r_;
    set_n_sides(n_sides_);
    if (mode_ == BUILD_BY_APOTHEM) {
      r = r_ / std::cos(M_PI / n_sides_);
    }
    set_r(r);
    return;
  }

  regular_polygon::~regular_polygon()
  {
    return;
  }

  double regular_polygon::get_reference_angle() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    return 2 * M_PI / _n_sides_;
  }

  void regular_polygon::get_vertex(int i_, double & x_, double & y_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    double alpha = get_reference_angle();
    double theta = i_ * alpha;
    x_ = _r_ * std::cos(theta);
    y_ = _r_ * std::sin(theta);
    return;
  }

  void regular_polygon::get_vertex(int n_, vector_3d & vertex_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    double x, y;
    get_vertex(n_, x, y);
    vertex_.set(x, y, 0.0);
    return;
  }

  bool regular_polygon::is_on_surface(const vector_3d & position_,
                                      double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    double z_tolerance = compute_tolerance(tolerance_);
    vector_3d v0, v1, v2;
    v0.set(0.0, 0.0, 0.0);
    double r = get_radius();
    v1.set(r, 0.0, 0.0);
    double theta = 2 * M_PI / get_n_sides();
    v2.set(r * std::cos(theta), r * std::sin(theta), 0.0);
    triangle t(v0, v1, v2);
    for (size_t i = 0; i < get_n_sides(); i++) {
      placement p(0.0, 0.0, 0.0, AXIS_Z, i * theta);
      vector_3d pos_c;
      p.mother_to_child(position_, pos_c);
      if (t.is_on_surface(pos_c, z_tolerance)) {
        return true;
      }
    }
    return false;
  }

  vector_3d regular_polygon::get_normal_on_surface(const vector_3d & position_,
                                                   bool check_,
                                                   double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    normal.set(0.0, 0.0, 1.0);
    return normal;
  }

  bool regular_polygon::find_intercept(const vector_3d & from_,
                                       const vector_3d & direction_,
                                       face_intercept_info & intercept_,
                                       double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    intercept_.reset();
    double tolerance = compute_tolerance(tolerance_);
    vector_3d v0, v1, v2;
    v0.set(0.0, 0.0, 0.0);
    double r = get_radius();
    v1.set(r, 0.0, 0.0);
    double theta = 2 * M_PI / get_n_sides();
    v2.set(r * std::cos(theta), r * std::sin(theta), 0.0);
    triangle t(v0, v1, v2);
    for (unsigned int ipart = 0; ipart < get_n_sides(); ipart++) {
      placement p(0.0, 0.0, 0.0, AXIS_Z, ipart * theta);
      vector_3d from_c;
      vector_3d direction_c;
      p.mother_to_child(from_, from_c);
      p.mother_to_child_direction(direction_, direction_c);
      face_intercept_info fii;
      fii.reset();
      if (t.find_intercept(from_c, direction_c, fii, tolerance)) {
        vector_3d impact;
        p.child_to_mother(fii.get_impact(), impact);
        // intercept_.grab_face_id().append_part(ipart);
        intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
        intercept_.set_impact(impact);
        break;
      }
    }
    return intercept_.is_ok();
  }

  void regular_polygon::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Number of sides : " << get_n_sides() << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "R : " << get_r() / CLHEP::mm << " mm" << std::endl;

    if (is_valid()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Side length : " << get_side_length() / CLHEP::mm << " mm" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Angle : " << get_reference_angle() / CLHEP::degree << " degree" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Apothem : " << get_apothem() / CLHEP::mm << " mm" << std::endl;
    }
    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Valid : " << is_valid() << std::endl;
    return;
  }

  void regular_polygon::generate_wires_self(wires_type & wires_,
                                            uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid regular polygon!");
    bool no_external_edges = (options_ & WR_RP_NO_EXTERNAL_EDGES);

    uint32_t base_options = options_ & WR_BASE_MASK;

    bool with_grid = (base_options & i_wires_3d_rendering::WR_BASE_GRID);

    {
      vector_3d v0, v1, v2;
      v0.set(0.0, 0.0, 0.0);
      double r = get_radius();
      v1.set(r, 0.0, 0.0);
      double theta = 2 * M_PI / get_n_sides();
      v2.set(r * std::cos(theta), r * std::sin(theta), 0.0);
      triangle t(v0, v1, v2);
      for (size_t i = 0; i < get_n_sides(); i++) {
        placement p(0.0, 0.0, 0.0, AXIS_Z, i * theta);
        uint32_t sector_options = base_options;
        if (!with_grid) {
          sector_options |= geomtools::triangle::WR_TRIANGLE_NO_FIRST_EDGE;
          sector_options |= geomtools::triangle::WR_TRIANGLE_NO_THIRD_EDGE;
        }
        if (no_external_edges) {
          sector_options |= geomtools::triangle::WR_TRIANGLE_NO_SECOND_EDGE;
        }
        t.generate_wires(wires_,
                         p,
                         sector_options);
      }

    }

    return;
  }

} // end of namespace geomtools
