// i_find_intercept.cc

// Ourselves:
#include <geomtools/i_find_intercept.h>


// This project:
#include <geomtools/placement.h>

namespace geomtools {

  i_find_intercept::i_find_intercept()
  {
    return;
  }

  i_find_intercept::~i_find_intercept()
  {
    return;
  }

  bool i_find_intercept::find_intercept_self(const vector_3d & a_from,
                                             const vector_3d & a_direction,
                                             face_intercept_info & a_intercept,
                                             double a_skin) const
  {
   return find_intercept(a_from, a_direction, a_intercept, a_skin);
   /*
    if (found) {
      if (a_intercept.get_shape_index() < 0) {
        a_intercept.set_shape_index(0);
      }
      if (a_intercept.get_face() == FACE_NONE) {
        a_intercept.set_face(FACE_ALL_BITS);
      }
    }
    return found;
   */
  }

  bool i_find_intercept::find_intercept(const vector_3d & a_from,
                                        const vector_3d & a_direction,
                                        const placement & a_placement,
                                        face_intercept_info & a_intercept,
                                        double a_skin) const
  {
    a_intercept.reset();
    vector_3d from_c;
    vector_3d direction_c;
    // Transform to the global coordinate system:
    a_placement.mother_to_child(a_from, from_c);
    a_placement.mother_to_child_direction(a_direction, direction_c);
    bool success = find_intercept_self(from_c, direction_c, a_intercept, a_skin);
    if (success) {
      // Recompute the impact in the global coordinate system:
      vector_3d impact;
      a_placement.child_to_mother(a_intercept.get_impact(), impact);
      a_intercept.set_impact(impact);
    }
    return a_intercept.is_ok();
  }

} // end of namespace geomtools
