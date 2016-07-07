// bounding_data.cc

// Ourselves:
#include <geomtools/bounding_data.h>

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/min_max.h>
// - Bayeux/geomtools:
#include <geomtools/box.h>
#include <geomtools/placement.h>

namespace geomtools {

  bool bounding_data::is_valid(bool verbose_) const
  {
    if (_type_ == BD_INVALID) {
      if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD: Invalid type!");
      return false;
    }

    if (_type_ == BD_BOX) {
      if (!datatools::is_valid(_xmin_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid X min!");
        return false;
      }
      if (!datatools::is_valid(_xmax_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid X max!");
        return false;
      }
      if (_xmin_ >= _xmax_) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid X min >= X max!");
        return false;
      }
      if (!datatools::is_valid(_ymin_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid Y min!");
        return false;
      }
      if (!datatools::is_valid(_ymax_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid Y max!");
        return false;
      }
      if (_ymin_ >= _ymax_) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid Y min >= Y max!");
        return false;
      }
      if (!datatools::is_valid(_zmin_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid Z min!");
        return false;
      }
      if (!datatools::is_valid(_zmax_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid Z max!");
        return false;
      }
      if (_zmin_ >= _zmax_) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD box: Invalid Z min >= Z max!");
        return false;
      }
    }

    if (_type_ == BD_CYLINDER) {
      if (!datatools::is_valid(_rmax_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD cylinder: Invalid R max!");
        return false;
      }
      if (_rmax_ <= 0.0) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD cylinder: Negative R max!");
        return false;
      }
      if (!datatools::is_valid(_zmin_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD cylinder: Invalid Z min!");
        return false;
      }
      if (!datatools::is_valid(_zmax_)) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD cylinder: Invalid Z max!");
        return false;
      }
      if (_zmin_ >= _zmax_) {
        if (verbose_) DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "BD cylinder: Invalid Z min >= Z max!");
        return false;
      }
    }

    return true;
  }

  double bounding_data::get_min_dimension() const
  {
    double min_dim;
    datatools::invalidate(min_dim);
    if (!is_valid()) {
      return min_dim;
    }
    mygsl::min_max mm;
    if (is_box()) {
      mm.add(std::abs(_xmax_ - _xmin_));
      mm.add(std::abs(_ymax_ - _ymin_));
      mm.add(std::abs(_zmax_ - _zmin_));
      min_dim = mm.get_min();
    }
    if (is_cylinder()) {
      mm.add(2 * _rmax_);
      mm.add(std::abs(_zmax_ - _zmin_));
      min_dim = mm.get_min();
    }
    return min_dim;
  }

  bool bounding_data::is_box() const
  {
    return _type_ == BD_BOX;
  }

  bool bounding_data::is_cylinder() const
  {
    return _type_ == BD_CYLINDER;
  }

  bounding_data::bounding_data()
  {
    reset();
    return;
  }

  bounding_data::~bounding_data()
  {
    return;
  }

  void bounding_data::reset()
  {
    _type_ = BD_INVALID;
    datatools::invalidate(_xmin_);
    datatools::invalidate(_xmax_);
    datatools::invalidate(_ymin_);
    datatools::invalidate(_ymax_);
    datatools::invalidate(_zmin_);
    datatools::invalidate(_zmax_);
    datatools::invalidate(_rmax_);
    return;
  }

  void bounding_data::make_box(double xmin_, double xmax_,
                               double ymin_, double ymax_,
                               double zmin_, double zmax_)
  {
    _type_ = BD_BOX;
    _xmin_ = xmin_;
    _xmax_ = xmax_;
    _ymin_ = ymin_;
    _ymax_ = ymax_;
    _zmin_ = zmin_;
    _zmax_ = zmax_;
    datatools::invalidate(_rmax_);
    return;
  }

  void bounding_data::make_cylinder(double rmax_, double zmin_, double zmax_)
  {
    _type_ = BD_CYLINDER;
    datatools::invalidate(_xmin_);
    datatools::invalidate(_xmax_);
    datatools::invalidate(_ymin_);
    datatools::invalidate(_ymax_);
    _rmax_ = rmax_;
    _zmin_ = zmin_;
    _zmax_ = zmax_;
    return;
  }

  double bounding_data::get_xmin() const
  {
    return _xmin_;
  }

  double bounding_data::get_xmax() const
  {
    return _xmax_;
  }

  double bounding_data::get_ymin() const
  {
    return _ymin_;
  }

  double bounding_data::get_ymax() const
  {
    return _ymax_;
  }

  double bounding_data::get_zmin() const
  {
    return _zmin_;
  }

  double bounding_data::get_zmax() const
  {
    return _zmax_;
  }

  double bounding_data::get_rmax() const
  {
    return _rmax_;
  }

  void bounding_data::make_box_from_points(const std::vector<vector_3d> & points_)
  {
    reset();
    mygsl::min_max mmx;
    mygsl::min_max mmy;
    mygsl::min_max mmz;
    for (int i = 0; i < (int) points_.size(); i++) {
      mmx.add(points_[i].x());
      mmy.add(points_[i].y());
      mmz.add(points_[i].z());
    }
    make_box(mmx.get_min(), mmx.get_max(),
             mmy.get_min(), mmy.get_max(),
             mmz.get_min(), mmz.get_max());
    return;
  }

  void bounding_data::compute_bounding_box_face_centers(std::vector<vector_3d> & face_centers_, double safe_skin_) const
  {
    face_centers_.clear();
    box bb;
    placement bbpl;
    compute_bounding_box(bb, bbpl, safe_skin_);
    vector_3d fc[6];
    fc[0].set(-0.5 * bb.get_x(), 0.0, 0.0);
    fc[1].set(+0.5 * bb.get_x(), 0.0, 0.0);
    fc[2].set(0.0, -0.5 * bb.get_y(), 0.0);
    fc[3].set(0.0, +0.5 * bb.get_y(), 0.0);
    fc[4].set(0.0, 0.0, -0.5 * bb.get_z());
    fc[5].set(0.0, 0.0, +0.5 * bb.get_z());
    face_centers_.reserve(6);
    for (int i = 0; i < 6; i++) {
      {
        vector_3d dummy;
        face_centers_.push_back(dummy);
      }
      vector_3d & last_fc = face_centers_.back();
      bbpl.child_to_mother(fc[i], last_fc);
    }
    return;
  }

  void bounding_data::build_stackable(stackable_data & stackable_info_) const
  {
    stackable_info_.invalidate();
    if (!is_valid()) return;
    if (is_box()) {
      stackable_info_.xmin = _xmin_;
      stackable_info_.xmax = _xmax_;
      stackable_info_.ymin = _ymin_;
      stackable_info_.ymax = _ymax_;
      stackable_info_.zmin = _zmin_;
      stackable_info_.zmax = _zmax_;
    } else if (is_cylinder()) {
      stackable_info_.xmin = -_rmax_;
      stackable_info_.xmax = _rmax_;
      stackable_info_.ymin = -_rmax_;
      stackable_info_.ymax = _rmax_;
      stackable_info_.zmin = _zmin_;
      stackable_info_.zmax = _zmax_;
    }
    return;
  }

  void bounding_data::compute_bounding_box_vertexes(std::vector<vector_3d> & vertexes_, double /*safe_skin_*/) const
  {
    vertexes_.clear();
    box bb;
    placement bbpl;
    compute_bounding_box(bb, bbpl);
    bb.compute_transformed_vertexes(bbpl, vertexes_);
    return;
  }

  void bounding_data::compute_bounding_box(box & bb_, placement & p_, double safe_skin_) const
  {
    bb_.reset();
    p_.reset();
    DT_THROW_IF(!is_valid(), std::logic_error, "Bounding data is not valid!");
    double dx = 0.0, dy = 0.0, dz = 0.0;
    double x0 = 0.0, y0 = 0.0, z0 = 0.0;
    if (is_box()) {
      dx = get_xmax() - get_xmin();
      dy = get_ymax() - get_ymin();
      dz = get_zmax() - get_zmin();
      x0 = 0.5 * (get_xmax() + get_xmin());
      y0 = 0.5 * (get_ymax() + get_ymin());
      z0 = 0.5 * (get_zmax() + get_zmin());
    }
    if (is_cylinder()) {
      dx = 2 * get_rmax();
      dy = 2 * get_rmax();
      dz = get_zmax() - get_zmin();
      x0 = 0.0;
      y0 = 0.0;
      z0 = 0.5 * (get_zmax() + get_zmin());
    }
    bb_.set(dx + 2 * safe_skin_, dy + 2 * safe_skin_, dz + 2 * safe_skin_);
    p_.set(x0, y0, z0, 0.0, 0.0, 0.0);
    return;
  }

  void bounding_data::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    if (! title_.empty ()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Type: " << _type_;
    if (is_box()) {
      out_ << ' ' << "(box)";
    } else if (is_cylinder()) {
      out_ << ' ' << "(cylinder)";
    }
    out_ << std::endl;
    if (is_valid()) {
      if (is_box()) {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "X min: " << _xmin_ << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "X max: " << _xmax_ << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Y min: " << _ymin_ << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Y max: " << _ymax_ << std::endl;
      }
      if (is_cylinder()) {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "R max: " << _rmax_ << std::endl;
      }
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Z min: " << _zmin_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Z max: " << _zmax_ << std::endl;
    }
    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Valid: " << is_valid() << std::endl;

    return;
  }

  void bounding_data::parse_bounding_data(const datatools::properties & config_)
  {

    bb_type type = BD_INVALID;
    if (config_.has_key("type")) {
      std::string bd_type_str = config_.fetch_string("type");
      if (bd_type_str == "box") {
        type = BD_BOX;
      } else if (bd_type_str == "cylinder") {
        type = BD_CYLINDER;
      } else {
        DT_THROW(std::logic_error, "Invalid bounding data type '" << bd_type_str << "'!");
      }
    }
    // Extract current bounding data:
    double xmin = _xmin_;
    double xmax = _xmax_;
    double ymin = _ymin_;
    double ymax = _ymax_;
    double zmin = _zmin_;
    double zmax = _zmax_;
    double rmax = _rmax_;
    // datatools::invalidate(xmin);
    // datatools::invalidate(xmax);
    // datatools::invalidate(ymin);
    // datatools::invalidate(ymax);
    // datatools::invalidate(zmin);
    // datatools::invalidate(zmax);
    // datatools::invalidate(rmax);

    double default_length_unit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string length_unit_str = config_.fetch_string("length_unit");
      default_length_unit = datatools::units::get_length_unit_from(length_unit_str);
    }

    if (type == BD_BOX && config_.has_key("xmin")) {
      xmin = config_.fetch_real("xmin");
      if (! config_.has_explicit_unit ("xmin")) {
        xmin *= default_length_unit;
      }
    }

    if (type == BD_BOX && config_.has_key("xmax")) {
      xmax = config_.fetch_real("xmax");
      if (! config_.has_explicit_unit ("xmax")) {
        xmax *= default_length_unit;
      }
    }

    if (type == BD_BOX && config_.has_key("ymin")) {
      ymin = config_.fetch_real("ymin");
      if (! config_.has_explicit_unit ("ymin")) {
        ymin *= default_length_unit;
      }
    }

    if (type == BD_BOX && config_.has_key("ymax")) {
      ymax = config_.fetch_real("ymax");
      if (! config_.has_explicit_unit ("ymax")) {
        ymax *= default_length_unit;
      }
    }

    if (config_.has_key("zmin")) {
      zmin = config_.fetch_real("zmin");
      if (! config_.has_explicit_unit ("zmin")) {
        zmin *= default_length_unit;
      }
    }

    if (config_.has_key("zmax")) {
      zmax = config_.fetch_real("zmax");
      if (! config_.has_explicit_unit ("zmax")) {
        zmax *= default_length_unit;
      }
    }

    if (type == BD_CYLINDER && config_.has_key("rmax")) {
      rmax = config_.fetch_real("rmax");
      if (! config_.has_explicit_unit ("rmax")) {
        rmax *= default_length_unit;
      }
    }

    if (type == BD_CYLINDER) {
      make_cylinder(rmax, zmin, zmax);
    }

    if (type == BD_BOX) {
      make_box(xmin, xmax, ymin, ymax, zmin, zmax);
    }

    return;
  }

  void bounding_data::generate_wires_self(wires_type & wires_,
                                          uint32_t /*options_*/) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid bounding data!");

    box bb;
    placement bb_pl;
    compute_bounding_box(bb, bb_pl);

    if (bb.is_valid()) {
      uint32_t options = 0;
      bb.generate_wires(wires_, bb_pl, options);
    }
    return;
  }

} // end of namespace geomtools
