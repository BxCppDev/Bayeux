// i_composite_shape_3d.cc

// Ourselves:
#include <geomtools/i_composite_shape_3d.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/utils.h>

namespace geomtools {

  using namespace std;

  // static
  const uint32_t i_composite_shape_3d::FIRST_PART;
  const uint32_t i_composite_shape_3d::SECOND_PART;

  i_composite_shape_3d::shape_type::~shape_type()
  {
    reset();
    return;
  }

  void
  i_composite_shape_3d::shape_type::reset()
  {
    if (_shape_) {
      _shape_.reset();
    }
    if (_shape_ext_ != nullptr) {
      _shape_ext_ = nullptr;
    }
    _shape_label_.clear();
    _placement_.reset();
    return;
  }

  bool
  i_composite_shape_3d::shape_type::is_valid() const
  {
    if ((! bool(_shape_)) and _shape_ext_ == nullptr) return false;
    if (! _placement_.is_valid()) return false;
    return true;
  }

  void i_composite_shape_3d::shape_type::set_shape_label(const std::string & shlabel_)
  {
    _shape_label_ = shlabel_;
    return;
  }

  const std::string & i_composite_shape_3d::shape_type::get_shape_label() const
  {
    return _shape_label_;
  }

  const i_shape_3d &
  i_composite_shape_3d::shape_type::get_shape() const
  {
    DT_THROW_IF((! bool(_shape_)) and _shape_ext_ == nullptr, std::logic_error,
                "No shape!")
    if (_shape_) {
      return *_shape_;
    }
    return *_shape_ext_;
  }

  const placement &
  i_composite_shape_3d::shape_type::get_placement() const
  {
    return _placement_;
  }

  void i_composite_shape_3d::shape_type::set_placement(const placement & p_)
  {
    _placement_ = p_;
    return;
  }

  // static
  i_composite_shape_3d::shape_type i_composite_shape_3d::make_shape(const i_shape_3d & shape_ext_,
                                                                    const placement & p_,
                                                                    const std::string & shape_label_)
  {
    shape_type s;
    s._shape_ext_ = &shape_ext_;
    s._placement_ = p_;
    s._shape_label_ = shape_label_;
    return s;
  }

  // static
  i_composite_shape_3d::shape_type i_composite_shape_3d::make_shape(const shape_handle & shape_,
                                                                    const placement & p_,
                                                                    const std::string & shape_label_)     
 {
    shape_type s;
    s._shape_ = shape_;
    s._placement_ = p_;
    s._shape_label_ = shape_label_;
    return s;
  }

  void i_composite_shape_3d::shape_type::tree_dump(std::ostream & out_,
                                                   const std::string & title_,
                                                   const std::string & indent_,
                                                   bool inherit_) const
  {

    std::string indent = indent_;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << endl;
    }

    {
      if (_shape_ or _shape_ext_ != nullptr) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Shape type = ";
        out_ << "'" << get_shape().get_shape_name() << "' "
             << (bool(_shape_) ? "(shared)": "(external)") << endl;
        std::ostringstream oss_indent;
        oss_indent << indent << datatools::i_tree_dumpable::skip_tag;
        get_shape().tree_dump(out_,
                           "",
                           oss_indent.str(),
                           false);
      } else {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Shape type = " << "<no shape>" << endl;
      }
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Shape label = ";
    if (_shape_label_.empty()) {
      out_ << "<none>";
    } else {
      out_ << "'" << _shape_label_ << "'";
    }
    out_ << endl;

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Placement : " << endl;
      std::ostringstream oss_indent;
      oss_indent << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      _placement_.tree_dump(out_,
                            "",
                            oss_indent.str(),
                            false);
    }

    return;
  }

  /***************************************/

  i_composite_shape_3d::i_composite_shape_3d(double skin_)
  {
    set_skin(skin_);
    return;
  }

  i_composite_shape_3d::~i_composite_shape_3d()
  {
    return;
  }

  bool i_composite_shape_3d::volume_can_be_forced() const
  {
    return true;
  }

  bool i_composite_shape_3d::is_valid() const
  {
    return _shape1_.is_valid() && _shape2_.is_valid();
  }

  bool
  i_composite_shape_3d::is_composite() const
  {
    return true;
  }

  bool i_composite_shape_3d::using_face_id_bits() const
  {
    return false;
  }

  bool i_composite_shape_3d::using_face_id_part_index() const
  {
    return true;
  }

  void
  i_composite_shape_3d::set_shape1(const i_shape_3d & sx_, const placement & p_, const std::string & shlabel_)
  {
    _shape1_ = make_shape(sx_, p_, shlabel_);
    return;
  }

  void
  i_composite_shape_3d::set_shape2(const i_shape_3d & sx_, const placement & p_, const std::string & shlabel_)
  {
    _shape2_ = make_shape(sx_, p_, shlabel_);
    return;
  }

  void
  i_composite_shape_3d::set_shape1(const shape_handle & sh_, const placement & p_, const std::string & shlabel_)
  {
    _shape1_ = make_shape(sh_, p_, shlabel_);
    return;
  }

  void
  i_composite_shape_3d::set_shape2(const shape_handle & sh_, const placement & p_, const std::string & shlabel_)
  {
    _shape1_ = make_shape(sh_, p_, shlabel_);
    return;
  }

  void
  i_composite_shape_3d::set_shapes(const i_shape_3d & shape1_,
                                   const i_shape_3d & shape2_,
                                   const placement & placement2_,
                                   const std::string & sh1label_,
                                   const std::string & sh2label_)
  {
    placement p1;
    set_shape1(shape1_, p1, sh1label_);
    set_shape2(shape2_, placement2_, sh2label_);
    return;
  }

  void
  i_composite_shape_3d::set_shapes(const shape_handle & sh1_,
                                   const shape_handle & sh2_,
                                   const placement & placement2_,
                                   const std::string & sh1label_,
                                   const std::string & sh2label_)
  {
    placement p1;
    set_shape1(sh1_, p1, sh1label_);
    set_shape2(sh2_, placement2_, sh2label_);
    return;
  }

  const i_composite_shape_3d::shape_type &
  i_composite_shape_3d::get_shape1() const
  {
    return _shape1_;
  }

  const i_composite_shape_3d::shape_type &
  i_composite_shape_3d::get_shape2() const
  {
    return _shape2_;
  }

  const i_composite_shape_3d::shape_type &
  i_composite_shape_3d::get_shape(int i_) const
  {
    if (i_ == 0) return _shape1_;
    return _shape2_;
  }

  void i_composite_shape_3d::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    i_shape_3d::tree_dump(out_, title_, indent_, true);

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Shape 1 : " << endl;
      std::ostringstream oss_indent;
      oss_indent << indent << i_tree_dumpable::skip_tag;
      get_shape1().tree_dump(out_,
                             "",
                             oss_indent.str(),
                             false);
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Shape 2 : " << endl;
      std::ostringstream oss_indent;
      oss_indent << indent << i_tree_dumpable::inherit_skip_tag(inherit_);
      get_shape2().tree_dump(out_,
                             "",
                             oss_indent.str(),
                             false);
    }

    return;
  }

  void i_composite_shape_3d::reset()
  {
    unlock();

    _shape1_.reset();
    _shape2_.reset();

    this->i_shape_3d::_reset();
    return;
  }

  void i_composite_shape_3d::initialize(const datatools::properties & config_,
                                        const i_object_3d::handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);

    if (!is_valid()) {

      if (! _shape1_.is_valid() || ! _shape1_.is_valid()) {
        DT_THROW_IF (!objects_,
                     std::logic_error,
                     "Missing dictionary of 3D objects!");
      }

      if (! _shape1_.is_valid()) {
        std::string sname;
        if (sname.empty() && config_.has_key("shape_1.name")) {
          sname = config_.fetch_string("shape_1.name");
        }
        if (sname.empty() && config_.has_key("first_shape.name")) {
          sname = config_.fetch_string("first_shape.name");
        }
        DT_THROW_IF (sname.empty(),
                     std::logic_error,
                     "Missing first shape's name!");

        DT_THROW_IF (objects_->find(sname) == objects_->end(),
                     std::logic_error,
                     "Cannot find first shape with name '" << sname << "'!");
        const i_object_3d::object_entry & oe = objects_->find(sname)->second;
        DT_THROW_IF (!oe.has_object(),
                     std::logic_error,
                     "3D object named '" << sname << "' is not valid!");
        const i_shape_3d & sh = dynamic_cast<const i_shape_3d &>(oe.get_object());
        placement pl;
        pl.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        /*
        // GDML does not allow a placement for the first shape in composite shapes:
        std::string splacement;
        if (splacement.empty() &&  config_.has_key("shape_1.placement")) {
        splacement = config_.fetch_string("shape_1.placement");
        }
        if (splacement.empty() &&  config_.has_key("first_shape.placement")) {
        splacement = config_.fetch_string("first_shape.placement");
        }
        if (! splacement.empty()) {
        DT_THROW_IF (!placement::from_string(splacement, pl),std::logic_error,
        "Cannot parse placement '" << splacement << "'!");
        }
        */
        set_shape1(const_cast<i_shape_3d &>(sh), pl, sname);
      }

      if (! _shape2_.is_valid()) {
        std::string sname;
        if (sname.empty() && config_.has_key("shape_2.name")) {
          sname = config_.fetch_string("shape_2.name");
        }
        if (sname.empty() && config_.has_key("second_shape.name")) {
          sname = config_.fetch_string("second_shape.name");
        }
        DT_THROW_IF (sname.empty(),
                     std::logic_error,
                     "Missing second shape's name!");
        DT_THROW_IF (objects_->find(sname) == objects_->end(),
                     std::logic_error,
                     "Cannot find second shape with name '" << sname << "'!");
        const i_object_3d::object_entry & oe = objects_->find(sname)->second;
        DT_THROW_IF (!oe.has_object(),
                     std::logic_error,
                     "3D object named '" << sname << "' is not valid!");
        const i_shape_3d & sh = dynamic_cast<const i_shape_3d &>(oe.get_object());
        placement pl;
        pl.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        std::string splacement;
        if (splacement.empty() &&  config_.has_key("shape_2.placement")) {
          splacement = config_.fetch_string("shape_2.placement");
        }
        if (splacement.empty() &&  config_.has_key("second_shape.placement")) {
          splacement = config_.fetch_string("second_shape.placement");
        }
        if (! splacement.empty()) {
          DT_THROW_IF (!placement::from_string(splacement, pl),std::logic_error,
                       "Cannot parse placement '" << splacement << "'!");

        }
        set_shape2(const_cast<i_shape_3d &>(sh), pl, sname);
      }
    }

    lock();
    return;
  }

  // // virtual
  void i_composite_shape_3d::make_any_face(face_identifier & fid_) const
  {
    fid_.invalidate();
    if (using_face_id_part_index()) {
      fid_.append_part(face_identifier::PART_INDEX_ANY);
      fid_.set_any_parts();
    }
    return;
  }

} // end of namespace geomtools
