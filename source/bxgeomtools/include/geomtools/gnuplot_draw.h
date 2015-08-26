/// \file geomtools/gnuplot_draw.h
/* Author(s):     F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-19
 * Last modified: 2015-03-21
 *
 * License:
 *
 * Description:
 *
 *   Gnuplot drawing tools
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_GNUPLOT_DRAW_H
#define GEOMTOOLS_GNUPLOT_DRAW_H 1

// Standard library:
#include <iostream>
#include <cmath>
#include <list>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/color.h>

namespace geomtools {

  // Forward declaration:
  class i_placement;
  class i_object_3d;
  class placement;

  // Curves:
  class line_3d;
  class polyline_3d;
  class circle;
  class ellipse;
  class helix_3d;

  // Surfaces:
  class triangle;
  class quadrangle;
  class rectangle;
  class regular_polygon;
  class simple_polygon;
  class disk;
  class elliptical_sector;
  class spherical_sector;
  class right_circular_conical_nappe;
  class ellipsoid_sector;
  class cylindrical_sector;
  class elliptical_cylinder_sector;
  class composite_surface;

  // Volumes/Solids:
  class box;
  class extruded_box;
  class cylinder;
  class tube;
  class sphere;
  class ellipsoid;
  class elliptical_cylinder;
  class right_circular_conical_frustrum;
  class right_polygonal_frustrum;
  class polycone;
  class polyhedra;
  class wall_solid;
  class tessellated_solid;
  class union_3d;
  class subtraction_3d;
  class intersection_3d;
  class display_data;

  /// \brief Drawing primitives for various solid shapes
  class gnuplot_draw
  {
  public:

    /// \brief Mode of drawing
    enum mode_flags_type {
      MODE_NULL = 0x0,
      MODE_WIRED_CYLINDER = 0x1
    };

    /// Default rendering option flags
    static const uint32_t DEFAULT_OPTIONS = 0;

  public:

    /// \brief A XYZ range used to record drawn points in an effective bounding box
    struct xyz_range {

      /// Constructor
      xyz_range();

      /// Reset the embedded X, Y, Z ranges
      void reset_ranges();

      /// Activate the recording of drawn points
      void activate();

      /// Deactivate the recording of drawn points
      void deactivate();

      /// Check activation flag
      bool is_activated() const;

      /// Return the const range on X axis
      const mygsl::min_max & get_x_range() const;

      /// Return the const range on Y axis
      const mygsl::min_max & get_y_range() const;

      /// Return the const range on Z axis
      const mygsl::min_max & get_z_range() const;

      /// Add a point only if activated
      void add_point(double x_, double y_, double z_);

      /// Add a point only if activated
      void add_point(const vector_3d & point_);

    private:

      bool           _activated_; //!< Activation flag
      mygsl::min_max _x_range_;   //!< Range on X axis
      mygsl::min_max _y_range_;   //!< Range on Y axis
      mygsl::min_max _z_range_;   //!< Range on Z axis

    };

    /// \brief On the fly bounding box action
    enum bounding_box_action_type {
      BB_ACTION_NONE       = 0,
      BB_ACTION_ACTIVATE   = 1,
      BB_ACTION_DEACTIVATE = 2,
      BB_ACTION_RESET      = 3
    };

    /// Return a mutable reference to the display bounding box singleton
    static xyz_range & bounding_box(bounding_box_action_type action_ = BB_ACTION_NONE);

    /// Return a const reference to the display bounding box singleton
    static const xyz_range & bounding_box_const();

    /// Return a mutable reference to the color context singleton
    static color::context & color_context();

    /// Return a const reference to the color context singleton
    static const color::context & color_context_const();

    /// Basic draw a colored point
    static void
    basic_draw_point_with_color(std::ostream &,
                                double x_, double y_, double z_,
                                double color_,
                                bool new_line_ = true);

    /// Basic draw a colored point
    static void
    basic_draw_point_with_color(std::ostream & out_,
                                const vector_3d & point_,
                                double color_,
                                bool new_line_ = true);

    /// Basic draw an uncolored point
    static void
    basic_draw_point(std::ostream &,
                     double x_, double y_, double z_,
                     bool new_line_ = true);

    /// Basic draw a uncolored point
    static void
    basic_draw_point(std::ostream &,
                     const vector_3d &,
                     bool new_line_ = true);

    /// Basic draw a 3 vertice facet
    static void
    basic_draw_facet3(std::ostream & out_,
                      const vector_3d & p1_,
                      const vector_3d & p2_,
                      const vector_3d & p3_,
                      double color_ = 1.0);

    /// Basic draw a 4 vertice facet
    static void
    basic_draw_facet4(std::ostream & out_,
                      const vector_3d & p1_,
                      const vector_3d & p2_,
                      const vector_3d & p3_,
                      const vector_3d & p4_,
                      double color_ = 1.0);

    /// Basic draw a segment
    static void
    basic_draw_segment(std::ostream &,
                       const vector_3d &,
                       const vector_3d &,
                       bool gp_trick_ = false);

    /// Basic draw a segment
    static void
    basic_draw_segment(std::ostream &,
                       const segment_type &,
                       bool gp_trick_ = false);

    /// Basic draw a polyline
    static bool
    basic_draw_polyline(std::ostream &,
                        const polyline_type &,
                        bool new_line_ = true,
                        bool gnuplot_trick = true);

    /// Basic draw a list of polylines
    static void
    basic_draw_wires(std::ostream & out_,
                     const wires_type & wires_);

    /// Basic draw a list of polylines
    static void
    basic_draw_polylines(std::ostream & out_,
                         const wires_type & wires_);

    /// Draw a list of polylines in some arbitrary reference frame
    static void
    draw_wires(std::ostream &,
               const wires_type & wires);

    /// Draw a list of polylines in some arbitrary reference frame
    static void
    draw_wires(std::ostream &,
               const placement & p_,
               const wires_type & wires);

    /// Draw a list of polylines in some arbitrary reference frame
    static void
    draw_wires(std::ostream &,
               const vector_3d & pos_,
               const rotation_3d & rot_,
               const wires_type & wires_);

    // Vertex:

    /// Draw a vertex
    static void
    draw_vertex(std::ostream &,
                const vector_3d & vertex_,
                uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a vertex
    static void
    draw_vertex(std::ostream &,
                double x_, double y_, double z_,
                uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a vertex
    static void
    draw_vertex(std::ostream &,
                const placement & placement_,
                const vector_3d & vertex_,
                uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a vertex
    static void
    draw_vertex(std::ostream &,
                const placement & placement_,
                double x_, double y_, double z_,
                uint32_t options_ = DEFAULT_OPTIONS);

    // Line:

    /// Draw a line object
    static void
    draw_line(std::ostream &,
              const line_3d &,
              uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a line
    static void
    draw_line(std::ostream &,
              const vector_3d & start_,
              const vector_3d & stop_,
              uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a line object
    static void
    draw_line(std::ostream &,
              const placement & placement_,
              const line_3d & line_,
              uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a line
    static void
    draw_line(std::ostream &,
              const vector_3d & pos_,
              const rotation_3d & rot_,
              const line_3d & line_,
              uint32_t options_ = DEFAULT_OPTIONS);

    // Helix:

    /// Draw a helix object
    static void
    draw_helix(std::ostream &,
               const helix_3d & helix_,
               uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a helix object
    static void
    draw_helix(std::ostream &,
               const placement & p_,
               const helix_3d & helix_,
               uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a helix object
    static void
    draw_helix(std::ostream &,
               const vector_3d & pos_,
               const rotation_3d & rot_,
               const helix_3d & helix_,
               uint32_t options_ = DEFAULT_OPTIONS);

    // Polyline:

    /// Draw a polyline
    static void
    draw_polyline(std::ostream &,
                  const polyline_3d & poly_,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a polyline
    static void
    draw_polyline(std::ostream &,
                  const placement & pl_,
                  const polyline_3d & poly_,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a polyline
    static void
    draw_polyline(std::ostream &,
                  const vector_3d & pos_,
                  const rotation_3d & rot_,
                  const polyline_3d & poly_,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a list of polylines
    static void
    draw_polylines(std::ostream &,
                   const std::list<polyline_3d> &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a list of polylines
    static void
    draw_polylines(std::ostream &,
                   const placement & pl_,
                   const std::list<polyline_3d> &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a list of polylines
    static void
    draw_polylines(std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const std::list<polyline_3d> &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    // Rectangle:

    /// Draw a rectangle
    static void
    draw_rectangle(std::ostream &,
                   const rectangle &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a rectangle
    static void
    draw_rectangle(std::ostream &,
                   const placement &,
                   const rectangle &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a rectangle
    static void
    draw_rectangle(std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const rectangle &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    // Circle:

    /// Draw a circle
    static void
    draw_circle(std::ostream &,
                const circle & ,
                uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a circle
    static void
    draw_circle(std::ostream &,
                const placement & p_,
                const circle & ,
                uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a circle
    static void
    draw_circle(std::ostream &,
                const vector_3d & pos_,
                const rotation_3d & rot_,
                const circle & ,
                uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a circle
    static void
    draw_circle(std::ostream &,
                const vector_3d &,
                const rotation_3d &,
                double radius_,
                uint32_t options_ = DEFAULT_OPTIONS );

    // Ellipse:

    /// Draw a ellipse
    static void
    draw_ellipse(std::ostream &,
                 const ellipse & ,
                 uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a ellipse
    static void
    draw_ellipse(std::ostream &,
                 const placement & p_,
                 const ellipse & ,
                 uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a ellipse
    static void
    draw_ellipse(std::ostream &,
                 const vector_3d & pos_,
                 const rotation_3d & rot_,
                 const ellipse & ,
                 uint32_t options_ = DEFAULT_OPTIONS );

    // Cylindrical sector:

    /// Draw a cylindrical sector
    static void
    draw_cylindrical_sector(std::ostream &,
                            const cylindrical_sector &,
                            uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a cylindrical sector
    static void
    draw_cylindrical_sector(std::ostream &,
                            const placement &,
                            const cylindrical_sector &,
                            uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a cylindrical sector
    static void
    draw_cylindrical_sector(std::ostream &,
                            const vector_3d &,
                            const rotation_3d &,
                            const cylindrical_sector &,
                            uint32_t options_ = DEFAULT_OPTIONS);

    // Elliptical sector:

    /// Draw a elliptical sector
    static void
    draw_elliptical_sector(std::ostream &,
                           const elliptical_sector &,
                           uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a elliptical sector
    static void
    draw_elliptical_sector(std::ostream &,
                           const placement &,
                           const elliptical_sector &,
                           uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a elliptical sector
    static void
    draw_elliptical_sector(std::ostream &,
                           const vector_3d &,
                           const rotation_3d &,
                           const elliptical_sector &,
                           uint32_t options_ = DEFAULT_OPTIONS);

    // Ellipsoid sector:

    /// Draw a ellipsoid sector
    static void
    draw_ellipsoid_sector(std::ostream &,
                          const ellipsoid_sector &,
                          uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an ellipsoid sector
    static void
    draw_ellipsoid_sector(std::ostream &,
                          const placement &,
                          const ellipsoid_sector &,
                          uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an ellipsoid sector
    static void
    draw_ellipsoid_sector(std::ostream &,
                          const vector_3d &,
                          const rotation_3d &,
                          const ellipsoid_sector &,
                          uint32_t options_ = DEFAULT_OPTIONS);

    // Elliptical cylinder sector:

    /// Draw an elliptical cylinder sector
    static void
    draw_elliptical_cylinder_sector(std::ostream &,
                                    const elliptical_cylinder_sector &,
                                    uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an elliptical cylinder sector
    static void
    draw_elliptical_cylinder_sector(std::ostream &,
                                    const placement &,
                                    const elliptical_cylinder_sector &,
                                    uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an elliptical cylinder sector
    static void
    draw_elliptical_cylinder_sector(std::ostream &,
                                    const vector_3d &,
                                    const rotation_3d &,
                                    const elliptical_cylinder_sector &,
                                    uint32_t options_ = DEFAULT_OPTIONS);

    // Spherical sector:

    /// Draw a spherical sector
    static void
    draw_spherical_sector(std::ostream &,
                          const spherical_sector &,
                          uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a spherical sector
    static void
    draw_spherical_sector(std::ostream &,
                          const placement &,
                          const spherical_sector &,
                          uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a spherical sector
    static void
    draw_spherical_sector(std::ostream &,
                          const vector_3d &,
                          const rotation_3d &,
                          const spherical_sector &,
                          uint32_t options_ = DEFAULT_OPTIONS);

    // Disk:

    /// Draw a disk
    static void
    draw_disk(std::ostream &,
              const disk & ,
              uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a disk
    static void
    draw_disk(std::ostream &,
              const placement &,
              const disk & ,
              uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a disk
    static void
    draw_disk(std::ostream &,
              const vector_3d &,
              const rotation_3d &,
              const disk & ,
              uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a disk
    static void
    draw_disk(std::ostream &,
              const vector_3d &,
              const rotation_3d &,
              double,
              uint32_t options_ = DEFAULT_OPTIONS );

    /// Draw a disk
    static void
    draw_disk(std::ostream &,
              const vector_3d &,
              const rotation_3d &,
              double inner_radius_,
              double outer_radius_,
              double start_angle_,
              double delta_angle_,
              uint32_t options_ = DEFAULT_OPTIONS );

    // Triangle:

    /// Draw a triangle
    static void
    draw_triangle(std::ostream &,
                  const triangle &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a triangle
    static void
    draw_triangle(std::ostream &,
                  const placement & pl_,
                  const triangle &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a triangle
    static void
    draw_triangle(std::ostream &,
                  const vector_3d &,
                  const rotation_3d &,
                  const triangle &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    // Quadrangle:

    /// Draw a quadrangle
    static void
    draw_quadrangle(std::ostream &,
                    const quadrangle &,
                    uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a quadrangle
    static void
    draw_quadrangle(std::ostream &,
                    const placement & pl_,
                    const quadrangle &,
                    uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a quadrangle
    static void
    draw_quadrangle(std::ostream &,
                    const vector_3d &,
                    const rotation_3d &,
                    const quadrangle &,
                    uint32_t options_ = DEFAULT_OPTIONS);

    // Composite surface:

    /// Draw a composite surface
    static void
    draw_composite_surface(std::ostream &,
                           const composite_surface &,
                           uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a composite surface
    static void
    draw_composite_surface(std::ostream &,
                           const placement & pl_,
                           const composite_surface &,
                           uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a composite surface
    static void
    draw_composite_surface(std::ostream &,
                           const vector_3d &,
                           const rotation_3d &,
                           const composite_surface &,
                           uint32_t options_ = DEFAULT_OPTIONS);

    // Regular polygon:

    /// Draw a regular polygon
    static void
    draw_regular_polygon(std::ostream &,
                         const regular_polygon &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a regular polygon
    static void
    draw_regular_polygon(std::ostream &,
                         const placement &,
                         const regular_polygon &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a regular polygon
    static void
    draw_regular_polygon(std::ostream &,
                         const vector_3d &,
                         const rotation_3d &,
                         const regular_polygon &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    // Simple polygon:

    /// Draw a simple polygon
    static void
    draw_simple_polygon(std::ostream &,
                         const simple_polygon &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a simple polygon
    static void
    draw_simple_polygon(std::ostream &,
                         const placement &,
                         const simple_polygon &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a simple polygon
    static void
    draw_simple_polygon(std::ostream &,
                         const vector_3d &,
                         const rotation_3d &,
                         const simple_polygon &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    // Box:

    /// Draw a box
    static void
    draw_box(std::ostream &,
             const box &,
             uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a box
    static void
    draw_box(std::ostream &,
             const placement &,
             const box &,
             uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a box
    static void
    draw_box(std::ostream &,
             const vector_3d &,
             const rotation_3d &,
             const box &,
             uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a box
    static void
    draw_box(std::ostream &,
             const vector_3d &,
             const rotation_3d &,
             double x_, double y_, double z_,
             uint32_t options_ = DEFAULT_OPTIONS);

    // Extruded box:

    /// Draw an extruded box
    static void
    draw_extruded_box(std::ostream &,
                      const extruded_box &,
                      uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an extruded box
    static void
    draw_extruded_box(std::ostream &,
                      const placement &,
                      const extruded_box &,
                      uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an extruded box
    static void
    draw_extruded_box(std::ostream &,
                      const vector_3d &,
                      const rotation_3d &,
                      const extruded_box &,
                      uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an extruded box
    static void
    draw_extruded_box(std::ostream &,
                      const vector_3d &,
                      const rotation_3d &,
                      double x_, double y_, double z_, double thickness_,
                      bool has_top_, bool has_bottom_,
                      uint32_t options_ = DEFAULT_OPTIONS);

    // Cylinder:

    /// Draw a cylinder
    static void
    draw_cylinder(std::ostream &,
                  const cylinder &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a cylinder
    static void
    draw_cylinder(std::ostream &,
                  const placement &,
                  const cylinder &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a cylinder
    static void
    draw_cylinder(std::ostream &,
                  const vector_3d &,
                  const rotation_3d &,
                  const cylinder &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a cylinder
    static void
    draw_cylinder(std::ostream &,
                  const vector_3d &,
                  const rotation_3d &,
                  double radius_,
                  double heigth_,
                  uint32_t options_ = DEFAULT_OPTIONS);

    // Tube:

    /// Draw a tube
    static void
    draw_tube(std::ostream &,
              const tube &,
              uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a tube
    static void
    draw_tube(std::ostream &,
              const placement &,
              const tube &,
              uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a tube
    static void
    draw_tube(std::ostream &,
              const vector_3d &,
              const rotation_3d &,
              double,
              double,
              double,
              double = 0.,
              double = 2 * M_PI,
              uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a tube
    static void
    draw_tube(std::ostream &,
              const vector_3d &,
              const rotation_3d &,
              const tube &,
              uint32_t options_ = DEFAULT_OPTIONS);

    // Elliptical cylinder/tube:

    /// Draw an elliptical cylinder/tube
    static void
    draw_elliptical_cylinder(std::ostream &,
                             const elliptical_cylinder &,
                             uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an elliptical cylinder/tube
    static void
    draw_elliptical_cylinder(std::ostream &,
                             const placement &,
                             const elliptical_cylinder &,
                             uint32_t options_ = DEFAULT_OPTIONS);


    /// Draw an elliptical cylinder/tube
    static void
    draw_elliptical_cylinder(std::ostream &,
                             const vector_3d &,
                             const rotation_3d &,
                             const elliptical_cylinder &,
                             uint32_t options_ = DEFAULT_OPTIONS);

    // Sphere:

    /// Draw a sphere
    static void
    draw_sphere(std::ostream &,
                const sphere & s_,
                uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a sphere
    static void
    draw_sphere(std::ostream &,
                const placement &,
                const sphere & s_,
                uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a sphere
    static void
    draw_sphere(std::ostream &,
                const vector_3d &,
                const rotation_3d &,
                const sphere & s_,
                uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a sphere
    static void
    draw_sphere(std::ostream &,
                const vector_3d &,
                const rotation_3d &,
                double radius_,
                uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a sphere
    static void
    draw_sphere(std::ostream &,
                const vector_3d &,
                const rotation_3d &,
                double r_min_,
                double r_max_,
                double phi_min_,
                double delta_phi_,
                double theta_min_,
                double delta_theta_,
                uint32_t options_ = DEFAULT_OPTIONS);

    // Ellipsoid:

    /// Draw an ellipsoid
    static void
    draw_ellipsoid(std::ostream &,
                   const placement &,
                   const ellipsoid &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an ellipsoid
    static void
    draw_ellipsoid(std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const ellipsoid &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an ellipsoid
    static void
    draw_ellipsoid(std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   double rx_, double ry_, double rz_,
                   double z_min_, double z_max_,
                   uint32_t options_ = DEFAULT_OPTIONS);

    // Polycone:

    /// Draw a polycone
    static void
    draw_polycone(std::ostream &,
                  const polycone &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a polycone
    static void
    draw_polycone(std::ostream &,
                  const placement &,
                  const polycone &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a polycone
    static void
    draw_polycone(std::ostream &,
                  const vector_3d &,
                  const rotation_3d &,
                  const polycone &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    // Right circular conical nappe:

    static void
    draw_right_circular_conical_nappe(std::ostream &,
                                      const right_circular_conical_nappe &,
                                      uint32_t options_ = DEFAULT_OPTIONS);

    static void
    draw_right_circular_conical_nappe(std::ostream &,
                                      const placement &,
                                      const right_circular_conical_nappe &,
                                      uint32_t options_ = DEFAULT_OPTIONS);

    static void
    draw_right_circular_conical_nappe(std::ostream &,
                                      const vector_3d &,
                                      const rotation_3d &,
                                      const right_circular_conical_nappe &,
                                      uint32_t options_ = DEFAULT_OPTIONS);

    // Right circular conical frustrum:

    static void
    draw_right_circular_conical_frustrum(std::ostream &,
                                         const right_circular_conical_frustrum &,
                                         uint32_t options_ = DEFAULT_OPTIONS);

    static void
    draw_right_circular_conical_frustrum(std::ostream &,
                                         const placement &,
                                         const right_circular_conical_frustrum &,
                                         uint32_t options_ = DEFAULT_OPTIONS);

    static void
    draw_right_circular_conical_frustrum(std::ostream &,
                                         const vector_3d &,
                                         const rotation_3d &,
                                         const right_circular_conical_frustrum &,
                                         uint32_t options_ = DEFAULT_OPTIONS);


    static void
    draw_right_circular_conical_frustrum(std::ostream &,
                                         const vector_3d &,
                                         const rotation_3d &,
                                         double rmin1_, double rmax1_,
                                         double rmin2_, double rmax2_,
                                         double z_,
                                         double phi1_ = 0.0, double phi2_ = 2 * M_PI,
                                         uint32_t options_ = DEFAULT_OPTIONS);

    // Right polygonal frustrum:

    static void
    draw_right_polygonal_frustrum(std::ostream &,
                                  const right_polygonal_frustrum &,
                                  uint32_t options_ = DEFAULT_OPTIONS);

    static void
    draw_right_polygonal_frustrum(std::ostream &,
                                  const placement &,
                                  const right_polygonal_frustrum &,
                                  uint32_t options_ = DEFAULT_OPTIONS);

    static void
    draw_right_polygonal_frustrum(std::ostream &,
                                  const vector_3d &,
                                  const rotation_3d &,
                                  const right_polygonal_frustrum &,
                                  uint32_t options_ = DEFAULT_OPTIONS);

    // Polyhedra:

    /// Draw a polyhedra object
    static void
    draw_polyhedra(std::ostream &,
                   const polyhedra &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a polyhedra object
    static void
    draw_polyhedra(std::ostream &,
                   const placement &,
                   const polyhedra &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a polyhedra object
    static void
    draw_polyhedra(std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const polyhedra &,
                   uint32_t options_ = DEFAULT_OPTIONS);

    // Wall:

    /// Draw a wall solid object
    static void
    draw_wall(std::ostream &,
              const wall_solid &,
              uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a wall solid object
    static void
    draw_wall(std::ostream &,
              const placement &,
              const wall_solid &,
              uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a wall solid object
    static void
    draw_wall(std::ostream &,
              const vector_3d &,
              const rotation_3d &,
              const wall_solid &,
              uint32_t options_ = DEFAULT_OPTIONS);

    // Tessellated:

    /// Draw a tessellated solid object
    static void
    draw_tessellated(std::ostream &,
                     const tessellated_solid &,
                     uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a tessellated solid object
    static void
    draw_tessellated(std::ostream &,
                     const placement &,
                     const tessellated_solid &,
                     uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a tessellated solid object
    static void
    draw_tessellated(std::ostream &,
                     const vector_3d &,
                     const rotation_3d &,
                     const tessellated_solid &,
                     uint32_t options_ = DEFAULT_OPTIONS);

    // Union:

    /// Draw an union object
    static void
    draw_union_3d(std::ostream &,
                  const union_3d &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an union object
    static void
    draw_union_3d(std::ostream &,
                  const placement &,
                  const union_3d &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an union object
    static void
    draw_union_3d(std::ostream &,
                  const vector_3d &,
                  const rotation_3d &,
                  const union_3d &,
                  uint32_t options_ = DEFAULT_OPTIONS);

    // Intersection:

    /// Draw an intersection object
    static void
    draw_intersection_3d(std::ostream &,
                         const intersection_3d &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an intersection object
    static void
    draw_intersection_3d(std::ostream &,
                         const placement &,
                         const intersection_3d &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw an intersection object
    static void
    draw_intersection_3d(std::ostream &,
                         const vector_3d &,
                         const rotation_3d &,
                         const intersection_3d &,
                         uint32_t options_ = DEFAULT_OPTIONS);

    // Subtraction:

    /// Draw a subtraction object
    static void
    draw_subtraction_3d(std::ostream &,
                        const subtraction_3d &,
                        uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a subtraction object
    static void
    draw_subtraction_3d(std::ostream &,
                        const placement &,
                        const subtraction_3d &,
                        uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a subtraction object
    static void
    draw_subtraction_3d(std::ostream &,
                        const vector_3d &,
                        const rotation_3d &,
                        const subtraction_3d &,
                        uint32_t options_ = DEFAULT_OPTIONS);

    // Display data:

    /// Draw a display_data object
    static void
    draw_display_data(std::ostream &,
                      const display_data &,
                      bool static_scene_ = true,
                      int frame_index_ = -1,
                      const std::string & color_ = "",
                      const std::string & group_ = "",
                      const std::string & name_ = "");

    /// Draw a display_data object
    static void
    draw_display_data(std::ostream &,
                      const placement &,
                      const display_data &,
                      bool static_scene_ = true,
                      int frame_index_ = -1,
                      const std::string & color_ = "",
                      const std::string & group_ = "",
                      const std::string & name_ = "");

    /// Draw a display_data object
    static void
    draw_display_data(std::ostream &,
                      const vector_3d &,
                      const rotation_3d &,
                      const display_data &,
                      bool static_scene_ = true,
                      int frame_index_ = -1,
                      const std::string & color_ = "",
                      const std::string & group_ = "",
                      const std::string & name_ = "");

    // Generic:

    /// Draw a 3D-object
    static void basic_draw(std::ostream & out_,
                           const i_object_3d & o_,
                           uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a 3D-object
    static void basic_draw(std::ostream & out_,
                           const placement & p_,
                           const i_object_3d & o_,
                           uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a 3D-object
    static void basic_draw(std::ostream & out_,
                           const vector_3d & position_,
                           const rotation_3d & rotation_,
                           const i_object_3d & o_,
                           uint32_t options_ = DEFAULT_OPTIONS);

    /// Draw a 3D-object
    static void draw(std::ostream & out_,
                     const i_placement &,
                     const i_object_3d &,
                     uint32_t options_ = DEFAULT_OPTIONS);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_GNUPLOT_DRAW_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
