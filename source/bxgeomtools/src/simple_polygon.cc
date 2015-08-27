// simple_polygon.cc

// Ourselves:
#include <geomtools/simple_polygon.h>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/temporary_files.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/linear_system_solver.h>

// This project:
#include <geomtools/triangle.h>
#include <geomtools/plane.h>
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

namespace geomtools {

  // static
  simple_polygon::build_mode_type
  simple_polygon::build_mode_from_label(const std::string & label_)
  {
    if (label_ == label_from_build_mode(BUILD_MODE_BY_VERTICES)) return BUILD_MODE_BY_VERTICES;
    if (label_ == label_from_build_mode(BUILD_MODE_BY_SEGMENTS)) return BUILD_MODE_BY_SEGMENTS;
    return BUILD_MODE_INVALID;
  }

  // static
  std::string simple_polygon::label_from_build_mode(build_mode_type mode_)
  {
    switch (mode_) {
    case BUILD_MODE_BY_VERTICES:
      return std::string("vertices");
    case BUILD_MODE_BY_SEGMENTS:
      return std::string("segments");
    default:
      return std::string("");
    }
  }

  const std::string & simple_polygon::simple_polygon_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "simple_polygon";
    }
    return label;
  }

  simple_polygon::wall_segment_type::wall_segment_type()
  {
    geomtools::invalidate(start);
    geomtools::invalidate(stop);
    datatools::invalidate(left_thickness);
    datatools::invalidate(right_thickness);
    datatools::invalidate(start_angle);
    datatools::invalidate(stop_angle);
    return;
  }

  bool simple_polygon::is_valid() const
  {
    return _vertices_.size() >= 3;
  }

  std::string simple_polygon::get_shape_name() const
  {
    return simple_polygon_label();
  }

  void simple_polygon::set_build_mode(build_mode_type m_)
  {
    _build_mode_ = m_;
    return;
  }

  simple_polygon::build_mode_type simple_polygon::get_build_mode() const
  {
    return _build_mode_;
  }

  // virtual
  double simple_polygon::get_perimeter(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    double p = 0;
    for (int i = 0; i < (int) _vertices_.size(); i++) {
      p += (
            get_vertex( (i+1) % _vertices_.size() )
            -
            get_vertex(i)
            ).mag()
        ;
    }
    return p;
  }

  // virtual
  double simple_polygon::get_surface(uint32_t flags_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    double s = 0.0;
    for (int i = 0; i < (int) _triangles_.size(); i++) {
      const triangle & t = _triangles_[i];
      s += t.get_surface(flags_);
    }
    return s;
  }

  unsigned int simple_polygon::get_number_of_vertices() const
  {
    return _vertices_.size();
  }

  const vector_2d & simple_polygon::get_vertex(int index_) const
  {
    DT_THROW_IF(index_ < 0 || index_ >= (int)_vertices_.size(),
                std::domain_error, "Invalid vertex index [" << index_ << "]!");
    return _vertices_[index_];
  }

  void simple_polygon::add_vertex(double x_, double y_)
  {
    add_vertex(vector_2d(x_, y_));
    return;
  }

  bool simple_polygon::_check_vertex(const vector_2d & vtx_, bool no_first_check_) const
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    logging = datatools::logger::PRIO_DEBUG;
    if (_vertices_.size() < 3) {
      DT_LOG_DEBUG(logging, "No need to check new vertex " << vtx_ / CLHEP::mm << " [mm]");
      return true;
    }
    DT_LOG_DEBUG(logging, "Check new vertex " << vtx_ / CLHEP::mm << " [mm]");

    const vector_2d & L = _vertices_.back();
    const vector_2d & N = vtx_;
    vector_2d uLN = (N - L).unit();

    /* Check intersection of the new edge with the current edges of the polygon:
     *                   N <- new vertex
     *                    *
     *                   / new candidate edge
     *                  /
     *   B    edge     /             E
     *     *----------o------------*
     *               / I <- intersection
     *              /
     *             /
     *            *
     *          L <- last vertex
     */
    int first_edge_index = 0;
    if (no_first_check_) first_edge_index = 1;
    for (int iedge = first_edge_index; iedge < (int) _vertices_.size() - 2; iedge++) {
      // Check each existing edge but the last one:
      DT_LOG_DEBUG(logging, "  Check new vertex " << vtx_ / CLHEP::mm << " [mm] with edge [#" << iedge << "]");
      const vector_2d & B = _vertices_[iedge];
      const vector_2d & E = _vertices_[iedge + 1];

      vector_2d I;
      bool found_intersection = find_segment_2d_intersection(N, L, B, E, I);
      if (found_intersection) {
        DT_LOG_DEBUG(logging,
                     "    Intersection on edge [#" << iedge<< "] at point " << I / CLHEP::mm << " [mm]!");
        return false;
      }
    } // for iedge
    return true;
  }

  void simple_polygon::_add_vertex(const vector_2d & vtx_)
  {
    {
      bool check = _check_vertex(vtx_);
      DT_THROW_IF(!check, std::logic_error, "Invalid vertex " << vtx_ / CLHEP::mm << " [mm] !");
    }
    _vertices_.push_back(vtx_);
    _xrange_.add(vtx_.x());
    _yrange_.add(vtx_.y());
    return;
  }

  void simple_polygon::add_vertex(const vector_2d & vtx_)
  {
    DT_THROW_IF(_build_mode_ != BUILD_MODE_BY_VERTICES, std::logic_error,
                "The simple polygon build mode is not valid!");
    _add_vertex(vtx_);
    return;
  }

  void simple_polygon::add_wall_segment(const vector_2d & start_,
                                        const vector_2d & stop_,
                                        double left_thickness_,
                                        double right_thickness_,
                                        double start_angle_,
                                        double stop_angle_)
  {
    DT_THROW_IF(_build_mode_ != BUILD_MODE_BY_SEGMENTS, std::logic_error,
                "The simple polygon build mode is not valid!");
    DT_THROW_IF(!_wall_segments_.empty(), std::logic_error,
                "There is already a first wall segment!");
    DT_THROW_IF(!geomtools::is_valid(start_), std::logic_error,
                "Invalid start for wall segment [#" << _wall_segments_.size() << "]!");
    DT_THROW_IF(!geomtools::is_valid(stop_), std::logic_error,
                "Invalid stop for wall segment [#" << _wall_segments_.size() << "]!");
    // std::cerr << "DEVEL: " << "simple_polygon::add_wall_segment: "
    //           << "left_thickness=" << left_thickness_ << std::endl;
    // std::cerr << "DEVEL: " << "simple_polygon::add_wall_segment: "
    //           << "right_thickness=" << right_thickness_ << std::endl;
    _add_wall_segment(start_, stop_,
                      left_thickness_, right_thickness_,
                      start_angle_, stop_angle_);
    return;
  }

  void simple_polygon::add_wall_segment(const vector_2d & stop_,
                                        double left_thickness_,
                                        double right_thickness_,
                                        double stop_angle_)
  {
    DT_THROW_IF(_build_mode_ != BUILD_MODE_BY_SEGMENTS, std::logic_error,
                "The simple polygon build mode is not valid!");
    DT_THROW_IF(_wall_segments_.size() == 0, std::logic_error,
                "There is no first wall segment!");
    vector_2d dummy_start;
    geomtools::invalidate(dummy_start);
    double dummy_start_angle = datatools::invalid_real_double();
    double stop_angle = datatools::invalid_real_double();
    if (datatools::is_valid(stop_angle_)) {
      stop_angle = stop_angle_;
    }
    // std::cerr << "DEVEL: " << "simple_polygon::add_wall_segment: "
    //           << "left_thickness=" << left_thickness_ << std::endl;
    // std::cerr << "DEVEL: " << "simple_polygon::add_wall_segment: "
    //           << "right_thickness=" << right_thickness_ << std::endl;
    _add_wall_segment(dummy_start, stop_,
                      left_thickness_, right_thickness_,
                      dummy_start_angle, stop_angle);
    return;
  }

  bool simple_polygon::walls_are_terminated() const
  {
    DT_THROW_IF(_build_mode_ != BUILD_MODE_BY_SEGMENTS, std::logic_error,
                "The simple polygon build mode is not valid!");
    if (_wall_segments_.size() == 0) {
      // std::cerr << "DEVEL: " << "simple_polygon::walls_are_terminated: "
      //           << "No wall segment yet!"
      //           << std::endl;
      return false;
    } else {
      if (! datatools::is_valid(_wall_segments_.back().stop_angle)) {
        // std::cerr << "DEVEL: " << "simple_polygon::walls_are_terminated: "
        //           << "Last wall segment has no valid stop angle!"
        //           << std::endl;
        return false;
      }
    }
    return true;
  }

  void simple_polygon::_add_wall_segment(const vector_2d & start_,
                                         const vector_2d & stop_,
                                         double left_thickness_,
                                         double right_thickness_,
                                         double start_angle_,
                                         double stop_angle_)
  {
    // std::cerr << "DEVEL: " << "simple_polygon::_add_wall_segment: "
    //           << "Entering..."
    //           << std::endl;
    double start_angle = 0.0;
    double stop_angle  = datatools::invalid_real_double();
    if (_wall_segments_.size() == 0) {
      // Adding first wall segment:
      DT_THROW_IF(!geomtools::is_valid(start_), std::logic_error,
                  "Invalid start for wall segment [#" << _wall_segments_.size() << "]!");
      DT_THROW_IF(!datatools::is_valid(start_angle), std::logic_error,
                  "Invalid start angle for wall segment [#" << _wall_segments_.size() << "]!");
      DT_THROW_IF(std::abs(start_angle) >= 0.5 * M_PI, std::logic_error, "Invalid start angle value !");
      start_angle = start_angle_;
    } else {
      DT_THROW_IF(geomtools::is_valid(start_angle_), std::logic_error,
                  "Unused start angle for wall segment [#" << _wall_segments_.size() << "]!");

    }
    DT_THROW_IF(walls_are_terminated(), std::logic_error, "Cannot add a new wall");
    DT_THROW_IF(!geomtools::is_valid(stop_), std::logic_error,
                "Invalid stop for wall segment [#" << _wall_segments_.size() << "]!");
    if (geomtools::is_valid(stop_angle_)) {
      DT_THROW_IF(std::abs(stop_angle_) >= 0.5 * M_PI, std::logic_error, "Invalid stop angle value !");
      stop_angle = stop_angle_;
    }
    DT_THROW_IF(!datatools::is_valid(left_thickness_), std::logic_error,
                "Invalid left thickness for wall segment [#" << _wall_segments_.size() << "]!");
    DT_THROW_IF(!datatools::is_valid(right_thickness_), std::logic_error,
                "Invalid right thickness for wall segment [#" << _wall_segments_.size() << "]!");
    // std::cerr << "DEVEL: " << "simple_polygon::_add_wall_segment: "
    //           << "left_thickness=" << left_thickness_ << std::endl;
    // std::cerr << "DEVEL: " << "simple_polygon::_add_wall_segment: "
    //           << "right_thickness=" << right_thickness_ << std::endl;
    DT_THROW_IF(left_thickness_ + right_thickness_ == 0.0, std::logic_error,
                "Invalid full thickness for wall segment [#" << _wall_segments_.size() << "]!");
    {
      wall_segment_type dummy_ws;
      _wall_segments_.push_back(dummy_ws);
    }
    wall_segment_type & wseg = _wall_segments_.back();
    if (geomtools::is_valid(start_)) {
      wseg.start = start_;
    }
    wseg.stop  = stop_;
    wseg.left_thickness  = left_thickness_;
    wseg.right_thickness = right_thickness_;
    wseg.start_angle = start_angle;
    wseg.stop_angle = stop_angle;
    return;
  }

  void simple_polygon::_set_defaults()
  {
    _build_mode_ = BUILD_MODE_INVALID;
    _convex_ = false;
    geomtools::invalidate(_normal_);
    return;
  }

  simple_polygon::simple_polygon()
  {
    _set_defaults();
    return;
  }

  // virtual
  simple_polygon::~simple_polygon()
  {
    reset();
    return;
  }

  void simple_polygon::reset()
  {
    _triangles_.clear();
    _vertices_.clear();
    _wall_segments_.clear();
    _xrange_.reset();
    _yrange_.reset();
    _set_defaults();
    return;
  }

  // virtual
  bool simple_polygon::is_on_surface(const vector_3d & pos_,
                                     double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    double z_tolerance = compute_tolerance(tolerance_);
    for (int i = 0; i < (int) _triangles_.size(); i++) {
      const triangle & t = _triangles_[i];
      if (t.is_on_surface(pos_, z_tolerance)) {
        return true;
      }
    }
    return false;
  }

  // virtual
  vector_3d simple_polygon::get_normal_on_surface(const vector_3d & position_,
                                                  bool check_,
                                                  double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    return _normal_;
  }

  bool simple_polygon::is_clockwise() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    return _normal_.z() < 0.0;
  }

  bool simple_polygon::is_anticlockwise() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    return _normal_.z() > 0.0;
  }

  // virtual
  bool simple_polygon::find_intercept(const vector_3d & from_,
                                      const vector_3d & direction_,
                                      face_intercept_info & intercept_,
                                      double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    intercept_.reset();
    vector_3d O(0.0, 0.0, 0.0);
    vector_3d I(1.0, 0.0, 0.0);
    vector_3d J(0.0, 1.0, 0.0);
    plane pxy(get_vertex(0), get_vertex(1) - get_vertex(0),  get_vertex(2) - get_vertex(0));
    bool res = pxy.find_intercept(from_, direction_, intercept_, tolerance_);
    if (res) {
      if (!is_on_surface(intercept_.get_impact(), tolerance_)) {
        intercept_.reset();
      }
    }
    return intercept_.is_ok();
  }

  void simple_polygon::tree_dump(std::ostream & out_,
                                 const std::string & title_,
                                 const std::string & indent_,
                                 bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Build mode : '" << label_from_build_mode(_build_mode_) << "'"
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Valid : " << is_valid()
         << std::endl;

    if (is_valid()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Convex : " << is_valid()
           << std::endl;
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Normal : " << _normal_
           << std::endl;
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "X range : " << '[' << _xrange_.get_min() / CLHEP::mm << ':' << _xrange_.get_max() / CLHEP::mm << ']'
           << " [mm]"
           << std::endl;
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Y range : " << '[' << _yrange_.get_min() / CLHEP::mm << ':' << _yrange_.get_max() / CLHEP::mm << ']'
           << " [mm]"
           << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Wall segments : " << '[' << _wall_segments_.size() << ']'
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Vertices : " << '[' << _vertices_.size() << ']'
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Triangles : " << '[' << _triangles_.size() << ']'
         << std::endl;

    return;
  }

  void simple_polygon::generate_wires_self(wires_type & wires_,
                                           uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    bool with_internal_edges = (options_ & WR_SP_INTERNAL_EDGES);
    uint32_t base_options = options_ & WR_BASE_MASK;
    bool with_grid = base_options & geomtools::i_wires_3d_rendering::WR_BASE_GRID;
    {
      {
        polyline_type dummy;
        wires_.push_back(dummy);
      }
      polyline_type & segment = wires_.back();
      for (int ivtx = 0; ivtx <= (int) _vertices_.size(); ivtx++) {
        const vector_3d & v = _vertices_[ivtx % _vertices_.size()];
        segment.push_back(v);
      }
    }

    {
      uint32_t triangle_options = base_options;
      if (! with_grid) {
        if (! with_internal_edges) {
          triangle_options |= triangle::WR_TRIANGLE_NO_EXTERNAL_EDGES;
        }
      }
      for (int i = 0; i < (int) _triangles_.size(); i++) {
        const triangle & t = _triangles_[i];
        t.generate_wires_self(wires_, triangle_options);
      }
    }

    return;
  }

  // virtual
  unsigned int simple_polygon::compute_vertexes(vertex_col_type & vertexes_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid simple polygon!");
    vertexes_.clear();
    vertexes_.reserve(_vertices_.size());
    for (int i = 0; i < (int) _vertices_.size(); i++) {
      vector_3d vtx(_vertices_[i].x(), _vertices_[i].y(), 0.0);
      vertexes_.push_back(vtx);
    }
    return vertexes_.size();
  }

  unsigned int simple_polygon::number_of_triangles() const
  {
    return _triangles_.size();
  }

  const triangle & simple_polygon::get_triangle(int index_) const
  {
    return _triangles_[index_];
  }

  bool simple_polygon::is_convex() const
  {
    return _convex_;
  }

  const mygsl::min_max & simple_polygon::get_xrange() const
  {
    return _xrange_;
  }

  const mygsl::min_max & simple_polygon::get_yrange() const
  {
    return _yrange_;
  }

  bool simple_polygon::is_initialized() const
  {
    return _build_mode_ != BUILD_MODE_INVALID
      && _triangles_.size() > 0;
  }

  void simple_polygon::initialize(const datatools::properties & config_)
  {
    // std::cerr << "DEVEL: simple_polygon::initialize: " << "Entering..." << std::endl;

    // Fetch property:
    if (_build_mode_ == BUILD_MODE_INVALID) {
      // std::cerr << "DEVEL: simple_polygon::initialize: " << "BUILD_MODE_INVALID..." << std::endl;

      if (config_.has_key("build_mode")) {
        std::string label = config_.fetch_string("build_mode");
        // std::cerr << "DEVEL: simple_polygon::initialize: " << "label='" << label << "'" << std::endl;
        build_mode_type bmode = build_mode_from_label(label);
        DT_THROW_IF(bmode == BUILD_MODE_INVALID,
                    std::logic_error,
                    "Invalid build mode!");
        set_build_mode(bmode);
      }

      double default_length_unit = CLHEP::mm;
      double default_angle_unit  = CLHEP::degree;

      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        default_length_unit = datatools::units::get_length_unit_from(lunit_str);
      }

      if (config_.has_key("angle_unit")) {
        const std::string aunit_str = config_.fetch_string("angle_unit");
        default_angle_unit = datatools::units::get_angle_unit_from(aunit_str);
      }

      if (_build_mode_ == BUILD_MODE_BY_VERTICES) {

        if (config_.has_key("vertices")) {
          std::vector<std::string> vertice_labels;
          config_.fetch("vertices", vertice_labels);
          DT_THROW_IF(vertice_labels.size() < 3,
                      std::logic_error,
                      "Number of vertices is too low!");
          for (int i = 0; i < (int) vertice_labels.size(); i++) {
            std::ostringstream x_vtx_key_oss;
            x_vtx_key_oss << "vertices." << vertice_labels[i] << ".x";
            std::ostringstream y_vtx_key_oss;
            y_vtx_key_oss << "vertices." << vertice_labels[i] << ".y";

            std::string x_vtx_key = x_vtx_key_oss.str();
            std::string y_vtx_key = y_vtx_key_oss.str();

            DT_THROW_IF(!config_.has_key(x_vtx_key), std::logic_error,
                        "Missing key '" << x_vtx_key << "'!");

            DT_THROW_IF(!config_.has_key(y_vtx_key), std::logic_error,
                        "Missing key '" << y_vtx_key << "'!");

            double x_vtx = config_.fetch_real(x_vtx_key);
            if (!config_.has_explicit_unit(x_vtx_key)) {
              x_vtx *= default_length_unit;
            }

            double y_vtx = config_.fetch_real(y_vtx_key);
            if (!config_.has_explicit_unit(y_vtx_key)) {
              y_vtx *= default_length_unit;
            }

            add_vertex(x_vtx, y_vtx);
          } // Loop on vertexes
        }

      } // _build_mode_ == BUILD_MODE_BY_VERTICES


      if (_build_mode_ == BUILD_MODE_BY_SEGMENTS) {
        if (config_.has_key("segments")) {
          std::vector<std::string> segment_labels;
          config_.fetch("segments", segment_labels);
          DT_THROW_IF(segment_labels.size() < 1,
                      std::logic_error,
                      "Number of wall segments is too low!");
          for (int i = 0; i < (int) segment_labels.size(); i++) {

            vector_2d segment_start;
            vector_2d segment_stop;
            double segment_right_thickness;
            double segment_left_thickness;
            double segment_start_angle;
            double segment_stop_angle;
            geomtools::invalidate(segment_start);
            geomtools::invalidate(segment_stop);
            datatools::invalidate(segment_right_thickness);
            datatools::invalidate(segment_left_thickness);
            datatools::invalidate(segment_start_angle);
            datatools::invalidate(segment_stop_angle);

            if (i == 0) {
              // First wall segment:

              // Start position:
              std::ostringstream start_x_segment_key_oss;
              start_x_segment_key_oss << "segments." << segment_labels[i] << ".start_x";
              std::ostringstream start_y_segment_key_oss;
              start_y_segment_key_oss << "segments." << segment_labels[i] << ".start_y";

              std::string start_x_segment_key = start_x_segment_key_oss.str();
              std::string start_y_segment_key = start_y_segment_key_oss.str();

              DT_THROW_IF(!config_.has_key(start_x_segment_key), std::logic_error,
                          "Missing key '" << start_x_segment_key << "'!");

              DT_THROW_IF(!config_.has_key(start_y_segment_key), std::logic_error,
                          "Missing key '" << start_y_segment_key << "'!");

              double start_x_wall = config_.fetch_real(start_x_segment_key);
              if (!config_.has_explicit_unit(start_x_segment_key)) {
                start_x_wall *= default_length_unit;
              }

              double start_y_wall = config_.fetch_real(start_y_segment_key);
              if (!config_.has_explicit_unit(start_y_segment_key)) {
                start_y_wall *= default_length_unit;
              }
              segment_start.set(start_x_wall, start_y_wall);

              // Start angle:
              std::ostringstream start_angle_segment_key_oss;
              start_angle_segment_key_oss << "segments." << segment_labels[i] << ".start_angle";
              std::string start_angle_segment_key = start_angle_segment_key_oss.str();
              if (config_.has_key(start_angle_segment_key)) {
                segment_start_angle = config_.fetch_real(start_angle_segment_key);
                if (!config_.has_explicit_unit(start_angle_segment_key)) {
                  segment_start_angle *= default_angle_unit;
                }
              } else {
                segment_start_angle = 0.0;
              }
            } // (i == 0)

            // Any wall segment:
            {
              // Stop position:
              std::ostringstream stop_x_segment_key_oss;
              stop_x_segment_key_oss << "segments." << segment_labels[i] << ".stop_x";
              std::ostringstream stop_y_segment_key_oss;
              stop_y_segment_key_oss << "segments." << segment_labels[i] << ".stop_y";

              std::string stop_x_segment_key = stop_x_segment_key_oss.str();
              std::string stop_y_segment_key = stop_y_segment_key_oss.str();

              DT_THROW_IF(!config_.has_key(stop_x_segment_key), std::logic_error,
                          "Missing key '" << stop_x_segment_key << "'!");

              DT_THROW_IF(!config_.has_key(stop_y_segment_key), std::logic_error,
                          "Missing key '" << stop_y_segment_key << "'!");

              double stop_x_wall = config_.fetch_real(stop_x_segment_key);
              if (!config_.has_explicit_unit(stop_x_segment_key)) {
                stop_x_wall *= default_length_unit;
              }

              double stop_y_wall = config_.fetch_real(stop_y_segment_key);
              if (!config_.has_explicit_unit(stop_y_segment_key)) {
                stop_y_wall *= default_length_unit;
              }
              segment_stop.set(stop_x_wall, stop_y_wall);

              if (i == ((int) segment_labels.size() - 1)) {
                // Stop angle:
                std::ostringstream stop_angle_segment_key_oss;
                stop_angle_segment_key_oss << "segments." << segment_labels[i] << ".stop_angle";
                std::string stop_angle_segment_key = stop_angle_segment_key_oss.str();
                if (config_.has_key(stop_angle_segment_key)) {
                  segment_stop_angle = config_.fetch_real(stop_angle_segment_key);
                  if (!config_.has_explicit_unit(stop_angle_segment_key)) {
                    segment_stop_angle *= default_angle_unit;
                  }
                } else {
                  segment_stop_angle = 0.0;
                }
              }

            }

            {
              std::ostringstream right_thickness_segment_key_oss;
              right_thickness_segment_key_oss << "segments." << segment_labels[i] << ".right_thickness";
              std::string right_thickness_segment_key = right_thickness_segment_key_oss.str();
              DT_THROW_IF (! config_.has_key(right_thickness_segment_key), std::logic_error,
                           "Missing '" << right_thickness_segment_key << "' property !");
              segment_right_thickness = config_.fetch_real(right_thickness_segment_key);
              if (! config_.has_explicit_unit(right_thickness_segment_key)) {
                segment_right_thickness *= default_length_unit;
              }
              // std::cerr << "DEVEL: " << "simple_polygon::initialize: "
              //           << "segment_right_thickness=" << segment_right_thickness << std::endl;
            }

            {
              std::ostringstream left_thickness_segment_key_oss;
              left_thickness_segment_key_oss << "segments." << segment_labels[i] << ".left_thickness";
              std::string left_thickness_segment_key = left_thickness_segment_key_oss.str();
              DT_THROW_IF (! config_.has_key(left_thickness_segment_key), std::logic_error,
                           "Missing '" << left_thickness_segment_key << "' property !");
              segment_left_thickness = config_.fetch_real(left_thickness_segment_key);
              if (! config_.has_explicit_unit(left_thickness_segment_key)) {
                segment_left_thickness *= default_length_unit;
              }
              // std::cerr << "DEVEL: " << "simple_polygon::initialize: "
              //           << "segment_left_thickness=" << segment_left_thickness << std::endl;
            }

            // Default thicknesses:
            if (! datatools::is_valid(segment_right_thickness)) {
              // std::cerr << "DEVEL: " << "simple_polygon::initialize: "
              //           << "NULL segment_right_thickness" << std::endl;
              segment_right_thickness = 0.0;
            }

            if (! datatools::is_valid(segment_left_thickness)) {
              // std::cerr << "DEVEL: " << "simple_polygon::initialize: "
              //           << "NULL segment_left_thickness" << std::endl;
              segment_left_thickness = 0.0;
            }

            // std::cerr << "DEVEL: " << "simple_polygon::initialize: "
            //           << "FINAL => segment_right_thickness = " << segment_right_thickness << std::endl;
            // std::cerr << "DEVEL: " << "simple_polygon::initialize: "
            //           << "FINAL => segment_left_thickness  = " << segment_left_thickness << std::endl;

            if (i == 0) {
              add_wall_segment(segment_start, segment_stop,
                               segment_left_thickness, segment_right_thickness,
                               segment_start_angle, segment_stop_angle);
            } else {
              add_wall_segment(segment_stop,
                               segment_left_thickness, segment_right_thickness,
                               segment_stop_angle);
            }

          } // Loop on vertexes

        } // _build_mode_ == BUILD_MODE_BY_SEGMENTS

      }

    }
    initialize();
    return;
  }

  void simple_polygon::initialize()
  {
    DT_THROW_IF(_build_mode_ == BUILD_MODE_INVALID,
                std::logic_error,
                "Invalid build mode!");
    if (_build_mode_ == BUILD_MODE_BY_VERTICES) {
      DT_THROW_IF(_vertices_.size() < 3,
                  std::logic_error,
                  "Number of vertices is too low!");
      _build_by_vertices();
    } else if (_build_mode_ == BUILD_MODE_BY_SEGMENTS) {
      _build_by_segments();
    }

    // Compute working internal data:
    _compute_normal();

    {
      std::vector<const vector_2d *> pvertices;
      pvertices.reserve(_vertices_.size());
      for (int i = 0; i < (int) _vertices_.size(); i++) {
        pvertices.push_back(&_vertices_[i]);
      }
      triangulation(pvertices, _normal_, _triangles_);
    }

    return;
  }

  void simple_polygon::_compute_normal()
  {
    double sum_angle = 0.0;
    int nvtx = _vertices_.size();
    unsigned int nplus = 0;
    unsigned int nminus = 0;
    for (int ivtx = 0; ivtx < nvtx; ivtx++) {
      int iv0 = ivtx;
      int iv1 = (ivtx+1) % nvtx;
      int iv2 = (ivtx+2) % nvtx;
      const vector_2d & v0 = _vertices_[iv0];
      const vector_2d & v1 = _vertices_[iv1];
      const vector_2d & v2 = _vertices_[iv2];
      vector_3d edge01 = v1 - v0;
      vector_3d edge12 = v2 - v1;
      vector_3d cross012 = edge01.cross(edge12);
      double sign012 = +1.0;
      if (cross012.z() < 0.0) {
        sign012 = -1.0;
        nminus++;
      } else {
        nplus++;
      }
      double a012 = edge01.angle(edge12);
      sum_angle += (sign012 * a012);
    }
    // std::cerr << "DEVEL: " << "simple_polygon::_compute_normal: "
    //           << "sum_angle = " << sum_angle / CLHEP::degree << " [degree]"
    //           << std::endl;
    if (std::abs(sum_angle - 2 * M_PI) < 0.1 * CLHEP::degree) {
      _normal_.set(0.0, 0.0, 1.0);
    } else if (std::abs(sum_angle + 2 * M_PI) < 0.1 * CLHEP::degree) {
      _normal_.set(0.0, 0.0, -1.0);
    } else {
      DT_THROW(std::logic_error, "Cannot compute the normal of the polygon!");
    }
    if (nplus > 0 && nminus > 0) {
      _convex_ = true;
    }
    // std::cerr << "DEVEL: " << "simple_polygon::_compute_normal: "
    //           << "Normal = " << _normal_
    //           << std::endl;
    // std::cerr << "DEVEL: " << "simple_polygon::_compute_normal: "
    //           << "Convex = " << _convex_
    //           << std::endl;
    return;
  }

  void simple_polygon::_build_by_vertices()
  {

    bool check_closing = _check_vertex(_vertices_.front(), true);
    DT_THROW_IF (! check_closing, std::domain_error,
                 "Cannot close the polygon properly!");
    return;
  }

  void simple_polygon::_build_by_segments()
  {
    DT_THROW_IF(_wall_segments_.size() == 0, std::logic_error,
                "Missing wall segments!");
    bool devel = false;
    // devel = true;
    if (devel) {
      std::cerr << "DEVEL: simple_polygon::_build_by_segments: "
                << "Entering..." << std::endl;
    }
    boost::scoped_ptr<datatools::temp_file> tmp_file;
    if (devel) {
      std::cerr << "DEVEL: simple_polygon::_build_by_segments: "
                << "Devel display..." << std::endl;
      tmp_file.reset(new datatools::temp_file);
      tmp_file->set_remove_at_destroy(false);
      tmp_file->create("/tmp", "simple_polygon_by_segments_");
    }

    if (tmp_file.get() != 0) {
      for (wall_segment_col_type::const_iterator iwall = _wall_segments_.begin();
           iwall != _wall_segments_.end();
           iwall++) {
        const wall_segment_type & ws = *iwall;
        if (iwall == _wall_segments_.begin()) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file->out(), ws.start, 1.0);
        }
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file->out(), ws.stop, 1.0);
      }
      tmp_file->out() << std::endl << std::endl;
    }

    std::vector<geomtools::vector_2d> last_right;
    std::vector<geomtools::vector_2d> last_left;
    std::vector<geomtools::vector_2d> last_u;

    // Process opening edge at first wall segment
    {
      geomtools::vector_2d A;
      geomtools::vector_2d B;
      geomtools::vector_2d uAB;
      geomtools::vector_2d R;
      geomtools::vector_2d L;
      geomtools::invalidate(R);
      geomtools::invalidate(L);
      geomtools::invalidate(uAB);
      const wall_segment_type & ws0 = _wall_segments_.front();
      A = ws0.start;
      B = ws0.stop;
      uAB = (B - A).unit();
      double thetaAB = std::atan2(uAB.y(), uAB.x());
      geomtools::vector_2d vAB(std::cos(thetaAB + 0.5 * M_PI), std::sin(thetaAB + 0.5 * M_PI));
      geomtools::vector_2d AR =
        -vAB * ws0.right_thickness
        +uAB * ws0.right_thickness * std::tan(ws0.start_angle);
      geomtools::vector_2d AL =
         vAB * ws0.left_thickness
        -uAB * ws0.left_thickness * std::tan(ws0.start_angle);
      // Right vertex:
      R = A + AR;
      // Left vertex:
      L = A + AL;
      last_right.push_back(R);
      last_left.push_back(L);
      last_u.push_back(uAB);
    }

    /*

    int ws_counter = 0;
    geomtools::vector_2d A, C, E, uAB;
    geomtools::invalidate(A);
    geomtools::invalidate(C);
    geomtools::invalidate(E);
    geomtools::invalidate(uAB);
    */
    geomtools::vector_2d A = _wall_segments_[0].stop;
    for (int iwall = 1; iwall < (int) _wall_segments_.size(); iwall++) {
      const wall_segment_type & ws = _wall_segments_[iwall];
      geomtools::vector_2d A = _wall_segments_[iwall-1].stop;
      geomtools::vector_2d B = ws.stop;
      geomtools::vector_2d uAB = (B - A).unit();
      double thetaAB = std::atan2(uAB.y(), uAB.x());
      geomtools::vector_2d vAB(std::cos(thetaAB + 0.5 * M_PI), std::sin(thetaAB + 0.5 * M_PI));
      geomtools::vector_2d A_right = A - vAB * ws.right_thickness;
      geomtools::vector_2d A_left  = A + vAB * ws.left_thickness;
      if (tmp_file.get() != 0) {
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file->out(), A_right, 2.0);
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file->out(), A_left, 2.0);
      }
      geomtools::vector_2d R, L;
      const geomtools::vector_2d & R0 = last_right.back();
      const geomtools::vector_2d & L0 = last_left.back();
      const geomtools::vector_2d & u0 = last_u.back();

      // Right vertex:
      {
        double lambda;
        double lambda_last;
        bool solved = mygsl::linear_system_2x2_solve(u0.x(), -uAB.x(), A_right.x() - R0.x(),
                                                     u0.y(), -uAB.y(), A_right.y() - R0.y(),
                                                     lambda_last, lambda);
        if (!solved) {
          DT_THROW(std::logic_error, "Cannot compute right vertex for wall segment [#" << iwall << "]!");
        }
        R = A_right + lambda * uAB;
      }

      // Left vertex:
      {
        double lambda;
        double lambda_last;
        bool solved = mygsl::linear_system_2x2_solve(u0.x(), -uAB.x(), A_left.x() - L0.x(),
                                                     u0.y(), -uAB.y(), A_left.y() - L0.y(),
                                                     lambda_last, lambda);
        if (!solved) {
          DT_THROW(std::logic_error, "Cannot compute left vertex for wall segment [#" << iwall << "]!");
        }
        L = A_left + lambda * uAB;
      }
      last_right.push_back(R);
      last_left.push_back(L);
      last_u.push_back(uAB);
    }
    if (tmp_file.get() != 0) {
      tmp_file->out() << std::endl << std::endl;
    }

    // Process closing edge at last wall segment
    {
      const wall_segment_type & ws_last = _wall_segments_.back();
      geomtools::vector_2d B = ws_last.stop;
      geomtools::vector_2d uAB = last_u.back();
      double thetaAB = std::atan2(uAB.y(), uAB.x());
      geomtools::vector_2d vAB(std::cos(thetaAB + 0.5 * M_PI), std::sin(thetaAB + 0.5 * M_PI));
      geomtools::vector_2d BR =
        -vAB * ws_last.right_thickness
        +uAB * ws_last.right_thickness * std::tan(ws_last.stop_angle);
      geomtools::vector_2d BL =
         vAB * ws_last.left_thickness
        -uAB * ws_last.left_thickness * std::tan(ws_last.stop_angle);
      // Right vertex:
      geomtools::vector_2d R = B + BR;
      // Left vertex:
      geomtools::vector_2d L = B + BL;
      last_right.push_back(R);
      last_left.push_back(L);
      last_u.push_back(uAB);
    }

    // Register the vertices:
    for (int i = 0; i < (int) last_right.size(); i++) {
      _add_vertex(last_right[i]);
    }
    for (int i = (int) last_left.size() - 1; i >= 0; i--) {
      _add_vertex(last_left[i]);
    }

    if (tmp_file.get() != 0) {
      for (int i = 0; i < (int) last_right.size(); i++) {
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file->out(), last_right[i], 3.0);
      }
      for (int i = (int) last_left.size() - 1; i >= 0; i--) {
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file->out(), last_left[i], 3.0);
      }
      tmp_file->out() << std::endl << std::endl;
    }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    if (tmp_file.get() != 0) {
      Gnuplot g1;
      g1.cmd("set title 'DEVEL: simple polygon built by walls");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      // g1.cmd("set xrange [-4:+4]");
      // g1.cmd("set yrange [-4:+4]");
      g1.set_xlabel("x").set_ylabel("y");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << tmp_file->get_filename()
                 << "' index 0 title '' with points lt 1 pt 6 ps 1.0 ";
        plot_cmd << " , '' index 0 title 'Wall path' with lines lt 1 ";
        plot_cmd << " , '' index 1 title 'Reference points' with points lt 2 pt 6 ps 1.0 ";
        plot_cmd << " , '' index 2 title 'Vertices' with points lt 3 pt 6 ps 1.0 ";
        plot_cmd << " , '' index 2 title '' with lines lt 3 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
    }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    tmp_file.reset();
    return;
  }

  // static
  bool simple_polygon::find_segment_2d_intersection(const vector_2d & a_,
                                                    const vector_2d & b_,
                                                    const vector_2d & c_,
                                                    const vector_2d & d_,
                                                    vector_2d & intersection_)
  {
    vector_2d uAB = (b_ - a_).unit();
    vector_2d uCD = (d_ - c_).unit();
    double lambda_AB;
    double lambda_CD;
    bool solved = mygsl::linear_system_2x2_solve(uAB.x(), -uCD.x(), c_.x() - a_.x(),
                                                 uAB.y(), -uCD.y(), c_.y() - a_.y(),
                                                 lambda_AB, lambda_CD);
    if (solved) {
      if (lambda_AB < 0.0 || lambda_AB > (b_ - a_).mag()) {
        return false;
      } else if (lambda_CD < 0.0 || lambda_CD > (d_ - c_).mag()) {
        return false;
      }
      intersection_ = a_ + uAB * lambda_AB;
      return true;
    }
    return false;
  }

  // static
  void simple_polygon::triangulation(const std::vector<const vector_2d *> & vertices_,
                                     const vector_3d & normal_,
                                     std::vector<triangle> & triangles_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    logging = datatools::logger::PRIO_DEBUG;
    if (vertices_.size() == 3) {
      triangle t;
      vector_3d vtx0(vertices_[0]->x(), vertices_[0]->y(), 0.0);
      vector_3d vtx1(vertices_[1]->x(), vertices_[1]->y(), 0.0);
      vector_3d vtx2(vertices_[2]->x(), vertices_[2]->y(), 0.0);
      t.set_vertexes(vtx0, vtx1, vtx2);
      triangles_.push_back(t);
      DT_LOG_DEBUG(logging, "Last triangle has been reached.");
      return;
    }
    // Ear clipping algorithm:
    int nvtx = (int) vertices_.size();
    DT_LOG_DEBUG(logging, "Searching for an ear to be clipped among " << nvtx << " vertices...");
    int iear = nvtx;
    triangle candidate_ear;
    for (int ivtx = 0; ivtx < nvtx; ivtx++) {
      int iv0 = ivtx;
      int iv1 = (ivtx+1) % nvtx;
      int iv2 = (ivtx+2) % nvtx;
      const vector_2d & v0 = *vertices_[iv0];
      const vector_2d & v1 = *vertices_[iv1];
      const vector_2d & v2 = *vertices_[iv2];
      vector_3d vtx0(v0.x(), v0.y(), 0.0);
      vector_3d vtx1(v1.x(), v1.y(), 0.0);
      vector_3d vtx2(v2.x(), v2.y(), 0.0);
      candidate_ear.set_vertexes(vtx0, vtx1, vtx2);
      vector_3d candidate_ear_normal;
      DT_LOG_DEBUG(logging, "Checking candidate ear normal...");
      DT_LOG_DEBUG(logging, "  V0 = " << *vertices_[iv0]);
      DT_LOG_DEBUG(logging, "  V1 = " << *vertices_[iv1]);
      DT_LOG_DEBUG(logging, "  V2 = " << *vertices_[iv2]);
      candidate_ear_normal = candidate_ear.get_normal_on_surface(vtx0, false);
      if (candidate_ear_normal.dot(normal_) < 0.0) {
        // Not a candidate ear (normal is reversed):
        DT_LOG_DEBUG(logging, "Not a candidate ear because its normal is reversed...");
        continue;
      }
      DT_LOG_DEBUG(logging, "Searching for an overlapping vertex for candidate ear "
                   << '[' << iv0 << ',' << iv1 << ',' << iv2 << ']' << " with "
                   << (nvtx - 3) << " vertices...");
      bool found_overlapping_vertex = false;
      for (int iloop = 0; iloop < (int) (nvtx - 3); iloop++) {
        int ivtx2 = (ivtx+3+iloop) % nvtx;
        const vector_3d & v = *vertices_[ivtx2];
        if (candidate_ear.is_on_surface(v)) {
          found_overlapping_vertex = true;
          DT_LOG_DEBUG(logging, "Found overlapping vertex [# " << ivtx2 << "].");
          break;
        }
      }
      if (found_overlapping_vertex) {
        DT_LOG_DEBUG(logging, "Candidate ear is rejected by overlapping vertex!");
        continue;
      }

      bool found_edge_intersection = false;
      int nedges = (int) (nvtx - 4);
      DT_LOG_DEBUG(logging, "Searching for an intersecting edge for candidate ear "
                   << '[' << iv0 << ',' << iv1 << ',' << iv2 << ']' << " with "
                   << nedges << " edges...");
      for (int iloop = 0; iloop < nedges; iloop++) {
        int iedge_begin = (ivtx+3+iloop) % nvtx;
        int iedge_end   = (ivtx+4+iloop) % nvtx;;
        DT_LOG_DEBUG(logging, "  Test intersecting edge "
                     << '[' << iedge_begin << ',' << iedge_end << ']');
        const vector_2d & va = *vertices_[iedge_begin];
        const vector_2d & vb = *vertices_[iedge_end];
        vector_2d I;
        if (find_segment_2d_intersection(v0, v2, va, vb, I)) {
          found_edge_intersection = true;
          DT_LOG_DEBUG(logging, "Found edge intersection at " << I / CLHEP::mm << " [mm].");
          break;
        }
      }
      if (!found_edge_intersection) {
        iear = iv0;
        DT_LOG_DEBUG(logging, "Found ear starting at " << '[' << iear << ']');
        break;
      }
    }
    DT_THROW_IF(iear == nvtx, std::logic_error, "Cannot find an ear!");
    {
      // triangle t;
      // int iv0 = iear;
      int iv1 = (iear+1) % nvtx;
      // int iv2 = (iear+2) % nvtx;
      // t.set_vertexes_ext(*vertices_[iv0], *vertices_[iv1], *vertices_[iv2]);
      // triangles_.push_back(t);
      triangles_.push_back(candidate_ear);
      std::vector<const vector_2d *> vertices;
      vertices.reserve(nvtx - 1);
      for (int i = 0; i < nvtx; i++) {
        if (i != iv1) {
          vertices.push_back(vertices_[i]);
        }
      }
      triangulation(vertices, normal_, triangles_);
    }
    return;
  }

} // end of namespace geomtools
