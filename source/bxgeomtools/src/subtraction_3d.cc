/** \file geomtools/subtraction_3d.cc */

// Ourselves:
#include <geomtools/subtraction_3d.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/box.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(subtraction_3d, "geomtools::subtraction_3d");

  const std::string & subtraction_3d::subtraction_3d_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "subtraction_3d";
    }
    return label;
  }

  std::string subtraction_3d::get_shape_name () const
  {
    return subtraction_3d_label();
  }

  subtraction_3d::subtraction_3d () : i_composite_shape_3d ()
  {
    return;
  }

  subtraction_3d::~subtraction_3d ()
  {
    return;
  }

  bool
  subtraction_3d::is_inside(const vector_3d & position_,
                            double skin_) const
  {
    double skin = get_skin(skin_);
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    vector_3d pos1;
    p1.mother_to_child(position_, pos1);
    vector_3d pos2;
    p2.mother_to_child(position_, pos2);
    if ( sh3d1.check_inside(pos1, skin) && sh3d2.check_outside(pos2, skin) ) {
      return true;
    }
    return false;
  }

  bool subtraction_3d::is_outside (const vector_3d & position_,
                                   double skin_) const
  {
    double skin = get_skin(skin_);
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    vector_3d pos1;
    p1.mother_to_child(position_, pos1);
    vector_3d pos2;
    p2.mother_to_child(position_, pos2);
    if ( sh3d1.check_outside(pos1, skin) || sh3d2.check_inside(pos2, skin) ) {
      return true;
    }
    return false;
  }

  vector_3d
  subtraction_3d::get_normal_on_surface (const vector_3d & a_position,
                                         const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid subtraction!");
    vector_3d normal;
    invalidate(normal);
    DT_THROW_IF(! a_surface_bit.has_parts(), std::logic_error,
                "Face identifier has no part!");
    uint32_t part = a_surface_bit.get_part(0);
    DT_THROW_IF(part > SECOND_PART, std::logic_error, "Invalid part index [" << part << "]!");
    if (part == FIRST_PART) {
      const shape_type & sh1   = get_shape1();
      const placement  & p1    = sh1.get_placement();
      const i_shape_3d & sh3d1 = sh1.get_shape();
      vector_3d pos1;
      p1.mother_to_child(a_position, pos1);
      face_identifier fid1;
      a_surface_bit.inherit_parts(fid1, 1);
      vector_3d normal_1 = sh3d1.get_normal_on_surface(pos1, fid1);
      p1.child_to_mother_direction(normal_1, normal);
    } else if (part == SECOND_PART) {
      const shape_type & sh2   = get_shape2();
      const placement &  p2    = sh2.get_placement();
      const i_shape_3d & sh3d2 = sh2.get_shape();
      vector_3d pos2;
      p2.mother_to_child(a_position, pos2);
      face_identifier fid2;
      a_surface_bit.inherit_parts(fid2, 1);
      vector_3d normal_2 = -sh3d2.get_normal_on_surface(pos2, fid2);
      p2.child_to_mother_direction(normal_2, normal);
    }
    return normal;
  }

  // virtual
  face_identifier subtraction_3d::on_surface(const vector_3d & a_position,
                                             const face_identifier & a_surface_mask,
                                             double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid subtraction!");
    double skin = get_skin(a_skin);

    face_identifier mask;
    if (a_surface_mask.is_valid()) {
      DT_THROW_IF(! a_surface_mask.has_parts(), std::logic_error,
                  "Face mask has no part!");
      mask = a_surface_mask;
    } else {
      mask.prepend_part(face_identifier::PART_INDEX_ANY);
    }

    for (uint32_t ipart = FIRST_PART; ipart <= SECOND_PART; ipart++) {

      if (!mask.match_part(0, ipart)) {
        // Deactivate this part:
        continue;
      }

      const shape_type & sh1   = get_shape1();
      const placement & p1     = sh1.get_placement();
      const i_shape_3d & sh3d1 = sh1.get_shape();
      const vector_3d pos1     = p1.mother_to_child(a_position);

      const shape_type & sh2   = get_shape2();
      const placement & p2     = sh2.get_placement();
      const i_shape_3d & sh3d2 = sh2.get_shape();
      const vector_3d pos2     = p2.mother_to_child(a_position);

      if (ipart == FIRST_PART) {
        face_identifier fid1;
        if (mask.can_inherit_parts(1)) {
          mask.inherit_parts(fid1, 1);
        } else {
          sh3d1.make_any_face(fid1);
        }
        if (! sh3d2.check_inside(pos2, skin)) {
          face_identifier on_fid = sh3d1.on_surface(pos1, fid1, skin);
          if (on_fid.is_ok()) {
            on_fid.prepend_part(FIRST_PART);
            return on_fid;
          }
        }
      }

      if (ipart == SECOND_PART) {
        face_identifier fid2;
        if (mask.can_inherit_parts(1)) {
          mask.inherit_parts(fid2, 1);
        } else {
          sh3d2.make_any_face(fid2);
        }
        if (! sh3d1.check_outside(pos1, skin)) {
          face_identifier on_fid = sh3d2.on_surface(pos2, fid2, skin);
          if (on_fid.is_ok()) {
            on_fid.prepend_part(SECOND_PART);
            return on_fid;
          }
        }
      }
    }

    return face_identifier::face_invalid();
  }

  bool
  subtraction_3d::find_intercept (const vector_3d & from_,
                                  const vector_3d & direction_,
                                  face_intercept_info & intercept_,
                                  double skin_) const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    DT_LOG_TRACE (local_priority, "Entering...");
    intercept_.reset ();
    double skin = get_skin (skin_);

    const shape_type & sh1   = get_shape1();
    const placement &  p1    = sh1.get_placement();
    const i_shape_3d & sh3d1 = sh1.get_shape();

    const shape_type & sh2   = get_shape2();
    const placement &  p2    = sh2.get_placement();
    const i_shape_3d & sh3d2 = sh2.get_shape();

    vector_3d direction = direction_.unit();

    double min_len;
    datatools::invalidate(min_len);
    face_intercept_info fii;
    size_t max_counter = 1000;

    // Search for candidate intercept point(s) on the first shape:
    vector_3d from1;
    p1.mother_to_child(from_, from1);
    vector_3d dir1;
    p1.mother_to_child_direction(direction, dir1);
    size_t counter = 0;
    do {
      face_intercept_info fii1;
      if (sh3d1.find_intercept(from1, dir1, fii1, skin)) {
        vector_3d impact1;
        p1.child_to_mother(fii1.get_impact(), impact1);
        double len1 = (impact1 - from_).mag();
        vector_3d impact1_2;
        p2.mother_to_child(impact1, impact1_2);
        if (! sh3d2.is_inside(impact1_2, skin)) {
          if (!datatools::is_valid(min_len) || len1 < min_len) {
            min_len = len1;
            fii = fii1;
            fii.grab_face_id().prepend_part(FIRST_PART);
            fii.set_impact(impact1);
          }
          break;
        } else {
          // Candidate intercept point was rejected, try another one:
          double safe_skin_step = skin;
          from1 += (safe_skin_step * dir1);
        }
      } else {
        // Not candidate intercept point was found, exit the loop:
        break;
      }
      counter++;
      DT_THROW_IF(counter > max_counter, std::runtime_error,
                  "Suspicion of infinite loop while searching for the intercept point!");
    } while (1);

    // Search for candidate intercept point(s) on the second shape:
    vector_3d from2;
    p2.mother_to_child(from_, from2);
    vector_3d dir2;
    p2.mother_to_child_direction(direction, dir2);
    counter = 0;
    do {
      face_intercept_info fii2;
      if (sh3d2.find_intercept(from2, dir2, fii2, skin)) {
        vector_3d impact2;
        p2.child_to_mother(fii2.get_impact(), impact2);
        double len2 = (impact2 - from_).mag();
        vector_3d impact2_1;
        p1.mother_to_child(impact2, impact2_1);
        if (! sh3d1.is_outside(impact2_1, skin)) {
          if (!datatools::is_valid(min_len) || len2 < min_len) {
            min_len = len2;
            fii = fii2;
            fii.grab_face_id().prepend_part(SECOND_PART);
            fii.set_impact(impact2);
          }
          break;
        } else {
          // Candidate intercept point was rejected, try another one:
          double safe_skin_step = skin;
          from2 += (safe_skin_step * dir2);
        }
      } else {
        // Not candidate intercept point was found, exit the loop:
        break;
      }
      counter++;
      DT_THROW_IF(counter > max_counter, std::runtime_error,
                  "Suspicion of infinite loop while searching for the intercept point!");
    } while (1);

    if (fii.is_ok()) {
      intercept_ = fii;
    }

    return intercept_.is_ok();
  }

  /*
    // extract shapes' infos:
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    DT_LOG_TRACE (local_priority, "Shape 1 & 2");
    if (local_priority >= datatools::logger::PRIO_TRACE) {
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
        do {
            DT_LOG_TRACE (local_priority, "ishape #" << ishape << " new loop ");
            vector_3d child_from_i = pi->mother_to_child (from_i);
            face_intercept_info child_intercept_i;
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
                intercepts[ishape]
                  = pi->child_to_mother (child_intercept_i.get_impact ());
                intercept_faces[ishape] = child_intercept_i.get_face ();
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " found intercept!");
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept=" << intercepts[ishape]);
                DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept face=" << intercept_faces[ishape]);
                vector_3d test_inside_j = pj->mother_to_child (intercepts[ishape]);
                bool ok = false;
                // if (ishape == 0) ok = shj->is_outside (test_inside_j);
                // else ok = ! shj->is_outside (test_inside_j);
                // XXX to be checked:
                if (ishape == 0) ok = ! shj->is_inside (test_inside_j);
                else ok = ! shj->is_outside (test_inside_j);
                if (ok) {
                    DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept is valid!");
                    break;
                  } else {
                    DT_LOG_TRACE (local_priority, "ishape #" << ishape << " intercept is invalid! continue...");
                    // compare this new candidate intercept with
                    // former candidate if any:
                    bool safety_skip = false;
                    if (is_valid (last_intercept_i)) {
                        double last_dist = (intercepts[ishape] - last_intercept_i).mag ();
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

    if (intercept_ok[0] && ! intercept_ok[1]) {
      intercept_.grab_face_id().append_part(0);
      intercept_.grab_face_id().set_face_bits(0x7FFFFFFF);
      intercept_.set_impact(intercepts[0]);
      // intercept_.set (0,intercept_faces[0], intercepts[0]);
    } else if (intercept_ok[1] && ! intercept_ok[0]) {
      intercept_.grab_face_id().append_part(1);
      intercept_.set_impact(intercepts[1]);
      // intercept_.set (1,intercept_faces[1], intercepts[1]);
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
    return intercept_.is_ok();
  }
*/

  void subtraction_3d::_build_bounding_data()
  {
    if (get_shape1().get_shape().has_bounding_data()) {
      const placement & pl1 = get_shape1().get_placement();
      box bb1;
      placement bbpl1;
      get_shape1().get_shape().get_bounding_data().compute_bounding_box(bb1, bbpl1);
      std::vector<vector_3d> vv1;
      bb1.compute_transformed_vertexes(bbpl1, vv1);
      std::vector<vector_3d> points;
      points.reserve(vv1.size());
      for (int i = 0; i < (int) vv1.size(); i++) {
        vector_3d vtx;
        pl1.child_to_mother(vv1[i], vtx);
        points.push_back(vtx);
      }
      _grab_bounding_data().make_box_from_points(points);
      /*
      if (pl1.is_identity()) {
        _grab_bounding_data() = get_shape1().get_shape().get_bounding_data();
      }
      */
    }
    return;
  }

  void subtraction_3d::generate_wires_self(wires_type & wires_,
                                           uint32_t options_) const
  {
    bool draw_only_bb       = (options_ & WR_COMPOSITE_ONLY_BB);
    bool draw_first_volume  = (options_ & WR_COMPOSITE_DRAW_FIRST_SOLID);
    bool draw_second_volume = (options_ & WR_COMPOSITE_DRAW_SECOND_SOLID);
    bool boost_sampling     = (options_ & WR_COMPOSITE_BOOST_SAMPLING);

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;
    if (boost_sampling) {
      boost_linear_sampling(base_options);
    }

    // First shape:
    const i_composite_shape_3d::shape_type & s1 = get_shape1();
    const i_shape_3d & sh1 = s1.get_shape();
    const placement & sh1_pl = s1.get_placement();

    // Second shape:
    const i_composite_shape_3d::shape_type & s2 = get_shape2();
    const i_shape_3d & sh2 = s2.get_shape();
    const placement & sh2_pl = s2.get_placement();

    const i_wires_3d_rendering * w3dr1 = dynamic_cast<const i_wires_3d_rendering *>(&sh1);
    const i_wires_3d_rendering * w3dr2 = dynamic_cast<const i_wires_3d_rendering *>(&sh2);

   {
      double min_dim1;
      datatools::invalidate(min_dim1);
      if (sh1.has_bounding_data()) {
        min_dim1 = sh1.get_bounding_data().get_min_dimension();
      }
      double min_dim2;
      datatools::invalidate(min_dim2);
      if (sh2.has_bounding_data()) {
        min_dim2 = sh2.get_bounding_data().get_min_dimension();
      }
      double step1 = min_dim1 / 100;
      double tolerance1 = 0.0;
      double step2 = min_dim2 / 100;
      double tolerance2 = 0.0;

      if (w3dr1 != 0 && w3dr2 != 0) {
        geomtools::split_segment_wrt_shape SSS;
        {
          SSS.configure(sh2, sh2_pl, step2, tolerance2);
          wires_type sh1_wires;
          uint32_t sh1_options = base_options;
          w3dr1->generate_wires(sh1_wires, sh1_pl, sh1_options);
          SSS.add(sh1_wires);
          SSS.fetch_outside_wires(wires_);
          SSS.clear();
        }

        {
          SSS.configure(sh1, sh1_pl, step1, tolerance1);
          wires_type sh2_wires;
          uint32_t sh2_options = base_options;
          w3dr2->generate_wires(sh2_wires, sh2_pl, sh2_options);
          SSS.add(sh2_wires);
          SSS.fetch_inside_wires(wires_);
          SSS.fetch_surface_wires(wires_);
          SSS.clear();
        }

        return;
      } else {
        DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Cannot render this intersection_3d shape!");
      }
    }

    if (has_bounding_data()) {
      draw_only_bb = true;
      if (draw_only_bb) {
        // Fall back to the rendering of the bounding box:
        box ibb;
        placement ibbp;
        get_bounding_data().compute_bounding_box(ibb, ibbp);
        wires_type pls;
        ibb.generate_wires(wires_, ibbp, 0);
        return;
      }
    }


    if (w3dr1 != 0 && w3dr2 != 0) {
      // Fall back to the rendering of both solids:
      draw_first_volume = true;
      draw_second_volume = true;
      if (draw_first_volume || draw_second_volume) {
        if (draw_first_volume) {
          uint32_t sh1_options = base_options;
          w3dr1->generate_wires(wires_, sh1_pl, sh1_options);
        }

        if (draw_second_volume) {
          uint32_t sh2_options = base_options;
          w3dr2->generate_wires(wires_, sh2_pl, sh2_options);
        }
        return;
      }
    }

    DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Cannot render this intersection_3d shape!");

    return;
  }

} // end of namespace geomtools
