/// regular_circular_placement.cc

// Ourselves:
#include <geomtools/regular_circular_placement.h>

// Standard library:
#include <stdexcept>
#include <sstream>

namespace geomtools {

  bool regular_circular_placement::is_valid() const
  {
    return _number_of_items_ > 0
      && datatools::is_valid(_step_angle_)
      && datatools::is_valid(_start_angle_)
      && datatools::is_valid(_radius_)
      && geomtools::is_valid(_center_)
      && _rotation_axis_ != ROTATION_AXIS_INVALID
      ;
  }

  void regular_circular_placement::invalidate()
  {
    _set_default();
    return;
  }

  void regular_circular_placement::_set_default()
  {
    datatools::invalidate(_radius_);
    datatools::invalidate(_step_angle_);
    datatools::invalidate(_start_angle_);
    geomtools::invalidate(_center_);
    _number_of_items_ = 0;
    _rotation_axis_ = ROTATION_AXIS_INVALID;
    _follow_rotation_ = true;
    return;
  }

  size_t regular_circular_placement::get_dimension() const
  {
    return 1;
  }

  bool regular_circular_placement::is_replica () const
  {
    // This placement is not considered as a (Geant4/GDML)
    // replica because we cannot ensure that no free space exists
    // between replicated daughter volumes:
    return false;
  }

  bool regular_circular_placement::has_only_one_rotation () const
  {
    return false;
  }

  void regular_circular_placement::set_rotation_axis (rotation_axis_type axis_)
  {
    DT_THROW_IF (axis_ == ROTATION_AXIS_INVALID,
                 std::logic_error,
                 "Invalid rotation axis !");
    // DT_THROW_IF (axis_ != ROTATION_AXIS_Z,
    //              std::logic_error,
    //              "Only Z-axis is supported for now !");
    _rotation_axis_ = axis_;
    return;
  }

  rotation_axis_type
  regular_circular_placement::get_rotation_axis() const
  {
    return _rotation_axis_;
  }

  bool regular_circular_placement::is_rotation_x_axis() const
  {
    return _rotation_axis_ == ROTATION_AXIS_X;
  }

  bool regular_circular_placement::is_rotation_y_axis() const
  {
    return _rotation_axis_ == ROTATION_AXIS_Y;
  }

  bool regular_circular_placement::is_rotation_z_axis() const
  {
    return _rotation_axis_ == ROTATION_AXIS_Z;
  }

  void regular_circular_placement::set_follow_rotation(bool fr_)
  {
    _follow_rotation_ = fr_;
    return;
  }

  bool regular_circular_placement::is_follow_rotation() const
  {
    return _follow_rotation_;
  }

  void regular_circular_placement::set_radius(double radius_)
  {
    _radius_ = radius_;
    return;
  }

  double regular_circular_placement::get_radius() const
  {
    return _radius_;
  }

  void regular_circular_placement::set_start_angle(double sa_)
  {
    _start_angle_ = sa_;
    return;
  }

  double regular_circular_placement::get_start_angle() const
  {
    return _start_angle_;
  }

  void regular_circular_placement::set_step_angle(double sa_)
  {
    DT_THROW_IF(sa_ == 0.0, std::logic_error,
                "Invalid step angle!");
    _step_angle_ = sa_;
    return;
  }

  double regular_circular_placement::get_step_angle() const
  {
    return _step_angle_;
  }

  void regular_circular_placement::set_center(const vector_3d & center_)
  {
    _center_ = center_;
    return;
  }

  const vector_3d & regular_circular_placement::get_center() const
  {
    return _center_;
  }

  void regular_circular_placement::set_number_of_items(size_t n_)
  {
    _number_of_items_ = n_;
  }

  size_t regular_circular_placement::get_number_of_items() const
  {
    return _number_of_items_;
  }

  size_t regular_circular_placement::compute_index_map(std::vector<uint32_t> & map_,
                                                       int item_) const
  {
    DT_THROW_IF ((item_ < 0) || (item_ >= (int)get_number_of_items ()),
                 std::domain_error,
                 "Invalid item index '" << item_ << "' !");
    map_.clear();
    map_.push_back(item_);
    return map_.size();
  }

  void regular_circular_placement::get_placement(int item_, placement & p_) const
  {
    p_.invalidate();
    p_.set_orientation_zyz(0, 0, 0);
    double angle = _start_angle_ + item_ * _step_angle_;
    double x0 = _radius_ * std::cos(angle);
    double x1 = _radius_ * std::sin(angle);
    double x = _center_.x();
    double y = _center_.y();
    double z = _center_.z();
    if (is_rotation_x_axis()) {
      y += x0;
      z += x1;
      if (_follow_rotation_) {
        p_.set_orientation_axis_angle(ROTATION_AXIS_X, angle);
      }
    } else  if (is_rotation_y_axis()) {
      z += x0;
      x += x1;
      if (_follow_rotation_) {
        p_.set_orientation_axis_angle(ROTATION_AXIS_Y, angle);
      }
    } else  if (is_rotation_z_axis()) {
      x += x0;
      y += x1;
      if (_follow_rotation_) {
        p_.set_orientation_axis_angle(ROTATION_AXIS_Z, angle);
      }
    }
    vector_3d trans(x, y, z);
    p_.set_translation(trans);
    return;
  }

  regular_circular_placement::regular_circular_placement() : i_placement ()
  {
    _set_default();
    return;
  }

  regular_circular_placement::regular_circular_placement(const vector_3d & center_,
                                                         double radius_,
                                                         double step_angle_,
                                                         double start_angle_,
                                                         size_t number_of_items_,
                                                         rotation_axis_type rotation_axis_) : i_placement ()
  {
    invalidate();
    initialize (center_,
                radius_,
                step_angle_,
                start_angle_,
                number_of_items_,
                rotation_axis_);
    return;
  }

  regular_circular_placement::~regular_circular_placement()
  {
    return;
  }

  void regular_circular_placement::initialize(const vector_3d & center_,
                                              double radius_,
                                              double step_angle_,
                                              double start_angle_,
                                              size_t number_of_items_,
                                              rotation_axis_type rotation_axis_)
  {
    set_center(center_);
    set_number_of_items(number_of_items_);
    set_radius(radius_);
    set_step_angle(step_angle_);
    set_start_angle(start_angle_);
    set_rotation_axis(rotation_axis_);
    return;
  }


  void regular_circular_placement::reset()
  {
    invalidate();
    return;
  }

  void regular_circular_placement::tree_dump(std::ostream & out_,
                                             const std::string & title_,
                                             const std::string & indent_,
                                             bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_placement::tree_dump (out_, title_, indent, true);

    {
      out_ << indent << datatools::i_tree_dumpable::tag << "Center : "
           << _center_ / CLHEP::mm << " mm"
           << std::endl;
      // std::ostringstream oss_indent;
      // oss_indent << indent << datatools::i_tree_dumpable::skip_tag;
      // _basic_placement_.tree_dump (out_,
      //                              "",
      //                              oss_indent.str ());
    }

    out_ << indent << datatools::i_tree_dumpable::tag << "Radius : "
         << _radius_ / CLHEP::mm << " mm"  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag << "Step angle : "
         << _step_angle_ / CLHEP::degree << " degree"  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag << "Start angle : "
         << _start_angle_ / CLHEP::degree << " degree"  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag << "Number of items : "
         << _number_of_items_  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Replica : " << is_replica ();
    if (is_rotation_x_axis ()) {
      out_ << " around [X]";
    }
    if (is_rotation_y_axis ()) {
      out_ << " around [Y]";
    }
    if (is_rotation_z_axis ()) {
      out_ << " around [Z]";
    }
    out_ << std::endl;

    return;
  }

} // end of namespace geomtools
