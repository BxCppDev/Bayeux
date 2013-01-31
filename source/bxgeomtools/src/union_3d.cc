// -*- mode: c++; -*- 
/* union_3d.cc
 */

#include <geomtools/union_3d.h>

#include <stdexcept>
#include <sstream>

namespace geomtools {

  using namespace std;  

  const std::string union_3d::UNION_3D_LABEL = "union_3d";

  bool union_3d::g_devel = false;

  std::string union_3d::get_shape_name () const
  {
    return UNION_3D_LABEL;
  }

  union_3d::union_3d () : i_composite_shape_3d ()
  {
  }
  
  union_3d::~union_3d ()
  {
  }
  
  bool union_3d::is_inside (const vector_3d & position_, 
                            double skin_) const
  {
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    vector_3d pos1 = p1.mother_to_child (position_);
    vector_3d pos2 = p2.mother_to_child (position_);
    bool res = false;
    res = sh3d1.is_inside (pos1, skin_) 
      || sh3d2.is_inside (pos2, skin_);
    return res;
  }

  vector_3d 
  union_3d::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    vector_3d pos1 = p1.mother_to_child (position_);
    vector_3d pos2 = p2.mother_to_child (position_);
    if (sh3d1.is_on_surface (pos1, ALL_SURFACES) 
        && ! sh3d2.is_inside (pos2))
      {
        normal = sh3d1.get_normal_on_surface (pos1);
      }
    if (sh3d2.is_on_surface (pos2, ALL_SURFACES) 
        && ! sh3d1.is_inside (pos1))
      {
        normal = sh3d2.get_normal_on_surface (pos2);
      }
    return (normal);
  }
    
  bool 
  union_3d::is_on_surface (const vector_3d & position_, 
                           int mask_, 
                           double skin_) const
  {
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    vector_3d pos1 = p1.mother_to_child (position_);
    vector_3d pos2 = p2.mother_to_child (position_);
    bool res = false;
    res = (sh3d1.is_on_surface (pos1, ALL_SURFACES, skin_) 
           && ! sh3d2.is_inside (pos2, skin_))
      || (sh3d2.is_on_surface (pos2, ALL_SURFACES, skin_)
          && ! sh3d1.is_inside (pos1, skin_));
    return res;
  }
  
  bool 
  union_3d::find_intercept (const vector_3d & from_, 
                            const vector_3d & direction_,
                            intercept_t & intercept_,
                            double skin_) const
  {
    bool debug = false;
    debug = g_devel;
    //debug = true;
    if (debug)
      {
        std::clog << "union_3d::find_intercept: entering..." << std::endl;
      }
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;
    //else skin = USING_PROPER_SKIN;
    // extract shapes' infos:
    const shape_type & sh1 = get_shape1 ();
    const shape_type & sh2 = get_shape2 ();
    if (debug)
      {
        sh1.dump (std::clog);
        sh2.dump (std::clog);
      }
    const placement & p1 = sh1.get_placement ();
    const placement & p2 = sh2.get_placement ();
    const i_shape_3d & sh3d1 = sh1.get_shape ();
    const i_shape_3d & sh3d2 = sh2.get_shape ();
    if (debug)
      {
        std::clog << "union_3d::find_intercept: shape1 is '" 
                  << sh3d1.get_shape_name () << "'" << std::endl;
        std::clog << "union_3d::find_intercept: shape2 is '" 
                  << sh3d2.get_shape_name () << "'" << std::endl;
      }
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
    vector_3d direction = direction_;
    direction.unit ();

    for (int ishape = 0; ishape < (int) NSHAPES; ishape++)
      {
        if (debug) std::clog << "DEVEL: ishape=" << ishape << std::endl;
        invalidate (intercepts[ishape]);
        intercept_ok[ishape] = false;
        intercept_faces[ishape] = FACE_NONE_BIT;
        const placement * pi = placements[ishape];
        const placement * pj = placements[(ishape + 1) % 2];
        const i_shape_3d * shi = shapes[ishape];
        const i_shape_3d * shj = shapes[(ishape + 1) % 2];
        vector_3d from_i = from_;
        vector_3d child_dir_i = pi->mother_to_child_direction (direction);
        if (debug)
          {
            std::clog << "union_3d::find_intercept: from_i=" << from_i << std::endl;
            std::clog << "union_3d::find_intercept: child_dir_i=" << child_dir_i << std::endl;
          }
        if (debug) std::clog << "DEVEL: ishape=" << ishape 
                  << " start intercept find loops..."
                  << std::endl; 
        vector_3d last_intercept_i;
        invalidate (last_intercept_i);
        size_t counter = 0;
        size_t max_counter = 1000;
        do
          {
            if (debug) std::clog << "DEVEL: ishape=" << ishape 
                                 << " new loop "
                                 << std::endl; 
            vector_3d child_from_i = pi->mother_to_child (from_i);
            intercept_t child_intercept_i;
            intercept_ok[ishape] = shi->find_intercept (child_from_i, 
                                                        child_dir_i, 
                                                        child_intercept_i,
                                                        skin);
            if (debug)
              { 
                std::clog << "DEVEL: ishape=" << ishape 
                          << " from_i=" << from_i
                          << std::endl; 
                std::clog << "DEVEL: ishape=" << ishape 
                          << " child_from_i=" << child_from_i
                          << std::endl; 
              }
            if (! intercept_ok[ishape]) 
              {
                if (debug) std::clog << "DEVEL: ishape=" << ishape 
                          << " no intercept!"
                          << std::endl; 
                break;
              }
            else
              {
                intercepts[ishape] = 
                  pi->child_to_mother (child_intercept_i.get_impact ());
                intercept_faces[ishape] = child_intercept_i.get_face ();
                if (debug) 
                  {
                    std::clog << "DEVEL: ishape=" << ishape 
                              << " found intercept!"
                              << std::endl; 
                    std::clog << "DEVEL: ishape=" << ishape 
                              << " intercept=" << intercepts[ishape]
                              << std::endl; 
                    std::clog << "DEVEL: ishape=" << ishape 
                              << " intercept face=" << intercept_faces[ishape]
                              << std::endl; 
                  }
                vector_3d test_inside_j = pj->mother_to_child (intercepts[ishape]);
                if (! shj->is_inside (test_inside_j))
                  {
                    if (debug) std::clog << "DEVEL: ishape=" << ishape 
                              << " intercept is valid!"
                              << std::endl; 
                    break;
                  }
                else
                  {
                    if (debug) std::clog << "DEVEL: ishape=" << ishape 
                              << " intercept is invalid! continue..."
                              << std::endl; 
                    // compare this new candidate intercept with
                    // former candidate if any:
                    bool safety_skip = false;
                    if (is_valid (last_intercept_i))
                      {
                        double last_dist = (intercepts[ishape] - last_intercept_i).mag ();
                        if (last_dist < skin)
                          {
                            if (debug) std::clog << "DEVEL: ishape=" << ishape 
                                      << " successive intercepts are very close (tolerance)!"
                                      << std::endl; 
                            safety_skip = true;
                          }
                      }
                    // restart search from the last rejected candidate
                    // intercept:
                    last_intercept_i = intercepts[ishape];
                    if (debug) std::clog << "DEVEL: ishape=" << ishape 
                                         << " skin=" << skin
                                         << std::endl; 
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
                    if (debug) 
                      {
                        std::clog << "DEVEL: ishape=" << ishape 
                                  << " direction=" << direction
                                  << std::endl; 
                        std::clog << "DEVEL: ishape=" << ishape 
                                  << " new from_i=" << from_i
                                  << std::endl; 
                      }
                    invalidate (intercepts[ishape]);
                    intercept_ok[ishape] = false;
                    intercept_faces[ishape] = FACE_NONE_BIT;
                  }
              }
            counter++;
            if (counter > max_counter)
              {
                std::clog << "DEVEL: CRITICAL ishape=" << ishape 
                          << " stop 'infinite' loop."
                          << std::endl; 
                std::ostringstream message;
                message << "union_3d::find_intercept: "
                        << "I suspect infinite loop while searching for "
                        << "intercept point! Abort!";
                throw std::runtime_error (message.str());
                //exit (1);
                //break;
              }
          } while (1);
        if (debug) std::clog << "DEVEL: ishape=" << ishape 
                  << " intercept find loops stopped."
                  << std::endl; 
      }
    if (debug)
      {
        std::clog << "union_3d::find_intercept: ok1=" 
                  << intercept_ok[0] << std::endl;
        std::clog << "union_3d::find_intercept: ok2="
                  << intercept_ok[1] << std::endl;
      }
    intercept_.reset ();
    if (intercept_ok[0] && ! intercept_ok[1])
      {
        intercept_.set (0,intercept_faces[0], intercepts[0]); 
      }
    else if (intercept_ok[1] && ! intercept_ok[0])
      {
        intercept_.set (1,intercept_faces[1], intercepts[1]); 
      }
    if (intercept_ok[0] && intercept_ok[1])
      {
        vector_3d intercept1 = intercepts[0];
        double dist1 = (intercept1 - from_).mag ();
        vector_3d intercept2 = intercepts[1];
        double dist2 = (intercept2 - from_).mag ();
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

// end of union_3d.cc
