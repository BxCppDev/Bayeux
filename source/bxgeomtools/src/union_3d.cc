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
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(union_3d, "geomtools::union_3d")

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

  union_3d::union_3d ()
    : i_composite_shape_3d ()
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
    double skin = get_skin(skin_);
    const shape_type & sh1 = get_shape1();
    const shape_type & sh2 = get_shape2();
    const placement & p1 = sh1.get_placement();
    const placement & p2 = sh2.get_placement();
    const i_shape_3d & sh3d1 = sh1.get_shape();
    const i_shape_3d & sh3d2 = sh2.get_shape();
    vector_3d pos1;
    p1.mother_to_child(position_, pos1);
    vector_3d pos2;
    p2.mother_to_child(position_, pos2);
    if (sh3d1.check_inside(pos1, skin) || sh3d2.check_inside(pos2, skin) ) {
      return true;
    }
    return false;
  }

  bool union_3d::is_outside (const vector_3d & position_,
                             double skin_) const
  {
    double skin = get_skin(skin_);
    const shape_type & sh1 = get_shape1();
    const shape_type & sh2 = get_shape2();
    const placement & p1 = sh1.get_placement();
    const placement & p2 = sh2.get_placement();
    const i_shape_3d & sh3d1 = sh1.get_shape();
    const i_shape_3d & sh3d2 = sh2.get_shape();
    vector_3d pos1;
    p1.mother_to_child(position_, pos1);
    vector_3d pos2;
    p2.mother_to_child(position_, pos2);
    if (sh3d1.check_outside(pos1, skin) && sh3d2.check_outside(pos2, skin) ) {
      return true;
    }
    return false;
  }

  vector_3d
  union_3d::get_normal_on_surface (const vector_3d & a_position,
                                   const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid union!");
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
      vector_3d normal_2 = sh3d2.get_normal_on_surface(pos2, fid2);
      p2.child_to_mother_direction(normal_2, normal);
    }
    return normal;
  }

  // virtual
  face_identifier union_3d::on_surface(const vector_3d & a_position,
                                       const face_identifier & a_surface_mask,
                                       double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid union!");
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
        if (! sh3d1.check_inside(pos1, skin)) {
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
  union_3d::find_intercept (const vector_3d & from_,
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
        if (! sh3d1.is_inside(impact2_1, skin)) {
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

  void union_3d::generate_wires_self(wires_type & wires_, uint32_t options_) const
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
          boost_linear_sampling(sh2_options);
          boost_linear_sampling(sh2_options);
          w3dr2->generate_wires(sh2_wires, sh2_pl, sh2_options);
          SSS.add(sh2_wires);
          SSS.fetch_outside_wires(wires_);
          SSS.clear();
        }

        return;
      } else {
        DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Cannot render this union_3d shape!");
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

    DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Cannot render this union_3d shape!");
    return;
  }

} // end of namespace geomtools
