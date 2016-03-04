// wall_solid.cc

// Ourselves:
#include <geomtools/wall_solid.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/properties.h>

// This project:
#include <geomtools/quadrangle.h>
#include <geomtools/triangle.h>
#include <geomtools/placement.h>
#include <geomtools/i_shape_2d.h>
#include <geomtools/tessellation.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(wall_solid, "geomtools::wall_solid")

  const std::string & wall_solid::wall_solid_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "wall_solid";
    }
    return label;
  }

  double wall_solid::get_xmin() const
  {
    return _base_.get_xrange().get_min();
  }

  double wall_solid::get_xmax() const
  {
    return _base_.get_xrange().get_max();
  }

  double wall_solid::get_ymin() const
  {
    return _base_.get_yrange().get_min();
  }

  double wall_solid::get_ymax() const
  {
    return _base_.get_yrange().get_max();
  }

  double wall_solid::get_zmin() const
  {
    return -0.5 * _z_;
  }

  double wall_solid::get_zmax() const
  {
    return +0.5 * _z_;
  }

  double wall_solid::get_z() const
  {
    return _z_;
  }

  void wall_solid::set_z(double z_)
  {
    DT_THROW_IF(z_ < 0.0, std::logic_error, "Invalid '" << z_ << "' Z value !");
    _z_ = z_;
    return;
  }

  const simple_polygon & wall_solid::get_base() const
  {
    return _base_;
  }

  simple_polygon & wall_solid::grab_base()
  {
    return _base_;
  }

  wall_solid::wall_solid() : i_shape_3d(DEFAULT_SKIN)
  {
    _set_defaults();
    return;
  }

  wall_solid::~wall_solid()
  {
    return;
  }

  std::string wall_solid::get_shape_name() const
  {
    return wall_solid_label();
  }

  bool wall_solid::is_valid() const
  {
    return datatools::is_valid(_z_) && _base_.is_valid();
  }

  void wall_solid::_set_defaults()
  {
    datatools::invalidate(_z_);
    return;
  }

  // virtual
  void wall_solid::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {

    if (! is_valid()) {
      this->i_shape_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF (! config_.has_key("z"), std::logic_error, "Missing box 'z' property !");

      if (!datatools::is_valid(_z_)) {
        double z = config_.fetch_real("z");
        if (! config_.has_explicit_unit("z")) {
          z *= lunit;
        }
        set_z(z);
      }

      if (!_base_.is_valid()) {
        datatools::properties base_config;
        config_.export_and_rename_starting_with(base_config, "base.", "");
        _base_.initialize(base_config, objects_);
      }

    } // ! is_valid
    lock();
    return;
  }

  // virtual
  void wall_solid::reset()
  {
    unlock();
    _base_.reset();
    _set_defaults();
    this->i_shape_3d::_reset();
    return;
  }

  void wall_solid::compute_bottom_face(composite_surface & face_,
                                       placement & face_placement_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    face_.reset();
    face_placement_.reset();
    for (int itriangle = 0; itriangle < (int) _base_.number_of_triangles(); itriangle++) {
      const triangle & t = _base_.get_triangle(itriangle);
      geomtools::face_info & finfo = face_.add();
      geomtools::triangle & tfacet = finfo.add_face<geomtools::triangle>();
      if (_base_.is_anticlockwise()) {
        tfacet.set_vertexes(t.get_vertex_0(), t.get_vertex_1(), t.get_vertex_2());
      } else {
        tfacet.set_vertexes(t.get_vertex_2(), t.get_vertex_1(), t.get_vertex_0());
      }
      finfo.set_identity_positioning();
    }
    face_placement_.set(0.0, 0.0, -0.5 * _z_, 0.0, 0.0, 0.0);
    return;
  }

  void wall_solid::compute_top_face(composite_surface & face_,
                                    placement & face_placement_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    face_.reset();
    face_placement_.reset();
    for (int itriangle = 0; itriangle < (int) _base_.number_of_triangles(); itriangle++) {
      const triangle & t = _base_.get_triangle(itriangle);
      geomtools::face_info & finfo = face_.add();
      geomtools::triangle & tfacet = finfo.add_face<geomtools::triangle>();
      if (_base_.is_clockwise()) {
        tfacet.set_vertexes(t.get_vertex_0(), t.get_vertex_1(), t.get_vertex_2());
      } else {
        tfacet.set_vertexes(t.get_vertex_2(), t.get_vertex_1(), t.get_vertex_0());
      }
      finfo.set_identity_positioning();
    }
    face_placement_.set(0.0, 0.0, +0.5 * _z_, 0.0, 0.0, 0.0);
    return;
  }

  void wall_solid::compute_side_face(composite_surface & face_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    face_.reset();
    for (int ivtx0 = 0; ivtx0 < (int) _base_.get_number_of_vertices(); ivtx0++) {
      int ivtx1 = (ivtx0 + 1) % _base_.get_number_of_vertices();
      const vector_2d & vtx0 = _base_.get_vertex(ivtx0);
      const vector_2d & vtx1 = _base_.get_vertex(ivtx1);
      geomtools::face_info & finfo = face_.add();
      geomtools::quadrangle & qfacet = finfo.add_face<geomtools::quadrangle>();
      geomtools::vector_3d A(vtx0.x(), vtx0.y(), -0.5 * _z_);
      geomtools::vector_3d B(vtx1.x(), vtx1.y(), -0.5 * _z_);
      geomtools::vector_3d C(vtx1.x(), vtx1.y(), +0.5 * _z_);
      geomtools::vector_3d D(vtx0.x(), vtx0.y(), +0.5 * _z_);
      if (_base_.is_clockwise()) {
        qfacet.set_vertexes(A, B, C, D);
      } else {
        qfacet.set_vertexes(B, A, D, C);
      }
      finfo.set_identity_positioning();
    }
    return;
  }

  // virtual
  unsigned int wall_solid::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    unsigned int nfaces = 0;

    {
      // Side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      finfo.set_identity_positioning();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_side_face(cs);
      face_identifier fid;
      fid.set_face_bit(FACE_SIDE);
      finfo.set_identifier(fid);
      nfaces++;
    }

    {
      // Bottom face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_bottom_face(cs, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_BOTTOM);
      finfo.set_identifier(fid);
      nfaces++;
    }

    {
      // Top face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_top_face(cs, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_TOP);
      finfo.set_identifier(fid);
      nfaces++;
    }

    return nfaces;
  }

  double wall_solid::get_surface(uint32_t flags_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    double s = 0.0;

    if (flags_ & FACE_SIDE) {
      composite_surface face;
      compute_side_face(face);
      double s_out = face.get_surface();
      s += s_out;
    }

    if (flags_ & FACE_BOTTOM) {
      composite_surface face;
      placement dummy;
      compute_bottom_face(face, dummy);
      s += face.get_surface();
    }

    if (flags_ & FACE_TOP) {
      composite_surface face;
      placement dummy;
      compute_top_face(face, dummy);
      s += face.get_surface();
    }

    return s;
  }

  double wall_solid::get_volume(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    double v = _z_ * get_surface(FACE_BOTTOM);
    return v;
  }

  // virtual
  bool wall_solid::is_inside(const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;
    if (std::abs(point_.z()) >= 0.5 * _z_ - hskin) {
      return false;
    }
    vector_3d pos(point_.x(), point_.y(), 0.0);
    if (!_base_.is_on_surface(pos, 0.0)) {
      return false;
    }
    return true;
  }

  // virtual
  bool wall_solid::is_outside(const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    double skin = compute_tolerance(skin_);
    double hskin = 0.5 * skin;
    if (std::abs(point_.z()) >= 0.5 * _z_ + hskin) {
      return true;
    }
    vector_3d pos(point_.x(), point_.y(), 0.0);
    if (!_base_.is_on_surface(pos, 0.0)) {
      return true;
    }
    return false;
  }

  // virtual
  face_identifier wall_solid::on_surface(const vector_3d & position_,
                                         const face_identifier & surface_mask_,
                                         double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    double skin = get_skin(skin_);

    face_identifier mask;
    if (surface_mask_.is_valid()) {
      DT_THROW_IF(! surface_mask_.is_face_bits_mode(), std::logic_error,
                  "Face mask uses no face bits!");
      mask = surface_mask_;
    } else {
      mask.set_face_bits_any();
    }

    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (finfo.is_valid() && mask.has_face_bit(finfo.get_identifier().get_face_bits())) {
        vector_3d position_c;
        finfo.get_positioning().mother_to_child(position_, position_c);
        if (finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
      }
    }

    return face_identifier::face_invalid();
  }

  // virtual
  vector_3d wall_solid::get_normal_on_surface(const vector_3d & position_,
                                              const face_identifier & surface_bit_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    vector_3d normal;
    geomtools::invalidate(normal);
    switch(surface_bit_.get_face_bits()) {
    case FACE_SIDE:
      {
        composite_surface face;
        compute_side_face(face);
        normal = face.get_normal_on_surface(position_, false);
      }
      break;
    case FACE_BOTTOM:
      {
        composite_surface face;
        placement face_pl;
        compute_bottom_face(face, face_pl);
        vector_3d position_c;
        face_pl.mother_to_child(position_c);
        vector_3d normal_c = face.get_normal_on_surface(position_c, false);
        normal = face_pl.child_to_mother_direction(normal_c);
      }
      break;
    case FACE_TOP:
      {
        composite_surface face;
        placement face_pl;
        compute_top_face(face, face_pl);
        vector_3d position_c;
        face_pl.mother_to_child(position_c);
        vector_3d normal_c = face.get_normal_on_surface(position_c, false);
        normal = face_pl.child_to_mother_direction(normal_c);
      }
      break;
    }
    return normal;

  }

  // virtual
  bool wall_solid::find_intercept(const vector_3d & from_,
                                  const vector_3d & direction_,
                                  face_intercept_info & intercept_,
                                  double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid wall solid!");
    intercept_.reset ();

    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 3;
    face_intercept_info intercepts[NFACES];
    unsigned int candidate_impact_counter = 0;


    int face_counter = 0;
    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (!finfo.is_valid()) {
        continue;
      }
      const i_shape_2d & face = finfo.get_face_ref();
      const placement & face_placement = finfo.get_positioning();
      const face_identifier & face_id = finfo.get_identifier();
      if (face.i_find_intercept::find_intercept(from_,
                                                direction_,
                                                face_placement,
                                                intercepts[face_counter],
                                                skin)) {
        intercepts[face_counter].set_face_id(face_id);
        candidate_impact_counter++;
      }
      face_counter++;
    }

    if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
        if (intercepts[iface].is_ok()) {
          double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
          if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            intercept_.set_face_id(intercepts[iface].get_face_id());
            intercept_.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    return intercept_.is_ok();
  }

  // virtual
  void wall_solid::_build_bounding_data()
  {
    _grab_bounding_data().make_box(get_xmin(), get_xmax(),
                                   get_ymin(), get_ymax(),
                                   get_zmin(), get_zmax());
    return;
  }

  void wall_solid::generate_wires_self(wires_type & wires_,
                                       uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    bool draw_bottom      = !(options_ & WR_WALL_NO_BOTTOM_FACE);
    bool draw_top         = !(options_ & WR_WALL_NO_TOP_FACE);
    bool draw_side        = !(options_ & WR_WALL_NO_SIDE_FACE);
    bool draw_boundings   =  (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings && has_bounding_data()) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    if (draw_bottom) {
      composite_surface bottom_face;
      placement bottom_pl;
      compute_bottom_face(bottom_face, bottom_pl);
      uint32_t options = base_options;
      bottom_face.generate_wires(wires_, bottom_pl, options);
    }

    if (draw_top) {
      composite_surface top_face;
      placement top_pl;
      compute_top_face(top_face, top_pl);
      uint32_t options = base_options;
      top_face.generate_wires(wires_, top_pl, options);
    }

    if (draw_side) {
      uint32_t options = base_options;
      composite_surface side_face;
      compute_side_face(side_face);
      side_face.generate_wires_self(wires_, options);
    }

    return;
  }

  void wall_solid::tree_dump(std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);


    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z : " << get_z() / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Base : " << std::endl;
    if (_base_.is_valid()) {
      std::ostringstream indent2_oss;
      indent2_oss << indent_ << datatools::i_tree_dumpable::skip_tag;
      _base_.tree_dump(out_, "", indent2_oss.str());
    }

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Valid : " << is_valid()
         << std::endl;

    return;
  }

  void wall_solid::convert_to_tessellated(tessellated_solid & ts_) const
  {
    bool devel = false;
    // devel = true;
    ts_.reset();

    // Build vertices:
    if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                         << "base.vertices = " << _base_.get_number_of_vertices()
                         << std::endl;
    for (int ivtx = 0; ivtx < (int) _base_.get_number_of_vertices(); ivtx++) {
      const vector_2d & vtx_base = _base_.get_vertex(ivtx);
      int key_vtx = 2 * ivtx;
      ts_.add_vertex(key_vtx++, vtx_base.x(), vtx_base.y(), -0.5 * _z_);
      ts_.add_vertex(key_vtx,   vtx_base.x(), vtx_base.y(), +0.5 * _z_);
    }
    if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                         << "ts.vertices = " << ts_.vertices().size()
                         << std::endl;

    // Build facets:
    int key_facet = 0;
    bool clockwise = _base_.is_clockwise();
    if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                         << "base.clockwise = " << clockwise
                         << std::endl;

    // Side:
    int modulo = 2 * _base_.get_number_of_vertices();
    for (int ifct4 = 0; ifct4 < (int) _base_.get_number_of_vertices(); ifct4++) {
      int iv0 = (2 *  ifct4)          % modulo;
      int iv1 = (2 * (ifct4 + 1))     % modulo;
      int iv2 = (2 *  ifct4) + 1      % modulo;
      int iv3 = (2 * (ifct4 + 1) + 1) % modulo;
      if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                           << "adding side facet [#" << key_facet << "] = "
                           << " (" << iv0 << ',' << iv1 << ',' << iv3 << ',' << iv2 << ')'
                           << "..."
                           << std::endl;
      if (clockwise) {
        ts_.add_facet4(key_facet++, iv2, iv3, iv1, iv0);
      } else {
        ts_.add_facet4(key_facet++, iv0, iv1, iv3, iv2);
      }
    }
    if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                         << "ts.facets = " << ts_.facets().size()
                         << std::endl;

    // Top/bottom:
    for (int itr = 0; itr < (int) _base_.number_of_triangles(); itr++) {
      if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                           << "Triangle [#" << itr << "]" << std::endl;
      const triangle & tr = _base_.get_triangle(itr);
      int key_vtx[3];
      key_vtx[0] = -1;
      key_vtx[1] = -1;
      key_vtx[2] = -1;
      for (int ivtx_tr = 0; ivtx_tr < 3; ivtx_tr++) {
        const vector_3d & vtx_tr = tr.get_vertex(ivtx_tr);
        if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                             << "  Vertex [#" << ivtx_tr << "] = " << vtx_tr << std::endl;
        for (int ivtx_base = 0; ivtx_base < (int) _base_.get_number_of_vertices(); ivtx_base++) {
          const vector_2d & vtx_base = _base_.get_vertex(ivtx_base);
          if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                               << "    Checking base vertex [#" << ivtx_base << "] = " << vtx_base << std::endl;
          if (vtx_base.x() == vtx_tr.x() && vtx_base.y() == vtx_tr.y() ) {
            key_vtx[ivtx_tr] = 2 * ivtx_base;
            if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                                 << "    --> Found a matching base vertex [#" << ivtx_base << "]" << std::endl;
            break;
          }
        }
        DT_THROW_IF(key_vtx[ivtx_tr] == -1, std::logic_error,
                    "Cannot find the key for vertex [#" << ivtx_tr << "] "
                    << "in base triangle [#" << itr << "]");
      }
      if (!clockwise) {
        ts_.add_facet3(key_facet++,
                       key_vtx[2],
                       key_vtx[1],
                       key_vtx[0]);
        ts_.add_facet3(key_facet++,
                       (key_vtx[0] + 1) % modulo,
                       (key_vtx[1] + 1) % modulo,
                       (key_vtx[2] + 1) % modulo);
      } else {
        ts_.add_facet3(key_facet++,
                       key_vtx[0],
                       key_vtx[1],
                       key_vtx[2]);
        ts_.add_facet3(key_facet++,
                       (key_vtx[2] + 1) % modulo,
                       (key_vtx[1] + 1) % modulo,
                       (key_vtx[0] + 1) % modulo);
      }
    }
    if (devel) std::cerr << "DEVEL: " << "wall_solid::convert_to_tessellated: "
                         << "ts.facets = " << ts_.facets().size()
                         << std::endl;
    ts_.lock();
    return;
  }

} // end of namespace geomtools
