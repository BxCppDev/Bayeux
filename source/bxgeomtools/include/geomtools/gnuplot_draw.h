// -*- mode: c++; -*- 
/* gnuplot_draw.h
 * Author(s):     F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: <2006-11-19>
 * Last modified: <2006-11-22>
 * 
 * License: 
 * 
 * Description: 
 *   Gnuplot drawing tools
 * 
 * History: 
 * 
 */

#ifndef GEOMTOOLS_GNUPLOT_DRAW_H_
#define GEOMTOOLS_GNUPLOT_DRAW_H_ 1

#include <iostream>

#include <geomtools/utils.h>
#include <mygsl/min_max.h>

namespace geomtools {

  class i_object_3d;
  class i_placement;
  class placement;
  class box;
  class cylinder;
  class sphere;
  class tube;
  class line_3d;
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

  class gnuplot_draw
  {
  public: 
    typedef basic_polyline_3d polyline_type;
  
    enum mode_flags_type
      {
        MODE_NULL = 0x0,
        MODE_WIRED_CYLINDER = 0x1
      };

  public: 

    struct xyz_range
    {
      mygsl::min_max x_range;
      mygsl::min_max y_range;
      mygsl::min_max z_range;
      void reset ();
      static xyz_range * instance (char mode_ = 'i');
    };

    static bool   g_using_color;
    static double g_current_color;

    static void 
    basic_draw_point_with_color (std::ostream &, 
                                 double x_, double y_, double z_, 
                                 double color_,
                                 bool endl_ = true);

    static void 
    basic_draw_point_with_color (std::ostream & out_, 
                                 const vector_3d & point_,
                                 double color_,
                                 bool   endl_ = true);

    static void 
    basic_draw_point (std::ostream &, 
                      double x_, double y_, double z_,
                      bool endl_);

    static void 
    basic_draw_point (std::ostream & out_, 
                      double x_, double y_, double z_);

    static void 
    basic_draw_point (std::ostream &, 
                      const vector_3d &,
                      bool endl_);

    static void 
    basic_draw_point (std::ostream & out_, 
                      const vector_3d & v_);

    static void 
    basic_draw_facet3 (std::ostream & out_, 
                       const vector_3d & p1_, 
                       const vector_3d & p2_, 
                       const vector_3d & p3_,
                       double color_ = 1.0);

    static void 
    basic_draw_facet3 (std::ostream & out_, 
                       const vector_3d & p1_, 
                       const vector_3d & p2_, 
                       const vector_3d & p3_, 
                       const vector_3d & p4_,
                       double color_ = 1.0);

    static void 
    basic_draw_polyline (std::ostream &, 
                         const polyline_type &);
    
    static void 
    draw_line (std::ostream &, 
               const vector_3d &, 
               const vector_3d &, 
               bool split_ = false);
    
    static void 
    draw_line (std::ostream &, 
               const line_3d &);
    
    static void 
    draw_polyline  (std::ostream &, 
                    const vector_3d &, 
                    const rotation_3d &,  
                    const polyline_type &, 
                    bool = false);
    
    static void 
    draw_polyline  (std::ostream &, 
                    const vector_3d &, 
                    const rotation_3d &,  
                    const polyline_3d &, 
                    bool = false);
    
    static void 
    draw_segment  (std::ostream &, 
                   const vector_3d &, 
                   const rotation_3d &,  
                   const vector_3d &, 
                   const vector_3d &);
    
    static void 
    draw_segment  (std::ostream &, 
                   const vector_3d &, 
                   const rotation_3d &,  
                   const line_3d &);

    static void 
    draw_line  (std::ostream &, 
                const vector_3d &, 
                const rotation_3d &,  
                const line_3d &);
     
    static void 
    draw_rectangle (std::ostream &, 
                    const vector_3d &, 
                    const rotation_3d &,  
                    double, 
                    double, 
                    bool = false);
     
    static void 
    draw_rectangle (std::ostream &, 
                    const vector_3d &, 
                    const rotation_3d &,  
                    const rectangle &,
                    bool = false);
     
    static void 
    draw_circle (std::ostream &, 
                 const vector_3d &, 
                 const rotation_3d &,
                 double, 
                 size_t = 36 );
     
    static void 
    draw_circle (std::ostream &, 
                 const vector_3d &, 
                 const rotation_3d &,
                 const circle & , 
                 size_t = 36 );
      
    static void 
    draw_disk (std::ostream &, 
               const vector_3d &, 
               const rotation_3d &,
               double, 
               size_t = 36 );
      
    static void 
    draw_disk (std::ostream &, 
               const vector_3d &, 
               const rotation_3d &,
               const disk & , 
               size_t = 36 );
      
    static void 
    draw_regular_polygon (std::ostream &, 
                          const vector_3d &, 
                          const rotation_3d &,
                          const regular_polygon &,
                          bool draw_radius_ = false);
   
    static void 
    draw_box (std::ostream &, 
              const vector_3d &, 
              const rotation_3d &,
              double, 
              double, 
              double,
              int tube_axis_ = AXIS_INVALID, size_t n_tube_sampling_ = 10);

    static void 
    draw_box (std::ostream &, 
              const vector_3d &, 
              const rotation_3d &,
              const box &,
              int tube_axis_ = AXIS_INVALID, size_t n_tube_sampling_ = 10);

    static void 
    draw_cylinder (std::ostream &, 
                   const vector_3d &, 
                   const rotation_3d &,
                   double, 
                   double, 
                   size_t = 36);

    static void 
    draw_cylinder (std::ostream &, 
                   const vector_3d &, 
                   const rotation_3d &,
                   const cylinder &, 
                   size_t = 36);

    static void 
    draw_tube (std::ostream &, 
               const vector_3d &, 
               const rotation_3d &,
               double, 
               double, 
               double, 
               size_t = 36);

    static void 
    draw_tube (std::ostream &, 
               const vector_3d &, 
               const rotation_3d &,
               const tube &, 
               size_t = 36);

    static void 
    draw_sphere (std::ostream &, 
                 const vector_3d &, 
                 const rotation_3d &,
                 double, 
                 size_t = 36,
                 size_t = 16);

    static void 
    draw_sphere (std::ostream &, 
                 const vector_3d &, 
                 const rotation_3d &,
                 const sphere &, 
                 size_t = 36,
                 size_t = 16);

    static void 
    draw_polycone (std::ostream &, 
                   const vector_3d &, 
                   const rotation_3d &,
                   const polycone &, 
                   size_t = 36);

    static void 
    draw_polyhedra (std::ostream &, 
                    const vector_3d &, 
                    const rotation_3d &,
                    const polyhedra &);

    static void 
    draw_tessellated (std::ostream &, 
                      const vector_3d &, 
                      const rotation_3d &,
                      const tessellated_solid &);

    static void 
    draw_union_3d (std::ostream &, 
                   const vector_3d &, 
                   const rotation_3d &,
                   const union_3d &);

    static void 
    draw_intersection_3d (std::ostream &, 
                          const vector_3d &, 
                          const rotation_3d &,
                          const intersection_3d &);

    static void 
    draw_subtraction_3d (std::ostream &, 
                         const vector_3d &, 
                         const rotation_3d &,
                         const subtraction_3d &);

    /*
    static void 
    draw_display_data (std::ostream &, 
                       const vector_3d &, 
                       const rotation_3d &,
                       const display_data &,
                       const std::string & display_rules_);
    */

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

    // draw factory:
     
    typedef void (*draw_user_function_type) (std::ostream &, 
                                             const vector_3d &, 
                                             const rotation_3d &,
                                             const i_object_3d &, 
                                             void *);
 
    static void basic_draw (std::ostream & out_,
                            const vector_3d & position_, 
                            const rotation_3d & rotation_, 
                            const i_object_3d & o_,
                            unsigned long mode_ = gnuplot_draw::MODE_NULL);
  
    static void draw (std::ostream & out_, 
                      const i_placement &, 
                      const i_object_3d &);
 
    static void draw (std::ostream & out_, 
                      const i_placement &, 
                      const i_object_3d &,
                      unsigned long mode_);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_GNUPLOT_DRAW_H_

// end of gnuplot_draw.h
