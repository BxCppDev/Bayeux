// \file geomtools/polyhedra.cc

// Ourselves:
#include <geomtools/polyhedra.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
#include <geomtools/regular_polygon.h>
#include <geomtools/right_polygonal_frustrum.h>
#include <geomtools/composite_surface.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(polyhedra, "geomtools::polyhedra")

  const size_t polyhedra::MIN_NUMBER_OF_SIDES;

  const std::string & polyhedra::polyhedra_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "polyhedra";
    }
    return label;
  }

  void polyhedra::_build_bounding_data()
  {
    _grab_bounding_data().make_cylinder(_xy_max_, get_zmin(), get_zmax());
    return;
  }

  void polyhedra::_set_defaults()
  {
    _computed_ = false;
    _n_sides_  = 0;
    _has_top_face_    = boost::logic::indeterminate;
    _has_bottom_face_ = boost::logic::indeterminate;
    _has_inner_face_  = boost::logic::indeterminate;
    datatools::invalidate(_top_surface_);
    datatools::invalidate(_bottom_surface_);
    datatools::invalidate(_outer_side_surface_);
    datatools::invalidate(_inner_side_surface_);
    datatools::invalidate(_outer_volume_);
    datatools::invalidate(_inner_volume_);
    datatools::invalidate(_volume_);
    datatools::invalidate(_z_min_);
    datatools::invalidate(_z_max_);
    datatools::invalidate(_r_max_);
    datatools::invalidate(_xy_max_);
    _extruded_ = false;
    return;
  }

  unsigned int polyhedra::number_of_frustra() const
  {
    return _points_.size() - 1;
  }

  void polyhedra::get_frustrum_data(size_t index_, frustrum_data & fd_) const
  {
    DT_THROW_IF (index_ >= number_of_frustra(),
                 std::range_error,
                 "Invalid frustrum index [" << index_ << "] !");
    size_t count = 0;
    for (rz_col_type::const_iterator it = _points_.begin ();
         it != _points_.end ();
         it++) {
      rz_col_type::const_iterator jt = it;
      jt++;
      if (count == index_) {
        fd_.z1 = it->first;
        fd_.a1 = it->second.rmin;
        fd_.b1 = it->second.rmax;
        fd_.z2 = jt->first;
        fd_.a2 = jt->second.rmin;
        fd_.b2 = jt->second.rmax;
        break;
      }
      count++;
      if (count == _points_.size()) break;
    }

    return;
  }

  void polyhedra::compute_frustrum(right_polygonal_frustrum & f_,
                                   placement & positioning_,
                                   int index_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Polyhedra is not valid !");
    f_.reset();
    positioning_.reset();
    DT_THROW_IF(index_ < 0 || index_ >= (int)number_of_frustra(),
                std::range_error,
                "Invalid frustrum index [" << index_ << "]!");
    frustrum_data fd;
    get_frustrum_data(index_, fd);
    double z = fd.z2 - fd.z1;
    double zpos = 0.5 * (fd.z2 + fd.z1);
    f_.set_n_sides(_n_sides_);
    f_.set_z(z);
    f_.set_outer_bottom_radius(fd.b1);
    f_.set_outer_top_radius(fd.b2);
    if (datatools::is_valid(fd.a1) && fd.a1 >= 0.0) {
      f_.set_inner_bottom_radius(fd.a1);
    }
    if (datatools::is_valid(fd.a2) && fd.a2 >= 0.0) {
      f_.set_inner_top_radius(fd.a2);
    }
    positioning_.set_identity();
    positioning_.set_translation(0.0, 0.0, zpos);
    return;
  }

  void polyhedra::compute_top_face(composite_surface & face_, placement & positioning_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    face_.reset();
    positioning_.reset();
    if (has_top_face()) {
      right_polygonal_frustrum rpf;
      placement rpf_placement;
      compute_frustrum(rpf, rpf_placement, number_of_frustra() - 1);
      if (rpf.is_valid()) {
        placement face_placement;
        rpf.compute_top_face(face_, face_placement);
        // std::cerr << "DEVEL: polyhedra::compute_top_face: checking cs..." << std::endl;
        if (face_.is_valid()) {
          double z = rpf_placement.get_translation().z();
          z += face_placement.get_translation().z();
          rpf_placement.grab_translation().setZ(z);
          positioning_ = rpf_placement;
        }
      }
    }
    return;
  }

  void polyhedra::compute_bottom_face(composite_surface & face_, placement & positioning_) const
  {
    // std::cerr << "DEVEL: polyhedra::compute_bottom_face: Entering..." << std::endl;
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    face_.reset();
    positioning_.reset();
    if (has_bottom_face()) {
      // std::cerr << "DEVEL: polyhedra::compute_bottom_face: has_bottom_face..." << std::endl;
      right_polygonal_frustrum rpf;
      placement rpf_placement;
      compute_frustrum(rpf, rpf_placement, 0);
      if (rpf.is_valid()) {
        // std::cerr << "DEVEL: polyhedra::compute_bottom_face: fetch first frustrum.." << std::endl;
        placement face_placement;
        rpf.compute_bottom_face(face_, face_placement);
        // std::cerr << "DEVEL: polyhedra::compute_bottom_face: checking cs..." << std::endl;
        if (face_.is_valid()) {
          double z = rpf_placement.get_translation().z();
          z += face_placement.get_translation().z();
          rpf_placement.grab_translation().setZ(z);
          positioning_ = rpf_placement;
        }
      }
    }
    // std::cerr << "DEVEL: polyhedra::compute_bottom_face: Exiting." << std::endl;
    return;
  }

  void polyhedra::compute_inner_face(composite_surface & face_,
                                     placement & positioning_,
                                     int index_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    face_.reset();
    positioning_.reset();
    if (has_inner_face()) {
      right_polygonal_frustrum rpf;
      placement rpf_placement;
      compute_frustrum(rpf, rpf_placement, index_);
      if (rpf.has_inner_face()) {
        rpf.compute_inner_face(face_);
        positioning_ = rpf_placement;
      }
    }
    return;
  }

  void polyhedra::compute_outer_face(composite_surface & face_,
                                     placement & positioning_,
                                     int index_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polycone is not valid !");
    face_.reset();
    positioning_.reset();
    right_polygonal_frustrum rpf;
    placement rpf_placement;
    compute_frustrum(rpf, rpf_placement, index_);
    rpf.compute_outer_face(face_);
    positioning_ = rpf_placement;
    return;
  }

  void polyhedra::compute_outer_face(composite_surface & face_,
                                     placement & positioning_) const
  {
    face_.reset();
    positioning_.reset();
    // Outer side face:
    for (size_t i = 0; i < number_of_frustra(); i++) {
      geomtools::face_info & finfo = face_.add();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_outer_face(cs, finfo.grab_positioning(), i);
      // std::cerr << "DEVEL: polyhedra::compute_outer_face: checking cs..." << std::endl;
      if (! cs.is_valid()) {
        face_.remove_last();
      }
      // std::cerr << "DEVEL: polyhedra::compute_outer_face: checked cs." << std::endl;
    }
    return;
  }

  void polyhedra::compute_inner_face(composite_surface & face_,
                                     placement & positioning_) const
  {
    face_.reset();
    positioning_.reset();
    if (has_inner_face()) {
      // Inner side face:
      for (size_t i = 0; i < number_of_frustra(); i++) {
        geomtools::face_info & finfo = face_.add();
        composite_surface & cs = finfo.add_face<composite_surface>();
        compute_inner_face(cs, finfo.grab_positioning(), i);
        // std::cerr << "DEVEL: polyhedra::compute_inner_face: checking cs..." << std::endl;
        if (! cs.is_valid()) {
          face_.remove_last();
        }
        // std::cerr << "DEVEL: polyhedra::compute_inner_face: checked cs." << std::endl;
      }
    }
    return;
  }

  // virtual
  unsigned int polyhedra::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Polyhedra is not valid !");
    unsigned int nfaces = 0;

    if (has_bottom_face()) {
      // Bottom face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_bottom_face(cs, finfo.grab_positioning());
      if (cs.is_valid()) {
        face_identifier fid;
        fid.set_face_bit(FACE_BOTTOM);
        finfo.set_identifier(fid);
        finfo.set_label("bottom");
        nfaces++;
      } else {
        faces_.pop_back();
      }
    }

    if (has_top_face()) {
      // Top face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_top_face(cs, finfo.grab_positioning());
      if (cs.is_valid()) {
        face_identifier fid;
        fid.set_face_bit(FACE_TOP);
        finfo.set_identifier(fid);
        finfo.set_label("top");
        nfaces++;
      } else {
        faces_.pop_back();
      }
    }

    {
      // Outer side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_outer_face(cs, finfo.grab_positioning());
      if (cs.is_valid()) {
        face_identifier fid;
        fid.set_face_bit(FACE_OUTER_SIDE);
        finfo.set_identifier(fid);
        finfo.set_label("outer_side");
        nfaces++;
      }
    }

    if (has_inner_face()) {
      // Inner side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_inner_face(cs, finfo.grab_positioning());
      if (cs.is_valid()) {
        face_identifier fid;
        fid.set_face_bit(FACE_INNER_SIDE);
        finfo.set_identifier(fid);
        finfo.set_label("inner_side");
        nfaces++;
      }
    }

    return nfaces;
  }

  bool polyhedra::has_top_face() const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    if (_has_top_face_) {
      return true;
    } else if (!_has_top_face_) {
      return false;
    } else {
      DT_THROW(std::logic_error, "Polyhedra is not properly initialized !");
    }
  }

  bool polyhedra::has_bottom_face() const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    if (_has_bottom_face_) {
      return true;
    } else if (!_has_bottom_face_) {
      return false;
    } else {
      DT_THROW(std::logic_error, "Polyhedra is not properly initialized !");
    }
  }

  bool polyhedra::has_inner_face() const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    if (_has_inner_face_) {
      return true;
    } else if (!_has_inner_face_) {
      return false;
    } else {
      DT_THROW(std::logic_error, "Polyhedra is not properly initialized !");
    }
  }

  double polyhedra::get_xmin() const
  {
    return -_xy_max_;
  }

  double polyhedra::get_xmax() const
  {
    return +_xy_max_;
  }

  double polyhedra::get_ymin() const
  {
    return -_xy_max_;
  }

  double polyhedra::get_ymax() const
  {
    return +_xy_max_;
  }

  double polyhedra::get_zmin() const
  {
    return _z_min_;
  }

  double polyhedra::get_zmax() const
  {
    return _z_max_;
  }

  std::string polyhedra::get_shape_name() const
  {
    return polyhedra_label();
  }

  void polyhedra::set_n_sides(size_t n_sides_)
  {
    DT_THROW_IF (n_sides_ < MIN_NUMBER_OF_SIDES, std::domain_error, "Invalid number of sides '" << n_sides_ << "'");
    _n_sides_ = n_sides_;
    return;
  }

  bool polyhedra::is_extruded() const
  {
    return _extruded_;
  }

  size_t polyhedra::get_n_sides() const
  {
    return _n_sides_;
  }

  double polyhedra::get_z_min() const
  {
    return _z_min_;
  }

  double polyhedra::get_z_max() const
  {
    return _z_max_;
  }

  double polyhedra::get_xy_max() const
  {
    return _xy_max_;
  }

  double polyhedra::get_r_max() const
  {
    return _r_max_;
  }

  const polyhedra::rz_col_type & polyhedra::points() const
  {
    return _points_;
  }

  polyhedra::polyhedra()
  {
    reset ();
    return;
  }

  polyhedra::~polyhedra()
  {
    return;
  }

  void polyhedra::_at_lock()
  {
    if (!_computed_) {
      _compute_all_();
    }
    return;
  }

  void polyhedra::initialize(const datatools::properties & setup_,
                             const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(setup_, objects_);

    if (!is_valid()) {
      double lunit = CLHEP::mm;

      if (setup_.has_key("length_unit")) {
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
        this->set_n_sides(n_sides);

        bool build_by_apothem = false;
        if (setup_.has_key("build_by_apothem")) {
          build_by_apothem = setup_.fetch_boolean("build_by_apothem");
        }

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
          if (datatools::is_valid(rmin)) {
            a_rmin = rmin;
          } else {
            a_rmin = rmins[i];
          }
          double a_rmax = rmaxs[i];
          if (build_by_apothem) {
            double apothem_factor = std::cos(M_PI / n_sides);
            // DT_LOG_NOTICE(0, "build_by_apothem=" << build_by_apothem << " apothem_factor="
            //            << apothem_factor);
            a_rmin /= apothem_factor;
            a_rmax /= apothem_factor;
          }
          this->add(a_z, a_rmin, a_rmax, false);
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
        this->initialize(datafile, dc_mode);
      } else {
        DT_THROW_IF (true, std::logic_error, "Invalid build mode '" << build_mode_label << "' !");
      }
    }
    lock();
    return;
  }


  void polyhedra::initialize(const std::string & filename_, int dc_mode_)
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
    bool   build_by_apothem = false;

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

    while (! ifs.eof()) {
      std::string line;
      std::getline(ifs, line);
      count++;
      {
        std::istringstream iss(line);
        std::string word;
        iss >> word;
        if (word.empty()) continue;
        if (word[0] == '#') {
          if (word.size() >= 2) {
            if (word == "#@sides") {
              int ns;
              iss >> ns;
              DT_THROW_IF(! iss, std::logic_error,
                          "Invalid format for the number of sides directive in data file '"
                          << filename << "' at line " << count << " !");
              DT_THROW_IF(ns < (int)MIN_NUMBER_OF_SIDES, std::domain_error,
                          "Number of sides is not large enough in data file '"
                          << filename << "' at line " << count << " !");
              const size_t nsides = (size_t) ns;
              set_n_sides(nsides);
            } else if (word == "#@length_unit") {
              std::string unit_str;
              iss >> unit_str;
              DT_THROW_IF(! iss, std::logic_error,
                          "Invalid format for the length unit directive in data file '"
                          << filename << "' at line " << count << " !");
              length_unit = datatools::units::get_length_unit_from(unit_str);
            } else if (word == "#@ignore_rmin") {
              ignore_rmin = true;
            } else if (word == "#@z_factor") {
              iss >> z_factor;
              DT_THROW_IF (! iss, std::logic_error,
                           "Invalid format for the Z-factor directive in data file '"
                           << filename << "' at line " << count << " !");
            } else if (word == "#@r_factor") {
              iss >> r_factor;
              DT_THROW_IF(! iss, std::logic_error,
                          "Invalid format for the R-factor directive in data file '"
                          << filename << "' at line " << count << " !");
            } else if (word == "#@build_by_apothem") {
              build_by_apothem = true;
            }
          }
          continue;
        }
      }
      // parse (z,r1) or (z,r1,r2) formated lines:
      {
        std::istringstream iss(line);
        double z, r1, r2;
        datatools::invalidate(z);
        datatools::invalidate(r1);
        datatools::invalidate(r2);
        iss >> z;
        DT_THROW_IF (! iss, std::logic_error,
                     "Format error for 'z' in data file '" << filename << "' at line " << count << " !");
        iss >> r1;
        DT_THROW_IF(! iss, std::logic_error,
                    "Format error for 'r1' in data file '" << filename << "' at line " << count << " !");
        // try to read a third column:
        std::string token;
        iss >> token;
        if (token.length() == 0) {
          // two columns format:
          r2 = r1;
          datatools::invalidate(r1);
        } else {
          if (token[0] == '#') {
            // if line ends with a comment: this is two columns format !
            r2 = r1;
            datatools::invalidate(r1);
          } else {
            // try three columns format (z, r1, r2):
            std::istringstream iss2(token);
            iss2 >> r2;
            DT_THROW_IF (! iss2, std::logic_error,
                         "Format error for 'r2' in data file '"
                         << filename << "' at line " << count << " !");
            if (ignore_rmin) {
              datatools::invalidate(r1);
            }
          }
        }
        DT_THROW_IF(datatools::is_valid(r2) && (r2 < 0.0), std::logic_error,
                    "Invalid value '" << r2 << "' for '2' in data file '"
                    << filename << "' at line " << count << " !");
        double apothem_factor = 1.0;
        if (build_by_apothem) {
          apothem_factor = std::cos(M_PI / get_n_sides());
        }
        const double tz  = z  * z_factor * length_unit;
        const double tr1 = r1 * r_factor * length_unit / apothem_factor;
        const double tr2 = r2 * r_factor * length_unit / apothem_factor;
        if (datatools::is_valid(r1)) {
          if (rmin_as_rmax) {
            this->add(tz, 0.0, tr1, false);
          } else {
            this->add(tz, tr1, tr2, false);
          }
        } else {
          this->add(tz, tr2, false);
        }
      }
    }
    this->_compute_all_();
    return;
  }

  void polyhedra::_compute_misc_()
  {
    DT_THROW_IF(_points_.size() < 2, std::logic_error, "No polyhedra sections !");
    _has_top_face_    = boost::logic::indeterminate;
    _has_bottom_face_ = boost::logic::indeterminate;
    _has_inner_face_  = boost::logic::indeterminate;

    {
      // Check for top face:
      _has_top_face_ = false;
      right_polygonal_frustrum last;
      placement dummy;
      compute_frustrum(last, dummy, number_of_frustra() - 1);
      if (last.has_top_face()) {
        _has_top_face_ = true;
      }
    }

    {
      // Check for bottom face:
      _has_bottom_face_ = false;
      right_polygonal_frustrum first;
      placement dummy;
      compute_frustrum(first, dummy, 0);
      if (first.has_bottom_face()) {
        _has_bottom_face_ = true;
      }
    }

    {
      // Check for inner face:
      _has_inner_face_ = false;
      for (size_t ifrustrum = 0; ifrustrum < number_of_frustra(); ifrustrum++) {
        // Check for inner face:
        right_polygonal_frustrum fi;
        placement dummy;
        compute_frustrum(fi, dummy, ifrustrum);
        if (fi.has_inner_face()) {
          _has_inner_face_ = true;
        }
      }
    }

    return;
  }

  void polyhedra::_compute_all_()
  {
    _compute_misc_();
    _compute_surfaces_();
    _compute_volume_();
    _compute_limits_();
    _computed_ = true;
    return;
  }

  void polyhedra::add(double z_, double rmax_, bool compute_)
  {
    DT_THROW_IF(rmax_ < 0.0, std::domain_error, "Invalid negative 'rmax' !");
    r_min_max RMM;
    RMM.rmin = 0.0;
    RMM.rmax = rmax_;
    _points_[z_] = RMM;
    if (_points_.size() > 1) {
      if (compute_) _compute_all_();
    }
    return;
  }

  void polyhedra::add(double z_, double rmin_,  double rmax_, bool compute_)
  {
    DT_THROW_IF(rmin_ < 0.0, std::domain_error, "Invalid negative 'rmin' !");
    DT_THROW_IF(rmax_ < rmin_, std::domain_error, "Invalid value for 'rmax' !");
    r_min_max RMM;
    if (rmin_ > 0.0) _extruded_ = true;
    RMM.rmin = rmin_;
    RMM.rmax = rmax_;
    _points_[z_] = RMM;
    if (_points_.size() > 1) {
      if (compute_) _compute_all_();
    }
    return;
  }

  bool polyhedra::is_valid() const
  {
    return (_n_sides_ >= 3) && (_points_.size() > 1);
  }

  void polyhedra::reset()
  {
    unlock();

    _points_.clear();
    _set_defaults();

    this->i_shape_3d::reset();
    return;
  }

  void polyhedra::_compute_limits_()
  {
    if (! is_valid()) return;
    _z_min_ = _z_max_ = _r_max_ = _xy_max_ = std::numeric_limits<double>::quiet_NaN();
    double max_radius = std::numeric_limits<double>::quiet_NaN();
    for (rz_col_type::const_iterator i = _points_.begin();
         i != _points_.end();
         i++) {
      const double z    = i->first;
      const double rmax = i->second.rmax;
      if (! datatools::is_valid(_z_min_)) {
        _z_min_ = z;
      } else if (z < _z_min_) {
        _z_min_ = z;
      }
      if (! datatools::is_valid(_z_max_)) {
        _z_max_ = z;
      } else if (z > _z_max_) {
        _z_max_ = z;
      }
      if (! datatools::is_valid(max_radius)) {
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
      const double xs  = _r_max_ * std::cos(theta);
      const double ys  = _r_max_ * std::sin(theta);
      const double axs = std::abs(xs);
      const double ays = std::abs(ys);
      if (! datatools::is_valid(_xy_max_)) {
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

  vector_3d polyhedra::get_corner(int zplane_index_,
                                  int corner_index_,
                                  bool inner_) const
  {
    vector_3d corner;
    geomtools::invalidate(corner);
    DT_THROW_IF((zplane_index_ < 0)  || (zplane_index_ > (int)_points_.size()),
                std::domain_error,
                "Invalid Z-plane index (" << zplane_index_ << ") !");
    DT_THROW_IF((corner_index_ < 0)  || (corner_index_ > (int)_n_sides_),
                std::domain_error,
                "Invalid corner index (" << corner_index_ << ") !");
    int zcount = 0;
    rz_col_type::const_iterator i = _points_.begin();
    for (; i != _points_.end(); i++) {
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
    const double x = r * std::cos(corner_index_ * delta_phi);
    const double y = r * std::sin(corner_index_ * delta_phi);
    corner.set(x, y, z);

    return corner;
  }

  void polyhedra::_compute_surfaces_()
  {
    if (! is_valid()) return;

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
        const double a1 = 2 * rmax0 * std::sin (angle);
        const double a2 = 2 * rmax1 * std::sin (angle);
        const double a1_2 = a1 * a1;
        const double a2_2 = a2 * a2;
        const double h = std::abs (z1 - z0);
        const double A = 0.25
          * M_PI
          * (
             (a1_2 + a2_2) / std::tan (angle)
             +
             std::sqrt ((a1_2 - a2_2) * (a1_2 - a2_2) / (std::cos (angle) * std::cos (angle))
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
        const double a1 = 2 * rmin0 * std::sin (angle);
        const double a2 = 2 * rmin1 * std::sin (angle);
        const double a1_2 = a1 * a1;
        const double a2_2 = a2 * a2;
        const double h = std::abs (z1 - z0);
        const double A = 0.25
          * M_PI
          * (
             (a1_2 + a2_2) / std::tan(angle)
             +
             std::sqrt((a1_2 - a2_2) * (a1_2 - a2_2) / (std::cos(angle) *std::cos(angle))
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
        const double a1 = 2 * rmax0 * std::sin (angle);
        const double a2 = 2 * rmax1 * std::sin (angle);
        const double h = std::abs (z1 - z0);
        const double V = n * h * (a1 * a1 + a2 * a2 + a1 * a2) / std::tan (angle) / 12;
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
        const double a1 = 2 * rmin0 * std::sin (angle);
        const double a2 = 2 * rmin1 * std::sin (angle);
        const double h = std::abs (z1 - z0);
        const double V = n * h * (a1 * a1 + a2 * a2 + a1 * a2) / std::tan (angle) / 12;
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
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
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
    ip_._compute_all_();
    return;
  }

  void polyhedra::compute_outer_polyhedra(polyhedra & op_)
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    op_.reset ();
    op_.set_n_sides (this->get_n_sides ());
    for (polyhedra::rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      const double z = i->first;
      const double rmax = i->second.rmax;
      op_.add (z, 0.0, rmax, false);
    }
    op_._compute_all_();
    return;
  }

  void polyhedra::compute_inflated(polyhedra & inflated_,
                                   double by_r_,
                                   double by_z_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polyhedra!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    bool apply_inflated_z = false;
    if (z_eps > 0.0) {
      apply_inflated_z = true;
    }
    inflated_.reset();
    inflated_.set_n_sides(this->get_n_sides());
    size_t sz = _points_.size();
    std::size_t counter = 0;
    double maxR = -1.0;
    for (polyhedra::rz_col_type::const_iterator i = _points_.begin();
         i != _points_.end();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      double rmax = i->second.rmax;
      rmax += r_eps;
      if (datatools::is_valid(rmin) and rmin >= 0.0) {
        rmin -= r_eps;
      } else {
        rmin = 0.0;
      }
      if (maxR < 0.0 or rmax > maxR) {
        maxR = rmax;
      }
      if (rmax < 0.0) rmax = 0.0;
      if (rmin < 0.0) rmin = 0.0;
      if (apply_inflated_z) {
        if (counter == 0) {
          if (rmin < rmax) {
            inflated_.add(z - z_eps, rmin, rmax, false);
          } else {
            inflated_.add(z - z_eps, i->second.rmin, i->second.rmax, false);
          }
        }
      }
      inflated_.add(z, rmin, rmax, false);
      if (apply_inflated_z) {
        if (counter + 1 == sz) {
          if (rmin < rmax) {
            inflated_.add(z + z_eps, rmin, rmax, false);
          }else {
            inflated_.add(z + z_eps, i->second.rmin, i->second.rmax, false);
          }
        }
      }
      counter++;
    }
    inflated_._compute_all_();
    inflated_.lock();
    return;
  }

  void polyhedra::compute_deflated(polyhedra & deflated_,
                                   double by_r_,
                                   double by_z_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polyhedra!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    bool apply_tolerance_z = false;
    if (z_eps > 0.0) {
      apply_tolerance_z = true;
    }
    deflated_.reset();
    deflated_.set_n_sides(this->get_n_sides());
    size_t sz = _points_.size();
    std::size_t counter = 0;
    bool skip_next = true;
    double maxR = -1.0;
    for (polyhedra::rz_col_type::const_iterator i = _points_.begin();
         i != _points_.end();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      double rmax = i->second.rmax;
      rmax -= r_eps;
      if (datatools::is_valid(rmin) and rmin > 0.0) {
        rmin += r_eps;
      } else {
        rmin = 0.0;
      }
      if (maxR < 0.0 or rmax > maxR) {
        maxR = rmax;
      }
      if (rmax < 0.0) rmax = 0.0;
      if (rmin < 0.0) rmin = 0.0;
      if (rmax < rmin) {
        double rmed = 0.5 * (rmax + rmin);
        rmax = rmed;
        rmin = rmed;
      }
      double z0 = z;
      if (apply_tolerance_z) {
        if (skip_next) {
          z0 += z_eps;
          auto j = i;
          j++;
          double z1 = j->first;
          if (z0 <= z1) skip_next = false;
        }
      }
      deflated_.add(z0, rmin, rmax, false);
      counter++;
      if (counter == sz / 2) {
        break;
      }
    }
    counter = 0;
    skip_next = true;
    for (polyhedra::rz_col_type::const_reverse_iterator i = _points_.rbegin ();
         i != _points_.rend ();
         i++) {
      double z = i->first;
      double rmin = i->second.rmin;
      double rmax = i->second.rmax;
      rmax -= r_eps;
      if (datatools::is_valid(rmin)) {
        rmin -= r_eps;
      } else {
        rmin = 0.0;
      }
      if (maxR < 0.0 or rmax > maxR) {
        maxR = rmax;
      }
      if (rmax < 0.0) rmax = 0.0;
      if (rmin < 0.0) rmin = 0.0;
      double z0 = z;
      if (apply_tolerance_z) {
        if (skip_next) {
          z0 -= z_eps;
          auto j = i;
          j++;
          double z1 = j->first;
          if (z0 <= z1) skip_next = false;
        }
      }
      deflated_.add(z0, rmin, rmax, false);
      counter++;
      if (counter > sz / 2) {
        break;
      }
    }
    deflated_._compute_all_();
    deflated_.lock();
    return;
  }

  void polyhedra::compute_envelope(polyhedra & envelope_,
                                  double r_tolerance_,
                                  double z_tolerance_)
  {
    compute_inflated(envelope_, r_tolerance_, z_tolerance_);
    return;
  }

  /*
  void polyhedra::compute_envelope(polyhedra & op_,
                                   double r_tolerance_,
                                   double z_tolerance_)
  {
    op_.reset ();
    op_.set_n_sides (this->get_n_sides());
    double r_eps = 0.0;
    double z_eps = 0.0;
    bool apply_tolerance_z = false;
    // bool apply_tolerance_r = false;
    if (datatools::is_valid(r_tolerance_) and r_tolerance_ > 0.0) {
      r_eps = r_tolerance_;
      // apply_tolerance_r = true;
    }
    if (datatools::is_valid(z_tolerance_) and z_tolerance_ > 0.0) {
      z_eps = z_tolerance_;
      apply_tolerance_z = true;
    }
    std::size_t counter = 0;
    for (polyhedra::rz_col_type::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) {
      double z = i->first;
      double rmax = i->second.rmax;
      rmax += r_eps;
      if (apply_tolerance_z and counter == 0) {
        op_.add(z - z_eps, 0.0, rmax, false);
      }
      op_.add(z, 0.0, rmax, false);
      if (apply_tolerance_z and (counter + 1) == _points_.size()) {
        op_.add(z + z_eps, 0.0, rmax, false);
      }
      counter++;
    }
    op_._compute_all_();
    op_.lock();
    return;
  }
  */
  
  double polyhedra::get_surface (uint32_t mask_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    double s = 0.0;
    int mask = mask_;
    if (has_inner_face()) {
      if (mask & FACE_INNER_SIDE) {
        s += _inner_side_surface_;
      }
    }
    if (mask & FACE_OUTER_SIDE) {
      s += _outer_side_surface_;
    }
    if (has_bottom_face()) {
      if (mask & FACE_BOTTOM) {
        s += _bottom_surface_;
      }
    }
    if (has_top_face()) {
      if (mask & FACE_TOP) {
        s += _top_surface_;
      }
    }
    return s;
  }

  double polyhedra::get_volume(uint32_t /*flags*/) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    return _volume_;
  }

  double polyhedra::get_parameter (const std::string & flag_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
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

  bool polyhedra::is_outside(const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polyhedra!");
    unsigned int noutside = 0;
    double skin =  get_skin(skin_);
    // Scan the conical frustra:
    for (unsigned int i = 0; i < number_of_frustra(); i++) {
      right_polygonal_frustrum rpf;
      placement rpf_placement;
      compute_frustrum(rpf, rpf_placement, i);
      vector_3d pos_c;
      rpf_placement.mother_to_child(point_, pos_c);
      if (rpf.is_outside(pos_c, skin)) {
        noutside++;
      }
    }
    return noutside == number_of_frustra();
  }

  bool polyhedra::is_inside (const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;

    double z = point_.z();
    // Exclude points outside the bounding cylinder:
    if (z > get_z_max() - hskin) return false;
    if (z < get_z_min() + hskin) return false;
    const double r = hypot(point_.x(), point_.y());
    if (r > get_r_max() - hskin) return false;

    bool inside = false;
    // Scan the conical frustra:
    for (unsigned int i = 0; i < number_of_frustra(); i++) {
      right_polygonal_frustrum rpf;
      placement rpf_placement;
      compute_frustrum(rpf, rpf_placement, i);
      vector_3d pos_c;
      rpf_placement.mother_to_child(point_, pos_c);
      if (rpf.is_inside(pos_c, 0.0)) {
        // std::cerr << "DEVEL: polyhedra::is_inside: " << "in frustrum #" << i << std::endl;
        face_identifier mask_id;
        uint32_t face_bits = 0;
        face_bits |= right_polygonal_frustrum::FACE_OUTER_SIDE;
        if (rpf.has_inner_face()) {
          face_bits |= right_polygonal_frustrum::FACE_INNER_SIDE;
        }
        if (i == 0 && rpf.has_top_face()) {
          face_bits |= right_polygonal_frustrum::FACE_TOP;
        }
        if (i == number_of_frustra() - 1 && rpf.has_bottom_face()) {
          face_bits |= right_polygonal_frustrum::FACE_BOTTOM;
        }
        mask_id.set_face_bits(face_bits);
        face_identifier surf_id = rpf.on_surface(point_, mask_id, skin);
        if (! surf_id.is_valid()) {
          inside = true;
        }
        break;
      }
    }

    return inside;
  }

  vector_3d polyhedra::get_normal_on_surface (const vector_3d & position_,
                                              const face_identifier & surface_bit_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    vector_3d normal;
    invalidate(normal);
    switch(surface_bit_.get_face_bits()) {
    case FACE_BOTTOM:
      if (has_bottom_face()) {
        normal.set(0.0, 0.0, -1.0);
      }
      break;
    case FACE_TOP:
      if (has_top_face()) {
        normal.set(0.0, 0.0, +1.0);
      }
      break;
    case FACE_OUTER_SIDE:
      // XXX
      for (size_t i = 0; i < number_of_frustra(); i++) {
        composite_surface outer;
        placement pl_outer;
        compute_outer_face(outer, pl_outer, i);
        vector_3d position_c;
        pl_outer.mother_to_child(position_, position_c);
        vector_3d normal_c;
        normal_c = outer.get_normal_on_surface(position_c, false);
        pl_outer.child_to_mother_direction(normal_c, normal);
      }
      break;
    case FACE_INNER_SIDE:
      // XXX
      if (has_inner_face()) {
        for (size_t i = 0; i < number_of_frustra(); i++) {
          composite_surface inner;
          placement pl_inner;
          compute_inner_face(inner, pl_inner, i);
          if (inner.is_valid()) {
            vector_3d position_c;
            pl_inner.mother_to_child(position_, position_c);
            vector_3d normal_c;
            normal_c = -inner.get_normal_on_surface(position_c, false);
            pl_inner.child_to_mother_direction(normal_c, normal);
          }
        }
      }
      break;
    }
    return normal;
  }

  // virtual
  face_identifier polyhedra::on_surface(const vector_3d & position_,
                                        const face_identifier & surface_mask_,
                                        double skin_) const
  {
    // std::cerr << "DEVEL: polyhedra::on_surface: Entering..." << std::endl;
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polyhedra!");
    double skin = get_skin(skin_);

    face_identifier mask;
    if (surface_mask_.is_valid()) {
      DT_THROW_IF(! surface_mask_.is_face_bits_mode(), std::logic_error,
                  "Face mask uses no face bits!");
      mask = surface_mask_;
    } else {
      mask.set_face_bits_any();
    }
    // std::cerr << "DEVEL: polyhedra::on_surface: mask=" << mask << std::endl;

    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (finfo.is_valid() && mask.has_face_bit(finfo.get_identifier().get_face_bits())) {
        // std::cerr << "DEVEL: polyhedra::on_surface: check face ref='"
        //           << finfo.get_face_ref().get_shape_name() << "'"
        //           << " with id=" << finfo.get_identifier() << std::endl;
        vector_3d position_c;
        finfo.get_positioning().mother_to_child(position_, position_c);
        if (finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
        // std::cerr << "DEVEL: polyhedra::on_surface: pass face id=" << finfo.get_identifier() << std::endl;
      }
    }

    return face_identifier::face_invalid();
  }

  bool polyhedra::find_intercept (const vector_3d & from_,
                                  const vector_3d & direction_,
                                  face_intercept_info & intercept_,
                                  double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polyhedra!");
    intercept_.reset();

    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 4;
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

  void polyhedra::generate_wires_self(wires_type & wires_,
                                      uint32_t options_) const
  {
    DT_THROW_IF (! is_valid (), std::logic_error, "Polyhedra is not valid !");
    bool devel = false;
    // devel = true;
    if (devel) std::cerr << "polyhedra::generate_wires_self: Entering..." << std::endl;

    bool draw_bottom      = !(options_ & WR_POLYHEDRA_NO_BOTTOM_FACE);
    bool draw_top         = !(options_ & WR_POLYHEDRA_NO_TOP_FACE);
    bool draw_inner       = !(options_ & WR_POLYHEDRA_NO_INNER_FACE);
    bool draw_outer       = !(options_ & WR_POLYHEDRA_NO_OUTER_FACE);
    bool draw_boundings   =  (options_ & WR_BASE_BOUNDINGS);

    // draw_boundings=true;
    if (draw_boundings && has_bounding_data()) {
      if (devel) std::cerr << "polyhedra::generate_wires_self: draw boundings..." << std::endl;
      get_bounding_data().generate_wires_self(wires_, 0);
      if (devel) std::cerr << "polyhedra::generate_wires_self: draw done." << std::endl;
    }

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    if (has_bottom_face() && draw_bottom) {
      if (devel) std::cerr << "polyhedra::generate_wires_self: draw bottom..." << std::endl;
      composite_surface bottom_face;
      placement bottom_pl;
      compute_bottom_face(bottom_face, bottom_pl);
      if (bottom_face.is_valid()) {
        uint32_t options = base_options;
        bottom_face.generate_wires(wires_, bottom_pl, options);
      }
      if (devel) std::cerr << "polyhedra::generate_wires_self: draw bottom done." << std::endl;
    }

    if (has_top_face() && draw_top) {
      if (devel) std::cerr << "polyhedra::generate_wires_self: draw top..." << std::endl;
      composite_surface top_face;
      placement top_pl;
      compute_top_face(top_face, top_pl);
      if (top_face.is_valid()) {
        uint32_t options = base_options;
        top_face.generate_wires(wires_, top_pl, options);
      }
      if (devel) std::cerr << "polyhedra::generate_wires_self: draw top done." << std::endl;
    }

    if (has_inner_face() && draw_inner) {
      if (devel) std::cerr << "polycone::generate_wires_self: draw inner..." << std::endl;
      composite_surface inner;
      placement pl_inner;
      compute_inner_face(inner, pl_inner);
      if (inner.is_valid()) {
        uint32_t options = base_options;
        inner.generate_wires(wires_, pl_inner, options);
      }
      if (devel) std::cerr << "polyhedra::generate_wires_self: draw inner done." << std::endl;
    }

    if (draw_outer) {
      if (devel) std::cerr << "polycone::generate_wires_self: draw outer..." << std::endl;
      composite_surface outer;
      placement pl_outer;
      compute_outer_face(outer, pl_outer);
      if (outer.is_valid()) {
        uint32_t options = base_options;
        outer.generate_wires(wires_, pl_outer, options);
      }
      if (devel) std::cerr << "polyhedra::generate_wires_self: draw outer done." << std::endl;
    }

    return;
  }

} // end of namespace geomtools
