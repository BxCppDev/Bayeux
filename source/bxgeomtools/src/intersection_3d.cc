// -*- mode: c++; -*-
/* intersection_3d.cc
 */

#include <geomtools/intersection_3d.h>

#include <stdexcept>
#include <sstream>

namespace geomtools {

  // static
  const std::string & intersection_3d::intersection_3d_label()
  {
    static std::string token = "intersection_3d";
    return token;
  }

  std::string intersection_3d::get_shape_name () const
  {
    return intersection_3d_label();
  }

  intersection_3d::intersection_3d () : i_composite_shape_3d ()
  {
    return;
  }

  intersection_3d::~intersection_3d ()
  {
    return;
  }

  bool
  intersection_3d::is_inside (const vector_3d & position_,
                              double skin_) const
  {
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    const vector_3d pos1 = p1.mother_to_child (position_);
    const vector_3d pos2 = p2.mother_to_child (position_);
    return sh3d1.is_inside (pos1, skin_) && sh3d2.is_inside (pos2, skin_);
  }

  vector_3d
  intersection_3d::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    const vector_3d pos1 = p1.mother_to_child (position_);
    const vector_3d pos2 = p2.mother_to_child (position_);
    if (sh3d1.is_on_surface (pos1, ALL_SURFACES) && ! sh3d2.is_outside (pos2))
      {
        normal = sh3d1.get_normal_on_surface (pos1);
      }
    if (sh3d2.is_on_surface (pos2, ALL_SURFACES) && ! sh3d1.is_outside (pos1))
      {
        normal = sh3d2.get_normal_on_surface (pos2);
      }
    return normal;
  }

  bool
  intersection_3d::is_on_surface (const vector_3d & position_,
                                  int /*mask_*/,
                                  double skin_) const
  {
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    const vector_3d pos1 = p1.mother_to_child (position_);
    const vector_3d pos2 = p2.mother_to_child (position_);
    return (sh3d1.is_on_surface (pos1, ALL_SURFACES, skin_) && ! sh3d2.is_inside (pos2, skin_))
      || (sh3d2.is_on_surface (pos2, ALL_SURFACES, skin_)   && ! sh3d1.is_inside (pos1, skin_));
  }

  bool
  intersection_3d::find_intercept (const vector_3d & from_,
                                   const vector_3d & direction_,
                                   intercept_t & intercept_,
                                   double skin_) const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    DT_LOG_TRACE (local_priority, "Entering...");
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;
    //else skin = USING_PROPER_SKIN;
    // extract shapes' infos:
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    DT_LOG_TRACE (local_priority, "Shape 1 & 2");
    if (local_priority >= datatools::logger::PRIO_TRACE)
      {
        sh1.dump (std::cerr);
        sh2.dump (std::cerr);
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

    for (unsigned int ishape = 0; ishape < (int) NSHAPES; ishape++)
      {
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
        do
          {
            DT_LOG_TRACE (local_priority, "ishape #" << ishape << " new loop ");
            vector_3d child_from_i = pi->mother_to_child (from_i);
            intercept_t child_intercept_i;
            intercept_ok[ishape] = shi->find_intercept (child_from_i,
                                                        child_dir_i,
                                                        child_intercept_i,
                                                        skin);
            DT_LOG_TRACE (local_priority, "ishape #" << ishape << " from_i=" << from_i);
            DT_LOG_TRACE (local_priority, "ishape #" << ishape << " child_from_i=" << child_from_i);
            if (! intercept_ok[ishape])
              {
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " no intercept");
                break;
              }
            else
              {
                intercepts[ishape]
                  = pi->child_to_mother (child_intercept_i.get_impact ());
                intercept_faces[ishape] = child_intercept_i.get_face ();
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " found intercept!");
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept=" << intercepts[ishape]);
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept face=" << intercept_faces[ishape]);
                vector_3d test_outside_j = pj->mother_to_child (intercepts[ishape]);
                if (! shj->is_outside (test_outside_j))
                  {
                    DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept is valid!");
                    break;
                  }
                else
                  {
                    DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept is invalid! continue...");
                    // compare this new candidate intercept with
                    // former candidate if any:
                    bool safety_skip = false;
                    if (is_valid (last_intercept_i))
                      {
                        double last_dist
                          = (intercepts[ishape] - last_intercept_i).mag ();
                        if (last_dist < skin)
                          {
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
                    if (safety_skip)
                      {
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
            if (counter > max_counter)
              {
                DT_LOG_CRITICAL (datatools::logger::PRIO_CRITICAL, "ishape=" << ishape << " stop 'infinite' loop.");
                DT_THROW_IF (true, std::runtime_error, "I suspect infinite loop while searching for the intercept point !");
              }
          } while (1);
        DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept find loops stopped.");
      }
    DT_LOG_TRACE (local_priority, "ok1=" << intercept_ok[0]);
    DT_LOG_TRACE (local_priority, "ok2=" << intercept_ok[1]);

    intercept_.reset ();
    if (intercept_ok[0] && ! intercept_ok[1])
      {
        intercept_.set (0,intercept_faces[0], intercepts[0]);
      }
    else if (intercept_ok[1] && ! intercept_ok[0])
      {
        intercept_.set (1,intercept_faces[1], intercepts[1]);
      }
    else if (intercept_ok[0] && intercept_ok[1])
      {
        const vector_3d intercept1 = intercepts[0];
        const double dist1 = (intercept1 - from_).mag ();
        const vector_3d intercept2 = intercepts[1];
        const double dist2 = (intercept2 - from_).mag ();
        if (dist1 < dist2)
          {
            intercept_.set (0,intercept_faces[0], intercepts[0]);
          }
        else
          {
            intercept_.set (1,intercept_faces[1], intercepts[1]);
          }
      }
    return intercept_.is_ok ();
  }

} // end of namespace geomtools

// end of intersection_3d.cc
