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

  i_wires_3d_rendering::~i_wires_3d_rendering ()
  {
    return;
  }

  void i_wires_3d_rendering::generate_wires_self(std::list<polyline_3d> & wires_, uint32_t options_) const
  {
    placement identity;
    identity.set_identity();
    generate_wires(wires_, identity, options_);
    return;
  }

  void save_wires(std::ostream & out_, const std::list<polyline_3d> & wires_, uint32_t /* flags_ */)
  {
    bool gnuplot_trick = true;
    placement identity;
    identity.set_identity();
    for (std::list<polyline_3d>::const_iterator i = wires_.begin();
         i != wires_.begin();
         i++) {
      gnuplot_draw::draw_polyline(out_,
                                  identity.get_translation(),
                                  identity.get_rotation(),
                                  *i,
                                  i->is_closed(),
                                  gnuplot_trick);
    }
    return;
  }

  bool parse_wires(std::istream & in_, std::list<polyline_3d> & wires_)
  {
    bool devel = false;
    // devel = true;
    bool success = true;
    polyline_3d * current_polyline = 0;
    int blank_line_counter = 0;
    while (in_) {
      std::string line;
      std::getline(in_, line);
      if (devel) std::cerr << "geomtools::parse_wires: line='" << line << "'" << std::endl;
      if (line.empty()) {
        if (current_polyline != 0) {
          if (devel) std::cerr << "geomtools::parse_wires: stop the current wire..." << std::endl;
          current_polyline = 0;
        } else {
        }
        blank_line_counter++;
        if (blank_line_counter == 2) {
          if (devel) std::cerr << "geomtools::parse_wires: end of the wires..." << std::endl;
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
        polyline_3d dummy;
        dummy.set_closed(false);
        wires_.push_back(dummy);
        current_polyline = &wires_.back();
        if (devel) std::cerr << "geomtools::parse_wires: start a new wire..." << std::endl;
      }
      current_polyline->add(vector_3d(x, y, z));

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

  void classified_segment::to_line(line_3d & segment_) const
  {
    segment_.set_first(first);
    segment_.set_last(last);
    return;
  }

  void classify_in_out_segment(const line_3d & segment_,
                               const i_shape_3d & shape_,
                               const placement & shape_placement_,
                               double step_,
                               double tolerance_,
                               std::list<classified_segment> & the_classified_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    // logging = datatools::logger::PRIO_DEBUG;
    DT_THROW_IF(!segment_.is_valid(), std::logic_error,
                "Segment is not valid!");
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
    shape_placement_.mother_to_child(segment_.get_first(), tp0);
    shape_placement_.mother_to_child(segment_.get_last(),  tp1);
    vector_3d unit = (tp1 - tp0).unit();

    double suggested_first_step;
    double length_to_bb_exit;
    datatools::invalidate(suggested_first_step);
    datatools::invalidate(length_to_bb_exit);
    if (shape_.has_bounding_data()) {
      box bb;
      placement bbplacement;
      shape_.get_bounding_data().compute_bounding_box(bb, bbplacement);
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
        intercept_data_type tp0_intercept;
        if (bb.find_intercept(tp0_bb, unit_bb, tp0_intercept, tolerance)) {
          impact0 = tp0_intercept.get_impact();
        }
        if (geomtools::is_valid(impact0)) {
          suggested_first_step = (impact0 - tp0_bb).mag();
        }
      }
      if (bb.is_outside(tp1_bb, tolerance)) {
        intercept_data_type tp1_intercept;
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
    DT_LOG_TRACE(logging, "Start at p0 = " << p0 << " with cd0 = '" << shape_domain_flag_label(cd0)<< "'");
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
      if ((p1 - tp0).mag() > segment_.get_length()) {
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
          }

          if (cda == SHAPE_DOMAIN_OUTSIDE && cdb == SHAPE_DOMAIN_ON_SURFACE) {
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
          }

          if (cda == SHAPE_DOMAIN_INSIDE && cdb == SHAPE_DOMAIN_OUTSIDE) {
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
          }

          if (cda == SHAPE_DOMAIN_INSIDE && cdb == SHAPE_DOMAIN_ON_SURFACE) {
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
          }

          if (cda == SHAPE_DOMAIN_ON_SURFACE && cdb == SHAPE_DOMAIN_INSIDE) {
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
          }

          if (cda == SHAPE_DOMAIN_ON_SURFACE && cdb == SHAPE_DOMAIN_OUTSIDE) {
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
            break;
          }
        } while (!found);
        DT_LOG_TRACE(logging, "Current cs is closed");
        p0  = p1;
        cd0 = cd1;
        current_cs = 0;
      }
      DT_LOG_TRACE(logging, "new p0=" << p0  << " with cd0 = '" << shape_domain_flag_label(cd0) << "'");
      if ((p0 - tp0).mag() >= (segment_.get_length() - 1e-3 * tolerance)) {
        if (current_cs != 0) {
          current_cs->last = segment_.get_last();
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

  void split_segment_wrt_shape::add(const line_3d & segment_)
  {
    classify_in_out_segment(segment_,
                            *_shape_,
                            _shape_placement_,
                            _step_,
                            _tolerance_,
                            _clsegs_);
    return;
  }

  void split_segment_wrt_shape::add(const polyline_3d & path_)
  {
    if (path_.get_number_of_points() < 2) {
      return;
    }
    for (int i = 1; i < path_.get_number_of_points(); i++) {
      const vector_3d & p0 = path_.get_point(i-1);
      const vector_3d & p1 = path_.get_point(i);
      line_3d l(p0, p1);
      add(l);
    }
    if (path_.is_closed()) {
      line_3d l(path_.get_point(path_.get_number_of_points() - 1),
                path_.get_point(0));
      add(l);
    }
    return;
  }

  void split_segment_wrt_shape::add(const std::list<polyline_3d> & paths_)
  {
    for (std::list<polyline_3d>::const_iterator i = paths_.begin();
         i != paths_.end();
         i++) {
      const polyline_3d & p = *i;
      add(p);
    }
    return;
  }

  unsigned int split_segment_wrt_shape::fetch_inside_segments(std::list<line_3d> & segments_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_INSIDE) {
        count++;
        {
          geomtools::line_3d newseg;
          segments_.push_back(newseg);
        }
        geomtools::line_3d & l = segments_.back();
        i->to_line(l);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_outside_segments(std::list<line_3d> & segments_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_OUTSIDE) {
        count++;
        {
          geomtools::line_3d newseg;
          segments_.push_back(newseg);
        }
        geomtools::line_3d & l = segments_.back();
        i->to_line(l);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_surface_segments(std::list<line_3d> & segments_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_ON_SURFACE) {
        count++;
        {
          geomtools::line_3d newseg;
          segments_.push_back(newseg);
        }
        geomtools::line_3d & l = segments_.back();
        i->to_line(l);
      }
    }
    return count;
  }

  unsigned int split_segment_wrt_shape::fetch_all_segments(std::list<line_3d> & segments_)
  {
    unsigned int count = 0;
    for (std::list<geomtools::classified_segment>::const_iterator i = _clsegs_.begin();
         i != _clsegs_.end();
         i++) {
      count++;
      {
        geomtools::line_3d newseg;
        segments_.push_back(newseg);
      }
      geomtools::line_3d & l = segments_.back();
      i->to_line(l);
    }
    return count;
  }

} // end of namespace geomtools
