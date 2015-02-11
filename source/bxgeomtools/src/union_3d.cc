/** \file geomtools/union_3d.cc */

// Ourselves:
#include <geomtools/union_3d.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(union_3d, "geomtools::union_3d");

  const std::string & union_3d::union_3d_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "union_3d";
    }
    return label;
  }

  std::string union_3d::get_shape_name () const
  {
    return union_3d_label();
  }

  union_3d::union_3d () : i_composite_shape_3d ()
  {
    return;
  }

  union_3d::~union_3d ()
  {
    return;
  }

  bool union_3d::is_inside (const vector_3d & position_,
                            double skin_) const
  {
    const shape_type & sh1 = get_shape1();
    const shape_type & sh2 = get_shape2();
    const placement & p1 = sh1.get_placement();
    const placement & p2 = sh2.get_placement();
    const i_shape_3d & sh3d1 = sh1.get_shape();
    const i_shape_3d & sh3d2 = sh2.get_shape();
    const vector_3d pos1 = p1.mother_to_child(position_);
    const vector_3d pos2 = p2.mother_to_child(position_);
    if (    sh3d1.is_inside(pos1, skin_)
            || sh3d2.is_inside(pos2, skin_) ) {
      return true;
    }
    return false;
  }

  bool union_3d::is_outside (const vector_3d & position_,
                             double skin_) const
  {
    if (is_inside(position_, skin_)) {
      return false;
    }
    if (skin_ != get_zero_skin()) {
      if (is_on_surface(position_, COMPONENT_SHAPE_ALL, skin_)) {
        return false;
      }
    }
    return true;
  }

  vector_3d
  union_3d::get_normal_on_surface(const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate(normal);
    const shape_type & sh1 = get_shape1();
    const shape_type & sh2 = get_shape2();
    const placement & p1 = sh1.get_placement();
    const placement & p2 = sh2.get_placement();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    const vector_3d pos1 = p1.mother_to_child (position_);
    const vector_3d pos2 = p2.mother_to_child (position_);
    if (sh3d1.is_on_surface (pos1, ALL_SURFACES) && ! sh3d2.is_inside (pos2)) {
      normal = sh3d1.get_normal_on_surface (pos1);
    }
    if (sh3d2.is_on_surface (pos2, ALL_SURFACES) && ! sh3d1.is_inside (pos1)) {
      normal = sh3d2.get_normal_on_surface (pos2);
    }
    return normal;
  }

  bool
  union_3d::is_on_surface(const vector_3d & position_,
                          int mask_,
                          double skin_) const
  {
    const shape_type & sh1 = get_shape1();
    const shape_type & sh2 = get_shape2();
    const placement & p1 = sh1.get_placement();
    const placement & p2 = sh2.get_placement();
    const i_shape_3d & sh3d1 = sh1.get_shape();
    const i_shape_3d & sh3d2 = sh2.get_shape();
    const vector_3d pos1 = p1.mother_to_child(position_);
    const vector_3d pos2 = p2.mother_to_child(position_);

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = COMPONENT_SHAPE_ALL;

    if (mask & COMPONENT_SHAPE_FIRST) {
      if (sh3d1.is_on_surface(pos1, ALL_SURFACES, skin_)
          && ! sh3d2.is_inside(pos2, skin_)) {
        return true;
      }
    }
    if (mask & COMPONENT_SHAPE_SECOND) {
      if (sh3d2.is_on_surface(pos2, ALL_SURFACES, skin_)
          && ! sh3d1.is_inside(pos1, skin_)) {
        return true;
      }
    }
    return false;
  }

  bool
  union_3d::find_intercept (const vector_3d & from_,
                            const vector_3d & direction_,
                            intercept_t & intercept_,
                            double skin_) const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    DT_LOG_TRACE (local_priority, "Entering...");
    double skin = get_skin (skin_);

    // extract shapes' infos:
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    DT_LOG_TRACE (local_priority, "Shape 1 & 2");
    if (local_priority >= datatools::logger::PRIO_TRACE) {
      sh1.dump(std::cerr);
      sh2.dump(std::cerr);
    }
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    DT_LOG_TRACE (local_priority, "shape1 is '" << sh3d1.get_shape_name () << "'");
    DT_LOG_TRACE (local_priority, "shape2 is '" << sh3d2.get_shape_name () << "'");

    const size_t NSHAPES = 2;
    vector_3d intercepts[NSHAPES];
    int intercept_faces[NSHAPES];
    bool intercept_ok[NSHAPES];
    const i_shape_3d * shapes[NSHAPES];
    const placement * placements[NSHAPES];
    shapes[0] = &sh3d1;
    shapes[1] = &sh3d2;
    placements[0] = &p1;
    placements[1] = &p2;

    // normalize direction (mother ref):
    const vector_3d direction = direction_.unit ();

    for (unsigned int ishape = 0; ishape < (int) NSHAPES; ishape++) {
        DT_LOG_TRACE (local_priority, "ishape #" << ishape);
        invalidate (intercepts[ishape]);
        intercept_ok[ishape] = false;
        intercept_faces[ishape] = geomtools::FACE_NONE;
        const placement * pi = placements[ishape];
        const placement * pj = placements[(ishape + 1) % 2];
        const i_shape_3d * shi = shapes[ishape];
        const i_shape_3d * shj = shapes[(ishape + 1) % 2];
        vector_3d from_i = from_;
        vector_3d child_dir_i = pi->mother_to_child_direction (direction);
        DT_LOG_TRACE (local_priority, "from_i=" << from_i);
        DT_LOG_TRACE (local_priority, "child_dir_i=" << child_dir_i);
        DT_LOG_TRACE (local_priority, "ishape #" << ishape << " start intercept find loops...");
        vector_3d last_intercept_i;
        invalidate (last_intercept_i);
        size_t counter = 0;
        size_t max_counter = 1000;
        do {
            DT_LOG_TRACE (local_priority, "ishape #" << ishape << " new loop ");
            vector_3d child_from_i = pi->mother_to_child (from_i);
            intercept_t child_intercept_i;
            intercept_ok[ishape] = shi->find_intercept (child_from_i,
                                                        child_dir_i,
                                                        child_intercept_i,
                                                        skin);
            DT_LOG_TRACE (local_priority, "ishape #" << ishape << " from_i=" << from_i);
            DT_LOG_TRACE (local_priority, "ishape #" << ishape << " child_from_i=" << child_from_i);

            if (! intercept_ok[ishape]) {
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " no intercept");
                break;
              } else {
                intercepts[ishape] =
                  pi->child_to_mother (child_intercept_i.get_impact ());
                intercept_faces[ishape] = child_intercept_i.get_face ();
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " found intercept!");
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept=" << intercepts[ishape]);
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept face=" << intercept_faces[ishape]);
                vector_3d test_inside_j = pj->mother_to_child (intercepts[ishape]);
                if (! shj->is_inside (test_inside_j)) {
                  DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept is valid!");
                  break;
                } else {
                  DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept is invalid! continue...");
                    // compare this new candidate intercept with
                    // former candidate if any:
                    bool safety_skip = false;
                    if (is_valid (last_intercept_i)) {
                      const double last_dist = (intercepts[ishape] - last_intercept_i).mag ();
                      if (last_dist < skin) {
                        DT_LOG_TRACE (local_priority, "ishape #" << ishape <<
                                      " successive intercepts are very close (tolerance)!");
                        safety_skip = true;
                      }
                    }
                    // restart search from the last rejected candidate
                    // intercept:
                    last_intercept_i = intercepts[ishape];
                    DT_LOG_TRACE (local_priority, "ishape #" << ishape << " skin=" << skin);
                    double skin_step = skin;
                    // ensure non-zero step forward:
                    if (skin_step <= 0.0) skin_step = i_shape_3d::DEFAULT_SKIN;
                    from_i = intercepts[ishape] + skin_step * direction;
                    // experimental:
                    if (safety_skip) {
                      geomtools::vector_3d orth_dir;
                      geomtools::randomize_orthogonal_direction (geomtools::random_tools::random_flat,
                                                                 direction,
                                                                 orth_dir);

                      from_i += 0.5 * skin_step * orth_dir;
                    }
                    DT_LOG_TRACE (local_priority, "ishape #" << ishape << " direction=" << direction);
                    DT_LOG_TRACE (local_priority, "ishape #" << ishape  << " new from_i=" << from_i);
                    invalidate (intercepts[ishape]);
                    intercept_ok[ishape] = false;
                    intercept_faces[ishape] = geomtools::FACE_NONE;
                  }
              }
            counter++;
            if (counter > max_counter) {
              DT_LOG_CRITICAL (datatools::logger::PRIO_CRITICAL, "ishape=" << ishape << " stop 'infinite' loop.");
              DT_THROW_IF (true, std::runtime_error, "I suspect infinite loop while searching for the intercept point !");
            }
          } while (1);
        DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept find loops stopped.");
      }
    DT_LOG_TRACE (local_priority, "ok1=" << intercept_ok[0]);
    DT_LOG_TRACE (local_priority, "ok2=" << intercept_ok[1]);

    intercept_.reset ();
    if (intercept_ok[0] && ! intercept_ok[1]) {
      intercept_.set (0,intercept_faces[0], intercepts[0]);
    } else if (intercept_ok[1] && ! intercept_ok[0]) {
      intercept_.set (1,intercept_faces[1], intercepts[1]);
    }
    if (intercept_ok[0] && intercept_ok[1]) {
      const vector_3d intercept1 = intercepts[0];
      const double dist1 = (intercept1 - from_).mag ();
      const vector_3d intercept2 = intercepts[1];
      const double dist2 = (intercept2 - from_).mag ();
      if (dist1 < dist2) {
        intercept_.set (0,intercept_faces[0], intercepts[0]);
      } else {
        intercept_.set (1,intercept_faces[1], intercepts[1]);
      }
    }
    return intercept_.is_ok ();
  }

  void union_3d::_build_bounding_data()
  {
    if (get_shape1().get_shape().has_bounding_data()
        && get_shape2().get_shape().has_bounding_data()) {
      const placement & pl1 = get_shape1().get_placement();
      const placement & pl2 = get_shape2().get_placement();
      box bb1;
      placement bbpl1;
      get_shape1().get_shape().get_bounding_data().compute_bounding_box(bb1, bbpl1);
      std::vector<vector_3d> vv1;
      bb1.compute_transformed_vertexes(bbpl1, vv1);
      box bb2;
      placement bbpl2;
      get_shape2().get_shape().get_bounding_data().compute_bounding_box(bb2, bbpl2);
      std::vector<vector_3d> vv2;
      bb2.compute_transformed_vertexes(bbpl2, vv2);
      std::vector<vector_3d> points;
      points.reserve(vv1.size() + vv2.size());
      for (int i = 0; i < (int) vv1.size(); i++) {
        vector_3d vtx;
        pl1.child_to_mother(vv1[i], vtx);
        points.push_back(vtx);
      }
      for (int i = 0; i < (int) vv2.size(); i++) {
        vector_3d vtx;
        pl2.child_to_mother(vv2[i], vtx);
        points.push_back(vtx);
      }
      _grab_bounding_data().make_box_from_points(points);
    }
    return;
  }


  // void union_3d::generate_wires(std::list<polyline_3d> & lpl_,
  //                            const placement & p_,
  //                            uint32_t /*options_*/) const
  // {
  //   const i_shape_3d & sh1 = get_shape1().get_shape();
  //   const i_shape_3d & sh2 = get_shape2().get_shape();
  //   std::list<polyline_3d> shpl1;
  //   sh1.generate_wires(shpl1, p_, 0);
  //   for (std::list<polyline_3d>::const_iterator ipl = shpl1.begin();
  //     ipl != shpl1.end();
  //     ipl++) {
  //     const polyline_3d & pl1 = *ipl;
  //   }
  //   return;
  // }


} // end of namespace geomtools
