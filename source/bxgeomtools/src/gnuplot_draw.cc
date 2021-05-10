/// \file gnuplot_draw.cc

// Ourselves:
#include <geomtools/gnuplot_draw.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <algorithm>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>

// This project:
#include <geomtools/i_placement.h>
#include <geomtools/i_object_3d.h>
#include <geomtools/placement.h>
#include <geomtools/color.h>
#include <geomtools/i_wires_3d_rendering.h>

// Curves:
#include <geomtools/line_3d.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/circle.h>
#include <geomtools/ellipse.h>
#include <geomtools/helix_3d.h>

// Surfaces:
#include <geomtools/triangle.h>
#include <geomtools/quadrangle.h>
#include <geomtools/rectangle.h>
#include <geomtools/regular_polygon.h>
#include <geomtools/simple_polygon.h>
#include <geomtools/disk.h>
#include <geomtools/elliptical_sector.h>
#include <geomtools/spherical_sector.h>
#include <geomtools/right_circular_conical_nappe.h>
#include <geomtools/ellipsoid_sector.h>
#include <geomtools/cylindrical_sector.h>
#include <geomtools/elliptical_cylinder_sector.h>
#include <geomtools/composite_surface.h>
#include <geomtools/toroid_nappe.h>

// Volumes/Solids:
#include <geomtools/box.h>
#include <geomtools/extruded_box.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/torus.h>
#include <geomtools/sphere.h>
#include <geomtools/ellipsoid.h>
#include <geomtools/elliptical_cylinder.h>
#include <geomtools/right_circular_conical_frustrum.h>
#include <geomtools/right_polygonal_frustrum.h>
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>
#include <geomtools/wall_solid.h>
#include <geomtools/tessellation.h>
#include <geomtools/union_3d.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/display_data.h>

// #include <geomtools/cone.h>
// #include <geomtools/i_wires_drawer.h>

namespace geomtools {

  using namespace std;

  // static
  const uint32_t gnuplot_draw::DEFAULT_OPTIONS;

  gnuplot_draw::xyz_range::xyz_range()
  {
    _activated_ = false;
    return;
  }

  bool gnuplot_draw::xyz_range::is_activated() const
  {
    return _activated_;
  }

  void gnuplot_draw::xyz_range::deactivate()
  {
    _activated_ = false;
    return;
  }

  void gnuplot_draw::xyz_range::activate()
  {
    _activated_ = true;
    return;
  }

  const mygsl::min_max & gnuplot_draw::xyz_range::get_x_range() const
  {
    return _x_range_;
  }

  const mygsl::min_max & gnuplot_draw::xyz_range::get_y_range() const
  {
    return _y_range_;
  }

  const mygsl::min_max & gnuplot_draw::xyz_range::get_z_range() const
  {
    return _z_range_;
  }

  void gnuplot_draw::xyz_range::reset_ranges()
  {
    _x_range_.reset();
    _y_range_.reset();
    _z_range_.reset();
    return;
  }

  void gnuplot_draw::xyz_range::add_point(double x_, double y_, double z_)
  {
    if (is_activated()) {
      this->_x_range_.add(x_);
      this->_y_range_.add(y_);
      this->_z_range_.add(z_);
    }
    return;
  }

  void gnuplot_draw::xyz_range::add_point(const vector_3d & point_)
  {
    this->add_point(point_.x(), point_.y(), point_.z());
    return;
  }

  // static
  gnuplot_draw::xyz_range & gnuplot_draw::bounding_box(gnuplot_draw::bounding_box_action_type action_)
  {
    // std::cerr << "DEVEL: gnuplot_draw::bounding_box: Entering..." << std::endl;
    static boost::scoped_ptr<gnuplot_draw::xyz_range> _xyz_instance;
    if (_xyz_instance.get() == nullptr ) {
      // std::cerr << "DEVEL: gnuplot_draw::bounding_box: Allocating a new BB..." << std::endl;
      _xyz_instance.reset(new gnuplot_draw::xyz_range);
    }
    if (action_ == BB_ACTION_ACTIVATE) {
      _xyz_instance->activate();
    } else if (action_ == BB_ACTION_DEACTIVATE){
      _xyz_instance->deactivate();
    } else if (action_ == BB_ACTION_RESET){
      _xyz_instance->reset_ranges();
    }
    // std::cerr << "DEVEL: gnuplot_draw::bounding_box: Exiting." << std::endl;
    return *_xyz_instance;
  }

  // static
  const gnuplot_draw::xyz_range & gnuplot_draw::bounding_box_const()
  {
    return const_cast<xyz_range &>(bounding_box(BB_ACTION_NONE));
  }

  // static
  color::context & gnuplot_draw::color_context()
  {
    // std::cerr << "DEVEL: gnuplot_draw::color_context: Entering..." << std::endl;
    static boost::scoped_ptr<color::context> _cc;
    if (!_cc) {
      _cc.reset(new color::context);
    }
    // std::cerr << "DEVEL: gnuplot_draw::color_context: Exiting." << std::endl;
    return *_cc;
  }

  // static
  const color::context & gnuplot_draw::color_context_const()
  {
    // std::cerr << "DEVEL: gnuplot_draw::color_context_const: Entering..." << std::endl;
    color::context & cc = color_context();
    // std::cerr << "DEVEL: gnuplot_draw::color_context_const: Exiting." << std::endl;
    return const_cast<color::context &>(cc);
  }

  void
  gnuplot_draw::basic_draw_point_with_color(std::ostream & out_,
                                            double x_,
                                            double y_,
                                            double z_,
                                            double color_,
                                            bool   new_line_)
  {
    double color = color_;
    out_.precision(15);
    // Record the point in the display bounding box(if activated):
    xyz_range & XYZ = gnuplot_draw::bounding_box(gnuplot_draw::gnuplot_draw::BB_ACTION_NONE);
    XYZ.add_point(x_, y_, z_);
    out_ << x_ << ' '
         << y_ << ' '
         << z_ << ' '
         << color;
    if (new_line_) out_ << std::endl;
    else out_ << ' ';
    return;
  }

  void
  gnuplot_draw::basic_draw_point_with_color(std::ostream & out_,
                                            const vector_3d & point_,
                                            double color_,
                                            bool   new_line_)
  {
    basic_draw_point_with_color(out_,
                                point_.x(),
                                point_.y(),
                                point_.z(),
                                color_,
                                new_line_);
    return;
  }

  void
  gnuplot_draw::basic_draw_point(std::ostream & out_,
                                 double x_, double y_, double z_,
                                 bool new_line_)
  {
    out_.precision(15);
    // Record the point in the bounding box(if activated):
    xyz_range & XYZ = gnuplot_draw::bounding_box(gnuplot_draw::gnuplot_draw::BB_ACTION_NONE);
    XYZ.add_point(x_, y_, z_);
    // Print rendering data for this point:
    out_ << x_ << ' '
         << y_ << ' '
         << z_;
    // Print color information if color context is activated:
    if (color_context_const().is_activated()) {
      out_ << ' ' << color_context_const().str();
    }
    if (new_line_) out_ << std::endl;
    else out_ << ' ';
    return;
  }

  void
  gnuplot_draw::basic_draw_point(std::ostream & out_,
                                 const vector_3d & point_,
                                 bool new_line_)
  {
    gnuplot_draw::basic_draw_point(out_,
                                   point_.x(),
                                   point_.y(),
                                   point_.z(),
                                   new_line_);
    return;
  }

  void
  gnuplot_draw::basic_draw_segment(std::ostream & out_,
                                   const vector_3d & start_ ,
                                   const vector_3d & stop_,
                                   bool gp_trick_)
  {
    basic_draw_point(out_, start_);
    if (gp_trick_) {
      basic_draw_point(out_, 0.5 *(start_ + stop_));
    }
    basic_draw_point(out_, stop_);
    out_ << std::endl;
    return;
  }

  void
  gnuplot_draw::basic_draw_segment(std::ostream & out_,
                                   const segment_type & seg_,
                                   bool gp_trick_)
  {
    basic_draw_segment(out_, seg_.first, seg_.second, gp_trick_);
    return;
  }

  void
  gnuplot_draw::basic_draw_facet3(std::ostream & out_,
                                  const vector_3d & p1_,
                                  const vector_3d & p2_,
                                  const vector_3d & p3_,
                                  double color_)
  {
    basic_draw_point_with_color(out_, p1_, color_, true);
    basic_draw_point_with_color(out_, p2_, color_, true);
    out_ << std::endl;
    basic_draw_point_with_color(out_, p3_, color_, true);
    basic_draw_point_with_color(out_, p3_, color_, true);
    out_ << std::endl;
    out_ << std::endl;
    return;
  }

  void
  gnuplot_draw::basic_draw_facet4(std::ostream & out_,
                                  const vector_3d & p1_,
                                  const vector_3d & p2_,
                                  const vector_3d & p3_,
                                  const vector_3d & p4_,
                                  double color_)
  {
    basic_draw_point_with_color(out_, p1_, color_);
    basic_draw_point_with_color(out_, p2_, color_);
    out_ << std::endl;
    basic_draw_point_with_color(out_, p4_, color_);
    basic_draw_point_with_color(out_, p3_, color_);
    out_ << std::endl;
    out_ << std::endl;
    return;
  }

  bool
  gnuplot_draw::basic_draw_polyline(std::ostream & out_,
                                    const polyline_type & wire_,
                                    bool new_line_,
                                    bool gnuplot_trick_)
  {
    if (wire_.size() < 2) {
      return false;
    }
    bool gp_trick_done = false;
    polyline_type::const_iterator i = wire_.begin();
    if (gnuplot_trick_) {
      if (wire_.size() > 1) {
        const vector_3d & first = *i++;
        const vector_3d & second  =*i;
        vector_3d mid1 = 0.5 * (first + second);
        vector_3d mid2 = 0.5 * (mid1 + second);
        basic_draw_point(out_, first);
        basic_draw_point(out_, mid1);
        out_ << std::endl;
        basic_draw_point(out_, mid1);
        basic_draw_point(out_, mid2);
        basic_draw_point(out_, second);
        out_ << std::endl;
        gp_trick_done = true;
      }
    }

    int count = 0;
    for (;
         i != wire_.end();
         ++i) {
      basic_draw_point(out_, *i);
      count++;
    }
    if (new_line_) out_ << std::endl;
    return gp_trick_done;
  }

  void
  gnuplot_draw::basic_draw_wires(std::ostream & out_,
                                 const wires_type & wires_)
  {
    bool gp_trick_done = false;
    for (wires_type::const_iterator iwire = wires_.begin();
         iwire != wires_.end();
         iwire++) {
      const polyline_type & wire = *iwire;
      bool gpt = basic_draw_polyline(out_, wire, true, !gp_trick_done);
      if (gpt) {
        gp_trick_done = true;
      }
    }
    return;
  }

  void
  gnuplot_draw::basic_draw_polylines(std::ostream & out_,
                                     const wires_type & wires_)
  {
    basic_draw_wires(out_, wires_);
    return;
  }

  void
  gnuplot_draw::draw_wires(std::ostream & out_,
                           const wires_type & wires_)
  {
    basic_draw_wires(out_, wires_);
    return;
  }

  void
  gnuplot_draw::draw_wires(std::ostream & out_,
                           const placement & positioning_,
                           const wires_type & wires_)
  {
    wires_type tr_wires;
    transform_wires_to(positioning_, wires_, tr_wires);
    basic_draw_wires(out_, tr_wires);
    return;
  }

  void
  gnuplot_draw::draw_wires(std::ostream & out_,
                           const vector_3d & pos_,
                           const rotation_3d & rot_,
                           const wires_type & wires_)
  {
    draw_wires(out_, placement(pos_, rot_), wires_);
    return;
  }

  // Vertex:

  void
  gnuplot_draw::draw_vertex(std::ostream & out_,
                            const vector_3d & vertex_,
                            uint32_t /* options_ */)
  {
    basic_draw_point(out_, vertex_, true);
    out_ << std::endl;
    return;
  }

  void
  gnuplot_draw::draw_vertex(std::ostream & out_,
                            double x_, double y_, double z_,
                            uint32_t options_)
  {
    draw_vertex(out_, vector_3d(x_, y_, z_), options_);
    return;
  }

  void
  gnuplot_draw::draw_vertex(std::ostream & out_,
                            const placement & placement_,
                            const vector_3d & vertex_,
                            uint32_t options_)
  {
    vector_3d tr_vtx;
    placement_.child_to_mother(vertex_, tr_vtx);
    draw_vertex(out_, tr_vtx, options_);
    return;
  }

  void
  gnuplot_draw::draw_vertex(std::ostream & out_,
                            const placement & placement_,
                            double x_, double y_, double z_,
                            uint32_t options_)
  {
    draw_vertex(out_, placement_, vector_3d(x_, y_, z_), options_);
    return;
  }

  // Line:

  void
  gnuplot_draw::draw_line(std::ostream & out_,
                          const line_3d & line_,
                          uint32_t options_)
  {
    wires_type wires;
    line_.generate_wires_self(wires, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_line(std::ostream & out_,
                          const vector_3d & start_ ,
                          const vector_3d & stop_,
                          uint32_t options_)
  {
    line_3d line(start_, stop_);
    draw_line(out_, line, options_);
    return;
  }

  void
  gnuplot_draw::draw_line(std::ostream & out_,
                          const placement & p_,
                          const line_3d & line_,
                          uint32_t options_)
  {
    wires_type wires;
    line_.generate_wires(wires, p_, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_line(std::ostream & out_,
                          const vector_3d & position_,
                          const rotation_3d & rotation_,
                          const line_3d & line_,
                          uint32_t options_)
  {
    wires_type wires;
    line_.generate_wires(wires, position_, rotation_, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  // Helix:

  void
  gnuplot_draw::draw_helix(std::ostream & out_,
                           const helix_3d & h_,
                           uint32_t options_)
  {
    wires_type wires;
    h_.generate_wires_self(wires, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_helix(std::ostream & out_,
                           const placement & p_,
                           const helix_3d & h_,
                           uint32_t options_)
  {
    wires_type wires;
    uint32_t options = options_;
    h_.generate_wires(wires, p_, options);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_helix(std::ostream & out_,
                           const vector_3d & position_,
                           const rotation_3d & rotation_,
                           const helix_3d & h_,
                           uint32_t options_)
  {
    wires_type wires;
    h_.generate_wires(wires, position_, rotation_, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  // Polyline:

  void
  gnuplot_draw::draw_polyline(std::ostream & out_,
                              const polyline_3d & poly_,
                              uint32_t options_)
  {
    wires_type wires;
    poly_.generate_wires_self(wires, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_polyline(std::ostream & out_,
                              const placement & p_,
                              const polyline_3d & poly_,
                              uint32_t options_)
  {
    wires_type wires;
    poly_.generate_wires(wires, p_, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_polyline(std::ostream & out_,
                              const vector_3d & position_,
                              const rotation_3d & rotation_,
                              const polyline_3d & poly_,
                              uint32_t options_)
  {
    wires_type wires;
    poly_.generate_wires(wires, position_, rotation_, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_polylines(std::ostream & out_,
                               const std::list<polyline_3d> & pls_,
                               uint32_t options_)
  {
    for (std::list<polyline_3d>::const_iterator i = pls_.begin();
         i != pls_.end();
         i++) {
      const polyline_3d & poly = *i;
      draw_polyline(out_, poly, options_);
    }
    return;
  }

  void
  gnuplot_draw::draw_polylines(std::ostream & out_,
                               const placement & p_,
                               const std::list<polyline_3d> & pls_,
                               uint32_t options_)
  {
    for (std::list<polyline_3d>::const_iterator i = pls_.begin();
         i != pls_.end();
         i++) {
      const polyline_3d & poly = *i;
      draw_polyline(out_, p_, poly, options_);
    }
    return;
  }

  void
  gnuplot_draw::draw_polylines(std::ostream & out_,
                               const vector_3d & position_,
                               const rotation_3d & rotation_,
                               const std::list<polyline_3d> & pls_,
                               uint32_t options_)
  {
    for (std::list<polyline_3d>::const_iterator i = pls_.begin();
         i != pls_.end();
         i++) {
      const polyline_3d & poly = *i;
      draw_polyline(out_, position_, rotation_, poly, options_);
    }
    return;
  }

  // Rectangle:
  void
  gnuplot_draw::draw_rectangle(std::ostream & out_,
                               const rectangle & r_,
                               uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_rectangle(std::ostream & out_,
                               const placement & p_,
                               const rectangle & r_,
                               uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_rectangle(std::ostream & out_,
                               const vector_3d & position_,
                               const rotation_3d & rotation_,
                               const rectangle & r_,
                               uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }


  // Cylindrical sector:

  void
  gnuplot_draw::draw_cylindrical_sector(std::ostream & out_,
                                        const cylindrical_sector & r_,
                                        uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_cylindrical_sector(std::ostream & out_,
                                        const placement & p_,
                                        const cylindrical_sector & r_,
                                        uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_cylindrical_sector(std::ostream & out_,
                                        const vector_3d & position_,
                                        const rotation_3d & rotation_,
                                        const cylindrical_sector & r_,
                                        uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Ellipsoid sector:

  void
  gnuplot_draw::draw_ellipsoid_sector(std::ostream & out_,
                                      const ellipsoid_sector & r_,
                                      uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_ellipsoid_sector(std::ostream & out_,
                                      const placement & p_,
                                      const ellipsoid_sector & r_,
                                      uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_ellipsoid_sector(std::ostream & out_,
                                      const vector_3d & position_,
                                      const rotation_3d & rotation_,
                                      const ellipsoid_sector & r_,
                                      uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Elliptical tube sector:

  void
  gnuplot_draw::draw_elliptical_cylinder_sector(std::ostream & out_,
                                                const elliptical_cylinder_sector & r_,
                                                uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_elliptical_cylinder_sector(std::ostream & out_,
                                                const placement & p_,
                                                const elliptical_cylinder_sector & r_,
                                                uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_elliptical_cylinder_sector(std::ostream & out_,
                                                const vector_3d & position_,
                                                const rotation_3d & rotation_,
                                                const elliptical_cylinder_sector & r_,
                                                uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Elliptical sector:

  void
  gnuplot_draw::draw_elliptical_sector(std::ostream & out_,
                                       const elliptical_sector & r_,
                                       uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_elliptical_sector(std::ostream & out_,
                                       const placement & p_,
                                       const elliptical_sector & r_,
                                       uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_elliptical_sector(std::ostream & out_,
                                       const vector_3d & position_,
                                       const rotation_3d & rotation_,
                                       const elliptical_sector & r_,
                                       uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Spherical sector:

  void
  gnuplot_draw::draw_spherical_sector(std::ostream & out_,
                                      const spherical_sector & r_,
                                      uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_spherical_sector(std::ostream & out_,
                                      const placement & p_,
                                      const spherical_sector & r_,
                                      uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_spherical_sector(std::ostream & out_,
                                      const vector_3d & position_,
                                      const rotation_3d & rotation_,
                                      const spherical_sector & r_,
                                      uint32_t options_)
  {
    wires_type wires;
    r_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Spherical sector:

  void
  gnuplot_draw::draw_toroid_nappe(std::ostream & out_,
                                  const toroid_nappe & tn_,
                                  uint32_t options_)
  {
    wires_type wires;
    tn_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_toroid_nappe(std::ostream & out_,
                                  const placement & p_,
                                  const toroid_nappe & tn_,
                                  uint32_t options_)
  {
    wires_type wires;
    tn_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_toroid_nappe(std::ostream & out_,
                                  const vector_3d & position_,
                                  const rotation_3d & rotation_,
                                  const toroid_nappe & tn_,
                                  uint32_t options_)
  {
    wires_type wires;
    tn_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Triangle:

  void
  gnuplot_draw::draw_triangle(std::ostream & out_,
                              const triangle & t_,
                              uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_triangle(std::ostream & out_,
                              const placement & pl_,
                              const triangle & t_,
                              uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_triangle(std::ostream & out_,
                              const vector_3d & position_,
                              const rotation_3d & rotation_,
                              const triangle & t_,
                              uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Quadrangle:

  void
  gnuplot_draw::draw_quadrangle(std::ostream & out_,
                                const quadrangle & q_,
                                uint32_t options_)
  {
    wires_type wires;
    q_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_quadrangle(std::ostream & out_,
                                const placement & pl_,
                                const quadrangle & q_,
                                uint32_t options_)
  {
    wires_type wires;
    q_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_quadrangle(std::ostream & out_,
                                const vector_3d & position_,
                                const rotation_3d & rotation_,
                                const quadrangle & q_,
                                uint32_t options_)
  {
    wires_type wires;
    q_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Composite surface:

  void
  gnuplot_draw::draw_composite_surface(std::ostream & out_,
                                       const composite_surface & cs_,
                                       uint32_t options_)
  {
    wires_type wires;
    cs_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_composite_surface(std::ostream & out_,
                                       const placement & pl_,
                                       const composite_surface & cs_,
                                       uint32_t options_)
  {
    wires_type wires;
    cs_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_composite_surface(std::ostream & out_,
                                       const vector_3d & position_,
                                       const rotation_3d & rotation_,
                                       const composite_surface & cs_,
                                       uint32_t options_)
  {
    wires_type wires;
    cs_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Box:

  void
  gnuplot_draw::draw_box(std::ostream & out_,
                         const box & b_,
                         uint32_t options_)
  {
    wires_type wires;
    b_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_box(std::ostream & out_,
                         const placement & pl_,
                         const box & b_,
                         uint32_t options_)
  {
    wires_type wires;
    b_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_box(std::ostream & out_,
                         const vector_3d & pos_,
                         const rotation_3d & rot_,
                         const box & b_,
                         uint32_t options_)
  {
    wires_type wires;
    b_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_box(std::ostream & out_,
                         const vector_3d & pos_,
                         const rotation_3d & rot_,
                         double x_, double y_, double z_,
                         uint32_t options_)
  {
    box b(x_, y_, z_);
    wires_type wires;
    b.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Extruded box:

  void
  gnuplot_draw::draw_extruded_box(std::ostream & out_,
                                  const extruded_box & eb_,
                                  uint32_t options_)
  {
    wires_type wires;
    eb_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_extruded_box(std::ostream & out_,
                                  const placement & p_,
                                  const extruded_box & eb_,
                                  uint32_t options_)
  {
    wires_type wires;
    eb_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_extruded_box(std::ostream & out_,
                                  const vector_3d & pos_,
                                  const rotation_3d & rot_,
                                  const extruded_box & eb_,
                                  uint32_t options_)
  {
    wires_type wires;
    eb_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_extruded_box(std::ostream & out_,
                                  const vector_3d & pos_,
                                  const rotation_3d & rot_,
                                  double length_,
                                  double width_,
                                  double height_,
                                  double thickness_,
                                  bool has_top_, bool has_bottom_,
                                  uint32_t options_)
  {
    extruded_box eb;
    eb.set_x(length_);
    eb.set_y(width_);
    eb.set_z(height_);
    eb.set_thickness(thickness_);
    eb.set_top(has_top_);
    eb.set_bottom(has_bottom_);
    wires_type wires;
    eb.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Cylinder:

  void
  gnuplot_draw::draw_cylinder(std::ostream & out_,
                              const cylinder & c_,
                              uint32_t options_)
  {
    wires_type wires;
    c_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_cylinder(std::ostream & out_,
                              const placement & pl_,
                              const cylinder & c_,
                              uint32_t options_)
  {
    wires_type wires;
    c_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_cylinder(std::ostream & out_,
                              const vector_3d & pos_,
                              const rotation_3d & rot_,
                              const cylinder & c_,
                              uint32_t options_)
  {
    wires_type wires;
    c_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_cylinder(std::ostream & out_,
                              const vector_3d & pos_,
                              const rotation_3d & rot_,
                              double radius_,
                              double height_,
                              uint32_t options_)
  {
    cylinder c;
    c.set_r(radius_);
    c.set_z(height_);
    draw_cylinder(out_, pos_, rot_, c, options_);
    return;
  }

  // Tube:

  void
  gnuplot_draw::draw_tube(std::ostream & out_,
                          const tube & t_,
                          uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_tube(std::ostream & out_,
                          const placement & pl_,
                          const tube & t_,
                          uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_tube(std::ostream & out_,
                          const vector_3d & pos_,
                          const rotation_3d & rot_,
                          const tube & t_,
                          uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_tube(std::ostream & out_,
                          const vector_3d & pos_,
                          const rotation_3d & rot_,
                          double inner_radius_,
                          double outer_radius_,
                          double start_phi_,
                          double delta_phi_,
                          double height_,
                          uint32_t options_)
  {
    tube t;
    t.set(inner_radius_, outer_radius_, height_, start_phi_, delta_phi_);
    draw_tube(out_, pos_, rot_, t, options_);
    return;
  }

  // Torus:

  void
  gnuplot_draw::draw_torus(std::ostream & out_,
                          const torus & t_,
                          uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_torus(std::ostream & out_,
                          const placement & pl_,
                          const torus & t_,
                          uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_torus(std::ostream & out_,
                          const vector_3d & pos_,
                          const rotation_3d & rot_,
                          const torus & t_,
                          uint32_t options_)
  {
    wires_type wires;
    t_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_torus(std::ostream & out_,
                          const vector_3d & pos_,
                          const rotation_3d & rot_,
                          double sweep_radius_,
                          double inner_radius_,
                          double outer_radius_,
                          double start_phi_,
                          double delta_phi_,
                          uint32_t options_)
  {
    torus t;
    t.set(sweep_radius_, inner_radius_, outer_radius_, start_phi_, delta_phi_);
    draw_torus(out_, pos_, rot_, t, options_);
    return;
  }

  // Elliptical cylinder/tube:

  // static
  void
  gnuplot_draw::draw_elliptical_cylinder(std::ostream & out_,
                                         const elliptical_cylinder & ec_,
                                         uint32_t options_)
  {
    wires_type wires;
    ec_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // static
  void
  gnuplot_draw::draw_elliptical_cylinder(std::ostream & out_,
                                         const placement & pl_,
                                         const elliptical_cylinder & ec_,
                                         uint32_t options_)
  {
    wires_type wires;
    ec_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_elliptical_cylinder(std::ostream & out_,
                                         const vector_3d & position_,
                                         const rotation_3d & rotation_,
                                         const elliptical_cylinder & ec_,
                                         uint32_t options_)
  {
    wires_type wires;
    ec_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Circle:

  void
  gnuplot_draw::draw_circle(std::ostream & out_,
                            const circle & c_,
                            uint32_t options_)
  {
    wires_type wires;
    c_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_circle(std::ostream & out_,
                            const placement & p_,
                            const circle & c_,
                            uint32_t options_)
  {
    wires_type wires;
    c_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_circle(std::ostream & out_,
                            const vector_3d & pos_,
                            const rotation_3d & rot_,
                            const circle & c_,
                            uint32_t options_)
  {
    wires_type wires;
    c_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_circle(std::ostream & out_,
                            const vector_3d & position_,
                            const rotation_3d & rotation_,
                            double radius_,
                            uint32_t options_)
  {
    circle c(radius_);
    draw_circle(out_, position_, rotation_, c, options_);
    return;
  }

  // Disk:

  void gnuplot_draw::draw_disk(std::ostream & out_,
                               const disk & d_,
                               uint32_t options_)
  {
    wires_type wires;
    d_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void gnuplot_draw::draw_disk(std::ostream & out_,
                               const placement & pl_,
                               const disk & d_,
                               uint32_t options_)
  {
    wires_type wires;
    d_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void gnuplot_draw::draw_disk(std::ostream & out_,
                               const vector_3d & pos_,
                               const rotation_3d & rot_,
                               const disk & d_,
                               uint32_t options_)
  {
    wires_type wires;
    d_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_disk(std::ostream & out_,
                          const vector_3d & pos_,
                          const rotation_3d & rot_,
                          double inner_radius_,
                          double outer_radius_,
                          double start_angle_,
                          double delta_angle_,
                          uint32_t options_)
  {
    disk d(inner_radius_, outer_radius_, start_angle_, delta_angle_);
    draw_disk(out_, pos_, rot_, d, options_);
    return;
  }

  void
  gnuplot_draw::draw_disk(std::ostream & out_,
                          const vector_3d & pos_,
                          const rotation_3d & rot_,
                          double radius_,
                          uint32_t options_)
  {
    disk d(radius_);
    draw_disk(out_, pos_, rot_, d, options_);
    return;
  }

  // Ellipse:

  void
  gnuplot_draw::draw_ellipse(std::ostream & out_,
                             const ellipse & e_,
                             uint32_t options_)
  {
    wires_type wires;
    e_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_ellipse(std::ostream & out_,
                             const placement & p_,
                             const ellipse & e_,
                             uint32_t options_)
  {
    wires_type wires;
    e_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_ellipse(std::ostream & out_,
                             const vector_3d & pos_,
                             const rotation_3d & rot_,
                             const ellipse & e_,
                             uint32_t options_)
  {
    wires_type wires;
    e_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Regular polygon:

  void
  gnuplot_draw::draw_regular_polygon(std::ostream & out_,
                                     const regular_polygon & rp_,
                                     uint32_t options_)
  {
    wires_type wires;
    rp_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_regular_polygon(std::ostream & out_,
                                     const placement & p_,
                                     const regular_polygon & rp_,
                                     uint32_t options_)
  {
    wires_type wires;
    rp_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_regular_polygon(std::ostream & out_,
                                     const vector_3d & pos_,
                                     const rotation_3d & rot_,
                                     const regular_polygon & rp_,
                                     uint32_t options_)
  {
    wires_type wires;
    rp_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Simple polygon:

  void
  gnuplot_draw::draw_simple_polygon(std::ostream & out_,
                                     const simple_polygon & rp_,
                                     uint32_t options_)
  {
    wires_type wires;
    rp_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_simple_polygon(std::ostream & out_,
                                     const placement & p_,
                                     const simple_polygon & rp_,
                                     uint32_t options_)
  {
    wires_type wires;
    rp_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_simple_polygon(std::ostream & out_,
                                     const vector_3d & pos_,
                                     const rotation_3d & rot_,
                                     const simple_polygon & rp_,
                                     uint32_t options_)
  {
    wires_type wires;
    rp_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Sphere:

  void
  gnuplot_draw::draw_sphere(std::ostream & out_,
                            const sphere & s_,
                            uint32_t options_)
  {
    wires_type wires;
    s_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
  }

  void
  gnuplot_draw::draw_sphere(std::ostream & out_,
                            const placement & p_,
                            const sphere & s_,
                            uint32_t options_)
  {
    wires_type wires;
    s_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
  }

  void
  gnuplot_draw::draw_sphere(std::ostream & out_,
                            const vector_3d & pos_,
                            const rotation_3d & rot_,
                            const sphere & s_,
                            uint32_t options_)
  {
    wires_type wires;
    s_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
  }

  void
  gnuplot_draw::draw_sphere(std::ostream & out_,
                            const vector_3d & pos_,
                            const rotation_3d & rot_,
                            double r_min_,
                            double r_max_,
                            double phi_min_,
                            double delta_phi_,
                            double theta_min_,
                            double delta_theta_,
                            uint32_t options_)
  {
    sphere s;
    s.set(r_min_, r_max_);
    s.set_phi(phi_min_, delta_phi_);
    s.set_theta(theta_min_, delta_theta_);
    draw_sphere(out_, pos_, rot_, s, options_);
    return;
  }

  void
  gnuplot_draw::draw_sphere(std::ostream & out_,
                            const vector_3d & pos_,
                            const rotation_3d & rot_,
                            double radius_,
                            uint32_t options_)
  {
    sphere s;
    s.set_r(radius_);
    draw_sphere(out_, pos_, rot_, s, options_);
    return;
  }

  // Ellipsoid:

  void
  gnuplot_draw::draw_ellipsoid(std::ostream & out_,
                               const placement & pl_,
                               const ellipsoid & e_,
                               uint32_t options_)
  {
    wires_type wires;
    e_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
  }

  void
  gnuplot_draw::draw_ellipsoid(std::ostream & out_,
                               const vector_3d & pos_,
                               const rotation_3d & rot_,
                               const ellipsoid & e_,
                               uint32_t options_)
  {
    wires_type wires;
    e_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
  }

  void
  gnuplot_draw::draw_ellipsoid(std::ostream & out_,
                               const vector_3d & pos_,
                               const rotation_3d & rot_,
                               double x_radius_,
                               double y_radius_,
                               double z_radius_,
                               double bottom_z_cut_,
                               double top_z_cut_,
                               uint32_t options_)
  {
    ellipsoid e;
    e.set(x_radius_, y_radius_, z_radius_, bottom_z_cut_, top_z_cut_);
    draw_ellipsoid(out_, pos_, rot_, e, options_);
    return;
  }

  // Conical nappe:

  void
  gnuplot_draw::draw_right_circular_conical_nappe(std::ostream & out_,
                                                  const right_circular_conical_nappe & rccn_,
                                                  uint32_t options_)
  {
    wires_type wires;
    rccn_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_right_circular_conical_nappe(std::ostream & out_,
                                                  const placement & p_,
                                                  const right_circular_conical_nappe & rccn_,
                                                  uint32_t options_)
  {
    wires_type wires;
    rccn_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_right_circular_conical_nappe(std::ostream & out_,
                                                  const vector_3d & pos_,
                                                  const rotation_3d & rot_,
                                                  const right_circular_conical_nappe & rccn_,
                                                  uint32_t options_)
  {
    wires_type wires;
    rccn_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Conical frustrum:

  void
  gnuplot_draw::draw_right_circular_conical_frustrum(std::ostream & out_,
                                                     const right_circular_conical_frustrum & rccf_,
                                                     uint32_t options_)
  {
    wires_type wires;
    rccf_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_right_circular_conical_frustrum(std::ostream & out_,
                                                     const placement & p_,
                                                     const right_circular_conical_frustrum & rccf_,
                                                     uint32_t options_)
  {
    wires_type wires;
    rccf_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_right_circular_conical_frustrum(std::ostream & out_,
                                                     const vector_3d & pos_,
                                                     const rotation_3d & rot_,
                                                     const right_circular_conical_frustrum & rccf_,
                                                     uint32_t options_)
  {
    wires_type wires;
    rccf_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_right_circular_conical_frustrum(std::ostream & out_,
                                                     const vector_3d & position_,
                                                     const rotation_3d & rotation_,
                                                     double rmin1_, double rmax1_,
                                                     double rmin2_, double rmax2_,
                                                     double z_,
                                                     double phi1_, double phi2_,
                                                     uint32_t options_)
  {
    right_circular_conical_frustrum rccf(rmin1_, rmax1_, rmin2_, rmax2_,
                                         z_,
                                         phi1_, phi2_ - phi1_);
    draw_right_circular_conical_frustrum(out_, position_, rotation_, rccf, options_);
    return;
  }

  // Polygonal frustrum:

  void
  gnuplot_draw::draw_right_polygonal_frustrum(std::ostream & out_,
                                              const right_polygonal_frustrum & rccf_,
                                              uint32_t options_)
  {
    wires_type wires;
    rccf_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_right_polygonal_frustrum(std::ostream & out_,
                                              const placement & p_,
                                              const right_polygonal_frustrum & rccf_,
                                              uint32_t options_)
  {
    wires_type wires;
    rccf_.generate_wires(wires, p_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_right_polygonal_frustrum(std::ostream & out_,
                                              const vector_3d & pos_,
                                              const rotation_3d & rot_,
                                              const right_polygonal_frustrum & rccf_,
                                              uint32_t options_)
  {
    wires_type wires;
    rccf_.generate_wires(wires, pos_, rot_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Polycone:

  void
  gnuplot_draw::draw_polycone(std::ostream & out_,
                              const polycone & p_,
                              uint32_t options_)
  {
    wires_type wires;
    p_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_polycone(std::ostream & out_,
                              const placement & pl_,
                              const polycone & p_,
                              uint32_t options_)
  {
    wires_type wires;
    p_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_polycone(std::ostream & out_,
                              const vector_3d & position_,
                              const rotation_3d & rotation_,
                              const polycone & p_,
                              uint32_t options_)
  {
    wires_type wires;
    p_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Polyhedra:

  void
  gnuplot_draw::draw_polyhedra(std::ostream & out_,
                               const polyhedra & p_,
                               uint32_t options_)
  {
    wires_type wires;
    p_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_polyhedra(std::ostream & out_,
                               const placement & pl_,
                               const polyhedra & p_,
                               uint32_t options_)
  {
    wires_type wires;
    p_.generate_wires(wires, pl_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_polyhedra(std::ostream & out_,
                               const vector_3d & position_,
                               const rotation_3d & rotation_,
                               const polyhedra & p_,
                               uint32_t options_)
  {
    wires_type wires;
    p_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Wall:

  void
  gnuplot_draw::draw_wall(std::ostream & out_,
                                 const wall_solid & ts_,
                                 uint32_t options_)
  {
    wires_type wires;
    ts_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_wall(std::ostream & out_,
                                 const placement & placement_,
                                 const wall_solid & ts_,
                                 uint32_t options_)
  {
    wires_type wires;
    ts_.generate_wires(wires, placement_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_wall(std::ostream & out_,
                                 const vector_3d & position_,
                                 const rotation_3d & rotation_,
                                 const wall_solid & ts_,
                                 uint32_t options_)
  {
    wires_type wires;
    ts_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Tessellated:
  void
  gnuplot_draw::draw_tessellated(std::ostream & out_,
                                 const tessellated_solid & ts_,
                                 uint32_t options_)
  {
    wires_type wires;
    ts_.generate_wires_self(wires, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_tessellated(std::ostream & out_,
                                 const placement & placement_,
                                 const tessellated_solid & ts_,
                                 uint32_t options_)
  {
    wires_type wires;
    ts_.generate_wires(wires, placement_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_tessellated(std::ostream & out_,
                                 const vector_3d & position_,
                                 const rotation_3d & rotation_,
                                 const tessellated_solid & ts_,
                                 uint32_t options_)
  {
    wires_type wires;
    ts_.generate_wires(wires, position_, rotation_, options_);
    gnuplot_draw::basic_draw_wires(out_, wires);
    return;
  }

  // Intersection:

  void
  gnuplot_draw::draw_intersection_3d(std::ostream & out_,
                                     const intersection_3d & i_,
                                     uint32_t options_)
  {
    wires_type wires;
    i_.generate_wires_self(wires, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_intersection_3d(std::ostream & out_,
                                     const placement & pl_,
                                     const intersection_3d & i_,
                                     uint32_t options_)
  {
    wires_type wires;
    i_.generate_wires(wires, pl_,options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_intersection_3d(std::ostream & out_,
                                     const vector_3d & position_,
                                     const rotation_3d & rotation_,
                                     const intersection_3d & i_,
                                     uint32_t options_)
  {
    wires_type wires;
    i_.generate_wires(wires, position_, rotation_,options_);
    basic_draw_wires(out_, wires);
    return;
  }

  // Union:

  void
  gnuplot_draw::draw_union_3d(std::ostream & out_,
                              const union_3d & u_,
                              uint32_t options_)
  {
    wires_type wires;
    u_.generate_wires_self(wires, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_union_3d(std::ostream & out_,
                              const placement & pl_,
                              const union_3d & u_,
                              uint32_t options_)
  {
    wires_type wires;
    u_.generate_wires(wires, pl_,options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_union_3d(std::ostream & out_,
                              const vector_3d & position_,
                              const rotation_3d & rotation_,
                              const union_3d & u_,
                              uint32_t options_)
  {
    wires_type wires;
    u_.generate_wires(wires, position_, rotation_, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  // Subtraction:

  void
  gnuplot_draw::draw_subtraction_3d(std::ostream & out_,
                                    const subtraction_3d & s_,
                                    uint32_t options_)
  {
    wires_type wires;
    s_.generate_wires_self(wires, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_subtraction_3d(std::ostream & out_,
                                    const placement & pl_,
                                    const subtraction_3d & s_,
                                    uint32_t options_)
  {
    wires_type wires;
    s_.generate_wires(wires, pl_, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void
  gnuplot_draw::draw_subtraction_3d(std::ostream & out_,
                                    const vector_3d & position_,
                                    const rotation_3d & rotation_,
                                    const subtraction_3d & s_,
                                    uint32_t options_)
  {
    wires_type wires;
    s_.generate_wires(wires, position_, rotation_, options_);
    basic_draw_wires(out_, wires);
    return;
  }

  void gnuplot_draw::basic_draw(std::ostream & out_,
                                const vector_3d & position_,
                                const rotation_3d & rotation_,
                                const i_object_3d & o_,
                                uint32_t options_)
  {
    placement pl(position_, rotation_);
    basic_draw(out_, pl, o_, options_);
    return;
  }

  void gnuplot_draw::basic_draw(std::ostream & out_,
                                const placement & pl_,
                                const i_object_3d & o_,
                                uint32_t options_)
  {
    // const vector_3d & pos = pl_.get_translation();
    // const rotation_3d & rot = pl_.get_rotation();
    std::string shape_name = o_.get_shape_name();

    bool mode_wired_cylinder = false;

    if (o_.has_wires_drawer()) {
      // Attempt to use a special wires drawer object:
      const i_wires_3d_rendering & wires_drawer_ref = o_.get_wires_drawer();
      wires_type wires;
      wires_drawer_ref.generate_wires(wires, pl_, options_);
      basic_draw_wires(out_, wires);
      return;
    }

    // Curves:

    if (shape_name == "line_3d")  {
      const line_3d & l = dynamic_cast<const line_3d &>(o_);
      draw_line(out_, pl_, l);
      return;
    }

    if (shape_name == "polyline_3d") {
      const polyline_3d & p = dynamic_cast<const polyline_3d &>(o_);
      draw_polyline(out_, pl_, p);
      return;
    }

    if (shape_name == "helix_3d")  {
      const helix_3d & l = dynamic_cast<const helix_3d &>(o_);
      draw_helix(out_, pl_, l);
      return;
    }

    if (shape_name == "circle") {
      const circle & c = dynamic_cast<const circle &>(o_);
      draw_circle(out_, pl_, c, options_);
      return;
    }

    if (shape_name == "ellipse") {
      const ellipse & e = dynamic_cast<const ellipse &>(o_);
      draw_ellipse(out_, pl_, e, options_);
      return;
    }

    // Surfaces:

    if (shape_name == "triangle") {
      const triangle & t = dynamic_cast<const triangle &>(o_);
      draw_triangle(out_, pl_, t, options_);
      return;
    }

    if (shape_name == "quadrangle") {
      const quadrangle & q = dynamic_cast<const quadrangle &>(o_);
      draw_quadrangle(out_, pl_, q, options_);
      return;
    }

    if (shape_name == "rectangle") {
      const rectangle & r = dynamic_cast<const rectangle &>(o_);
      draw_rectangle(out_, pl_, r, options_);
      return;
    }

    if (shape_name == "regular_polygon") {
      const regular_polygon & rp = dynamic_cast<const regular_polygon &>(o_);
      draw_regular_polygon(out_, pl_, rp, options_);
      return;
    }

    if (shape_name == "simple_polygon") {
      const simple_polygon & rp = dynamic_cast<const simple_polygon &>(o_);
      draw_simple_polygon(out_, pl_, rp, options_);
      return;
    }

    if (shape_name == "disk") {
      const disk & d = dynamic_cast<const disk &>(o_);
      draw_disk(out_, pl_, d, options_);
      return;
    }

    // Solids:

    if (shape_name == "box") {
      const box & b = dynamic_cast<const box &>(o_);
      draw_box(out_, pl_, b, options_);
      return;
    }

    if (shape_name == "extruded_box") {
      const extruded_box & b = dynamic_cast<const extruded_box &>(o_);
      draw_extruded_box(out_, pl_, b, options_);
      return;
    }

    if (shape_name == "cylinder") {
      const cylinder & c = dynamic_cast<const cylinder &>(o_);
      if (! mode_wired_cylinder) {
        draw_cylinder(out_, pl_, c, options_);
      } else {
        vector_3d first(0, 0, -c.get_half_z()), last(0, 0, +c.get_half_z());
        line_3d l(first, last);
        draw_line(out_, pl_, l);
      }
      return;
    }

    if (shape_name == "tube") {
      const tube & t = dynamic_cast<const tube &>(o_);
      if (! mode_wired_cylinder) {
        draw_tube(out_, pl_, t, options_);
      } else {
        vector_3d first(0, 0, -t.get_half_z()), last(0, 0, +t.get_half_z());
        line_3d l(first, last);
        draw_line(out_, pl_, l);
      }
      return;
    }

    if (shape_name == "elliptical_cylinder") {
      const elliptical_cylinder & t = dynamic_cast<const elliptical_cylinder &>(o_);
      if (! mode_wired_cylinder) {
        draw_elliptical_cylinder(out_, pl_, t, options_);
      } else {
        vector_3d first(0, 0, -t.get_half_z()), last(0, 0, +t.get_half_z());
        line_3d l(first, last);
        draw_line(out_, pl_, l);
      }
      return;
    }

    if (shape_name == "ellipsoid") {
      const ellipsoid & e = dynamic_cast<const ellipsoid &>(o_);
      draw_ellipsoid(out_, pl_, e, options_);
      return;
    }

    if (shape_name == "spherical_sector") {
      const spherical_sector & ss = dynamic_cast<const spherical_sector &>(o_);
      draw_spherical_sector(out_, pl_, ss, options_);
      return;
    }

    if (shape_name == "sphere") {
      const sphere & s = dynamic_cast<const sphere &>(o_);
      draw_sphere(out_, pl_, s, options_);
      return;
    }

    if (shape_name == "right_circular_conical_frustrum") {
      const right_circular_conical_frustrum & cf = dynamic_cast<const right_circular_conical_frustrum &>(o_);
      draw_right_circular_conical_frustrum(out_, pl_, cf, options_);
      return;
    }

    if (shape_name == "polycone") {
      const polycone & pc = dynamic_cast<const polycone &>(o_);
      draw_polycone(out_, pl_, pc, options_);
      return;
    }

    if (shape_name == "polyhedra") {
      const polyhedra & ph = dynamic_cast<const polyhedra &>(o_);
      draw_polyhedra(out_, pl_, ph, options_);
      return;
    }

    if (shape_name == "tessellated") {
      const tessellated_solid & ts = dynamic_cast<const tessellated_solid &>(o_);
      draw_tessellated(out_, pl_, ts, options_);
      return;
    }

    if (shape_name == "wall") {
      const wall_solid & ts = dynamic_cast<const wall_solid &>(o_);
      draw_wall(out_, pl_, ts, options_);
      return;
    }

    if (shape_name == "union_3d") {
      const union_3d & u = dynamic_cast<const union_3d &>(o_);
      draw_union_3d(out_, pl_, u, options_);
      return;
    }

    if (shape_name == "subtraction_3d") {
      const subtraction_3d & s = dynamic_cast<const subtraction_3d &>(o_);
      draw_subtraction_3d(out_, pl_, s, options_);
      return;
    }

    if (shape_name == "intersection_3d") {
      const intersection_3d & i = dynamic_cast<const intersection_3d &>(o_);
      draw_intersection_3d(out_, pl_, i, options_);
      return;
    }

    {
      const i_wires_3d_rendering * w3dr = dynamic_cast<const i_wires_3d_rendering *>(&o_);
      if (w3dr != 0) {
        wires_type wires;
        w3dr->generate_wires(wires, pl_, options_);
        basic_draw_wires(out_, wires);
        return;
      }
    }

    DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                   "No method to draw an 3D object of type '"
                   << shape_name << "' !");
    return;
  }

  // Display data:

  void gnuplot_draw::draw_display_data(std::ostream & out_,
                                       const display_data & dd_,
                                       bool static_scene_,
                                       int frame_index_,
                                       const std::string & color_,
                                       const std::string & group_,
                                       const std::string & name_)
  {
    placement pl;
    pl.set_identity();
    draw_display_data(out_, pl, dd_, static_scene_, frame_index_, color_, group_, name_);
    return;
  }

  void gnuplot_draw::draw_display_data(std::ostream & out_,
                                       const vector_3d & position_,
                                       const rotation_3d & rotation_,
                                       const display_data & dd_,
                                       bool static_scene_,
                                       int frame_index_,
                                       const std::string & color_,
                                       const std::string & group_,
                                       const std::string & name_)
  {
    draw_display_data(out_, placement(position_, rotation_), dd_, static_scene_, frame_index_, color_, group_, name_);
    return;
  }

  void gnuplot_draw::draw_display_data(std::ostream & out_,
                                       const placement & pl_,
                                       const display_data & dd_,
                                       bool static_scene_,
                                       int frame_index_,
                                       const std::string & color_,
                                       const std::string & group_,
                                       const std::string & name_)
  {
    // int last_np = -1;
    // bool safe = false;
    for(geomtools::display_data::entries_dict_type::const_iterator i
          = dd_.get_entries().begin();
        i != dd_.get_entries().end();
        i++) {
      const std::string & entry_name = i->first;
      const geomtools::display_data::display_entry & de = i->second;
      if (! name_.empty() && entry_name != name_) continue;
      if (! group_.empty() && de.group != group_) continue;
      if (static_scene_ && de.is_static()) {
        const geomtools::display_data::display_item & di = de.get_static_item();
        if (color_.empty() || di.color == color_) {
          wires_type tr_wires;
          transform_wires_to(pl_,
                             di.wires,
                             tr_wires);
          draw_wires(out_, tr_wires);
        }
      }
      if (frame_index_ >= 0 && de.is_framed()) {
        const geomtools::display_data::display_item & di = de.get_framed_item(frame_index_);
        if (color_.empty() || di.color == color_) {
          wires_type tr_wires;
          transform_wires_to(pl_,
                             di.wires,
                             tr_wires);
          draw_wires(out_, tr_wires);
          /*
           for(std::list<geomtools::polyline_3d>::const_iterator ip = di.paths.begin();
              ip != di.paths.end();
              ip++) {
            //const geomtools::polyline_3d & wires = *ip;
            geomtools::polyline_3d wires = *ip;
            int np = wires.get_number_of_points();
            // std::cerr << "DEVEL ************ geomtools::gnuplot_draw::draw_display_data: Processing polyline np="
            //           << np << " with color '" << color_ << "'\n";
            if (! safe && np > 1) {
              if (last_np < 0) {
                last_np = np;
                // std::cerr << "DEVEL ************ geomtools::gnuplot_draw::draw_display_data: record last_np="
                //           << last_np << "\n";
              } else {
                if (last_np == np) {
                  // std::cerr << "DEVEL ************ force duplicate the last vertex: np=" << np << "\n";
                  // force duplicate the last vertex :
                  wires.add(wires.get_vertex(np-1));
                  safe = true;
                } else {
                  safe = true;
                }
              }
            }
            geomtools::gnuplot_draw::draw_polyline(out_,
                                                   pl_,
                                                   wires);
          }
          */
        } // if (color_.empty()
      } // if (frame_index_ >= 0
    } // for(

    return;
  }

  void gnuplot_draw::draw(std::ostream & out_,
                          const i_placement & p_,
                          const i_object_3d & o_,
                          uint32_t options_)
  {
    // multi-placement:
    // std::cerr << "DEVEL: gnuplot_draw::draw: "
    //           << "Number of items = " << p_.get_number_of_items()
    //           << " for object with shape '" << o_.get_shape_name() << "'"
    //           << std::endl;
    for (uint32_t i = 0; i < p_.get_number_of_items(); i++) {
      placement p;
      p_.get_placement(i, p);
      // std::cerr << "DEVEL: gnuplot_draw::draw: "
      //           << "  items #" << i
      //           << "  placement = " << p
      //           << std::endl;
      basic_draw(out_, p, o_, options_);
    }
    return;
  }

} // end of namespace geomtools
