/// composite_surface.cc

// Ourselves:
#include <geomtools/composite_surface.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/utils.h>

namespace geomtools {

  // static
  const std::string & composite_surface::composite_surface_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "composite_surface";
    }
    return label;
  }

  std::string composite_surface::get_shape_name() const
  {
    return composite_surface_label();
  }

  bool composite_surface::is_valid() const
  {
    bool devel = false;
    devel = true;
    if (_surfaces_.size() == 0) {
      if (devel) std::cerr << "DEVEL: composite_surface::is_valid: No surface." << std::endl;
      return false;
    }
    for (unsigned int i = 0; i < _surfaces_.size(); i++) {
      const face_info & fi = _surfaces_[i];
      // if (devel) std::cerr << "DEVEL: composite_surface::is_valid: Fetched face info at index [" << i << "]." << std::endl;
      if (! fi.is_valid()) {
        if (devel) std::cerr << "DEVEL: composite_surface::is_valid: Surface at index [" << i << "] is not valid";
        if (fi.has_face()) {
          if (devel) std::cerr << " (type='" << typeid(fi.get_face_ref()).name() << "')";
        }
        if (devel) std::cerr << std::endl;
        return false;
      }
    }
    return true;
  }

  // virtual
  double composite_surface::get_perimeter(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid composite surface!");
    double p;
    datatools::invalidate(p);
    p = 0.0;
    for (unsigned int  i = 0; i < _surfaces_.size(); i++) {
      const face_info & fi = get(i);
      const i_shape_2d & shape = fi.get_face_ref();
      p += shape.get_perimeter();
    }
    return p;
  }

  // virtual
  double composite_surface::get_surface(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid composite surface!");
    double s;
    datatools::invalidate(s);
    s = 0.0;
    for (unsigned int  i = 0; i < _surfaces_.size(); i++) {
      const face_info & fi = get(i);
      const i_shape_2d & shape = fi.get_face_ref();
      s += shape.get_surface();
    }
    return s;
  }

  composite_surface::composite_surface()
  {
    return;
  }

  composite_surface::composite_surface(double /*tolerance_*/)
  {
    return;
  }

  composite_surface::composite_surface(double /*tolerance_*/, double /*angular_tolerance_*/)
  {
    return;
  }

  // virtual
  composite_surface::~composite_surface()
  {
    return;
  }

  void composite_surface::reset()
  {
    _surfaces_.clear();
    return;
  }

  unsigned int composite_surface::get_number_of_surfaces() const
  {
    return _surfaces_.size();
  }

  const face_info_collection_type &
  composite_surface::get_surfaces() const
  {
    return _surfaces_;
  }

  face_info & composite_surface::add()
  {
    int32_t face_index = _surfaces_.size();
    {
      face_info dummy;
      _surfaces_.push_back(dummy);
    }
    face_info & fi = _surfaces_.back();
    fi.grab_identifier().set_face_index(face_index);
    // fi.grab_positioning().set_identity();
    return fi;
  }

  void composite_surface::remove_last()
  {
    if (_surfaces_.size() > 0) {
      _surfaces_.pop_back();
    }
    return;
  }

  const face_info & composite_surface::get(unsigned int index_) const
  {
    // std::cerr << "DEVEL: composite_surface::get: index=" << index_ << std::endl;
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid composite surface!");
    DT_THROW_IF(index_ >= get_number_of_surfaces(), std::range_error,
                "Invalid embedded surface index [" << index_ << "]!");
    return _surfaces_[index_];
  }

  bool composite_surface::is_on_surface(const vector_3d & position_,
                                        double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid composite surface!");
    double tolerance = compute_tolerance(tolerance_);
    for (unsigned int i = 0; i < _surfaces_.size();
         i++) {
      const face_info & fi = _surfaces_[i];
      const i_shape_2d & shape = fi.get_face_ref();
      const placement & shape_pl = fi.get_positioning();
      vector_3d position_c;
      shape_pl.mother_to_child(position_, position_c);
      if (shape.is_on_surface(position_c, tolerance)) {
        return true;
      }
    }
    return false;
  }

  vector_3d composite_surface::get_normal_on_surface(const vector_3d & position_,
                                                     bool /* check_ */,
                                                     double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid composite surface!");
    vector_3d normal;
    geomtools::invalidate(normal);
    double tolerance = compute_tolerance(tolerance_);
    for (unsigned int i = 0; i < _surfaces_.size(); i++) {
      const face_info & fi = _surfaces_[i];
      const i_shape_2d & shape = fi.get_face_ref();
      const placement & shape_pl = fi.get_positioning();
      vector_3d position_c;
      shape_pl.mother_to_child(position_, position_c);
      if (shape.is_on_surface(position_c, tolerance)) {
        vector_3d normal_c = shape.get_normal_on_surface(position_c, false, tolerance);
        if (geomtools::is_valid(normal_c)) {
          shape_pl.child_to_mother_direction(normal_c, normal);
          break;
        }
      }
    }
    return normal;
  }

  bool composite_surface::find_intercept(const vector_3d & from_,
                                         const vector_3d & direction_,
                                         face_intercept_info & intercept_,
                                         double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid composite surface!");
    intercept_.reset();
    double tolerance = compute_tolerance(tolerance_);
    double l_min = -1.0;
    for (unsigned int i = 0; i < _surfaces_.size(); i++) {
      const face_info & fi = _surfaces_[i];
      const i_shape_2d & shape = fi.get_face_ref();
      const placement & shape_pl = fi.get_positioning();
      vector_3d from_c;
      vector_3d direction_c;
      shape_pl.mother_to_child(from_, from_c);
      shape_pl.mother_to_child_direction(direction_, direction_c);
      face_intercept_info fii;
      if (shape.find_intercept(from_c, direction_c, fii, tolerance)) {
        vector_3d impact;
        shape_pl.child_to_mother(fii.get_impact(), impact);
        double len = (impact - from_).mag();
        if (l_min < 0.0 || len < l_min) {
          l_min = len;
          intercept_.grab_face_id() = fii.get_face_id();
          intercept_.grab_face_id().prepend_part(i);
          intercept_.set_impact(impact);
        }
      }
    }
    return intercept_.is_ok();
  }

  void composite_surface::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Surfaces : ";
    if (_surfaces_.size()) {
      out_ << '[' << _surfaces_.size() << ']';
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Valid : ";
    out_ << is_valid();
    out_ << std::endl;

    return;
  }

  void composite_surface::generate_wires_self(wires_type & wires_,
                                              uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid composite surface!");

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    for (unsigned int i = 0; i < _surfaces_.size(); i++) {
      // std::cerr << "DEVEL: composite_surface::generate_wires_self: "
      //           << "Surface index = " << i
      //           << std::endl;
      // DEVEL
      // if (i != 2 && i != _surfaces_.size() - 1) continue;
      const face_info & fi = _surfaces_[i];
      const i_shape_2d & shape = fi.get_face_ref();
      const placement & shape_pl = fi.get_positioning();
      uint32_t options = base_options;
      // std::cerr << "DEVEL: composite_surface::generate_wires_self: "
      //           << " Nb of wires (before) = " << wires_.size()
      //           << std::endl;
      shape.generate_wires(wires_, shape_pl, options);
      // std::cerr << "DEVEL: composite_surface::generate_wires_self: "
      //           << " Nb of wires (after) = " << wires_.size()
      //           << std::endl;
    }

    return;
  }

} // end of namespace geomtools
