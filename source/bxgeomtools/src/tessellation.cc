// tessellation.cc

/*
  http://www.blackpawn.com/texts/pointinpoly/default.html
  http://realtimecollisiondetection.net/books/rtcd/
*/

// Ourselves:
#include <geomtools/tessellation.h>

// Standard library:
#include <sstream>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>
#include <set>

// Third party:
// - Boost:
#include <boost/spirit/include/support_istream_iterator.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/box.h>
#include <geomtools/stl_tools.h>
#include <geomtools/triangle.h>
#include <geomtools/quadrangle.h>
#include <geomtools/line_3d.h>

namespace geomtools {

  /* facet_vertex */

  bool facet_vertex::is_valid () const
  {
    return geomtools::is_valid(position) && ref_facets.size () > 2;
  }

  void facet_vertex::reset ()
  {
    geomtools::invalidate (position);
    ref_facets.clear ();
    return;
  }

  facet_vertex::facet_vertex ()
  {
    geomtools::invalidate (position);
    return;
  }

  facet_vertex::facet_vertex (double x_,
                              double y_,
                              double z_)
  {
    position.set (x_, y_, z_);
    return;
  }

  const vector_3d & facet_vertex::get_position () const
  {
    return position;
  }

  void facet_vertex::set_position(const vector_3d & p_)
  {
    position = p_;
    return;
  }

  vector_3d & facet_vertex::grab_position()
  {
    return position;
  }

  void facet_vertex::add_ref_facet (int facet_index_, int facet_node_)
  {
    ref_facets[facet_index_] = facet_node_;
    return;
  }

  void facet_vertex::print_xyz (std::ostream & out_, int color_) const
  {
    out_ << position.x () << ' ' << position.y () << ' ' << position.z () << ' ' << color_ << std::endl;
    return;
  }

  void facet_vertex::print (std::ostream & out_) const
  {
    out_ << '(' << position.x () << ' ' << position.y () << ' ' << position.z () << ')'
         << " #ref=" << ref_facets.size();
    if ( ref_facets.size()) {
      out_ << " facets=";
      for (std::map<int32_t, int32_t>::const_iterator ifr = ref_facets.begin ();
           ifr != ref_facets.end ();
           ifr++) {
        out_ << " " << ifr->first;
      }
    }
    out_ << std::endl;
    return;
  }

  std::ostream & operator<< (std::ostream & out_, const facet_vertex & vtx_)
  {
    out_ << "position=" << '(' << vtx_.position.x () << ';' << vtx_.position.y () << ';' << vtx_.position.z () << ')'
         << ";facets={";
    if ( vtx_.ref_facets.size  ()) {
      for (std::map<int32_t, int32_t>::const_iterator ifr = vtx_.ref_facets.begin ();
           ifr != vtx_.ref_facets.end ();
           ifr++) {
        if (ifr != vtx_.ref_facets.begin ())
          out_ << ";";
        out_ << ifr->first;
      }
    }
    out_ << "}";
    return out_;
  }

  /* facet34 */


  // static
  const unsigned int facet34::MAX_CATEGORY;
  const int32_t      facet34::INVALID_CATEGORY;

  uint32_t facet34::get_number_of_vertices () const
  {
    return _number_of_vertices_;
  }

  const facet_vertex & facet34::get_vertex (int i_) const
  {
    DT_THROW_IF((i_ < 0) || (i_ >= (int)_number_of_vertices_),
                std::logic_error,
                "Invalid vertex index " << i_ << " !");
    return *_vertices_[i_];
  }

  int32_t facet34::get_vertex_key (int i_) const
  {
    DT_THROW_IF((i_ < 0) || (i_ >= (int)_number_of_vertices_),
                std::logic_error,
                "Invalid vertex index " << i_ << " !");
    return _vertices_keys_[i_];
  }

  const geomtools::vector_3d & facet34::get_normal () const
  {
    return _normal_;
  }

  bool facet34::is_valid () const
  {
    if (_number_of_vertices_ < 3) return false;
    if (_number_of_vertices_ < 4) return false;
    if (! has_normal ()) return false;
    return true;
  }

  bool facet34::has_normal () const
  {
    return geomtools::is_valid (_normal_);
  }

  void facet34::_compute_normal ()
  {
    if (_number_of_vertices_ < 3 || _number_of_vertices_ > 4)
      {
        return;
      }
    const geomtools::vector_3d & v0 = _vertices_[0]->get_position ();
    const geomtools::vector_3d & v1 = _vertices_[1]->get_position ();
    const geomtools::vector_3d & v2 = _vertices_[2]->get_position ();
    const geomtools::vector_3d u01 = v1 - v0;
    const geomtools::vector_3d u12 = v2 - v1;
    geomtools::vector_3d n = u01.cross (u12);
    const double m = n.mag ();
    n /= m;
    if (_number_of_vertices_ == 4) {
      const geomtools::vector_3d & v3 = _vertices_[3]->get_position ();
      const geomtools::vector_3d u23 = v3 - v2;
      geomtools::vector_3d n2 = u12.cross (u23);
      n2 /= m;
      if (! geomtools::are_near (n, n2, 1.e-5)) {
        invalidate (_normal_);
        return;
      }
    }
    _normal_ = n;
    return;
  }

  void facet34::_set_defaults ()
  {
    _number_of_vertices_ = 0;
    for (unsigned int i = 0; i < 4; i++) {
      _vertices_[i] = 0;
      _vertices_keys_[i] = -1;
      datatools::invalidate(_internal_angles_[i]);
    }
    geomtools::invalidate(_normal_);
    datatools::invalidate(_surface_);
    _category_ = INVALID_CATEGORY;
    return;
  }

  bool facet34::has_category () const
  {
    return _category_ > INVALID_CATEGORY;
  }

  void facet34::set_category (unsigned int c_)
  {
    DT_THROW_IF (c_ > MAX_CATEGORY, std::logic_error,"Invalid category '" << c_ << "' !");
    _category_ = c_;
  }

  void facet34::unset_category ()
  {
    _category_ = INVALID_CATEGORY;
    return;
  }

  facet34::facet34 ()
  {
    _set_defaults ();
    return;
  }

  void facet34::set_triangle (const facet_vertex & v0_,
                              const facet_vertex & v1_,
                              const facet_vertex & v2_,
                              int iv0_,
                              int iv1_,
                              int iv2_)
  {
    _number_of_vertices_ = 3;
    _vertices_[0] = &v0_;
    _vertices_[1] = &v1_;
    _vertices_[2] = &v2_;
    _vertices_[3] = 0;

    DT_THROW_IF ((! check_triangle (v0_.get_position (),
                                    v1_.get_position (),
                                    v2_.get_position ())),
                 std::logic_error,
                 "Invalid set of vertices for a triangle !");
    _vertices_keys_[0] = iv0_;
    _vertices_keys_[1] = iv1_;
    _vertices_keys_[2] = iv2_;
    _vertices_keys_[3] = -1;
    _compute_internals ();
    return;
  }

  facet34::facet34 (const facet_vertex & v0_,
                    const facet_vertex & v1_,
                    const facet_vertex & v2_,
                    int iv0_,
                    int iv1_,
                    int iv2_)
  {
    _set_defaults ();
    set_triangle (v0_,
                  v1_,
                  v2_,
                  iv0_,
                  iv1_,
                  iv2_);
    return;
  }

  bool facet34::check_triangle (const geomtools::vector_3d & v0_,
                                const geomtools::vector_3d & v1_,
                                const geomtools::vector_3d & v2_,
                                double /*tolerance_*/)
  {
    // Check that all 3 vertices are different :
    const geomtools::vector_3d u01 = v1_ - v0_;
    if (u01.mag2 () == 0) {
      return false;
    }
    const geomtools::vector_3d u12 = v2_ - v1_;
    if (u12.mag2 () == 0) {
      return false;
    }
    const geomtools::vector_3d u20 = v0_ - v2_;
    if (u20.mag2 () == 0) {
      return false;
    }

    // Check non-alignment of vertices :

    // double tolerance = tolerance_;
    // if (tolerance_ == 0.0 || ! datatools::is_valid (tolerance_))
    //   {
    //     // use default (rather strict) tolerance :
    //     tolerance = 1.e-13;
    //   }

    // Vertices 0, 1, 2 :
    const geomtools::vector_3d n012 = u01.cross (u12);
    const double m012 = n012.mag ();
    if (m012 == 0.0) {
      DT_LOG_WARNING (datatools::logger::PRIO_WARNING, "Vertices 0, 1, 2 are aligned !");
      return false;
    }
    //n012 /= m012;
    return true;
  }

  bool facet34::check_quadrangle (const geomtools::vector_3d & v0_,
                                  const geomtools::vector_3d & v1_,
                                  const geomtools::vector_3d & v2_,
                                  const geomtools::vector_3d & v3_,
                                  double tolerance_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    // Check that all 4 vertices are different :
    const geomtools::vector_3d u01 = v1_ - v0_;
    if (u01.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u12 = v2_ - v1_;
    if (u12.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u23 = v3_ - v2_;
    if (u23.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u30 = v0_ - v3_;
    if (u30.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u02 = v2_ - v0_;
    if (u02.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u13 = v3_ - v1_;
    if (u13.mag2 () == 0)
      {
        return false;
      }

    // Vertices 0, 1, 2 :
    geomtools::vector_3d n012 = u01.cross (u12);
    {
      const double m012 = n012.mag ();
      if (m012 == 0.0)
        {
          DT_LOG_WARNING (local_priority, "Vertices 0, 1, 2 are aligned !");
          return false;
        }
      n012 /= m012;
    }

    // Vertices 1, 2, 3 :
    geomtools::vector_3d n123 = u12.cross (u23);
    {
      const double m123 = n123.mag ();
      if (m123 == 0.0)
        {
          DT_LOG_WARNING (local_priority, "Vertices 1, 2, 3 are aligned !");
          return false;
        }
      n123 /= m123;
    }

    // Vertices 2, 3, 0 :
    geomtools::vector_3d n230 = u23.cross (u30);
    {
      const double m230 = n230.mag ();
      if (m230 == 0.0)
        {
          DT_LOG_WARNING (local_priority, "Vertices 2, 3, 0 are aligned !");
          return false;
        }
      n230 /= m230;
    }

    // Vertices 3, 0, 1 :
    geomtools::vector_3d n301 = u30.cross (u01);
    {
      const double m301 = n301.mag ();
      if (m301 == 0.0)
        {
          DT_LOG_WARNING (local_priority, "Vertices 3, 0, 1 are aligned !");
          return false;
        }
      n301 /= m301;
    }

    // Check planarity of the quadrangle :
    DT_LOG_TRACE (local_priority, "n012=" << n012 << " vs n123=" << n123);

    // Check non-alignment of vertices :
    double tolerance = tolerance_;
    if (tolerance_ == 0.0 || ! datatools::is_valid (tolerance_))
      {
        // use default (rather strict) tolerance :
        tolerance = 1.e-6;
      }

    if (! n012.isNear (n123, tolerance))
      {
        DT_LOG_WARNING (local_priority, "Quadrangle is not convex !");
        return false;
      }

    return true;
  }

  void facet34::_compute_face()
  {
    if (is_triangle()) {
      triangle * t = new triangle;
      t->set_vertexes_ext(_vertices_[0]->get_position(),
                          _vertices_[1]->get_position(),
                          _vertices_[2]->get_position());
      _tface_.reset(t);
    }
    if (is_quadrangle()) {
      quadrangle * q = new quadrangle;
      q->set_vertexes_ext(_vertices_[0]->get_position(),
                          _vertices_[1]->get_position(),
                          _vertices_[2]->get_position(),
                          _vertices_[3]->get_position());
      _qface_.reset(q);
    }
    return;
  }

  void facet34::_compute_internal_angles ()
  {
    // Not implemented yet :
    /*
      geomtools::vector_3d u01 = _vertices_[1]->get_position () - _vertices_[0]->get_position ();
      if (u01.mag2 () == 0)
      {
      return false;
      }
      geomtools::vector_3d u12 = _vertices_[2]->get_position () - _vertices_[1]->get_position ();
      if (u12.mag2 () == 0)
      {
      return false;
      }
      if (is_triangle ())
      {
      }
      else if (is_quadrangle ())
      {
      geomtools::vector_3d u23 = _vertices_[3]->get_position () - _vertices_[2]->get_position ();
      if (u23.mag2 () == 0)
      {
      return false;
      }
      geomtools::vector_3d u20 = _vertices_[2]->get_position () - _vertices_[0]->get_position ();
      if (u20.mag2 () == 0)
      {
      return false;
      }
      bool rev = false;
      double a012 = (-u01).angle (u12);
      if (a012 > M_PI)
      {
      a012 = 2 * M_PI - a012;
      if (devel)
      {
      std::cerr << "DEVEL: geomtools::facet34::_compute_internal_angles: "
      << "reverse flag !"
      << std::endl;
      }
      rev = true;
      }
      if (devel)
      {
      std::cerr << "DEVEL: geomtools::facet34::_compute_internal_angles: "
      << "a012=" << a012 / CLHEP::degree << " °"
      << std::endl;
      }
      double a123 = (-u12).angle (u23);
      if (rev)
      {
      a123 = 2 * M_PI - a123;
      }
      if (devel)
      {
      std::cerr << "DEVEL: geomtools::facet34::_compute_internal_angles: "
      << "a123=" << a123 / CLHEP::degree << " °"
      << std::endl;
      }
      if (a123 > M_PI) return false;
      double a230 = (-u23).angle (u30);
      if (rev)
      {
      a230 = 2 * M_PI - a230;
      }
      if (devel)
      {
      std::cerr << "DEVEL: geomtools::facet34::_compute_internal_angles: "
      << "a230=" << a230 / CLHEP::degree << " °"
      << std::endl;
      }
      if (a230 > M_PI) return false;
      double a301 = (-u30).angle (u01);
      if (rev)
      {
      a301 = 2 * M_PI - a301;
      }
      if (devel)
      {
      std::cerr << "DEVEL: geomtools::facet34::_compute_internal_angles: "
      << "a301=" << a301 / CLHEP::degree << " °"
      << std::endl;
      }
      if (a301 > M_PI) return false;

      double asum = a012 + a123 + a230 + a301;
      if (devel)
      {
      std::cerr << "DEVEL: geomtools::facet34::_compute_internal_angles: "
      << "asum=" << asum / CLHEP::degree << " °"
      << std::endl;
      }
    */
    return;
  }

  void facet34::set_quadrangle (const facet_vertex & v0_,
                                const facet_vertex & v1_,
                                const facet_vertex & v2_,
                                const facet_vertex & v3_,
                                int iv0_,
                                int iv1_,
                                int iv2_,
                                int iv3_)
  {
    _number_of_vertices_ = 4;
    _vertices_[0] = &v0_;
    _vertices_[1] = &v1_;
    _vertices_[2] = &v2_;
    _vertices_[3] = &v3_;

    DT_THROW_IF ((! check_quadrangle (v0_.get_position (), v1_.get_position (),
                                      v2_.get_position (), v3_.get_position ())),
                 std::logic_error,
                 "Invalid set of vertices for a quadrangle !");

    _vertices_keys_[0] = iv0_;
    _vertices_keys_[1] = iv1_;
    _vertices_keys_[2] = iv2_;
    _vertices_keys_[3] = iv3_;
    _compute_internals ();
    return;
  }

  facet34::facet34 (const facet_vertex & v0_,
                    const facet_vertex & v1_,
                    const facet_vertex & v2_,
                    const facet_vertex & v3_,
                    int iv0_,
                    int iv1_,
                    int iv2_,
                    int iv3_)
  {
    _set_defaults ();
    set_quadrangle (v0_,
                    v1_,
                    v2_,
                    v3_,
                    iv0_,
                    iv1_,
                    iv2_,
                    iv3_);
    return;
  }

  bool facet34::is_triangle () const
  {
    return _number_of_vertices_ == 3;
  }

  bool facet34::is_quadrangle () const
  {
    return _number_of_vertices_ == 4;
  }

  void facet34::_compute_internals ()
  {
    _compute_face();
    _compute_normal();
    _compute_surface();
    _compute_internal_angles();
    return;
  }

  double facet34::get_internal_angle (int i_) const
  {
    DT_THROW_IF ((i_ < 0) || (i_ >= (int)_number_of_vertices_),
                 std::logic_error ,
                 "Invalid vertex index " << i_ << " !");
    return _internal_angles_[i_];
  }

  bool facet34::has_surface() const
  {
    return datatools::is_valid(_surface_);
  }

  double facet34::get_surface() const
  {
    return _surface_;
  }

  void facet34::_compute_surface()
  {
    datatools::invalidate(_surface_);
    if (is_valid()) {
      if (!has_face()) {
        _compute_face();
      }
      if (has_tface()) {
        _surface_ = get_tface().get_surface();
      }
      if (has_qface()) {
        _surface_ = get_qface().get_surface();
      }
    }
    return;
  }

  bool facet34::has_face() const
  {
    return has_tface() || has_qface();
  }

  bool facet34::has_tface() const
  {
    return _tface_.has_data();
  }

  bool facet34::has_qface() const
  {
    return _qface_.has_data();
  }

  const triangle & facet34::get_tface() const
  {
    return dynamic_cast<const triangle &>(_tface_.get());
  }

  const quadrangle & facet34::get_qface() const
  {
    return dynamic_cast<const quadrangle &>(_qface_.get());
  }

  const face_handle_type & facet34::get_tface_handle() const
  {
    return _tface_;
  }

  face_handle_type & facet34::grab_tface_handle()
  {
    return _tface_;
  }

  const face_handle_type &facet34:: get_qface_handle() const
  {
    return _qface_;
  }

  face_handle_type & facet34::grab_qface_handle()
  {
    return _qface_;
  }

  void facet34::print (std::ostream & out_) const
  {
    out_ << "#vertices=" << get_number_of_vertices();
    out_ << ";vertices={" << get_vertex_key (0) << ", "
         << get_vertex_key(1) << ", "
         << get_vertex_key(2);
    if (get_number_of_vertices() == 4)
      {
        out_ << ", " <<  get_vertex_key(3);
      }
    out_ << "}";
    out_ << ",angles={";
    for (size_t i=0; i<get_number_of_vertices(); i++)
      {
        if (i!=0) out_ << ',';
        out_ << get_internal_angle (i) / CLHEP::degree << " °";
      }
    out_ << '}';
    out_ << ";surface=" << get_surface() / CLHEP::mm2 << " mm2";
    out_ << ";category=" <<_category_;
    out_ << std::endl;
    return;
  }

  /* facet_segment */

  bool facet_segment::is_valid () const
  {
    return vertex0_key >= 0 && vertex1_key >= 0 && facet0_key >= 0 && facet1_key;
  }

  bool facet_segment::is_shown () const
  {
    return _shown_;
  }

  void facet_segment::set_shown (bool s_)
  {
    _shown_ = s_;
    return;
  }

  facet_segment::facet_segment ()
  {
    vertex0_key = vertex1_key = facet0_key = facet1_key = -1;
    _shown_ = false;
    _vertex0_ = 0;
    _vertex1_ = 0;
    return;
  }

  void facet_segment::set_vertex_keys (int vk1_, int vk2_)
  {
    DT_THROW_IF (vk1_ == vk2_,
                 std::logic_error,
                 "Invalid vertex indexes " << vk1_ << "/" << vk2_ << " !");
    DT_THROW_IF (vk1_ < 0,
                 std::logic_error,
                 "Invalid vertex indexes " << vk1_ << " !");
    DT_THROW_IF (vk2_ < 0,
                 std::logic_error,
                 "Invalid vertex indexes " << vk2_ << " !");
    if (vk1_ < vk2_)
      {
        vertex0_key = vk1_;
        vertex1_key = vk2_;
      }
    else
      {
        vertex0_key = vk2_;
        vertex1_key = vk1_;
      }

    return;
  }

  void facet_segment::set_vertexes(const facet_vertex & vtx0_, const facet_vertex & vtx1_)
  {
    _vertex0_ = &vtx0_;
    _vertex1_ = &vtx1_;
    return;
  }

  bool facet_segment::has_vertex(vertex_id_type vid_) const
  {
    if (vid_ == VTX_ID_FIRST) {
      return _vertex0_ != 0 && geomtools::is_valid(_vertex0_->get_position());
    } else if (vid_ == VTX_ID_SECOND) {
      return _vertex1_ != 0 && geomtools::is_valid(_vertex1_->get_position());
    } else  {
      DT_THROW(std::logic_error, "Invalid vertex identifier !");
    }
  }

  const vector_3d & facet_segment::get_vertex_position(vertex_id_type vid_) const
  {
    if (vid_ == VTX_ID_FIRST) {
      DT_THROW_IF(!has_vertex(VTX_ID_FIRST), std::logic_error, "Missing first vertex!");
      return _vertex0_->get_position();
    } else if (vid_ == VTX_ID_SECOND) {
      DT_THROW_IF(!has_vertex(VTX_ID_SECOND), std::logic_error, "Missing second vertex!");
      return _vertex1_->get_position();
    } else {
      DT_THROW(std::logic_error, "Invalid vertex identifier !");
    }
  }

  void facet_segment::set_facet_keys (int fk1_, int fk2_)
  {
    DT_THROW_IF (fk1_ == fk2_,
                 std::logic_error,
                 "Invalid facet indexes " << fk1_ << "/" << fk2_ << " !");
    DT_THROW_IF (fk1_ < 0,
                 std::logic_error,
                 "Invalid facet indexes " << fk1_ << " !");
    DT_THROW_IF (fk2_ < 0,
                 std::logic_error,
                 "Invalid facet indexes " << fk2_ << " !");
    if (fk1_ < fk2_)
      {
        facet0_key = fk1_;
        facet1_key = fk2_;
      }
    else
      {
        facet0_key = fk2_;
        facet1_key = fk1_;
      }

    return;
  }

  void facet_segment::set (int vk1_, int vk2_, int fk1_, int fk2_)
  {
    set_vertex_keys (vk1_, vk2_);
    set_facet_keys (fk1_, fk2_);
    return;
  }

  bool operator==(const facet_segment & fs1_, const facet_segment & fs2_)
  {
    return (fs1_.vertex0_key == fs2_.vertex0_key)
      && (fs1_.vertex1_key == fs2_.vertex1_key);
  }

  bool operator<(const facet_segment & fs1_, const facet_segment & /*fs2_*/)
  {
    if (fs1_.vertex0_key < fs1_.vertex1_key) return true;
    return false;
  }

  void facet_segment::dump (std::ostream & out_) const
  {
    out_ << "geomtools::facet_segment: " << std::endl;
    out_ << "|-- Vertex 0 key = " << vertex0_key << std::endl;
    out_ << "|-- Vertex 1 key = " << vertex1_key << std::endl;
    out_ << "|-- Facet 0 key  = " << facet0_key << std::endl;
    out_ << "|-- Facet 1 key  = " << facet1_key << std::endl;
    out_ << "`-- The end " << std::endl;

    return;
  }

  /* tessellated_solid */

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(tessellated_solid,
                                             "geomtools::tessellated_solid")

  // static
  const int tessellated_solid::INVALID_VERTEX;
  const int tessellated_solid::MAX_VERTEX;
  const unsigned int tessellated_solid::INVALID_FACET_INDEX;


  const std::string & tessellated_solid::tessellated_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "tessellated";
    }
    return label;
  }

  bool tessellated_solid::using_face_id_bits() const
  {
    return false;
  }

  bool tessellated_solid::using_face_id_index() const
  {
    return true;
  }

  bool tessellated_solid::is_consistent() const
  {
    return _consistent_;
  }

  bool tessellated_solid::_check_()
  {
    _consistent_ = true;
    for (vertices_col_type::const_iterator i = _vertices_.begin();
         i != _vertices_.end();
         i++)
      {
        if (i->second.ref_facets.size() == 0)
          {
            DT_LOG_WARNING (datatools::logger::PRIO_WARNING,
                            "Vertex " << i->first << " is not used at all !");
            _consistent_ = false;
          }
        else if (i->second.ref_facets.size () < 3)
          {
            DT_LOG_WARNING (datatools::logger::PRIO_WARNING,
                            "Vertex " << i->first << " is not properly referenced (#ref="
                            << i->second.ref_facets.size () << ") !");
            _consistent_ = false;
            return _consistent_;
          }
      }
    // missing check:

    return _consistent_;
  }

  void tessellated_solid::_compute_facet_segments ()
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE (local_priority, "Entering...");
    DT_LOG_TRACE (local_priority, "Number of facets = " << _facets_.size ());
    if (local_priority >= datatools::logger::PRIO_TRACE) dump (std::cerr);

    _facet_segments_.clear ();
    int facet_segment_counter = 0;
    for (facets_col_type::const_iterator facet_iter = _facets_.begin ();
         facet_iter != _facets_.end ();
         facet_iter++) {
      const int current_facet_key = facet_iter->first;
      const facet34 & the_facet = facet_iter->second;
      if (local_priority >= datatools::logger::PRIO_TRACE) {
        std::ostringstream message;
        message << "Facet #" << current_facet_key << " with "
                << the_facet.get_number_of_vertices () << " vertices : ";
        for (unsigned int ivtx = 0; ivtx < the_facet.get_number_of_vertices (); ivtx++) {
          message << the_facet.get_vertex_key (ivtx) << " ";
        }
        DT_LOG_TRACE (local_priority, message.str ());
      }

      for (unsigned int ivtx = 0; ivtx < the_facet.get_number_of_vertices (); ivtx++) {
        const unsigned int iref = ivtx;
        const unsigned int irefnext = (ivtx + 1 ) % the_facet.get_number_of_vertices ();
        DT_LOG_TRACE (local_priority, "Indexes=[" << iref << '-' << irefnext << "]");
        const facet_vertex & v1 = the_facet.get_vertex (iref);
        const facet_vertex & v2 = the_facet.get_vertex (irefnext);
        const int vk1 = the_facet.get_vertex_key (ivtx);
        const int vk2 = the_facet.get_vertex_key ((ivtx + 1) % the_facet.get_number_of_vertices ());
        DT_LOG_TRACE (local_priority, "Segment from vtx " << vk1 << "=(" << v1 << ") to vtx " << vk2 << "=(" << v2 << ")");

        // Facet segment candidate :
        facet_segment fseg;
        fseg.set_vertex_keys(vk1, vk2);
        fseg.set_vertexes(v1, v2);

        // Check if it is already registered :
        bool already_exists = false;
        for (facet_segments_col_type::const_iterator fsi = _facet_segments_.begin ();
             fsi != _facet_segments_.end ();
             fsi++) {
          if (local_priority >= datatools::logger::PRIO_TRACE) {
            fsi->second.dump (std::cerr);
          }
          if (fseg == fsi->second) {
            already_exists = true;
            DT_LOG_TRACE (local_priority, "Segment already exists as #" << fsi->first << " !");
            break;
          }
        }
        if (already_exists) {
          continue;
        }

        const int vf1 = current_facet_key;
        // Search the other facet that shares this [v1,v2] segment :
        int vf2 = -1;
        DT_LOG_TRACE (local_priority, "Search the other facet (!=" << vf1  << ") that shares segment [vk1=" << vk1 << ";vk2=" << vk2 << "]");
        if (local_priority >= datatools::logger::PRIO_TRACE) {
          std::ostringstream message;
          message << "Now loop on " << v1.ref_facets.size () << " facets"
                  << " at vertex #" << vk1 << ": ";
          for (std::map<int, int>::const_iterator ifv1 = v1.ref_facets.begin ();
               ifv1 != v1.ref_facets.end ();
               ifv1++) message << ifv1->first << " ";
          DT_LOG_TRACE (local_priority, message.str ());
        }
        for (std::map<int, int>::const_iterator ifv1 = v1.ref_facets.begin ();
             ifv1 != v1.ref_facets.end ();
             ifv1++) {
          const int chck_fk1 = ifv1->first;
          DT_LOG_TRACE (local_priority, "Loop on V1's facet #" << chck_fk1);
          if (chck_fk1 == vf1) {
            DT_LOG_TRACE (local_priority, "Skipping facet #" << chck_fk1);
            continue;
          }
          if (local_priority >= datatools::logger::PRIO_TRACE) {
            std::ostringstream message;
            message << "Now loop on " << v2.ref_facets.size () << " facets"
                    << " at vertex #" << vk2 << ": ";
            for (std::map<int, int>::const_iterator ifv2 = v2.ref_facets.begin ();
                 ifv2 != v2.ref_facets.end ();
                 ifv2++) message << ifv2->first << " ";
            DT_LOG_TRACE (local_priority, message.str ());
          }
          for (std::map<int, int>::const_iterator ifv2 = v2.ref_facets.begin ();
               ifv2 != v2.ref_facets.end ();
               ifv2++) {
            const int chck_fk2 = ifv2->first;
            DT_LOG_TRACE (local_priority, "Loop on V2's facet #" << chck_fk2);
            if (chck_fk2 == vf1) {
              DT_LOG_TRACE (local_priority, "Skipping  facet #" << chck_fk2);
              continue;
            }
            if (chck_fk1 == chck_fk2) {
              vf2 = chck_fk2;
              break;
            }
          }
          if (vf2 >= 0) {
            DT_LOG_TRACE (local_priority, "Break for found  facet #" << vf2);;
            break;
          }
        }
        DT_THROW_IF (vf2 < 0, std::logic_error, "Cannot compute facet segments !");
        DT_LOG_TRACE (local_priority, "Segment belongs to facets #" << vf1 << " and " << vf2 << " !");

        fseg.set_facet_keys (vf1,vf2);
        _facet_segments_[facet_segment_counter] = fseg;
        DT_LOG_TRACE (local_priority, "Segment is registered as #" << facet_segment_counter << " !");
        facet_segment_counter++;
      }

    }

    // Compute the 'show' flag :
    for (facet_segments_col_type::iterator fsi = _facet_segments_.begin ();
         fsi != _facet_segments_.end ();
         fsi++) {
      facet_segment & the_fsgmt = fsi->second;

      const facet34 & fct0 = facets ().find (the_fsgmt.facet0_key)->second;
      const facet34 & fct1 = facets ().find (the_fsgmt.facet1_key)->second;

      if (fct0.get_normal ().isNear (fct1.get_normal (), 1.0e-4)) {
        the_fsgmt.set_shown (false);
      } else {
        the_fsgmt.set_shown (true);
      }
    }

    DT_LOG_TRACE (local_priority, "Compute " << _facet_segments_.size () << " facet segments !");
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  void tessellated_solid::_at_lock()
  {
    this->i_shape_3d::_at_lock();
    _compute_facet_segments();
    bool checked = _check_();
    DT_THROW_IF(not checked, std::logic_error, "This tessellated solid is not consistent!");
    return;
  }

  void tessellated_solid::_at_unlock()
  {
    _facet_segments_.clear();
    this->i_shape_3d::_at_unlock();
    return;
  }

  void tessellated_solid::_build_bounding_data()
  {
    if (!_xrange_.is_valid() || !_yrange_.is_valid() || !_zrange_.is_valid()) {
      compute_bounding_box();
    }
    _grab_bounding_data().make_box(_xrange_.get_min(), _xrange_.get_max(),
                                   _yrange_.get_min(), _yrange_.get_max(),
                                   _zrange_.get_min(), _zrange_.get_max());
    return;
  }

  bool tessellated_solid::validate_index (int i_)
  {
    return (i_ > INVALID_VERTEX) && (i_ < MAX_VERTEX);
  }

  tessellated_solid::tessellated_solid ()
  {
    _consistent_ = false;
    return;
  }

  tessellated_solid::~tessellated_solid ()
  {
    _facet_segments_.clear ();
    // for (facets_col_type::iterator i = _facets_.begin ();
    //      i != _facets_.end ();
    //      i++)
    //   {
    //     delete i->second;
    //   }
    _facets_.clear ();
    _vertices_.clear ();
    return;
  }

  const tessellated_solid::vertices_col_type & tessellated_solid::vertices () const
  {
    return _vertices_;
  }

  const tessellated_solid::facets_col_type & tessellated_solid::facets () const
  {
    return _facets_;
  }

  tessellated_solid::vertices_col_type & tessellated_solid::grab_vertices ()
  {
    return _vertices_;
  }

  tessellated_solid::facets_col_type & tessellated_solid::grab_facets ()
  {
    return _facets_;
  }

  const tessellated_solid::facet_segments_col_type & tessellated_solid::facet_segments () const
  {
    return _facet_segments_;
  }

  tessellated_solid::facet_segments_col_type & tessellated_solid::grab_facet_segments ()
  {
    return _facet_segments_;
  }

  int tessellated_solid::add_vertex (unsigned int vtx_key_, const facet_vertex & vtx_)
  {
    DT_THROW_IF (is_locked (), std::logic_error, "Object is locked !");
    _vertices_[vtx_key_] = vtx_;
    _xrange_.add (vtx_.position.x ());
    _yrange_.add (vtx_.position.y ());
    _zrange_.add (vtx_.position.z ());
    return vtx_key_;
  }

  const mygsl::min_max & tessellated_solid::get_bounding_box_x () const
  {
    return _xrange_;
  }

  const mygsl::min_max & tessellated_solid::get_bounding_box_y () const
  {
    return _yrange_;
  }

  const mygsl::min_max & tessellated_solid::get_bounding_box_z () const
  {
    return _zrange_;
  }

  double tessellated_solid::get_xmin() const
  {
    return _xrange_.get_min();
  }

  double tessellated_solid::get_xmax() const
  {
    return _xrange_.get_max();
  }

  double tessellated_solid::get_ymin() const
  {
    return _yrange_.get_min();
  }

  double tessellated_solid::get_ymax() const
  {
    return _yrange_.get_max();
  }

  double tessellated_solid::get_zmin() const
  {
    return _zrange_.get_min();
  }

  double tessellated_solid::get_zmax() const
  {
    return _zrange_.get_max();
  }

  // // virtual
  // void tessellated_solid::compute_bounding_box(box & bb_)
  // {
  //   this->i_object_3d::compute_bounding_box(bb_);
  //   if (_xrange_.is_valid() && _yrange_.is_valid() && _zrange_.is_valid()) {
  //     double x = std::abs(_xrange_.get_min());
  //     if (std::abs(_xrange_.get_max()) > x) {
  //       x = std::abs(_xrange_.get_max());
  //     }
  //     double y = std::abs(_yrange_.get_min());
  //     if (std::abs(_yrange_.get_max()) > y) {
  //       y = std::abs(_yrange_.get_max());
  //     }
  //     double z = std::abs(_zrange_.get_min());
  //     if (std::abs(_zrange_.get_max()) > z) {
  //       z = std::abs(_zrange_.get_max());
  //     }
  //     bb_.set_x(2 * x);
  //     bb_.set_y(2 * y);
  //     bb_.set_z(2 * z);
  //   }
  //   return;
  // }

  void tessellated_solid::compute_bounding_box()
  {
    _xrange_.reset();
    _yrange_.reset();
    _zrange_.reset();
    for (vertices_col_type::const_iterator i = _vertices_.begin();
         i != _vertices_.end();
         i++) {
      const facet_vertex & vtx = i->second;
      _xrange_.add(vtx.position.x());
      _yrange_.add(vtx.position.y());
      _zrange_.add(vtx.position.z());
    }
    return;
  }

  int tessellated_solid::add_vertex (unsigned int vtx_key_,
                                     double x_,
                                     double y_,
                                     double z_)
  {
    facet_vertex vtx;
    vtx.position.set (x_, y_, z_);
    return add_vertex (vtx_key_, vtx);
  }

  void tessellated_solid::add_facet3 (unsigned int facet_key_,
                                      int ivtx0_, int ivtx1_, int ivtx2_)
  {
    add_facet (facet_key_,  ivtx0_, ivtx1_, ivtx2_, INVALID_VERTEX);
    return;
  }

  void tessellated_solid::add_facet4 (unsigned int facet_key_,
                                      int ivtx0_, int ivtx1_, int ivtx2_, int ivtx3_)
  {
    add_facet (facet_key_,  ivtx0_, ivtx1_, ivtx2_, ivtx3_);
    return;
  }

  void tessellated_solid::add_facet (unsigned int facet_key_,
                                     int ivtx0_,
                                     int ivtx1_,
                                     int ivtx2_,
                                     int ivtx3_)
  {
    DT_THROW_IF (is_locked (), std::logic_error, "Object is locked !");
    size_t dim = 3;
    if (validate_index (ivtx3_)) {
      dim = 4;
    }
    int ivtx[4];
    ivtx[0] = ivtx0_;
    ivtx[1] = ivtx1_;
    ivtx[2] = ivtx2_;
    ivtx[3] = ivtx3_;
    vertices_col_type::iterator vtx_it[4];
    for (size_t i = 0; i < dim; i++) {
      vtx_it[i] = _vertices_.find (ivtx[i]);
      DT_THROW_IF (vtx_it[i] == _vertices_.end (),
                   std::logic_error,
                   "Invalid vertex index " << ivtx[i] << " !");
    }
    //i_facet * fct = 0;
    facet34 fct;
    if (dim == 3) {
      vtx_it[0]->second.add_ref_facet (facet_key_, 0);
      vtx_it[1]->second.add_ref_facet (facet_key_, 1);
      vtx_it[2]->second.add_ref_facet (facet_key_, 2);
      fct = facet34 ((vtx_it[0]->second),
                     (vtx_it[1]->second),
                     (vtx_it[2]->second),
                     ivtx[0],ivtx[1],ivtx[2]);
    } else {
      vtx_it[0]->second.add_ref_facet (facet_key_, 0);
      vtx_it[1]->second.add_ref_facet (facet_key_, 1);
      vtx_it[2]->second.add_ref_facet (facet_key_, 2);
      vtx_it[3]->second.add_ref_facet (facet_key_, 3);
      fct = facet34 ((vtx_it[0]->second),
                     (vtx_it[1]->second),
                     (vtx_it[2]->second),
                     (vtx_it[3]->second),
                     ivtx[0],ivtx[1],ivtx[2],ivtx[3]);
    }
    _facets_[facet_key_] = fct;
    return;
  }

  bool tessellated_solid::has_facet(unsigned int facet_key_) const
  {
    return _facets_.find(facet_key_) != _facets_.end();
  }

  const facet34 & tessellated_solid::get_facet(unsigned int facet_key_) const
  {
    facets_col_type::const_iterator found = _facets_.find(facet_key_);
    DT_THROW_IF(found == _facets_.end(), std::logic_error,
                "No facet with index [" << facet_key_ << "] was found!");
    return found->second;
  }

  void tessellated_solid::remove_facet (unsigned int facet_key_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE (local_priority, "Before: #facets=" << _facets_.size ());

    // Check the existence of the facet :
    facets_col_type::iterator facet_found = _facets_.find (facet_key_);
    DT_THROW_IF (facet_found ==_facets_.end (),
                 std::logic_error,
                 "Facet #" << facet_key_ << "  does not exist !");

    // Get a pointer to the facet to be removed :
    //i_facet * the_facet_ptr = facet_found->second;
    facet34 the_facet = facet_found->second;

    // Traverse the collection of vertices in the facet :
    for (size_t i = 0; i < the_facet.get_number_of_vertices (); i++) {
      //int vtxkey = the_facet_ptr->get_vertex_key (i);
      int vtxkey = the_facet.get_vertex_key (i);

      // Check if the vertex exists in the dictionary of vertices :
      vertices_col_type::iterator vtx_found = _vertices_.find (vtxkey);
      DT_THROW_IF (vtx_found == _vertices_.end (),
                   std::logic_error,
                   "Vertex #" << vtxkey << "  does not exist !");

      // Get a reference to the vertex :
      facet_vertex & the_vertex = vtx_found->second;

      // Search the reference to the facet in the current vertex :
      std::map<int, int>::iterator found2 = the_vertex.ref_facets.find (facet_key_);
      DT_THROW_IF (found2 == the_vertex.ref_facets.end (),
                   std::logic_error,
                   "Vertex #" << vtxkey << " has is not used by facet #" << facet_key_ << " !");
      // Remove the facet reference in this vertex:
      the_vertex.ref_facets.erase (found2);
    }

    DT_LOG_TRACE (local_priority, "After: #facets=" << _facets_.size ());

    // Remove the facet from the dictionnary of facets :
    //facet_found->second = 0;
    _facets_.erase (facet_found);

    // Delete the facet :
    //delete the_facet_ptr;
    return;
  }

  void tessellated_solid::tree_dump(std::ostream & a_out,
                                    const std::string & a_title,
                                    const std::string & a_indent,
                                    bool a_inherit) const
  {
    i_shape_3d::tree_dump(a_out, a_title, a_indent, true);

    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Vertices: " << _vertices_.size () << std::endl;
    for (vertices_col_type::const_iterator i = _vertices_.begin ();
         i != _vertices_.end ();
         i++) {
      a_out << a_indent << datatools::i_tree_dumpable::skip_tag;
      {
        vertices_col_type::const_iterator j = i;
        if (++j == _vertices_.end ()) {
          a_out << datatools::i_tree_dumpable::last_tag;
        } else {
          a_out << datatools::i_tree_dumpable::tag;
        }
      }
      a_out << "Vertex[" << i->first << "] : ";
      i->second.print(a_out);
    }

    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Bounding box: " << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::skip_tag
          << datatools::i_tree_dumpable::tag
          << "X: ["  << _xrange_.get_min () << ';'
          << _xrange_.get_max () << ']' << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::skip_tag
          << datatools::i_tree_dumpable::tag
          << "Y: ["  << _yrange_.get_min () << ';'
          << _yrange_.get_max () << ']' << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::skip_tag
          << datatools::i_tree_dumpable::last_tag
          << "Z: ["  << _zrange_.get_min () << ';'
          << _zrange_.get_max () << ']' << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Facet segments: " << _facet_segments_.size() << std::endl;
    for (facet_segments_col_type::const_iterator i = _facet_segments_.begin ();
         i != _facet_segments_.end ();
         i++) {
      a_out << a_indent << datatools::i_tree_dumpable::skip_tag;
      {
        facet_segments_col_type::const_iterator j = i;
        if (++j == _facet_segments_.end ()) {
          a_out << datatools::i_tree_dumpable::last_tag;
        } else {
          a_out << datatools::i_tree_dumpable::tag;
        }
      }
      a_out << "Facet segment[" << i->first << "] : ";
      const facet_segment & fseg = i->second;
      a_out << "from vertex [#" << fseg.vertex0_key << "] to vertex [#"
            << fseg.vertex1_key << "] with facets [#" << fseg.facet0_key
            << "/#" << fseg.facet1_key << "]"
            << std::endl;
    }

    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Facets: " << _facets_.size () << std::endl;
    for (facets_col_type::const_iterator i = _facets_.begin ();
         i != _facets_.end ();
         i++) {
      a_out << a_indent << datatools::i_tree_dumpable::inherit_skip_tag(a_inherit);
      {
        facets_col_type::const_iterator j = i;
        if (++j == _facets_.end ()) {
          a_out << datatools::i_tree_dumpable::last_tag;
        } else {
          a_out << datatools::i_tree_dumpable::tag;
        }
      }
      a_out << "Facet[" << i->first << "] : ";
      i->second.print(a_out);
    }

    return;
  }

  void tessellated_solid::dump (std::ostream & out_) const
  {
    std::string local_tag   = "|-- ";
    std::string stag  = "|   ";
    std::string ltag  = "`-- ";
    std::string sltag = "    ";
    out_ << "geomtools::tessellated_solid" << std::endl;
    out_ << local_tag << "Locked: " << is_locked () << std::endl;
    out_ << local_tag << "Vertices: " << _vertices_.size () << std::endl;
    for (vertices_col_type::const_iterator i = _vertices_.begin ();
         i != _vertices_.end ();
         i++) {
      std::string tag2 = local_tag;
      {
        vertices_col_type::const_iterator j = i;
        if (++j == _vertices_.end ()) tag2 = ltag;
      }
      out_ << stag << tag2 << "Vertex[" << i->first << "] : ";
      i->second.print (out_);
    }
    out_ << local_tag << "Bounding box: " << std::endl;
    out_ << stag << local_tag << "X: ["  << _xrange_.get_min () << ';' << _xrange_.get_max () << ']' << std::endl;
    out_ << stag << local_tag << "Y: ["  << _yrange_.get_min () << ';' << _yrange_.get_max () << ']' << std::endl;
    out_ << stag << ltag << "Z: [" << _zrange_.get_min () << ';' << _zrange_.get_max () << ']' << std::endl;

    out_ << local_tag << "Facet segments: " << _facet_segments_.size () << std::endl;

    out_ << ltag << "Facets: " << _facets_.size () << std::endl;
    for (facets_col_type::const_iterator i = _facets_.begin ();
         i != _facets_.end ();
         i++) {
      std::string tag2 = local_tag;
      {
        facets_col_type::const_iterator j = i;
        if (++j == _facets_.end ()) tag2 = ltag;
      }
      out_ << sltag << tag2 << "Facet[" << i->first << "] : ";
      i->second.print (out_);
    }
    return;
  }

  void tessellated_solid::print_xyz (std::ostream & out_) const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE (local_priority, "# vertices = " << _vertices_.size());
    DT_LOG_TRACE (local_priority, "# facets   = " << _facets_.size());

    // facets:
    int count = 0;
    for (facets_col_type::const_iterator i = _facets_.begin ();
         i != _facets_.end ();
         i++) {
      //const i_facet * fct = i->second;
      const facet34 & fct = i->second;
      out_ << "# facet " << count
           << " with " <<  fct.get_number_of_vertices ()
           << " vertices" << std::endl;
      const size_t nvtx = fct.get_number_of_vertices ();
      DT_LOG_TRACE (local_priority, "# nvtx = " << nvtx);

      // vertices:
      const geomtools::vector_3d u1 = fct.get_vertex (1).position - fct.get_vertex (0).position;
      const geomtools::vector_3d u2 = fct.get_vertex (2).position - fct.get_vertex (1).position;
      const geomtools::vector_3d u3 = u1.cross (u2);
      const int color = 5 + (int) (5 * u3.cosTheta ());
      fct.get_vertex (0).print_xyz (out_, color);
      fct.get_vertex (1).print_xyz (out_, color);
      out_ << std::endl;
      if (nvtx == 3) {
        fct.get_vertex (2).print_xyz (out_, color);
        fct.get_vertex (2).print_xyz (out_, color);
        out_ << std::endl;
      } else {
        fct.get_vertex (3).print_xyz (out_, color);
        fct.get_vertex (2).print_xyz (out_, color);
        out_ << std::endl;
      }
      /*
        const double dx = fct.get_vertex (1).position.x () - fct.get_vertex (0).position.x ();
        const double dy = fct.get_vertex (1).position.y () - fct.get_vertex (0).position.y ();
        const double dz = fct.get_vertex (1).position.z () - fct.get_vertex (0).position.z ();
        if (nvtx == 3)
        {
        facet_vertex last;
        last.position.setX (fct.get_vertex (2).position.x () - 1e-6 * dx / m);
        last.position.setY (fct.get_vertex (2).position.y () - 1e-6 * dy / m);
        last.position.setY (fct.get_vertex (2).position.z () - 1e-6 * dz / m);
        last.print_xyz (out_, color);
        }
        else
        {
        fct.get_vertex (fct.get_number_of_vertices () - 1)
        .print_xyz (out_, color);
        }
        fct.get_vertex (2).print_xyz (out_, color);
        out_ << std::endl;
      */
      out_ << std::endl;
      count++;
    }
    return;
  }

  bool tessellated_solid::is_outside( const vector_3d & position_, double skin_ ) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tessellated solid!");
    double skin = get_skin(skin_);

    unsigned int facet_index = INVALID_FACET_INDEX;
    if (_on_facet(position_, facet_index, skin_)) {
      return false;
    }

    // Find the target point as the nearest bounding box's face:
    std::vector<vector_3d> fcs;
    get_bounding_data().compute_bounding_box_face_centers(fcs, 2 * skin);
    vector_3d target;
    double dist_to_target = -1.0;
    for (size_t i = 0; i < fcs.size(); i++) {
      double dist = (position_ - fcs[i]).mag();
      if (dist_to_target < 0.0 || dist < dist_to_target) {
        dist = dist_to_target;
        target = fcs[i];
      }
    }

    // Compute the direction from the source to the target
    vector_3d source = position_;
    vector_3d direction = (target - source).unit();

    std::vector<face_intercept_info> intercepts;
    std::set<unsigned int> dummy_excluded;
    // Find all intercept points with all possible facets:
    unsigned int nintercepts = _find_intercepts_exclude(source,
                                                        direction,
                                                        intercepts,
                                                        skin,
                                                        dummy_excluded);
    return (nintercepts % 2) == 0;
  }

  bool tessellated_solid::is_inside(const vector_3d & position_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tessellated solid!");
    double skin = get_skin(skin_);

    unsigned int facet_index = INVALID_FACET_INDEX;
    if (_on_facet(position_, facet_index, skin)) {
      return false;
    }

    // Find the target point as the nearest bounding box's face:
    std::vector<vector_3d> fcs;
    get_bounding_data().compute_bounding_box_face_centers(fcs, 2 * skin);
    vector_3d target;
    double dist_to_target = -1.0;
    for (size_t i = 0; i < fcs.size(); i++) {
      double dist = (position_ - fcs[i]).mag();
      if (dist_to_target < 0.0 || dist < dist_to_target) {
        dist = dist_to_target;
        target = fcs[i];
      }
    }

    // Compute the direction from the source to the target
    vector_3d source = position_;
    vector_3d direction = (target - source).unit();

    std::vector<face_intercept_info> intercepts;
    std::set<unsigned int> dummy_excluded;
    // Find all intercept points with all possible facets:
    unsigned int nintercepts = _find_intercepts_exclude(source,
                                                        direction,
                                                        intercepts,
                                                        skin,
                                                        dummy_excluded);
    return (nintercepts % 2) == 1;
  }

  bool tessellated_solid::_on_facet(const vector_3d & position_,
                                    unsigned int & facet_index_,
                                    double skin_) const
  {
    facet_index_ = INVALID_FACET_INDEX;

    for (facets_col_type::const_iterator ifacet = _facets_.begin();
         ifacet != _facets_.end();
         ifacet++) {
      uint32_t facet_index = ifacet->first;
      const facet34 & face = ifacet->second;
      if (face.has_tface()) {
        if (face.get_tface().is_on_surface(position_, skin_)) {
          facet_index_ = facet_index;
          break;
        }
      } else if (face.has_qface()) {
        if (face.get_qface().is_on_surface(position_, skin_)) {
          facet_index_ = facet_index;
          break;
        }
      }
    }
    return facet_index_ != INVALID_FACET_INDEX ;
  }

  face_identifier tessellated_solid::on_surface(const vector_3d & position_,
                                                const face_identifier & surface_mask_,
                                                double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tessellated solid!");
    double skin = get_skin(skin_);
    face_identifier mask;
    if (surface_mask_.is_valid()) {
      DT_THROW_IF(! surface_mask_.is_face_index_mode(), std::logic_error,
                  "Face mask uses no face index!");
      mask = surface_mask_;
    } else {
      mask.set_face_index_any();
    }
    unsigned int facet_index;
    if (_on_facet(position_, facet_index, skin)) {
      return face_identifier(facet_index, face_identifier::MODE_FACE_INDEX);
    }
    return face_identifier::face_invalid();
  }

  vector_3d tessellated_solid::get_normal_on_surface(const vector_3d & position_,
                                                     const face_identifier & a_surface_id) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(!a_surface_id.is_face_index_mode(), std::logic_error,
                "Surface identifier does not use face index mode!");
    uint32_t facet_index = a_surface_id.get_face_index();
    facets_col_type::const_iterator found = _facets_.find(facet_index);
    DT_THROW_IF(found == _facets_.end(), std::logic_error, "Invalid facet index!");
    const facet34 & facet = found->second;
    if (facet.has_tface()) {
      normal = facet.get_tface().get_normal_on_surface(position_, false);
    } else if (facet.has_qface()) {
      normal = facet.get_qface().get_normal_on_surface(position_, false);
    }
    return(normal);
  }

  bool tessellated_solid::_find_intercept_exclude(const vector_3d & from_,
                                                  const vector_3d & direction_,
                                                  face_intercept_info & intercept_,
                                                  double skin_,
                                                  const std::set<unsigned int> & excluded_facet_indexes_) const
  {
    intercept_.reset ();
    std::vector<face_intercept_info> intercepts;
    if (_find_intercepts_exclude(from_, direction_, intercepts, skin_, excluded_facet_indexes_)) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < intercepts.size(); iface++) {
        if (intercepts[iface].is_ok()) {
          double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
          if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            intercept_.grab_face_id().set_face_index(intercepts[iface].get_face_id().get_face_index());
            intercept_.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    /*
      double skin = compute_tolerance(skin_);
      const unsigned int NFACES = facets().size();
      std::vector<face_intercept_info> intercepts;
      intercepts.reserve(20);
      {
      face_intercept_info dummy;
      intercepts.push_back(dummy);
      }
      unsigned int candidate_impact_counter = 0;

      for (facets_col_type::const_iterator ifacet = _facets_.begin();
      ifacet != _facets_.end();
      ifacet++) {
      uint32_t facet_index = ifacet->first;
      // Search for excluded facet indexes:
      for (std::set<unsigned int>::const_iterator iex = excluded_facet_indexes_.begin();
      iex != excluded_facet_indexes_.end();
      iex++) {
      unsigned int excluded_facet_index = *iex;
      if (excluded_facet_index_ != INVALID_FACET_INDEX) {
      // Skip this facet:
      if (facet_index == excluded_facet_index_) {
      continue;
      }
      }
      }
      const facet34 & face = ifacet->second;
      if (face.has_tface()) {
      face_intercept_info & fii = intercepts.back();
      if (face.get_tface().find_intercept(from_, direction_, fii, skin)) {
      fii.grab_face_id().set_face_index(facet_index);
      {
      face_intercept_info dummy;
      intercepts.push_back(dummy);
      }
      } else {
      fii.reset();
      }
      } else if (face.has_qface()) {
      face_intercept_info & fii = intercepts.back();
      if (face.get_qface().find_intercept(from_, direction_, fii, skin)) {
      fii.grab_face_id().set_face_index(facet_index);
      {
      face_intercept_info dummy;
      intercepts.push_back(dummy);
      }
      } else {
      fii.reset();
      }
      }
      }
      if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
      if (intercepts[iface].is_ok()) {
      double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
      if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
      min_length_to_impact = length_to_impact;
      intercept_.grab_face_id().set_face_index(intercepts[iface].get_face_id().get_face_index());
      intercept_.set_impact(intercepts[iface].get_impact());
      }
      }
      }
      }
    */

    return intercept_.is_ok();
  }

  unsigned int tessellated_solid::_find_intercepts_exclude(const vector_3d & from_,
                                                           const vector_3d & direction_,
                                                           std::vector<face_intercept_info> & intercepts_,
                                                           double skin_,
                                                           const std::set<unsigned int> & excluded_facet_indexes_) const
  {
    double skin = compute_tolerance(skin_);
    intercepts_.clear();
    intercepts_.reserve(20);
    {
      face_intercept_info dummy;
      intercepts_.push_back(dummy);
    }
    unsigned int candidate_impact_counter = 0;

    for (facets_col_type::const_iterator ifacet = _facets_.begin();
         ifacet != _facets_.end();
         ifacet++) {
      uint32_t facet_index = ifacet->first;
      // Search for excluded facet indexes:
      for (std::set<unsigned int>::const_iterator iex = excluded_facet_indexes_.begin();
           iex != excluded_facet_indexes_.end();
           iex++) {
        unsigned int excluded_facet_index = *iex;
        if (excluded_facet_index != INVALID_FACET_INDEX) {
          // Skip this excluded facet:
          if (facet_index == excluded_facet_index) {
            continue;
          }
        }
      }
      const facet34 & face = ifacet->second;
      if (face.has_tface()) {
        face_intercept_info & fii = intercepts_.back();
        if (face.get_tface().find_intercept(from_, direction_, fii, skin)) {
          fii.grab_face_id().set_face_index(facet_index);
          candidate_impact_counter++;
          {
            face_intercept_info dummy;
            intercepts_.push_back(dummy);
          }
        } else {
          fii.reset();
        }
      } else if (face.has_qface()) {
        face_intercept_info & fii = intercepts_.back();
        if (face.get_qface().find_intercept(from_, direction_, fii, skin)) {
          fii.grab_face_id().set_face_index(facet_index);
          candidate_impact_counter++;
          {
            face_intercept_info dummy;
            intercepts_.push_back(dummy);
          }
        } else {
          fii.reset();
        }
      }
    }

    if (!intercepts_.back().is_ok()) {
      intercepts_.pop_back();
    }

    return intercepts_.size();
  }

  bool tessellated_solid::find_intercept (const vector_3d & from_,
                                          const vector_3d & direction_,
                                          face_intercept_info & intercept_,
                                          double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tessellated solid!");
    std::set<unsigned int> dummy;
    return _find_intercept_exclude(from_, direction_, intercept_, skin_, dummy);
    /*
      intercept_.reset ();
      double skin = compute_tolerance(skin_);
      const unsigned int NFACES = facets().size();
      std::vector<face_intercept_info> intercepts;
      intercepts.reserve(20);
      {
      face_intercept_info dummy;
      intercepts.push_back(dummy);
      }
      unsigned int candidate_impact_counter = 0;

      for (facets_col_type::const_iterator ifacet = _facets_.begin();
      ifacet != _facets_.end();
      ifacet++) {
      uint32_t facet_index = ifacet->first;
      const facet34 & face = ifacet->second;
      if (face.has_tface()) {
      face_intercept_info & fii = intercepts.back();
      if (face.get_tface().find_intercept(from_, direction_, fii, skin)) {
      fii.grab_face_id().set_face_index(facet_index);
      {
      face_intercept_info dummy;
      intercepts.push_back(dummy);
      }
      } else {
      fii.reset();
      }
      } else if (face.has_qface()) {
      face_intercept_info & fii = intercepts.back();
      if (face.get_qface().find_intercept(from_, direction_, fii, skin)) {
      fii.grab_face_id().set_face_index(facet_index);
      {
      face_intercept_info dummy;
      intercepts.push_back(dummy);
      }
      } else {
      fii.reset();
      }
      }
      }

      if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
      if (intercepts[iface].is_ok()) {
      double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
      if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
      min_length_to_impact = length_to_impact;
      intercept_.grab_face_id().set_face_index(intercepts[iface].get_face_id().get_face_index());
      intercept_.set_impact(intercepts[iface].get_impact());
      }
      }
      }
      }

      return intercept_.is_ok();
    */
  }

  std::string tessellated_solid::get_shape_name () const
  {
    return tessellated_solid::tessellated_label();
  }

  void tessellated_solid::initialize_from_stl(const std::string & filename_,
                                              double length_unit_)
  {
    std::ifstream fin(filename_.c_str(), std::ios::binary);
    std::string solid_token;
    {
      fin.seekg(0, std::ios::end);
      std::streampos length = fin.tellg();
      fin.seekg(0, std::ios::beg);
      // std::cerr << "length : " << length << '\n';
      solid_token.reserve(length);
      fin.seekg(0, std::ios::beg);
      solid_token.assign((std::istreambuf_iterator<char>(fin)),
                         (std::istreambuf_iterator<char>()));
    }

    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef geomtools::stl::solid_parser<iterator_type> solid_parser_type;

    iterator_type solid_iter = solid_token.begin();
    iterator_type solid_end  = solid_token.end();
    solid_parser_type     sld_grammar;
    geomtools::stl::solid sld;
    bool result = boost::spirit::qi::phrase_parse(solid_iter,
                                                  solid_end,
                                                  sld_grammar,
                                                  boost::spirit::ascii::space,
                                                  sld);
    if (result && solid_iter == solid_end) {
      geomtools::stl::stl_to_geomtools_converter stl2gt;
      stl2gt.set_length_unit(length_unit_);
      stl2gt.set_fix_attempt(true);
      stl2gt.set_check_normal(true);
      stl2gt.convert(sld, *this);
      this->lock();
    } else {
      DT_THROW(std::logic_error, "STL parsing from file '" << filename_ << "' failed!");
    }
    return;
  }

  void tessellated_solid::initialize(const datatools::properties & config_,
                                     const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);
    if (! is_valid()) {

      double default_length_unit = 1.0 * CLHEP::mm;
      if (config_.has_key ("length_unit")) {
        const std::string length_unit_str = config_.fetch_string("length_unit");
        default_length_unit = datatools::units::get_length_unit_from(length_unit_str);
      }

      if (config_.has_key ("length_unit_value")) {
        double length_unit_value = config_.fetch_real("length_unit_value");
        if (! config_.has_explicit_unit("length_unit_value")) {
          length_unit_value *= default_length_unit;
        }
        default_length_unit = length_unit_value;
      }

      if (config_.has_key("stl_file")) {
        std::string stl_filename = config_.fetch_string("stl_file");
        datatools::fetch_path_with_env(stl_filename);
        initialize_from_stl(stl_filename, default_length_unit);
      } else if (config_.has_key("load_file")) {
        std::string filename = config_.fetch_string("load_file");
        datatools::fetch_path_with_env(filename);
        load(filename);
      }
    }

    if (not is_locked()) lock();
    return;
  }

  void tessellated_solid::reset()
  {
    unlock();

    _vertices_.clear();
    _facets_.clear();
    _xrange_.reset();
    _yrange_.reset();
    _zrange_.reset();
    _facet_segments_.clear();
    _consistent_ = false;

    this->i_shape_3d::_reset();
    return;
  }

  bool tessellated_solid::is_valid() const
  {
    return _facets_.size() >= 4;
  }

  // virtual
  unsigned int tessellated_solid::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Invalid tessellated solid !");
    unsigned int nfaces = 0;

    for (facets_col_type::const_iterator ifacet = _facets_.begin();
         ifacet != _facets_.end();
         ifacet++) {
      uint32_t facet_index = ifacet->first;
      const facet34 & facet = ifacet->second;
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      finfo.grab_positioning().set_identity();
      if (facet.has_tface()) {
        finfo.set_face(facet.get_tface_handle());
      }
      if (facet.has_qface()) {
        finfo.set_face(facet.get_qface_handle());
      }
      face_identifier fid;
      fid.set_face_index(facet_index);
      finfo.set_identifier(fid);
      nfaces++;
    }

    return nfaces;
  }

  void tessellated_solid::generate_wires_self(wires_type & wires_,
                                              uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tessellated solid!");

    bool all_segments = (options_ & WR_TESSELLA_ALL_SEGMENTS);

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    if (!all_segments) {
      for (facet_segments_col_type::const_iterator iseg = _facet_segments_.begin();
           iseg != _facet_segments_.end();
           iseg++) {
        const facet_segment & fseg = iseg->second;
        if (!fseg.is_shown()) {
          continue;
        }
        line_3d l(fseg.get_vertex_position(facet_segment::VTX_ID_FIRST),
                  fseg.get_vertex_position(facet_segment::VTX_ID_SECOND));
        l.generate_wires_self(wires_, 0);
      }
    } else {
      for (facets_col_type::const_iterator ifacet = _facets_.begin();
           ifacet != _facets_.end();
           ifacet++) {
        const facet34 & facet = ifacet->second;
        if (facet.has_tface()) {
          facet.get_tface().generate_wires_self(wires_, base_options);
        }
        if (facet.has_qface()) {
          facet.get_qface().generate_wires_self(wires_, base_options);
        }
      }
    }
    return;
  }

  void tessellated_solid::store(const std::string & filename_, double length_unit_) const
  {
    std::string filename = filename_;
    datatools::fetch_path_with_env(filename);
    std::ofstream fout(filename);
    DT_THROW_IF(!fout, std::runtime_error, "Cannot open file '" << filename << "'");
    fout << "#!bayeux::geomtools::tessellated_solid" << '\n';
    store(fout, length_unit_);
    fout.close();
  }

  void tessellated_solid::load(const std::string & filename_)
  {
    std::string filename = filename_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open file '" << filename << "'");
    std::string header;
    std::getline(fin, header);
    if (header != "#!bayeux::geomtools::tessellated_solid") {
      DT_THROW(std::logic_error, "Invalid header");
    }
    load(fin);
    fin.close();
  }

  void tessellated_solid::store(std::ostream & out_,
                                double length_unit_) const
  {
    double length_unit = length_unit_;
    if (length_unit < 0.0) {
      length_unit = 1.0 * CLHEP::mm;
    }
    out_.precision(15);
    out_ << length_unit / CLHEP::mm << '\n';
    out_ << _vertices_.size() << '\n';
    for (vertices_col_type::const_iterator i = _vertices_.begin();
         i != _vertices_.end();
         i++) {
      out_ << i->first << ' ';
      const facet_vertex & vtx = i->second;
      double x = vtx.position.x() / length_unit;
      double y = vtx.position.y() / length_unit;
      double z = vtx.position.z() / length_unit;
      out_ << x << ' ' << y << ' ' << z << '\n';
    }
    out_ << _facets_.size() << '\n';
    for (facets_col_type::const_iterator i = _facets_.begin();
         i != _facets_.end();
         i++) {
       out_ << i->first << ' ';
       const facet34 & f34 = i->second;
       out_ << f34.get_number_of_vertices() << ' ';
       for (unsigned int j = 0; j < f34.get_number_of_vertices(); j++) {
         out_ << f34.get_vertex_key(j) << ' ';
       }
       out_ << '\n';
    }
    return;
  }

  void tessellated_solid::load(std::istream & in_)
  {
    reset();
    double length_unit = 1.0 * CLHEP::mm;
    in_ >> length_unit >> std::ws;
    if (!in_) {
      DT_THROW(std::logic_error, "Cannot read length unit");
    }
    length_unit *= CLHEP::mm;
    uint32_t nb_vertexes;
    in_ >> nb_vertexes >> std::ws;
    if (!in_) {
      DT_THROW(std::logic_error, "Cannot read number of vertexes");
    }
    for (uint32_t i = 0; i < nb_vertexes; i++) {
      uint32_t vtxKey;
      double x, y, z;
      in_ >> vtxKey >> x >> y >> z >> std::ws;
      if (!in_) {
        DT_THROW(std::logic_error, "Cannot read vertex");
      }
      x *= length_unit;
      y *= length_unit;
      z *= length_unit;
      add_vertex(vtxKey, x, y, z);
    }
    uint32_t nb_facets;
    in_ >> nb_facets >> std::ws;
    for (uint32_t i = 0; i < nb_facets; i++) {
      uint32_t facetKey;
      in_ >> facetKey >> std::ws;
      if (!in_) {
        DT_THROW(std::logic_error, "Cannot read facet key");
      }
      uint32_t nbVtx;
      in_ >> nbVtx >> std::ws;
      if (!in_) {
        DT_THROW(std::logic_error, "Cannot read number of vertexes");
      }
      uint32_t vtx0key, vtx1key, vtx2key;
      in_ >> vtx0key >> vtx1key >> vtx2key >> std::ws;
      if (!in_) {
        DT_THROW(std::logic_error, "Cannot read vertex keys");
      }
      if (nbVtx == 3) {
        add_facet3(facetKey, vtx0key, vtx1key, vtx2key);
      }
      if (nbVtx == 4) {
        uint32_t vtx3key;
        in_ >> vtx3key >> std::ws;
        if (!in_) {
          DT_THROW(std::logic_error, "Cannot read fourth vertex key");
        }
        add_facet4(facetKey, vtx0key, vtx1key, vtx2key, vtx3key);
      }
    }
    lock();
    return ;
  }

} // end of namespace geomtools
