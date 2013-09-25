// -*- mode: c++ ; -*-
/* polycone.cc
 */

#include <geomtools/polycone.h>

#include <cmath>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>
#include <limits>

#include <datatools/utils.h>
#include <datatools/units.h>

#include <mygsl/tabulated_function.h>
#include <mygsl/numerical_differentiation.h>
#include <mygsl/interval.h>

namespace geomtools {

  const std::string & polycone::polycone_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "polycone";
    }
    return label;
  }

  std::string polycone::get_shape_name () const
  {
    return polycone_label();
  }

  double polycone::get_xmin () const
  {
    return -_r_max_;
  }

  double polycone::get_xmax () const
  {
    return +_r_max_;
  }

  double polycone::get_ymin () const
  {
    return -_r_max_;
  }

  double polycone::get_ymax () const
  {
    return +_r_max_;
  }

  double polycone::get_zmin () const
  {
    return _z_min_;
  }

  double polycone::get_zmax () const
  {
    return _z_max_;
  }

  double polycone::get_z () const
  {
    return _z_max_ - _z_min_;
  }

  double polycone::get_z_min () const
  {
    return _z_min_;
  }

  double polycone::get_z_max () const
  {
    return _z_max_;
  }

  double polycone::get_r_max () const
  {
    return _r_max_;
  }

  const polycone::rz_col_type & polycone::points () const
  {
    return _points_;
  }

  // ctor:
  polycone::polycone ()
  {
    reset ();
    return;
  }

  // dtor:
  polycone::~polycone ()
  {
    return;
  }

  void polycone::compute_inner_polycone (polycone & ip_)
  {
    ip_.reset ();
    for (polycone::rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      double z = i->first;
      double rmax = i->second.rmin;
      bool add_it = true;
      if (add_it) {
        ip_.add (z, rmax, false);
      }
    }
    ip_._compute_all_ ();
    return;
  }

  void polycone::compute_outer_polycone (polycone & op_)
  {
    op_.reset ();
    for (polycone::rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      double z = i->first;
      double rmax = i->second.rmax;
      op_.add (z, 0.0, rmax, false);
    }
    op_._compute_all_ ();
    return;
  }

  void polycone::_build_from_envelope_and_skin_ (double skin_thickness_,
                                                 double skin_step_)
  {
    _build_from_envelope_and_skin_ (skin_thickness_,
                                    skin_step_,
                                    std::numeric_limits<double>::quiet_NaN (),
                                    std::numeric_limits<double>::quiet_NaN ());
    return;
  }

  void polycone::_build_from_envelope_and_skin_ (double skin_thickness_,
                                                 double skin_step_,
                                                 double zmin_,
                                                 double zmax_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_TRACE;
    DT_LOG_TRACE (local_priority, "Entering...");

    DT_THROW_IF (skin_thickness_ < 0.0, std::logic_error, "Invalid skin thickness !");
    DT_THROW_IF (_points_.size () < 2, std::logic_error, "Not enough points !");

    DT_LOG_TRACE (local_priority, "Building interpolated primary outer envelope...");

    const std::string interpolation_mode = "linear";
    //interpolation_mode = "akima";
    mygsl::tabulated_function tf (interpolation_mode);
    for (rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      double z = i->first;
      double rmax = i->second.rmax;
      tf.add_point (z, rmax);
    }
    tf.lock_table ();

    // thickness of the polycone's skin:
    const double skin = skin_thickness_;

    // compute the recommended sampling step along the envelope:
    const double dx0 = 5 * skin; // default
    double dx = dx0;
    if (skin_step_ > 0.0) {
      dx = skin_step_;
    }
    DT_LOG_TRACE (local_priority, "Building interpolated inner envelope...");
    //mygsl::tabulated_function tf2 (interpolation_mode);
    mygsl::tabulated_function tf3 (interpolation_mode);
    for (mygsl::tabulated_function::points_map_type::const_iterator
           i = tf.points ().begin ();
         i != tf.points ().end ();
         i++) {
      mygsl::tabulated_function::points_map_type::const_iterator j = i;
      const double xi = i->first;
      const double yi = i->second;
      j++;
      mygsl::interval domain (tf.x_min (), tf.x_max (), 0.01 * skin);
      const double drdz = mygsl::derivative (tf, xi, domain);
      const double uy = drdz;
      const double dyij = skin / sqrt (1 + uy * uy);
      const double dxij = - dyij * uy;
      const double xk = xi - dxij;
      const double yk = yi - dyij;
      //tf2.add_point (xj, yj, false);
      tf3.add_point (xk, yk, false);
    }
    tf3.lock_table ();

    // Manage bounds:
    DT_LOG_TRACE (local_priority, "Building properly bounded interpolated inner envelope...");
    mygsl::tabulated_function tf3bis (interpolation_mode);
    for (mygsl::tabulated_function::points_map_type::const_iterator
           i = tf.points ().begin ();
         i != tf.points ().end ();
         i++) {
      const double x = i->first;
      double y;
      if (tf3.is_valid (x)) {
        y = tf3 (x);
      } else {
        if (x < tf3.x_min ()) {
          const double df3dx = mygsl::derivative_forward (tf3, tf3.x_min (), 0.05 * dx);
          y = tf3 (tf3.x_min ()) + (x - tf3.x_min ()) * df3dx;
        } else { //if (x > tf3.x_max ())

          const double df3dx = mygsl::derivative_backward (tf3, tf3.x_max (), 0.05 * dx);
          y = tf3 (tf3.x_max ()) + (x - tf3.x_max ()) * df3dx;
        }
      }
      tf3bis.add_point (x, y, false);
    }
    tf3bis.lock_table ();

    DT_LOG_TRACE (local_priority, "Building final interpolated outer and inner envelopes with requested Z sampling...");

    mygsl::tabulated_function tf_outer ("linear");
    mygsl::tabulated_function tf_inner ("linear");
    double z1 = tf.x_min ();
    double z2 = tf.x_max ();
    DT_LOG_TRACE (local_priority, "z1=" << z1);
    DT_LOG_TRACE (local_priority, "z2=" << z2);
    const double zmin = zmin_;
    if (datatools::is_valid (zmin)) {
      DT_LOG_TRACE (local_priority, "Z(min)=" << zmin);
      if (zmin > tf.x_min ()) {
        z1 = zmin;
      }
    }
    const double zmax = zmax_;
    if (datatools::is_valid (zmax)) {
      DT_LOG_TRACE (local_priority, "Z(max)=" << zmax);
      if (zmax < tf.x_max ()) {
        z2 = zmax;
      }
    }
    double dz = dx;
    double za, ra;
    datatools::invalidate (za);
    datatools::invalidate (ra);
    double z = z1;
    bool stop = false;
    do {
      if (z >= z2) {
        DT_LOG_TRACE (local_priority, "z2 stop");
        z = z2;
        stop = true;
      }
      //double safe = 0.05 * skin;
      const double safe = 0.1 * dz;
      mygsl::interval domain (tf.x_min (), tf.x_max (), safe);
      const double drdz = mygsl::derivative (tf, z, domain);
      dz = dx / sqrt (1.0 + drdz * drdz);
      if (dz > dx) {
        dz = dx;
      }
      double r_outer = tf (z);
      if (r_outer < 0.0) { // should not occur

        r_outer = 0.0;
      }
      double r_inner = tf3bis (z);
      if (datatools::is_valid (za) && datatools::is_valid (ra)) {
        const double zb = z;
        const double rb = r_inner;
        if ((ra * rb) < 0.0) {
          const double a = (rb - ra) / (zb - za);
          const double b = rb - a * zb;
          const double z_r = -b / a;
          const double rmin_r = 0.0;
          const double rmax_r = tf (z_r);
          tf_outer.add_point (z_r, rmax_r, false);
          tf_inner.add_point (z_r, rmin_r, false);
        }
      }
      za = z;
      ra = r_inner;
      if (r_inner < 0.0) {
        r_inner = 0.0;
      }
      tf_outer.add_point (z, r_outer, false);
      tf_inner.add_point (z, r_inner, false);
      z += dz;
      if (stop) break;
    } while (! stop);
    // lock interpolators:
    tf_outer.lock_table ();
    tf_inner.lock_table ();
    DT_LOG_TRACE (local_priority, "Locked !");
    _points_.clear ();

    mygsl::tabulated_function::points_map_type::const_iterator i = tf_outer.points ().begin ();
    mygsl::tabulated_function::points_map_type::const_iterator j = tf_inner.points ().begin ();
    for (size_t k = 0; k < tf_outer.points ().size (); k++) {
      double z = i->first;
      double rmin = j->second;
      double rmax = i->second;
      if (rmin > rmax) rmin = rmax;
      if (rmin < 0.0) { // should not occur
        rmin = 0.0;
      }
      if (rmin < 1e-300) {
        rmin = 0.0;
      }
      this->add (z, rmin, rmax, false);
      i++;
      j++;
    }

    _compute_all_ ();
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  void polycone::initialize (const datatools::properties & setup_)
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
      DT_THROW_IF (! setup_.has_key ("list_of_z"), std::logic_error, "Missing 'list_of_z' property !");
      std::vector<double> zs;
      setup_.fetch ("list_of_z", zs);
      DT_THROW_IF (zs.size () < 2, std::logic_error, "'list_of_z' has not enough points !");

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
                     "'list_of_z' and 'list_of_rmin' have not the same size !");
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
        const double a_rmax = rmaxs[i];
        this->add (a_z, a_rmin, a_rmax, false);
      }
      this->_compute_all_ ();
    } else if (build_mode_label == "datafile") {
      DT_THROW_IF (! setup_.has_key ("datafile"), std::logic_error,"Missing 'datafile' property !");
      std::string datafile = setup_.fetch_string ("datafile");
      datatools::fetch_path_with_env (datafile);
      double zmin;
      datatools::invalidate (zmin);
      if (setup_.has_key ("zmin")) {
        zmin = setup_.fetch_real ("zmin");
        if (! setup_.has_explicit_unit ("zmin")) {
          zmin *= lunit;
        }
      }
      double zmax;
      datatools::invalidate (zmax);
      if (setup_.has_key ("zmax")) {
        zmax = setup_.fetch_real ("zmax");
        if (! setup_.has_explicit_unit ("zmax")) {
          zmax *= lunit;
        }
      }
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
      this->initialize (datafile, zmin, zmax, dc_mode);
    } else {
      DT_THROW_IF (true, std::logic_error, "Invalid build mode '" << build_mode_label << "' !");
    }

    return;
  }

  void polycone::initialize (const std::string & filename_)
  {
    polycone::initialize (filename_,
                          std::numeric_limits<double>::quiet_NaN (),
                          std::numeric_limits<double>::quiet_NaN ());
    return;
  }

  void polycone::initialize (const std::string & filename_,
                             double zmin_, double zmax_, int dc_mode_)
  {
    const std::string filename = filename_;
    std::ifstream ifs;
    ifs.open (filename.c_str ());
    DT_THROW_IF (! ifs, std::logic_error, "Cannot open data file '" << filename << "' !");

    size_t count = 0;
    double length_unit = CLHEP::mm;
    double z_factor = 1.0;
    double r_factor = 1.0;
    double skin_thickness;
    datatools::invalidate (skin_thickness);
    double skin_step;
    datatools::invalidate (skin_step);
    bool ignore_rmin = false;
    if (dc_mode_ == RMIN_RMAX) {
      //std::cerr << "DEVEL: ****** polycone::initialize: RMIN_RMAX\n";
    }
    if (dc_mode_ == IGNORE_RMIN) {
      ignore_rmin = true;
      //std::cerr << "DEVEL: ****** polycone::initialize: IGNORE_RMIN\n";
    }
    bool rmin_as_rmax = false;
    if (dc_mode_ == RMIN_AS_RMAX) {
      rmin_as_rmax = true;
      //std::cerr << "DEVEL: ****** polycone::initialize: RMIN_AS_RMAX\n";
    }
    while (! ifs.eof ()) {
      std::string line;
      std::getline (ifs, line);
      count++;
      {
        std::istringstream iss (line);
        std::string  word;
        iss >> word;
        if (word.empty ()) continue;
        if (word[0] == '#') {
          if (word.size () >= 2) {
            if (word == "#@length_unit") {
              std::string unit_str;
              iss >> unit_str;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the length unit directive in data file '"
                           << filename << "' at line " << count << " !");
              length_unit = datatools::units::get_length_unit_from (unit_str);
            } else if (word == "#@ignore_rmin") {
              ignore_rmin = true;
              //std::cerr << "DEVEL: ****** polycone::initialize: form file: IGNORE_RMIN\n";
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
            } else if (word == "#@skin_thickness") {
              iss >> skin_thickness;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the skin_thickness directive in data file '"
                           << filename << "' at line " << count << " !");
            } else if (word == "#@skin_step") {
              iss >> skin_step;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the skin_thickness directive in data file '"
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
                     "Format error for 'z' in data file '"
                     << filename << "' at line " << count << " !");
        iss >> r1;
        DT_THROW_IF (! iss, std::logic_error,
                     "Format error for 'r1' in data file '"
                     << filename << "' at line " << count << " !");
        // try to read a third column:
        std::string token;
        iss >> token;
        if (token.length () == 0) {
          // two columns format:
          r2 = r1;
          datatools::invalidate (r1);
          DT_THROW_IF(rmin_as_rmax,
                      std::logic_error,
                      "Polycone build mode 'rmin_as_rmax' cannot be used "
                      << "with the two-columns data file '"
                      << filename << "' !");
        } else {
          if (token[0] == '#') {
            // if line ends with a comment: this is two columns format !
            r2 = r1;
            datatools::invalidate (r1);
          } else {
            // try three columns format:
            std::istringstream iss2 (token);
            iss2 >> r2;
            DT_THROW_IF (! iss2, std::logic_error,
                         "Format error for 'r2' in data file '"
                         << filename << "' at line " << count << " !");
            if (ignore_rmin) {
              datatools::invalidate (r1);
            } else if (datatools::is_valid (skin_thickness)) {
              DT_THROW_IF (true, std::logic_error,
                           "Invalid format for 'z r2' pair "
                           << "in 'skin_thickness' mode from data file '"
                           << filename << "' at line " << count << " !");
            }
          }
        }
        DT_THROW_IF (datatools::is_valid (r2) && (r2 < 0.0), std::logic_error,
                     "Invalid value '" << r2 << "' for '2' in data file '"
                     << filename << "' at line " << count << " !");
        double tz, tr1, tr2;
        tz  = z  * z_factor * length_unit;
        tr1 = r1 * r_factor * length_unit;
        tr2 = r2 * r_factor * length_unit;
        if (datatools::is_valid (r1)) {
          if (rmin_as_rmax) {
            this->add (tz, 0.0, tr1, false);
          } else {
            this->add (tz, tr1, tr2, false);
          }
        } else {
          this->add (tz, tr2, false);
        }
      }
    }
    this->_compute_all_ ();

    if (datatools::is_valid (skin_thickness)) {
      if (! datatools::is_valid (skin_step)) {
        skin_step = abs (zmax_ - zmin_) / 20.0;
      }
      _build_from_envelope_and_skin_ (skin_thickness, skin_step, zmin_, zmax_);
    }
    return;
  }

  void polycone::_compute_all_ ()
  {
    _compute_surfaces_ ();
    _compute_volume_ ();
    _compute_limits_ ();
    return;
  }

  void polycone::add (double z_, double rmax_, bool compute_)
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

  bool polycone::is_extruded () const
  {
    return _extruded_;
  }

  void polycone::add (double z_, double rmin_,  double rmax_, bool compute_)
  {
    DT_THROW_IF (rmin_ < 0.0, std::domain_error, "Invalid negative 'rmin' !");
    DT_THROW_IF (rmax_ < rmin_, std::domain_error,
                 "Invalid value for 'rmax==" << rmax_ << "' ! ('rmin==" << rmin_ << "')");
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

  bool polycone::is_valid () const
  {
    return _points_.size () > 1;
  }

  void polycone::reset ()
  {
    _points_.clear ();
    _top_surface_        = std::numeric_limits<double>::quiet_NaN();
    _bottom_surface_     = std::numeric_limits<double>::quiet_NaN();
    _inner_side_surface_ = std::numeric_limits<double>::quiet_NaN();
    _outer_side_surface_ = std::numeric_limits<double>::quiet_NaN();
    _inner_volume_       = std::numeric_limits<double>::quiet_NaN();
    _outer_volume_       = std::numeric_limits<double>::quiet_NaN();
    _volume_             = std::numeric_limits<double>::quiet_NaN();
    _z_min_ = _z_max_ = _r_max_ = std::numeric_limits<double>::quiet_NaN();
    _extruded_ = false;
    return;
  }

  void polycone::initialize ()
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    _compute_all_ ();
    return;
  }

  void polycone::_compute_limits_ ()
  {
    if (! is_valid ()) return;
    _z_min_ = _z_max_ = _r_max_ = std::numeric_limits<double>::quiet_NaN();
    for (rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      const double z = i->first;
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
      if (! datatools::is_valid (_r_max_)) {
        _r_max_ = rmax;
      } else if (rmax > _r_max_) {
        _r_max_ = rmax;
      }
    }
    return;
  }

  void polycone::_compute_surfaces_ ()
  {
    if (! is_valid ()) return;

    // bottom surface:
    {
      rz_col_type::const_iterator i = _points_.begin ();
      const double rmin0 = i->second.rmin;
      const double rmax0 = i->second.rmax;
      _bottom_surface_ = M_PI * (rmax0 * rmax0 - rmin0 * rmin0);
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
        const double R1 = rmax0;
        const double R2 = rmax1;
        const double R1_2 = R1 * R1;
        const double R2_2 = R2 * R2;
        const double h = std::abs (z1 - z0);
        const double A = M_PI * (R1_2 + R2_2 +
                                 std::sqrt ((R1_2 - R2_2) * (R1_2 - R2_2)
                                            + h * h * (R1_2 + R2_2) * (R1_2 + R2_2)));
        s += A;
        // increment:
        j++;
        z0 = z1;
        rmin0 = rmin1;
        rmax0 = rmax1;
      }
      _outer_side_surface_ = s;

      // top surface:
      _top_surface_ = M_PI * (rmax0 * rmax0 - rmin0 * rmin0);
    }

    {
      // inner side surface:
      rz_col_type::const_iterator i = _points_.begin ();
      double z0 = i->first;
      double rmin0 = i->second.rmin;
      double rmax0 = i->second.rmax;
      double s = 0.0;
      rz_col_type::const_iterator j = i;
      j++;
      while (j != _points_.end ()) {
        const double z1 = j->first;
        const double rmin1 = j->second.rmin;
        const double rmax1 = j->second.rmax;
        // See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
        const double R1 = rmin0;
        const double R2 = rmin1;
        const double R1_2 = R1 * R1;
        const double R2_2 = R2 * R2;
        const double h = std::abs (z1 - z0);
        const double A = M_PI * (R1_2 + R2_2 +
                                 std::sqrt ((R1_2 - R2_2) * (R1_2 - R2_2)
                                            + h * h * (R1_2 + R2_2) * (R1_2 + R2_2)));
        s += A;
        // increment:
        j++;
        z0 = z1;
        rmin0 = rmin1;
        rmax0 = rmax1;
      }
      _inner_side_surface_ = s;
    }
    return;
  }

  void polycone::_compute_volume_ ()
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
        const double r1 = j->second.rmax;
        // See: http://en.wikipedia.org/wiki/Frustum# Volume
        const double R1 = rmax0;
        const double R2 = r1;
        const double R1_2 = R1 * R1;
        const double R2_2 = R2 * R2;
        const double h = abs (z1 - z0);
        const double V = M_PI * h * (R1_2 + R2_2 + R1 * R2) / 3;
        vext += V;
        // increment:
        j++;
        z0 = z1;
        rmax0 = r1;
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
        const double R1 = rmin0;
        const double R2 = rmin1;
        const double R1_2 = R1 * R1;
        const double R2_2 = R2 * R2;
        const double h = abs (z1 - z0);
        const double V = M_PI * h * (R1_2 + R2_2 + R1 * R2) / 3;
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

  double polycone::get_surface (uint32_t mask_) const
  {
    double s = 0.0;
    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;
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

  double polycone::get_volume (uint32_t flags) const
  {
    return _volume_;
  }

  double polycone::get_parameter (const std::string & flag_) const
  {
    if ( flag_ == "r_max" )              return get_r_max ();
    if ( flag_ == "z_min" )              return get_z_min ();
    if ( flag_ == "z_max" )              return get_z_max ();
    if ( flag_ == "volume" )             return get_volume ();
    if ( flag_ == "surface.top" )        return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" )     return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.inner_side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer_side" ) return get_surface (FACE_OUTER_SIDE);
    if ( flag_ == "surface" )            return get_surface (FACE_ALL);

    DT_THROW_IF (true, std::logic_error, "Unknown parameter '" << flag_ << "' flag !");
  }

  bool polycone::is_inside (const vector_3d & point_,
                            double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    const double z = point_.z ();
    if (z > get_z_max () + 0.5 * skin) return false;
    if (z < get_z_min () - 0.5 * skin) return false;
    const double r = hypot (point_.x (), point_.y ());
    if (r > get_r_max () + 0.5 * skin) return false;
    for (rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      const double z1 = i->first;
      rz_col_type::const_iterator j = i;
      j++;
      if (j == _points_.end ()) {
        break;
      }
      const double z2 = j->first;
      if ((z >= z1) && (z <= z2)) {
        const double r1 = i->second.rmax;
        const double r2 = j->second.rmax;
        const double alpha = atan2 (r2 - r1, z2 - z1);
        const double epsilon = skin / cos (alpha);
        const double rs = r1 + (r2 - r1) * (z2 - z1) / ( z - z1);
        if (r < (rs + 0.5 * epsilon)) {
          return true;
        }
        break;
      }
    }
    return false;
  }

  vector_3d polycone::get_normal_on_surface (const vector_3d & position_) const
  {
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    vector_3d normal;
    invalidate (normal);
    const double x = position_.x ();
    const double y = position_.y ();
    const double z = position_.z ();
    if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0);
    else if (is_on_surface (position_, FACE_OUTER_SIDE)) {
      for (rz_col_type::const_iterator i = _points_.begin ();
           i != _points_.end ();
           i++) {
        const double z1 = i->first;
        rz_col_type::const_iterator j = i;
        j++;
        if (j == _points_.end ()) {
          break;
        }
        const double z2 = j->first;
        if ((z >= z1) && (z <= z2)) {
          const double r1 = i->second.rmax;
          const double r2 = j->second.rmax;
          const double alpha = atan2 (r2 - r1, z2 - z1);
          const double phi = atan2 (y, x);
          const double theta = alpha + 0.5 * M_PI;
          make_phi_theta (normal, phi, theta);
          break;
        }
      } // for
    } else if (is_on_surface (position_, FACE_INNER_SIDE)) {
      //...
    }
    return normal;
  }

  bool polycone::is_on_surface (const vector_3d & point_ ,
                                int    mask_ ,
                                double skin_) const
  {
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;
    const double z = point_.z ();
    const double r = hypot (point_.x (), point_.y ());
    const double hskin = 0.5 * skin;

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    if (mask & FACE_BOTTOM) {
      const double zbottom = _points_.begin ()->first;
      const double rbottom = _points_.begin ()->second.rmax;
      if ((abs(z - zbottom) < hskin)
          && (r < (rbottom + hskin))) return true;
    }

    if (mask & FACE_TOP) {
      const double ztop = _points_.rbegin ()->first;
      const double rtop = _points_.rbegin ()->second.rmax;
      if ((abs(z - ztop) < hskin)
          && (r < (rtop + hskin))) return true;
    }

    DT_THROW_IF (mask & FACE_INNER_SIDE, std::logic_error, "Not implemented yet !");

    if (mask & FACE_OUTER_SIDE) {
      for (rz_col_type::const_iterator i = _points_.begin ();
           i != _points_.end ();
           i++) {
        const double z1 = i->first;
        rz_col_type::const_iterator j = i;
        j++;
        if (j == _points_.end ()) {
          break;
        }
        const double z2 = j->first;

        if ((z >= z1) && (z <= z2)) {
          const double r1 = i->second.rmax;
          const double r2 = j->second.rmax;
          const double alpha = atan2 (r2 - r1, z2 - z1);
          const double epsilon = skin / cos (alpha);
          const double rs = r1 + (r2 - r1) * (z2 - z1) / ( z - z1);
          if (std::abs(r - rs) < 0.5 * epsilon)
            {
              return true;
            }
          break;
        }
      }
    }
    return false;
  }

  bool polycone::find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_) const
  {
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    return false;
  }

  std::ostream & operator<< (std::ostream & out_, const polycone & p_)
  {
    out_ << '{' << polycone::polycone_label();
    out_ << ' ' << p_._points_.size ();
    for (polycone::rz_col_type::const_iterator i = p_._points_.begin ();
         i != p_._points_.end ();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      double rmax = i->second.rmax;
      out_ << ' ' << z << ' ' << rmin << ' ' << rmax;
    }
    out_ << '}';
    return out_;
  }

  std::istream & operator>> (std::istream & in_, polycone & p_)
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
    if (name != polycone::polycone_label()) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
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
        p_.add (z, rmin, rmax, false);
      }
      catch (...)
        {
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
    p_._compute_all_ ();
    return in_;
  }

  void polycone::tree_dump (std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_,
                            bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z(min) : " << get_z_min () / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z(max) : " << get_z_max () / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "R(max) : " << get_r_max () / CLHEP::mm << " mm" << std::endl;
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

  // end of polycone.cc
