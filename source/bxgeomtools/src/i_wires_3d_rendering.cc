// i_wires_3d_rendering.cc

// Ourselves:
#include <geomtools/i_wires_3d_rendering.h>

// This project:
#include <geomtools/box.h>
// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/placement.h>

namespace geomtools {

  // static
  void i_wires_3d_rendering::boost_linear_sampling(uint32_t & bits_)
  {
    if (!(bits_ & WR_BASE_GRID)) {
      bits_ |= WR_BASE_GRID;
    } else if (bits_ & WR_BASE_GRID_LOW_DENSITY) {
      bits_ &= ~(WR_BASE_GRID_LOW_DENSITY);
      // Use normal density:
    } else if (bits_ & WR_BASE_GRID) {
      // Leave normal density:
      bits_ |=   WR_BASE_GRID_HIGH_DENSITY;
    } else if (bits_ & WR_BASE_GRID_HIGH_DENSITY) {
      bits_ &= ~(WR_BASE_GRID_HIGH_DENSITY);
      bits_ |=   WR_BASE_GRID_VERY_HIGH_DENSITY;
    } else if (bits_ & WR_BASE_GRID_HIGH_DENSITY) {
      bits_ &= ~(WR_BASE_GRID_HIGH_DENSITY);
      bits_ |=   WR_BASE_GRID_VERY_HIGH_DENSITY;
    } else if (bits_ & WR_BASE_GRID_VERY_HIGH_DENSITY) {
      bits_ &= ~(WR_BASE_GRID_VERY_HIGH_DENSITY);
      bits_ |=   WR_BASE_GRID_HUGE_DENSITY;
    }
    return;
  }

  // static
  uint32_t i_wires_3d_rendering::linear_sampling(sampling_level_type sl_)
  {
    switch (sl_) {
    case SL_NONE      : return 0;
    case SL_LOW       : return 3;
    case SL_NORMAL    : return 5;
    case SL_HIGH      : return 15;
    case SL_VERY_HIGH : return 45;
    case SL_HUGE      : return 135;
    }
    return 0;
  }

  // static
  uint32_t i_wires_3d_rendering::linear_sampling_from_options(uint32_t options_)
  {
    uint32_t nsamples = 2;
    if (options_ & i_wires_3d_rendering::WR_BASE_GRID) {
      nsamples = linear_sampling(SL_NORMAL);
      if (options_ & i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY) {
        nsamples = linear_sampling(SL_LOW);
      }
      if (options_ & i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY) {
        nsamples = linear_sampling(SL_HIGH);
      }
      if (options_ & i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY) {
        nsamples = linear_sampling(SL_VERY_HIGH);
      }
      if (options_ & i_wires_3d_rendering::WR_BASE_GRID_HUGE_DENSITY) {
        nsamples = linear_sampling(SL_HUGE);
      }
    }
    return nsamples;
  }

  // static
  uint32_t i_wires_3d_rendering::angular_sampling(sampling_level_type sl_)
  {
    switch (sl_) {
    case SL_NONE      : return 0;
    case SL_LOW       : return 8;
    case SL_NORMAL    : return 32;
    case SL_HIGH      : return 64;
    case SL_VERY_HIGH : return 128;
    case SL_HUGE      : return 256;
    }
    return 0;
  }

  // static
  uint32_t i_wires_3d_rendering::angular_sampling_from_options(uint32_t options_)
  {
    uint32_t nsamples = angular_sampling(SL_NORMAL);
    if (options_ & i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING) {
      nsamples = angular_sampling(SL_LOW);
    }
    if (options_ & i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING) {
      nsamples = angular_sampling(SL_HIGH);
    }
    if (options_ & i_wires_3d_rendering::WR_BASE_VERY_HIGH_ANGLE_SAMPLING) {
      nsamples = angular_sampling(SL_VERY_HIGH);
    }
    if (options_ & i_wires_3d_rendering::WR_BASE_HUGE_ANGLE_SAMPLING) {
      nsamples = angular_sampling(SL_HUGE);
    }
    return nsamples;
  }

  i_wires_3d_rendering::~i_wires_3d_rendering ()
  {
    return;
  }

  void i_wires_3d_rendering::generate_wires(wires_type & wires_,
                                            const placement & positioning_,
                                            uint32_t options_) const
  {
    wires_type wires;
    generate_wires_self(wires, options_);
    transform_wires_to(positioning_, wires, wires_);
    return;
  }

  void i_wires_3d_rendering::generate_wires(wires_type & wires_,
                                            const geomtools::vector_3d & position_,
                                            const geomtools::rotation_3d & rotation_,
                                            uint32_t options_) const
  {
    placement p(position_, rotation_);
    generate_wires(wires_, p, options_);
    return;
  }

  void transform_wire_to(const placement & p_,
                         const polyline_type & wire_,
                         polyline_type & tr_wire_)
  {
    for (polyline_type::const_iterator i = wire_.begin();
         i != wire_.end();
         i++) {
      const vector_3d & vertex = *i;
      {
        vector_3d dummy;
        tr_wire_.push_back(dummy);
      }
      vector_3d & tr_vertex = tr_wire_.back();
      p_.child_to_mother(vertex, tr_vertex);
    }
    return;
  }

  void transform_wires_to(const placement & p_,
                          const wires_type & wires_,
                          wires_type & tr_wires_)
  {
    for (wires_type::const_iterator i = wires_.begin();
         i != wires_.end();
         i++) {
      const polyline_type & wire = *i;
      {
        polyline_type dummy;
        tr_wires_.push_back(dummy);
      }
      polyline_type & tr_wire = tr_wires_.back();
      transform_wire_to(p_, wire, tr_wire);
    }
    return;
  }

  void save_wires(std::ostream & out_, const wires_type & wires_, uint32_t /* flags_ */)
  {
    for (wires_type::const_iterator i = wires_.begin();
         i != wires_.begin();
         i++) {
      gnuplot_draw::basic_draw_polyline(out_, *i);
    }
    return;
  }

  bool parse_wires(std::istream & in_, wires_type & wires_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    bool success = true;
    polyline_type * current_polyline = 0;
    int blank_line_counter = 0;
    while (in_) {
      std::string line;
      std::getline(in_, line);
      DT_LOG_DEBUG(logging, "line='" << line << "'");
      if (line.empty()) {
        if (current_polyline != 0) {
          DT_LOG_DEBUG(logging, "stop the current wire...");
          current_polyline = 0;
        } else {
        }
        blank_line_counter++;
        if (blank_line_counter == 2) {
          DT_LOG_DEBUG(logging, "end of the wires...");
          break;
        }
        continue;
      }
      if (line[0] == '#') {
        continue;
      }
      blank_line_counter = 0;
      double x, y, z;
      std::istringstream line_oss(line);
      line_oss >> x >> y >> z >> std::ws;
      if (!line_oss) {
        wires_.clear();
        return false;
      }
      if (current_polyline == 0) {
        {
          polyline_type dummy;
          wires_.push_back(dummy);
        }
        current_polyline = &wires_.back();
        DT_LOG_DEBUG(logging, "start a new wire...");
      }
      current_polyline->push_back(vector_3d(x, y, z));

      if (in_.eof()) {
        break;
      }
    }

    return success;
  }

  classified_segment::classified_segment()
  {
    reset();
    return;
  }

  void classified_segment::reset()
  {
    geomtools::invalidate(first);
    geomtools::invalidate(last);
    domain = SHAPE_DOMAIN_NONE;
    return;
  }

  bool classified_segment::is_inside() const
  {
    return domain == SHAPE_DOMAIN_INSIDE;
  }

  bool classified_segment::is_outside() const
  {
    return domain == SHAPE_DOMAIN_OUTSIDE;
  }

  bool classified_segment::is_on_surface() const
  {
    return domain == SHAPE_DOMAIN_ON_SURFACE;
  }

  void classified_segment::to_line(segment_type & segment_) const
  {
    segment_.first  = first;
    segment_.second = last;
    return;
  }

  void classified_segment::to_wire(polyline_type & wire_) const
  {
    // vector_3d u = (last - first).unit();
    // double l = (last - first).mag();
    wire_.push_back(first);
    // int n = 1 + ((int) 3 * drand48());
    // double dl = l / (n+1);
    // for (int i = 0; i < n; i++) {
    //   wire_.push_back(first + (i + 1) * dl * u);
    // }
    wire_.push_back(last);
    return;
  }

  void classify_in_out_segment(const segment_type & segment_,
                               const i_shape_3d & shape_,
                               const placement & shape_placement_,
                               double step_,
                               double tolerance_,
                               std::list<classified_segment> & the_classified_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    // logging = datatools::logger::PRIO_DEBUG;
    DT_THROW_IF(!geomtools::is_valid(segment_.first), std::logic_error,
                "Segment first point is not valid!");
    DT_THROW_IF(!geomtools::is_valid(segment_.second), std::logic_error,
                "Segment second point is not valid!");
    DT_THROW_IF(!shape_placement_.is_valid(), std::logic_error,
                "Shape placement is not valid!");
    DT_THROW_IF(! shape_.is_locked(), std::logic_error,
                "Shape (" << shape_.get_shape_name() << ") is not locked!");
    DT_THROW_IF(step_ <= 0.0, std::logic_error,
                "Invalid step size (" << step_ / CLHEP::mm << " mm)!");
    double tolerance = tolerance_;
    if (tolerance == 0.0) {
      tolerance = step_ / 20.0;
    } else if (tolerance < 0.0) {
      tolerance = shape_.get_tolerance();
    }
    vector_3d tp0;
    vector_3d tp1;
    // Change to the child coordinate frame of the shape:
    shape_placement_.mother_to_child(segment_.first, tp0);
    shape_placement_.mother_to_child(segment_.second,  tp1);
    vector_3d unit = (tp1 - tp0).unit();
    double segment_length = (tp1 - tp0).mag();
    double suggested_first_step;
    double length_to_bb_exit;
    datatools::invalidate(suggested_first_step);
    datatools::invalidate(length_to_bb_exit);
    if (shape_.has_bounding_data()) {
      box bb;
      placement bbplacement;
      shape_.get_bounding_data().compute_bounding_box(bb, bbplacement, 2.0 * tolerance);
      vector_3d tp0_bb;
      vector_3d tp1_bb;
      bbplacement.mother_to_child(tp0, tp0_bb);
      bbplacement.mother_to_child(tp1, tp1_bb);
      vector_3d unit_bb = (tp1_bb - tp0_bb).unit();
      vector_3d impact0;
      geomtools::invalidate(impact0);
      vector_3d impact1;
      geomtools::invalidate(impact1);
      if (bb.is_outside(tp0_bb, tolerance)) {
        /*
         *              tp0                  tp0
         *             +                    +
         *              \                    \
         *               \ impact0            \
         *             +--*-----------------+  \
         *             |   \                |   \
         *             |    +tp0            |    \
         *             |     \              |     \
         *             |      +tp1   BB     |      \
         *             |       \            |       \
         *             +--------*-----------+        \
         *                       \ impact1            \
         *                        \                    \
         *                         + tp1                + tp1
         */
        face_intercept_info tp0_intercept;
        if (bb.find_intercept(tp0_bb, unit_bb, tp0_intercept, tolerance)) {
          impact0 = tp0_intercept.get_impact();
        }
        if (geomtools::is_valid(impact0)) {
          suggested_first_step = (impact0 - tp0_bb).mag();
        }
      }
      if (bb.is_outside(tp1_bb, tolerance)) {
        face_intercept_info tp1_intercept;
        if (bb.find_intercept(tp1_bb, -unit_bb, tp1_intercept, tolerance)) {
          impact1 = tp1_intercept.get_impact();
        }
        if (geomtools::is_valid(impact1)) {
          length_to_bb_exit = (impact1 - tp0_bb).mag();
        }
      }
    }

    vector_3d p0 = tp0;
    shape_domain_flags_type cd0 = shape_.where_is(p0, tolerance);
    classified_segment * current_cs = 0;
    DT_LOG_TRACE(logging, "Start at p0 = " << p0 << " with cd0 = '" << shape_domain_flag_label(cd0) << "'");
    bool go_on = true;
    while (go_on) {
      if (current_cs == 0) {
        DT_LOG_TRACE(logging, "Start a new segment...");
        {
          classified_segment dummy;
          the_classified_.push_back(dummy);
        }
        current_cs = &the_classified_.back();
        shape_placement_.child_to_mother(p0, current_cs->first);
        geomtools::invalidate(current_cs->last);
        if (cd0 == SHAPE_DOMAIN_ON_SURFACE) {
          current_cs->domain = SHAPE_DOMAIN_NONE;
        } else {
          current_cs->domain = cd0;
        }
      }
      double step = step_;
      if (datatools::is_valid(suggested_first_step)) {
        DT_LOG_DEBUG(logging, "Using suggested first step = " << suggested_first_step / CLHEP::mm << " mm");
        step = suggested_first_step;
        datatools::invalidate(suggested_first_step);
      }
      if (datatools::is_valid(length_to_bb_exit)) {
        if ((p0 - tp0).mag() > length_to_bb_exit) {
          DT_LOG_DEBUG(logging, "Using length_to_bb_exit info = " << length_to_bb_exit / CLHEP::mm << " mm");
          step = (tp1 - p0).mag();
        }
      }
      vector_3d p1 = p0 + step * unit;
      if ((p1 - tp0).mag() > segment_length) {
        p1 = tp1;
      }
      shape_domain_flags_type cd1 = shape_.where_is(p1, tolerance);
      DT_LOG_TRACE(logging, "Step up to p1 = " << p1 << " with cd1 = '" << shape_domain_flag_label(cd1)<< "'");
      bool frontier_detected = true;
      if (current_cs->domain == SHAPE_DOMAIN_NONE) {
        current_cs->domain = cd1;
        DT_LOG_TRACE(logging, "Flag new current segment with classification '"
                     << shape_domain_flag_label(current_cs->domain) << "'");
        frontier_detected = false;
      }
      if (cd1 == current_cs->domain) {
        frontier_detected = false;
        DT_LOG_TRACE(logging, "Current segment continues...");
        DT_LOG_TRACE(logging, "p0/cd0 are updated.");
        p0  = p1;
        cd0 = cd1;
      }

      if (frontier_detected) {
        DT_LOG_TRACE(logging, "Current segment is interrupted by a frontier/surface...");
        int bisection_count = 0;
        // Use bisection method to estimate the point on the surface:
        DT_LOG_TRACE(logging, "Start bisection...");
        bool found = false;
        vector_3d pa = p0;
        shape_domain_flags_type cda = cd0;
        vector_3d pb = p1;
        shape_domain_flags_type cdb = cd1;
        do {
          vector_3d pc = 0.5 * (pa + pb);
          shape_domain_flags_type cdc = shape_.where_is(pc, tolerance);
          DT_LOG_TRACE(logging, "Bisection point pc = " << pc
                       << " with cdc=" << shape_domain_flag_label(cdc));

          if (cda == SHAPE_DOMAIN_OUTSIDE && cdb == SHAPE_DOMAIN_INSIDE) {
            if (cdc == SHAPE_DOMAIN_OUTSIDE) {
              /*
               *      a         c   | |   b
               *   ---o->-------o---|*|---o--------------->
               *    [out]     [out] | |  [in]
               *
               */
              pa = pc;
            } else if (cdc == SHAPE_DOMAIN_INSIDE) {
              /*
               *      a    | |  c         b
               *   ---o->--|*|--o---------o--------------->
               *    [out]  | | [in]      [in]
               *
               */
              pb = pc;
            } else if (cdc == SHAPE_DOMAIN_ON_SURFACE) {
              /*
               *      a        |c|        b
               *   ---o->------|o|--------o--------------->
               *    [out]      | |[surf]  [in]
               *
               */
              // pb = pc;
              pb = pc + tolerance * unit;
              cdb = SHAPE_DOMAIN_ON_SURFACE;
            } else {
              DT_THROW(std::logic_error, "Invalid shape domain while bisecting the segment!");
            }
          } else if (cda == SHAPE_DOMAIN_OUTSIDE && cdb == SHAPE_DOMAIN_ON_SURFACE) {
            if (cdc == SHAPE_DOMAIN_OUTSIDE) {
              /*
               *      a         c        |b|
               *   ---o->-------o--------|o|-------------->
               *    [out]     [out]      | |[surf]
               *
               */
              pa = pc;
            } else if (cdc == SHAPE_DOMAIN_INSIDE) {
              /*          | |            | |
               *      a   | |   c        |b|
               *   ---o->-| |---o--------|o|-------------->
               *    [out] | | [in]       | |[surf]
               *          | +------------+ |
               *          +----------------+
               */
              pb = pc;
              cdb = SHAPE_DOMAIN_INSIDE;
            } else if (cdc == SHAPE_DOMAIN_ON_SURFACE) {
              /*               +-----------+
               *               | +-------+ |
               *      a        |c| [in]  |b|
               *   ---o->------|o|-------|o|-------------->
               *    [out]     [surf]     | |[surf]
               *               | |       | |
               */
              pb = pc;
            } else {
              DT_THROW(std::logic_error, "Invalid shape domain while bisecting the segment!");
            }
          } else if (cda == SHAPE_DOMAIN_INSIDE && cdb == SHAPE_DOMAIN_OUTSIDE) {
            if (cdc == SHAPE_DOMAIN_INSIDE) {
              /*
               *      a         c   | |   b
               *   ---o->-------o---| |---o--------------->
               *    [in]      [in]  | |  [out]
               *
               */
              pa = pc;
            } else if (cdc == SHAPE_DOMAIN_OUTSIDE) {
              /*
               *      a   | |   c         b
               *   ---o->-| |---o---------o--------------->
               *    [in]  | | [out]     [out]
               *
               */
              pb = pc;
            } else if (cdc == SHAPE_DOMAIN_ON_SURFACE) {
              /*
               *      a        |c|        b
               *   ---o->------|o|--------o--------------->
               *    [in]       | |[surf] [out]
               *
               */
              pb = pc + tolerance * unit;
              cdb = SHAPE_DOMAIN_ON_SURFACE;
            } else {
              DT_THROW(std::logic_error, "Invalid shape domain while bisecting the segment!");
            }
          } else if (cda == SHAPE_DOMAIN_INSIDE && cdb == SHAPE_DOMAIN_ON_SURFACE) {
            if (cdc == SHAPE_DOMAIN_INSIDE) {
              /*
               *      a         c        |b|
               *   ---o->-------o--------|o|-------------->
               *    [in]      [in]       | |[surf]
               *
               */
              pa = pc;
            } else if (cdc == SHAPE_DOMAIN_OUTSIDE) {
              /*
               *      a   | |   c        |b|
               *   ---o->-| |---o--------|o|-------------->
               *    [in]  | | [out]      | |[surf]
               *          | +------------+ |
               *          +----------------+
               */
              pb = pc;
              cdb = SHAPE_DOMAIN_OUTSIDE;
            } else if (cdc == SHAPE_DOMAIN_ON_SURFACE) {
              /*
               *      a        |c|       |b|
               *   ---o->------|o|-------|o|-------------->
               *    [in]  [srf]| | [out] | |[surf]
               *               | +-------+ |
               *               +-----------+
               */
              pb = pc;
            } else {
              DT_THROW(std::logic_error, "Invalid shape domain while bisecting the segment!");
            }
          } else if (cda == SHAPE_DOMAIN_ON_SURFACE && cdb == SHAPE_DOMAIN_INSIDE) {
            if (cdc == SHAPE_DOMAIN_INSIDE) {
              /*
               *     |a|        c         b
               *   --|o|>-------o---------o--------------->
               *[srf]| |       [in]       [in]
               *
               */
              pb = pc;
            } else if (cdc == SHAPE_DOMAIN_OUTSIDE) {
              /*
               *     |a|        c   | |   b
               *   --|o|>-------o---| |---o--------------->
               *[srf]| |       [out]| |  [in]
               *     | +------------+ |
               *     +----------------+
               */
              pb = pc;
              cdb = SHAPE_DOMAIN_OUTSIDE;
            } else if (cdc == SHAPE_DOMAIN_ON_SURFACE) {
              /*     +-----------+
               *     | +-------+ |
               *     |a|       |c|        b
               *   --|o|>------|o|--------o--------------->
               *[surf] |       | |[surf] [in]
               *
               */
               pa = pc;
            } else {
              DT_THROW(std::logic_error, "Invalid shape domain while bisecting the segment!");
            }
          } else if (cda == SHAPE_DOMAIN_ON_SURFACE && cdb == SHAPE_DOMAIN_OUTSIDE) {
            if (cdc == SHAPE_DOMAIN_INSIDE) {
              /*
               *     |a|        c   | |   b
               *   --|o|>-------o---| |---o--------------->
               *[srf]| |       [in] | |  [out]
               *     | +------------+ |
               *     +----------------+
               */
              pb = pc;
              cdb = SHAPE_DOMAIN_INSIDE;
            } else if (cdc == SHAPE_DOMAIN_OUTSIDE) {
              /*
               *     |a|        c         b
               *   --|o|>-------o---------o--------------->
               *[srf]| |       [out]     [out]
               *
               */
              pb = pc;
            } else if (cdc == SHAPE_DOMAIN_ON_SURFACE) {
              /*
               *     |a|       |c|        b
               *   --|o|>------|o|--------o--------------->
               *[srf]| +-------+ |[srf]  [out]
               *     +-----------+
               */
              pa = pc;
            } else {
              DT_THROW(std::logic_error, "Invalid shape domain while bisecting the segment!");
            }
          }

          if (cdc == cda) {
            pa = pc;
          } else if (cdc == cdb) {
            pb = pc;
          }

          if ((pa - pb).mag() < 2 * tolerance) {
            // if ((pa - pb).mag() < 0.1 * tolerance) {
            p1 = pc;
            cd1 = SHAPE_DOMAIN_ON_SURFACE;
            DT_LOG_TRACE(logging, "Approximated bisection point found at " << pc);
            shape_placement_.child_to_mother(p1, current_cs->last);
            current_cs = 0;
            DT_LOG_TRACE(logging, "Stopped current segment");
            found = true;
          }
          DT_LOG_TRACE(logging, "Bisection_count=" << bisection_count);
          bisection_count++;
          if (bisection_count > 100) {
            DT_LOG_WARNING(logging, "BREAK bisection...");
            DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "BREAK bisection...");
            break;
          }
        } while (!found);
        DT_LOG_TRACE(logging, "Current cs is closed");
        p0  = p1;
        cd0 = cd1;
        current_cs = 0;
      }
      DT_LOG_TRACE(logging, "new p0=" << p0  << " with cd0 = '" << shape_domain_flag_label(cd0) << "'");
      if ((p0 - tp0).mag() >= (segment_length - 1e-3 * tolerance)) {
        if (current_cs != 0) {
          current_cs->last = segment_.second;
          current_cs = 0;
        }
        DT_LOG_TRACE(logging, "last reached at " << p0);
        go_on = false;
      }
      DT_LOG_TRACE(logging, "Go_on = " << go_on);
    } // while (go_on)

    return;
  }

  split_segment_wrt_shape::split_segment_wrt_shape()
  {
    _shape_ = 0;
    _shape_placement_.set_identity();
    datatools::invalidate(_step_);
    datatools::invalidate(_tolerance_);
    return;
  }

  void split_segment_wrt_shape::reset()
  {
    this->clear();
    _shape_ = 0;
    _shape_placement_.set_identity();
    datatools::invalidate(_step_);
    datatools::invalidate(_tolerance_);
    return;
  }

  void split_segment_wrt_shape::clear()
  {
   _clsegs_.clear();
    return;
  }

  bool split_segment_wrt_shape::is_configured() const
  {
    return _shape_ != 0
      && _shape_placement_.is_valid()
      && datatools::is_valid(_step_)
      && datatools::is_valid(_tolerance_);
  }

  void split_segment_wrt_shape::configure(const i_shape_3d & shape_,
                                          const placement & shape_placement_,
                                          double step_,
                                          double tolerance_)
  {
    configure(shape_, step_, tolerance_);
    _shape_placement_ = shape_placement_;
    return;
  }

  void split_segment_wrt_shape::configure(const i_shape_3d & shape_,
                                          double step_,
                                          double tolerance_)
  {
    DT_THROW_IF(! shape_.is_locked(), std::logic_error,
                "Shape (" << shape_.get_shape_name() << ") is not locked!");
    _shape_ = &shape_;
    DT_THROW_IF(step_ <= 0.0, std::logic_error,
                "Invalid step size (" << step_ / CLHEP::mm << " mm)!");
    _step_ = step_;
    double tolerance = tolerance_;
    if (tolerance == 0.0) {
      tolerance = _step_ / 20.0;
    } else if (tolerance < 0.0) {
      tolerance = shape_.get_tolerance();
    }
    _tolerance_ = tolerance_;
    if (_tolerance_ > _step_) {
      _tolerance_ = _step_ / 20.0;
    }
    return;
  }

  void split_segment_wrt_shape::add(const segment_type & segment_)
  {
    classify_in_out_segment(segment_,
                            *_shape_,
                            _shape_placement_,
                            _step_,
                            _tolerance_,
                            _clsegs_);
    return;
  }

  void split_segment_wrt_shape::add(const polyline_type & path_)
  {
    if (path_.size() < 2) {
      return;
    }
    polyline_type::const_iterator it_previous = path_.begin();
    polyline_type::const_iterator it = it_previous;
    it++;
    for (; it != path_.end(); it++) {
      const vector_3d & p0 = *it_previous;
      const vector_3d & p1 = *it;
      segment_type seg;
      seg.first  = p0;
      seg.second = p1;
      add(seg);
      it_previous++;
    }
    return;
  }

  void split_segment_wrt_shape::add(const wires_type & paths_)
  {
    for (wires_type::const_iterator i = paths_.begin();
         i != paths_.end();
         i++) {
      const polyline_type & p = *i;
      add(p);
    }
    return;
  }

  unsigned int split_segment_wrt_shape::fetch_inside_segments(std::list<segment_type> & segments_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_INSIDE) {
        count++;
        {
          segment_type newseg;
          segments_.push_back(newseg);
        }
        segment_type & l = segments_.back();
        i->to_line(l);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_inside_wires(wires_type & wires_)
  {
    unsigned int count = 0;
    // polyline_type * pl = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_INSIDE) {
        count++;
        // if (pl == 0) {
        //   polyline_type newseg;
        //   wires_.push_back(newseg);
        //   pl = &wires_.back();
        // }
        // if (pl->size() == 0) {
        //   pl->push_back(i->first);
        //   pl->push_back(i->last);
        // } else {
        //   if (i->first == pl.back()) {
        //     pl->push_back(i->last);
        //   } else
        // }
        {
          polyline_type newseg;
          wires_.push_back(newseg);
        }
        polyline_type & wire = wires_.back();
        i->to_wire(wire);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_outside_segments(std::list<segment_type> & segments_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_OUTSIDE) {
        count++;
        {
          segment_type newseg;
          segments_.push_back(newseg);
        }
        segment_type & l = segments_.back();
        i->to_line(l);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_outside_wires(wires_type & wires_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_OUTSIDE) {
        count++;
        {
          polyline_type newseg;
          wires_.push_back(newseg);
        }
        polyline_type & wire = wires_.back();
        i->to_wire(wire);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_surface_segments(std::list<segment_type> & segments_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_ON_SURFACE) {
        count++;
        {
          segment_type newseg;
          segments_.push_back(newseg);
        }
        segment_type & l = segments_.back();
        i->to_line(l);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_surface_wires(wires_type & wires_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_ON_SURFACE) {
        count++;
        {
          polyline_type newseg;
          wires_.push_back(newseg);
        }
        polyline_type & wire = wires_.back();
        i->to_wire(wire);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_all_segments(std::list<segment_type> & segments_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      count++;
      {
        segment_type newseg;
        segments_.push_back(newseg);
      }
      segment_type & l = segments_.back();
      i->to_line(l);
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_all_wires(wires_type & wires_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      count++;
      {
        polyline_type newseg;
        wires_.push_back(newseg);
      }
      polyline_type & wire = wires_.back();
      i->to_wire(wire);
    }
    return count;
  }

  void wires_3d_rendering_hints::initialize(const datatools::properties & config_)
  {
    if (config_.has_flag("reset")) {
      wr_options = i_wires_3d_rendering::WR_NONE;
    }
     if (config_.has_flag("grid")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_GRID;
    }
    if (config_.has_flag("grid_low_density")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY;
    }
    if (config_.has_flag("grid_high_density")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY;
    }
    if (config_.has_flag("grid_very_high_density")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY;
    }
    if (config_.has_flag("grid_huge_density")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_GRID_HUGE_DENSITY;
    }
    if (config_.has_flag("low_angle_sampling")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING;
    }
    if (config_.has_flag("high_angle_sampling")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING;
    }
    if (config_.has_flag("very_high_angle_sampling")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_VERY_HIGH_ANGLE_SAMPLING;
    }
    if (config_.has_flag("huge_angle_sampling")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_HUGE_ANGLE_SAMPLING;
    }
    if (config_.has_flag("boundings")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_BOUNDINGS;
    }
    if (config_.has_flag("explode")) {
      wr_options |= i_wires_3d_rendering::WR_BASE_EXPLODE;
    }
    return;
  }

} // end of namespace geomtools
