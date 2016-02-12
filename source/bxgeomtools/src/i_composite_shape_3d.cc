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

  void
  i_composite_shape_3d::shape_type::copy(shape_type & sh_)
  {
    _delete_ = false;
    _shape_ = sh_._shape_;
    _placement_ = sh_._placement_;
    if (sh_._delete_) {
      _delete_ = true;
      sh_._delete_ = false;
    }
    return;
  }

  i_composite_shape_3d::shape_type::shape_type()
  {
    _delete_ = false;
    _shape_ = 0;
    return;
  }

  i_composite_shape_3d::shape_type::~shape_type()
  {
    reset();
    return;
  }

  void
  i_composite_shape_3d::shape_type::reset()
  {
    if (_delete_) {
      delete _shape_;
      _shape_ = 0;
      _delete_ = false;
    }
    _shape_ref_.clear();
    _placement_.reset();
    return;
  }

  bool
  i_composite_shape_3d::shape_type::is_delete() const
  {
    return _delete_;
  }

  bool
  i_composite_shape_3d::shape_type::is_valid() const
  {
    return _shape_ != 0;
  }

  void i_composite_shape_3d::shape_type::set_shape_ref(const std::string & shref_)
  {
    _shape_ref_ = shref_;
    return;
  }

  const std::string & i_composite_shape_3d::shape_type::get_shape_ref() const
  {
    return _shape_ref_;
  }

  const i_shape_3d &
  i_composite_shape_3d::shape_type::get_shape() const
  {
    return *_shape_;
  }

  i_shape_3d &
  i_composite_shape_3d::shape_type::grab_shape()
  {
    return *_shape_;
  }

  const placement &
  i_composite_shape_3d::shape_type::get_placement() const
  {
    return _placement_;
  }

  placement &
  i_composite_shape_3d::shape_type::grab_placement()
  {
    return _placement_;
  }

  void
  i_composite_shape_3d::shape_type::make_shape(i_shape_3d & sh3d_,
                                               const placement & p_,
                                               const std::string & shape_ref_,
                                               shape_type & sh_)
  {
    DT_THROW_IF (! sh3d_.is_locked(),
                 std::logic_error,
                 "Shape '" << sh3d_.get_shape_name() << "' is not locked!");
    sh_.reset();
    sh_._delete_ = false;
    sh_._shape_ = &sh3d_;
    sh_._shape_ref_ = shape_ref_;
    sh_._placement_ = p_;
    return;
  }

  void
  i_composite_shape_3d::shape_type::make_shape(i_shape_3d & sh3d_,
                                               const placement & p_,
                                               i_composite_shape_3d::shape_type & sh_)
  {
    make_shape(sh3d_, p_, "", sh_);
    return;
  }

  void
  i_composite_shape_3d::shape_type::make_shape(i_shape_3d * sh3d_,
                                               const placement & p_,
                                               i_composite_shape_3d::shape_type & sh_)
  {
    DT_THROW_IF (! sh3d_->is_locked(),
                 std::logic_error,
                 "Shape '" << sh3d_->get_shape_name() << "' is not locked!");
    sh_.reset();
    sh_._delete_ = true;
    sh_._shape_ = sh3d_;
    sh_._placement_ = p_;
    return;
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
      if (_shape_ != 0) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Shape = ";
        out_ << "'" << _shape_->get_shape_name() << "' "
             << (_delete_? "(owned)": "(not owned)") << endl;
        std::ostringstream oss_indent;
        oss_indent << indent << datatools::i_tree_dumpable::skip_tag;
        _shape_->tree_dump(out_,
                           "",
                           oss_indent.str(),
                           false);
      } else {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Shape = " << "<no shape>" << endl;
      }
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Shape ref = ";
    if (_shape_ref_.empty()) {
      out_ << "<none>";
    } else {
      out_ << "'" << _shape_ref_ << "'";
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

  void i_composite_shape_3d::shape_type::dump(std::ostream & out_) const
  {
    out_ << "i_composite_shape_3d::shape_type" << std::endl;
    out_ << "|-- delete: " << _delete_ << std::endl;
    out_ << "|-- shape_ref: '" << _shape_ref_ << "'" << std::endl;
    out_ << "|-- shape: " << std::hex << (void *) _shape_ << std::dec << std::endl;
    out_ << "`-- placement: " << std::endl;
    return;
  }

  /***************************************/

  void
  i_composite_shape_3d::dump(std::ostream & out_) const
  {
    out_ << "i_composite_shape_3d::i_composite_shape_3d: " << get_shape_name() << std::endl;
    out_ << "|-- shape1: " << std::endl;
    _shape1_.dump(out_);
    out_ << "|-- shape2: " << std::endl;
    _shape2_.dump(out_);
    out_ << "`-- end." << std::endl;
    return;
  }

  i_composite_shape_3d::i_composite_shape_3d(double skin_)
  {
    set_skin(skin_);
    return;
  }

  i_composite_shape_3d::~i_composite_shape_3d()
  {
    return;
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
  i_composite_shape_3d::set_shape1(i_shape_3d & sh3d_, const placement & p_, const std::string & shref_)
  {
    shape_type::make_shape(sh3d_, p_, shref_, _shape1_);
    return;
  }

  void
  i_composite_shape_3d::set_shape2(i_shape_3d & sh3d_, const placement & p_, const std::string & shref_)
  {
    shape_type::make_shape(sh3d_, p_, shref_, _shape2_);
    return;
  }

  void
  i_composite_shape_3d::set_shape1(i_shape_3d * sh3d_, const placement & p_)
  {
    shape_type::make_shape(sh3d_, p_, _shape1_);
    return;
  }

  void
  i_composite_shape_3d::set_shapes(i_shape_3d & sh3d1_,
                                   i_shape_3d & sh3d2_, const placement & p2_,
                                   const std::string & sh1ref_, const std::string & sh2ref_)
  {
    placement p1;
    shape_type::make_shape(sh3d1_, p1,  sh1ref_, _shape1_);
    shape_type::make_shape(sh3d2_, p2_, sh2ref_, _shape2_);
    return;
  }

  void
  i_composite_shape_3d::set_shapes(i_shape_3d * sh3d1_,
                                   i_shape_3d * sh3d2_, const placement & p2_)
  {
    placement p1;
    shape_type::make_shape(sh3d1_, p1, _shape1_);
    shape_type::make_shape(sh3d2_, p2_, _shape2_);
    return;
  }

  void
  i_composite_shape_3d::set_shape2(i_shape_3d * sh3d_, const placement & p_)
  {
    shape_type::make_shape(sh3d_, p_, _shape2_);
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
