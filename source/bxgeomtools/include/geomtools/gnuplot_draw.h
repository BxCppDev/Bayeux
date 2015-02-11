// -*- mode: c++; -*-
/// \file geomtools/gnuplot_draw.h
/* Author(s):     F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-19
 * Last modified: 2014-11-18
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

// Third party:
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/color.h>

namespace geomtools {

  // Forward declaration:
  class i_object_3d;
  class i_placement;
  class placement;
  class box;
  class extruded_box;
  class cylinder;
  class sphere;
  class ellipsoid;
  class tube;
  class elliptical_tube;
  class line_3d;
  class helix_3d;
  class polyline_3d;
  class rectangle;
  class circle;
  class disk;
  class regular_polygon;
  class polycone;
  class polyhedra;
  class tessellated_solid;
  class union_3d;
  class subtraction_3d;
  class intersection_3d;
  class display_data;

  /// \brief Drawing primitives for various solid shapes
  class gnuplot_draw
  {
  public:

    /// \brief Alias for a 3D polyline
    typedef basic_polyline_3d polyline_type;

    /// \brief Mode of drawing
    enum mode_flags_type {
      MODE_NULL = 0x0,
      MODE_WIRED_CYLINDER = 0x1
    };

    static const uint32_t DEFAULT_SAMPLING = 0;

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

      bool           _activated_; /// Activation flag
      mygsl::min_max _x_range_;   /// Range on X axis
      mygsl::min_max _y_range_;   /// Range on Y axis
      mygsl::min_max _z_range_;   /// Range on Z axis

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
    basic_draw_point_with_color (std::ostream &,
                                 double x_, double y_, double z_,
                                 double color_,
                                 bool new_line_ = true);

    /// Basic draw a colored point
    static void
    basic_draw_point_with_color (std::ostream & out_,
                                 const vector_3d & point_,
                                 double color_,
                                 bool new_line_ = true);

    /// Basic draw a point
    static void
    basic_draw_point (std::ostream &,
                      double x_, double y_, double z_,
                      bool new_line_);

    /// Basic draw a point
    static void
    basic_draw_point (std::ostream &,
                      const vector_3d &,
                      bool new_line_ = true);

    /// Basic draw a 3 vertice facet
    static void
    basic_draw_facet3 (std::ostream & out_,
                       const vector_3d & p1_,
                       const vector_3d & p2_,
                       const vector_3d & p3_,
                       double color_ = 1.0);

    /// Basic draw a 4 vertice facet
    static void
    basic_draw_facet3 (std::ostream & out_,
                       const vector_3d & p1_,
                       const vector_3d & p2_,
                       const vector_3d & p3_,
                       const vector_3d & p4_,
                       double color_ = 1.0);

    /// Basic draw a polyline
    static void
    basic_draw_polyline (std::ostream &,
                         const polyline_type &);

    /// Basic draw a polyline
    static void
    basic_draw_polyline (std::ostream &,
                         const polyline_type &,
                         bool new_line_);

    /// Draw a line
    static void
    draw_line (std::ostream &,
               const vector_3d &,
               const vector_3d &,
               bool gp_trick_ = false);

    /// Draw a line object
    static void
    draw_line (std::ostream &,
               const line_3d &,
               bool gp_trick_ = false);

    /// Draw a helix object
    static void
    draw_helix (std::ostream &,
                const helix_3d &,
                double step_angle_ = 0.0,
                bool gp_trick_ = false);

    /// Draw a polyline
    static void
    draw_polyline  (std::ostream &,
                    const vector_3d &,
                    const rotation_3d &,
                    const polyline_type &,
                    bool closed_ = false,
                    bool gp_trick_ = false);

    /// Draw a polyline
    static void
    draw_polyline  (std::ostream &,
                    const vector_3d &,
                    const rotation_3d &,
                    const polyline_3d &,
                    bool closed_ = false,
                    bool gp_trick_ = false);

    /// Draw a segment
    static void
    draw_segment  (std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const vector_3d &,
                   const vector_3d &,
                   bool gp_trick_ = false);

    /// Draw a segment
    static void
    draw_segment  (std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const line_3d &,
                   bool gp_trick_ = false);

    /// Draw a line
    static void
    draw_line  (std::ostream &,
                const vector_3d &,
                const rotation_3d &,
                const line_3d &,
                bool gp_trick_ = false);

    /// Draw a helix
    static void
    draw_helix  (std::ostream &,
                 const vector_3d &,
                 const rotation_3d &,
                 const helix_3d &,
                 double step_angle_ = 0.0,
                 bool gp_trick_ = false);

    /// Draw a rectangle
    static void
    draw_rectangle (std::ostream &,
                    const vector_3d &,
                    const rotation_3d &,
                    double,
                    double,
                    bool closed_ = true,
                    bool gp_trick_ = false);

    /// Draw a rectangle
    static void
    draw_rectangle (std::ostream &,
                    const vector_3d &,
                    const rotation_3d &,
                    const rectangle &,
                    bool closed_ = true,
                    bool gp_trick_ = false);

    /// Draw a circle
    static void
    draw_circle (std::ostream &,
                 const vector_3d &,
                 const rotation_3d &,
                 double,
                 size_t = DEFAULT_SAMPLING );

    /// Draw a circle
    static void
    draw_circle (std::ostream &,
                 const vector_3d &,
                 const rotation_3d &,
                 const circle & ,
                 size_t = DEFAULT_SAMPLING );

    /// Draw a disk
    static void
    draw_disk (std::ostream &,
               const vector_3d &,
               const rotation_3d &,
               double,
               size_t = DEFAULT_SAMPLING );

    /// Draw a disk
    static void
    draw_disk (std::ostream &,
               const vector_3d &,
               const rotation_3d &,
               const disk & ,
               size_t = DEFAULT_SAMPLING );

    /// Draw a regular polygon
    static void
    draw_regular_polygon (std::ostream &,
                          const vector_3d &,
                          const rotation_3d &,
                          const regular_polygon &,
                          bool draw_radius_ = false);

    /// Draw a box
    static void
    draw_box (std::ostream &,
              const vector_3d &,
              const rotation_3d &,
              double,
              double,
              double,
              int tube_axis_ = AXIS_INVALID,
              size_t n_tube_sampling_ = DEFAULT_SAMPLING);

    /// Draw a box
    static void
    draw_box (std::ostream &,
              const vector_3d &,
              const rotation_3d &,
              const box &,
              int tube_axis_ = AXIS_INVALID,
              size_t n_tube_sampling_ = DEFAULT_SAMPLING);

    /// Draw a extruded box
    static void
    draw_extruded_box (std::ostream &,
                       const vector_3d &,
                       const rotation_3d &,
                       double, double, double, double,
                       bool, bool,
                       int tube_axis_ = AXIS_INVALID,
                       size_t n_tube_sampling_ = DEFAULT_SAMPLING);

    /// Draw a extruded box
    static void
    draw_extruded_box (std::ostream &,
                       const vector_3d &,
                       const rotation_3d &,
                       const extruded_box &,
                       int tube_axis_ = AXIS_INVALID,
                       size_t n_tube_sampling_ = DEFAULT_SAMPLING);

    /// Draw a cylinder
    static void
    draw_cylinder (std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   double,
                   double,
                   size_t = DEFAULT_SAMPLING);

    /// Draw a cylinder
    static void
    draw_cylinder (std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const cylinder &,
                   size_t = DEFAULT_SAMPLING);

    /// Draw a tube
    static void
    draw_tube (std::ostream &,
               const vector_3d &,
               const rotation_3d &,
               double,
               double,
               double,
               double = 0.,
               double = 2 * M_PI,
               size_t = 36);

    /// Draw a tube
    static void
    draw_tube (std::ostream &,
               const vector_3d &,
               const rotation_3d &,
               const tube &,
               size_t = DEFAULT_SAMPLING);

    /// Draw an elliptical tube
    static void
    draw_elliptical_tube (std::ostream &,
                          const vector_3d &,
                          const rotation_3d &,
                          double,
                          double,
                          double,
                          size_t = 36);

    /// Draw an elliptical tube
    static void
    draw_elliptical_tube (std::ostream &,
                          const vector_3d &,
                          const rotation_3d &,
                          const elliptical_tube &,
                          size_t = DEFAULT_SAMPLING);

    /// Draw a sphere
    static void
    draw_sphere (std::ostream &,
                 const vector_3d &,
                 const rotation_3d &,
                 double radius_,
                 size_t = DEFAULT_SAMPLING,
                 size_t = 16);

    /// Draw a sphere
    static void
    draw_sphere (std::ostream &,
                 const vector_3d &,
                 const rotation_3d &,
                 double r_min_,
                 double r_max_,
                 double phi_min_,
                 double delta_phi_,
                 double theta_min_,
                 double delta_theta_,
                 size_t arc_sampling_ = DEFAULT_SAMPLING,
                 size_t z_sampling_ = 16);

    /// Draw a sphere
    static void
    draw_sphere (std::ostream &,
                 const vector_3d &,
                 const rotation_3d &,
                 const sphere &,
                 size_t = DEFAULT_SAMPLING,
                 size_t = 16);

    /// Draw an ellipsoid
    static void
    draw_ellipsoid (std::ostream &,
                    const vector_3d &,
                    const rotation_3d &,
                    double, double, double,
                    double, double,
                    size_t = DEFAULT_SAMPLING,
                    size_t = 16);

    /// Draw an ellipsoid
    static void
    draw_ellipsoid (std::ostream &,
                    const vector_3d &,
                    const rotation_3d &,
                    const ellipsoid &,
                    size_t = DEFAULT_SAMPLING,
                    size_t = 16);

    /// Draw a polycone sector
    static void
    draw_polycone_sector (std::ostream &,
                          const vector_3d &,
                          const rotation_3d &,
                          const polycone &,
                          double phi1_, double phi2_,
                          size_t arc_sampling_ = DEFAULT_SAMPLING);

    /// Draw a polycone object
    static void
    draw_polycone (std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const polycone &,
                   size_t arc_sampling_ = DEFAULT_SAMPLING);

    /// Draw a right circular conical frustrum object
    static void
    draw_right_circular_conical_frustrum (std::ostream &,
                                          const vector_3d &,
                                          const rotation_3d &,
                                          double z1_, double rmin1_, double rmax1_,
                                          double z2_, double rmin2_, double rmax2_,
                                          double phi1_ = 0.0, double phi2_ = 2 * M_PI,
                                          size_t arc_sampling_ = DEFAULT_SAMPLING,
                                          uint32_t iobt_mask_ = 0xF);

    /// Draw a polyhedra object
    static void
    draw_polyhedra (std::ostream &,
                    const vector_3d &,
                    const rotation_3d &,
                    const polyhedra &);

    /// Draw a tessellated solid object
    static void
    draw_tessellated (std::ostream &,
                      const vector_3d &,
                      const rotation_3d &,
                      const tessellated_solid &);

    /// Draw an union object
    static void
    draw_union_3d (std::ostream &,
                   const vector_3d &,
                   const rotation_3d &,
                   const union_3d &);

    /// Draw an intersection object
    static void
    draw_intersection_3d (std::ostream &,
                          const vector_3d &,
                          const rotation_3d &,
                          const intersection_3d &);

    /// Draw a subtraction object
    static void
    draw_subtraction_3d (std::ostream &,
                         const vector_3d &,
                         const rotation_3d &,
                         const subtraction_3d &);

    /// Draw a display_data object
    static void
    draw_display_data (std::ostream &,
                       const vector_3d &,
                       const rotation_3d &,
                       const display_data &,
                       bool static_scene_ = true,
                       int frame_index_ = -1,
                       const std::string & color_ = "",
                       const std::string & group_ = "",
                       const std::string & name_ = "");

    /// Draw factory type
    typedef void (*draw_user_function_type) (std::ostream &,
                                             const vector_3d &,
                                             const rotation_3d &,
                                             const i_object_3d &,
                                             void *);

    /// Draw a 3D-object
    static void basic_draw (std::ostream & out_,
                            const vector_3d & position_,
                            const rotation_3d & rotation_,
                            const i_object_3d & o_,
                            unsigned long mode_ = gnuplot_draw::MODE_NULL);

    /*
    template<class Drawable>
    static void generic_draw(std::ostream & out_,
                             const vector_3d & position_,
                             const rotation_3d & rotation_,
                             const Drawable & d_,
                             const datatools::properties & options_);
    */

    /// Draw a 3D-object
    static void draw (std::ostream & out_,
                      const i_placement &,
                      const i_object_3d &);

    /// Draw a 3D-object
    static void draw (std::ostream & out_,
                      const i_placement &,
                      const i_object_3d &,
                      unsigned long mode_);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_GNUPLOT_DRAW_H
