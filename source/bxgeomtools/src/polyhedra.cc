// -*- mode: c++ ; -*-
/* polyhedra.cc
 */

#include <geomtools/polyhedra.h>

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>
#include <limits>

#include <datatools/utils.h>
#include <datatools/units.h>

#include <geomtools/regular_polygon.h>

namespace geomtools {

  const std::string & polyhedra::polyhedra_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "polyhedra";
    }
    return label;
  }

  double polyhedra::get_xmin () const
  {
    return -_xy_max_;
  }

  double polyhedra::get_xmax () const
  {
    return +_xy_max_;
  }

  double polyhedra::get_ymin () const
  {
    return -_xy_max_;
  }

  double polyhedra::get_ymax () const
  {
    return +_xy_max_;
  }

  double polyhedra::get_zmin () const
  {
    return _z_min_;
  }

  double polyhedra::get_zmax () const
  {
    return _z_max_;
  }

  std::string polyhedra::get_shape_name () const
  {
    return polyhedra_label();
  }

  void polyhedra::set_n_sides (size_t n_sides_)
  {
    DT_THROW_IF (n_sides_ < MIN_NUMBER_OF_SIDES, std::domain_error, "Invalid number of sides '" << n_sides_ << "'");
    _n_sides_ = n_sides_;
    return;
  }

  bool polyhedra::is_extruded () const
  {
    return _extruded_;
  }

  size_t polyhedra::get_n_sides () const
  {
    return _n_sides_;
  }

  double polyhedra::get_z_min () const
  {
    return _z_min_;
  }

  double polyhedra::get_z_max () const
  {
    return _z_max_;
  }

  double polyhedra::get_xy_max () const
  {
    return _xy_max_;
  }

  double polyhedra::get_r_max () const
  {
    return _r_max_;
  }

  const polyhedra::rz_col_type & polyhedra::points () const
  {
    return _points_;
  }

  // ctor:
  polyhedra::polyhedra ()
  {
    reset ();
    return;
  }

  // dtor:
  polyhedra::~polyhedra ()
  {
    return;
  }

  void polyhedra::initialize (const datatools::properties & setup_)
  {
    double lunit = CLHEP::mm;
    if (setup_.has_key ("length_unit")) {
      const std::string lunit_str = setup_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    std::string build_mode_label;
    if (setup_.has_key ("build_mode")) {
      build_mode_label = setup_.fetch_string ("build_mode");
    }

    if (build_mode_label == "points") {
      size_t n_sides = 0;
      if (setup_.has_key ("sides")) {
        const int ns = setup_.fetch_integer ("sides");
        DT_THROW_IF (ns < (int)MIN_NUMBER_OF_SIDES, std::domain_error, "'sides' is not large enough !");
        n_sides = ns;
      }
      this->set_n_sides (n_sides);

      DT_THROW_IF (!setup_.has_key ("list_of_z"), std::logic_error, "Missing 'list_of_z' property !");
      std::vector<double> zs;
      setup_.fetch ("list_of_z", zs);
      DT_THROW_IF (zs.size () < 2, std::domain_error, "'list_of_z' has not enough points !");

      DT_THROW_IF (! setup_.has_key ("list_of_rmax"), std::logic_error, "Missing 'list_of_rmax' property !");
      std::vector<double> rmaxs;
      setup_.fetch ("list_of_rmax", rmaxs);
      DT_THROW_IF (rmaxs.size () != zs.size (), std::logic_error,
                   "'list_of_z' and 'list_of_rmax' have not the same size !");

      std::vector<double> rmins;
      double rmin;
      datatools::invalidate (rmin);
      if (setup_.has_key ("list_of_rmin")) {
        setup_.fetch ("list_of_rmin", rmins);
        DT_THROW_IF (rmins.size () != zs.size (), std::logic_error,
                     "'list_of_rmin' and 'list_of_rmax' have not the same size !");
      } else if (setup_.has_key ("rmin")) {
        rmin = setup_.fetch_real ("rmin");
        if (! setup_.has_explicit_unit ("rmin")) {
          rmin *= lunit;
        }
      } else {
        rmin = 0.0 * lunit;
      }

      for (size_t i = 0; i < zs.size (); i++) {
        const double a_z = zs[i];
        double a_rmin;
        if (datatools::is_valid (rmin)) {
          a_rmin = rmin;
        } else {
          a_rmin = rmins[i];
        }
        double a_rmax = rmaxs[i];
        this->add (a_z, a_rmin, a_rmax, false);
      }
      this->_compute_all_ ();
    } else if (build_mode_label == "datafile") {
      DT_THROW_IF (! setup_.has_key ("datafile"), std::logic_error, "Missing 'datafile' property !");
      std::string datafile = setup_.fetch_string ("datafile");
      datatools::fetch_path_with_env (datafile);
      int dc_mode = RMIN_RMAX;
      if (setup_.has_key ("datafile.columns")) {
        std::string dc = setup_.fetch_string ("datafile.columns");
        if (dc == "rmin_as_rmax") {
          dc_mode = RMIN_AS_RMAX;
        } else if (dc == "ignore_rmin") {
          dc_mode = IGNORE_RMIN;
        } else if (dc == "rmin_rmax") {
          dc_mode = RMIN_RMAX;
        } else {
          DT_THROW_IF (true,
                       std::logic_error,
                       "Invalid 'datafile.columns' mode ('"
                       << dc << "' !");
        }
      }
      this->initialize (datafile, dc_mode);
    } else {
      DT_THROW_IF (true, std::logic_error, "Invalid build mode '" << build_mode_label << "' !");
    }

    return;
  }


  void polyhedra::initialize (const std::string & filename_, int dc_mode_)
  {
    const std::string filename = filename_;
    std::ifstream ifs;
    ifs.open (filename.c_str ());
    DT_THROW_IF (! ifs, std::logic_error, "Cannot open data file '" << filename << "' !");

    size_t count = 0;
    double length_unit = CLHEP::mm;
    double z_factor = 1.0;
    double r_factor = 1.0;
    bool   ignore_rmin = false;
    if (dc_mode_ == RMIN_RMAX) {
      //std::cerr << "DEVEL: ****** polyhedra::initialize: RMIN_RMAX\n";
    }
    if (dc_mode_ == IGNORE_RMIN) {
      ignore_rmin = true;
      //std::cerr << "DEVEL: ****** polyhedra::initialize: IGNORE_RMIN\n";
    }
    bool rmin_as_rmax = false;
    if (dc_mode_ == RMIN_AS_RMAX) {
      rmin_as_rmax = true;
      //std::cerr << "DEVEL: ****** polyhedra::initialize: RMIN_AS_RMAX\n";
    }

    while (! ifs.eof ()) {
      std::string line;
      getline (ifs, line);
      count++;
      {
        std::istringstream iss (line);
        std::string word;
        iss >> word;
        if (word.empty ()) continue;
        if (word[0] == '#') {
          if (word.size () >= 2) {
            if (word == "#@sides") {
              int ns;
              iss >> ns;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the number of sides directive in data file '"
                           << filename << "' at line " << count << " !");
              DT_THROW_IF (ns < (int)MIN_NUMBER_OF_SIDES, std::domain_error,
                           "Number of sides is not large enough in data file '"
                           << filename << "' at line " << count << " !");
              const size_t nsides = (size_t) ns;
              set_n_sides (nsides);
            } else if (word == "#@length_unit") {
              std::string unit_str;
              iss >> unit_str;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the length unit directive in data file '"
                           << filename << "' at line " << count << " !");
              length_unit = datatools::units::get_length_unit_from (unit_str);
            } else if (word == "#@ignore_rmin") {
              ignore_rmin = true;
            } else if (word == "#@z_factor") {
              iss >> z_factor;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the Z-factor directive in data file '"
                           << filename << "' at line " << count << " !");
            } else if (word == "#@r_factor") {
              iss >> r_factor;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the R-factor directive in data file '"
                           << filename << "' at line " << count << " !");
            }
          }
          continue;
        }
      }
      // parse (z,r1) or (z,r1,r2) formated lines:
      {
        std::istringstream iss (line);
        double z, r1, r2;
        datatools::invalidate (z);
        datatools::invalidate (r1);
        datatools::invalidate (r2);
        iss >> z;
        DT_THROW_IF (! iss, std::logic_error,
                     "Format error for 'z' in data file '" << filename << "' at line " << count << " !");
        iss >> r1;
        DT_THROW_IF (! iss, std::logic_error,
                     "Format error for 'r1' in data file '" << filename << "' at line " << count << " !");
        // try to read a third column:
        std::string token;
        iss >> token;
        if (token.length () == 0) {
          // two columns format:
          r2 = r1;
          datatools::invalidate (r1);
        } else {
          if (token[0] == '#') {
            // if line ends with a comment: this is two columns format !
            r2 = r1;
            datatools::invalidate (r1);
          } else {
            // try three columns format (z, r1, r2):
            std::istringstream iss2 (token);
            iss2 >> r2;
            DT_THROW_IF (! iss2, std::logic_error,
                         "Format error for 'r2' in data file '"
                         << filename << "' at line " << count << " !");
            if (ignore_rmin) {
              datatools::invalidate (r1);
            }
          }
        }
        DT_THROW_IF (datatools::is_valid (r2) && (r2 < 0.0), std::logic_error,
                     "Invalid value '" << r2 << "' for '2' in data file '"
                     << filename << "' at line " << count << " !");
        const double tz  = z  * z_factor * length_unit;
        const double tr1 = r1 * r_factor * length_unit;
        const double tr2 = r2 * r_factor * length_unit;
        if (datatools::is_valid (r1)) {
          if (rmin_as_rmax) {
            this->add (tz, 0.0, tr1, false);
          } else {
            this->add (tz, tr1, tr2, false);
          }
        } else {
          this->add (tz, tr2, false);
        }
        // if (datatools::is_valid (r1)) {
        //   this->add (tz, tr1, tr2, false);
        // } else {
        //   this->add (tz, tr2, false);
        // }
      }
    }
    this->_compute_all_ ();
    return;
  }

  void polyhedra::_compute_all_ ()
  {
    _compute_surfaces_ ();
    _compute_volume_ ();
    _compute_limits_ ();
    return;
  }

  void polyhedra::add (double z_, double rmax_, bool compute_)
  {
    DT_THROW_IF (rmax_ < 0.0, std::domain_error, "Invalid negative 'rmax' !");
    r_min_max RMM;
    RMM.rmin = 0.0;
    RMM.rmax = rmax_;
    _points_[z_] = RMM;
    if (_points_.size () > 1) {
      if (compute_) _compute_all_ ();
    }
    return;
  }

  void polyhedra::add (double z_, double rmin_,  double rmax_, bool compute_)
  {
    DT_THROW_IF (rmin_ < 0.0, std::domain_error, "Invalid negative 'rmin' !");
    DT_THROW_IF (rmax_ < rmin_, std::domain_error, "Invalid value for 'rmax' !");
    r_min_max RMM;
    if (rmin_ > 0.0) _extruded_ = true;
    RMM.rmin = rmin_;
    RMM.rmax = rmax_;
    _points_[z_] = RMM;
    if (_points_.size () > 1) {
      if (compute_) _compute_all_ ();
    }
    return;
  }

  bool polyhedra::is_valid () const
  {
    return (_n_sides_ >= 3) && (_points_.size () > 1);
  }

  void polyhedra::reset ()
  {
    _n_sides_ = 0;
    _points_.clear ();
    _top_surface_        = std::numeric_limits<double>::quiet_NaN();
    _bottom_surface_     = std::numeric_limits<double>::quiet_NaN();
    _inner_side_surface_ = std::numeric_limits<double>::quiet_NaN();
    _outer_side_surface_ = std::numeric_limits<double>::quiet_NaN();
    _inner_volume_       = std::numeric_limits<double>::quiet_NaN();
    _outer_volume_       = std::numeric_limits<double>::quiet_NaN();
    _volume_             = std::numeric_limits<double>::quiet_NaN();
    _z_min_ = _z_max_ = _r_max_ = _xy_max_ = std::numeric_limits<double>::quiet_NaN();
    _extruded_ = false;
    return;
  }

  void polyhedra::_compute_limits_ ()
  {
    if (! is_valid ()) return;
    _z_min_ = _z_max_ = _r_max_ = _xy_max_ = std::numeric_limits<double>::quiet_NaN ();
    double max_radius = std::numeric_limits<double>::quiet_NaN ();
    for (rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      const double z    = i->first;
      const double rmax = i->second.rmax;
      if (! datatools::is_valid (_z_min_)) {
        _z_min_ = z;
      } else if (z < _z_min_) {
        _z_min_ = z;
      }
      if (! datatools::is_valid (_z_max_)) {
        _z_max_ = z;
      } else if (z > _z_max_) {
        _z_max_ = z;
      }
      if (! datatools::is_valid (max_radius)) {
        max_radius = rmax;
      } else if (rmax > max_radius) {
        max_radius = rmax;
      }
    }
    _r_max_ = max_radius;
    // compute the XY-bounding box:
    const double alpha = 2.0 * M_PI / _n_sides_;
    for (size_t i = 0; i < _n_sides_; i++) {
      const double theta = alpha * i;
      const double xs  = _r_max_ * cos (theta);
      const double ys  = _r_max_ * sin (theta);
      const double axs = std::abs (xs);
      const double ays = std::abs (ys);
      if (! datatools::is_valid (_xy_max_)) {
        _xy_max_ = axs;
      }
      if (axs > _xy_max_) {
        _xy_max_ = axs;
      }
      if (ays > _xy_max_) {
        _xy_max_ = ays;
      }
    }
    return;
  }

  vector_3d polyhedra::get_corner (int zplane_index_,
                                   int corner_index_,
                                   bool inner_) const
  {
    vector_3d corner;
    geomtools::invalidate (corner);
    DT_THROW_IF ((zplane_index_ < 0)  || (zplane_index_ > (int)_points_.size ()),
                 std::domain_error,
                 "Invalid Z-plane index (" << zplane_index_ << ") !");
    DT_THROW_IF ((corner_index_ < 0)  || (corner_index_ > (int)_n_sides_),
                 std::domain_error,
                 "Invalid corner index (" << corner_index_ << ") !");
    int zcount = 0;
    rz_col_type::const_iterator i = _points_.begin ();
    for (; i != _points_.end (); i++) {
      if (zcount == zplane_index_) {
        break;
      }
      zcount++;
    }
    const double z = i->first;
    const double rmin = i->second.rmin;
    const double rmax = i->second.rmax;
    double r = rmax;
    if (inner_) r = rmin;
    const double delta_phi = 2. * M_PI / _n_sides_;
    const double x = r * cos (corner_index_ * delta_phi);
    const double y = r * sin (corner_index_ * delta_phi);
    corner.set (x, y, z);

    return corner;
  }

  void polyhedra::_compute_surfaces_ ()
  {
    if (! is_valid ()) return;

    // bottom surface:
    {
      rz_col_type::const_iterator i = _points_.begin ();
      const double rmin0 = i->second.rmin;
      const double rmax0 = i->second.rmax;
      regular_polygon bottom_inner_polygon (_n_sides_, rmin0);
      regular_polygon bottom_outer_polygon (_n_sides_, rmax0);
      _bottom_surface_ = bottom_outer_polygon.get_surface ()
        - bottom_inner_polygon.get_surface ();
    }

    // outer side surface:
    {
      rz_col_type::const_iterator i = _points_.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      double rmax0 = i->second.rmax;
      double s = 0.0;
      rz_col_type::const_iterator j = _points_.begin ();
      j++;
      while (j != _points_.end ()) {
        const double z1 = j->first;
        const double rmin1 = j->second.rmin;
        const double rmax1 = j->second.rmax;
        // See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
        const size_t n = _n_sides_;
        const double angle = M_PI / n;
        const double a1 = 2 * rmax0 * sin (angle);
        const double a2 = 2 * rmax1 * sin (angle);
        const double a1_2 = a1 * a1;
        const double a2_2 = a2 * a2;
        const double h = std::abs (z1 - z0);
        const double A = 0.25
          * M_PI
          * (
             (a1_2 + a2_2) / tan (angle)
             +
             sqrt ((a1_2 - a2_2) * (a1_2 - a2_2) / (cos (angle) * cos (angle))
                   + 4 * h * h * (a1_2 + a2_2) * (a1_2 + a2_2)));
        s += A;
        // increment:
        j++;
        z0 = z1;
        rmin0 = rmin1;
        rmax0 = rmax1;
      }
      _outer_side_surface_ = s;

      // top surface:
      regular_polygon top_inner_polygon (_n_sides_, rmin0);
      regular_polygon top_outer_polygon (_n_sides_, rmax0);
      _top_surface_ = top_outer_polygon.get_surface ()
        - top_inner_polygon.get_surface ();
    }

    {
      // inner side surface:
      rz_col_type::const_iterator i = _points_.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      // double rmax0 = i->second.rmax;
      double s = 0.0;
      rz_col_type::const_iterator j = i;
      j++;
      while (j != _points_.end ()) {
        const double z1 = j->first;
        const double rmin1 = j->second.rmin;
        // const double rmax1 = j->second.rmax;
        // See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
        const size_t n = _n_sides_;
        const double angle = M_PI / n;
        const double a1 = 2 * rmin0 * sin (angle);
        const double a2 = 2 * rmin1 * sin (angle);
        const double a1_2 = a1 * a1;
        const double a2_2 = a2 * a2;
        const double h = std::abs (z1 - z0);
        const double A = 0.25
          * M_PI
          * (
             (a1_2 + a2_2) / tan (angle)
             +
             sqrt ((a1_2 - a2_2) * (a1_2 - a2_2) / (cos (angle) * cos (angle))
                   + 4 * h * h * (a1_2 + a2_2) * (a1_2 + a2_2)));
        s += A;
        // increment:
        j++;
        z0 = z1;
        rmin0 = rmin1;
        // rmax0 = rmax1;
      }
      _inner_side_surface_ = s;
    }

    return;
  }

  void polyhedra::_compute_volume_ ()
  {
    if (! is_valid ()) return;
    double vext = 0.0;
    double vint = 0.0;
    // Outer envelope:
    {
      rz_col_type::const_iterator i = _points_.begin ();
      double z0 = i->first;
      double rmax0 = i->second.rmax;
      rz_col_type::const_iterator j = i;
      j++;
      while (j != _points_.end ()) {
        const double z1 = j->first;
        const double rmax1 = j->second.rmax;
        // See: http://en.wikipedia.org/wiki/Frustum# Volume
        const size_t n = _n_sides_;
        const double angle = M_PI / n;
        const double a1 = 2 * rmax0 * sin (angle);
        const double a2 = 2 * rmax1 * sin (angle);
        const double h = std::abs (z1 - z0);
        const double V = n * h * (a1 * a1 + a2 * a2 + a1 * a2) / tan (angle) / 12;
        vext += V;
        // increment:
        j++;
        z0 = z1;
        rmax0 = rmax1;
      }
    }

    // Inner envelope:
    {
      rz_col_type::const_iterator i = _points_.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      rz_col_type::const_iterator j = i;
      j++;
      while (j != _points_.end ()) {
        const double z1 = j->first;
        const double rmin1 = j->second.rmin;
        // See: http://en.wikipedia.org/wiki/Frustum# Volume
        const size_t n = _n_sides_;
        const double angle = M_PI / n;
        const double a1 = 2 * rmin0 * sin (angle);
        const double a2 = 2 * rmin1 * sin (angle);
        const double h = std::abs (z1 - z0);
        const double V = n * h * (a1 * a1 + a2 * a2 + a1 * a2) / tan (angle) / 12;
        vint += V;
        // increment:
        j++;
        z0 = z1;
        rmin0 = rmin1;
      }
    }

    _outer_volume_ = vext;
    _inner_volume_ = vint;
    _volume_ = _outer_volume_ - _inner_volume_;
    return;
  }

  void polyhedra::compute_inner_polyhedra (polyhedra & ip_)
  {
    ip_.reset ();
    ip_.set_n_sides (this->get_n_sides ());
    for (polyhedra::rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      const double z = i->first;
      const double rmax = i->second.rmin;
      const bool add_it = true;
      if (add_it) {
        ip_.add (z, rmax, false);
      }
    }
    ip_._compute_all_ ();
    return;
  }

  void polyhedra::compute_outer_polyhedra (polyhedra & op_)
  {
    op_.reset ();
    op_.set_n_sides (this->get_n_sides ());
    for (polyhedra::rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      const double z = i->first;
      const double rmax = i->second.rmax;
      op_.add (z, 0.0, rmax, false);
    }
    op_._compute_all_ ();
    return;
  }


  double polyhedra::get_surface (uint32_t mask_) const
  {
    double s = 0.0;
    int mask = mask_;
    if ((int) mask_ == (int) ALL_SURFACES) mask = FACE_ALL;
    if (mask & FACE_INNER_SIDE) {
      s += _inner_side_surface_;
    }
    if (mask & FACE_OUTER_SIDE) {
      s += _outer_side_surface_;
    }
    if (mask & FACE_BOTTOM) {
      s += _bottom_surface_;
    }
    if (mask & FACE_TOP) {
      s += _top_surface_;
    }
    return s;
  }

  double polyhedra::get_volume (uint32_t /*flags*/) const
  {
    return _volume_;
  }

  double polyhedra::get_parameter (const std::string & flag_) const
  {
    if ( flag_ == "n_sides" )            return get_n_sides ();
    if ( flag_ == "r_max" )              return get_r_max ();
    if ( flag_ == "z_min" )              return get_z_min ();
    if ( flag_ == "z_max" )              return get_z_max ();
    if ( flag_ == "volume" )             return get_volume ();
    if ( flag_ == "surface.top" )        return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" )     return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.inner_side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer_side" ) return get_surface (FACE_OUTER_SIDE);
    if ( flag_ == "surface" )            return get_surface (FACE_ALL);

    DT_THROW_IF (true, std::logic_error, "Unknown '" << flag_ << " flag!");
  }

  bool polyhedra::is_inside (const vector_3d & point_,
                             double skin_) const
  {
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    double z = point_.z ();
    if (z > get_z_max () + 0.5 * skin) return false;
    if (z < get_z_min () - 0.5 * skin) return false;
    double r = hypot (point_.x (), point_.y ());
    if (r > get_r_max () + 0.5 * skin) return false;
    /*
      for (rz_col_type::const_iterator i = _points_.begin ();
      i != _points_.end ();
      i++)
      {
      double z1 = i->first;
      rz_col_type::const_iterator j = i;
      j++;
      if (j == _points_.end ())
      {
      break;
      }
      double z2 = j->first;
      if ((z >= z1) && (z <= z2))
      {
      double r1 = i->second;
      double r2 = j->second;
      double alpha = atan2 (r2 - r1, z2 - z1);
      double epsilon = skin / cos (alpha);
      double rs = r1 + (r2 - r1) * (z2 - z1) / ( z - z1);
      if (r < (rs + 0.5 * epsilon))
      {
      return true;
      }
      break;
      }
      }
    */
    return false;
  }

  vector_3d polyhedra::get_normal_on_surface (const vector_3d & /*position_*/) const
  {
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    vector_3d normal;
    invalidate (normal);
    /*
      double x = position_.x ();
      double y = position_.y ();
      double z = position_.z ();
      if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
      else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0);
      else if (is_on_surface (position_, FACE_SIDE))
      {
      for (rz_col_type::const_iterator i = _points_.begin ();
      i != _points_.end ();
      i++)
      {
      double z1 = i->first;
      rz_col_type::const_iterator j = i;
      j++;
      if (j == _points_.end ())
      {
      break;
      }
      double z2 = j->first;
      if ((z >= z1) && (z <= z2))
      {
      double r1 = i->second;
      double r2 = j->second;
      double alpha = atan2 (r2 - r1, z2 - z1);
      double phi = atan2 (y, x);
      double theta = alpha + 0.5 * M_PI;
      make_phi_theta (normal, phi, theta);
      break;
      }
      } // for
      }
    */
    return normal;
  }

  bool polyhedra::is_on_surface (const vector_3d & /*point_*/ ,
                                 int    /*mask_*/ ,
                                 double /* skin_ */) const
  {
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    /*
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;
      double z = point_.z ();
      double r = hypot (point_.x (), point_.y ());
      double hskin = 0.5 * skin;

      int mask = mask_;
      if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

      if (mask & FACE_BOTTOM)
      {
      double zbottom = _points_.begin ()->first;
      double rbottom = _points_.begin ()->second;
      if ((std::abs(z - zbottom) < hskin)
      && (r < (rbottom + hskin))) return true;
      }

      if (mask & FACE_TOP)
      {
      double ztop = _points_.rbegin ()->first;
      double rtop = _points_.rbegin ()->second;
      if ((std::abs(z - ztop) < hskin)
      && (r < (rtop + hskin))) return true;
      }

      if (mask & FACE_SIDE)
      {
      for (rz_col_type::const_iterator i = _points_.begin ();
      i != _points_.end ();
      i++)
      {
      double z1 = i->first;
      rz_col_type::const_iterator j = i;
      j++;
      if (j == _points_.end ())
      {
      break;
      }
      double z2 = j->first;

      if ((z >= z1) && (z <= z2))
      {
      double r1 = i->second;
      double r2 = j->second;
      double alpha = atan2 (r2 - r1, z2 - z1);
      double epsilon = skin / cos (alpha);
      double rs = r1 + (r2 - r1) * (z2 - z1) / ( z - z1);
      if (std::abs(r - rs) < 0.5 * epsilon)
      {
      return true;
      }
      break;
      }
      }
      }
    */
    return false;
  }

  bool polyhedra::find_intercept (const vector_3d & /*from_*/,
                                  const vector_3d & /*direction_*/,
                                  intercept_t & /*intercept_*/,
                                  double /*skin_*/) const
  {
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    return false;
  }

  std::ostream & operator<< (std::ostream & out_, const polyhedra & p_)
  {
    out_ << '{' << polyhedra::polyhedra_label();
    out_ << ' ' << p_._n_sides_;
    out_ << ' ' << p_._points_.size ();
    for (polyhedra::rz_col_type::const_iterator i = p_._points_.begin ();
         i != p_._points_.end ();
         i++) {
      const double z = i->first;
      const double rmin = i->second.rmin;
      const double rmax = i->second.rmax;
      out_ << ' ' << z << ' ' << rmin << ' ' << rmax;
    }
    out_ << '}';
    return out_;
  }

  std::istream & operator>> (std::istream & in_, polyhedra & p_)
  {
    p_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    std::string name;
    in_ >> name;
    if (name != polyhedra::polyhedra_label()) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    size_t n_sides;
    in_ >> n_sides;
    p_.set_n_sides (n_sides);
    size_t n;
    in_ >> n;
    for (size_t i = 0; i < n; i++) {
      double rmin, rmax, z;
      in_ >> z >> rmin >> rmax;
      if (! in_) {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
      try {
        p_.add (z, rmin, rmax);
      }
      catch (...) {
        p_.reset ();
        in_.clear (std::ios_base::failbit);
      }
    }
    c = 0;
    in_.get (c);
    if (c != '}') {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    return in_;
  }

  void polyhedra::tree_dump (std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_shape_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "N(sides) : " << get_n_sides () << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z(min) : " << get_z_min () / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z(max) : " << get_z_max () / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "R(max) : " << get_r_max () / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Extruded : " << is_extruded () << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Number of points : " << _points_.size () << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Volume : " << get_volume () / CLHEP::cm3 << " cm3" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Bottom surface : " << get_surface (FACE_BOTTOM) / CLHEP::cm2 << " cm2" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Top surface : " << get_surface (FACE_TOP) / CLHEP::cm2 << " cm2" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Outer side surface : " << get_surface (FACE_OUTER_SIDE) / CLHEP::cm2 << " cm2" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Inner side surface : " << get_surface (FACE_INNER_SIDE) / CLHEP::cm2 << " cm2" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Total surface : " << get_surface (FACE_ALL) / CLHEP::cm2 << " cm2" << std::endl;
    return;
  }

} // end of namespace geomtools

// end of polyhedra.cc
