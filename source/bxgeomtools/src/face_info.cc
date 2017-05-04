/// \file geomtools/face_info.cc

// Ourselves:
#include <geomtools/face_info.h>

// This project:
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  face_info::face_info()
  {
    _positioning_.reset();
    return;
  }

  face_info::~face_info()
  {
    return;
  }

  bool face_info::is_valid() const
  {
    if (!has_identifier()) {
      // std::cerr << "DEVEL: face_info::is_valid: No identifier." << std::endl;
      return false;
    }
    if (!has_face()) {
      // std::cerr << "DEVEL: face_info::is_valid: No face." << std::endl;
      return false;
    }
    if (!has_positioning()) {
      // std::cerr << "DEVEL: face_info::is_valid: No positioning." << std::endl;
      return false;
    }
    return true;
  }

  bool face_info::has_identifier() const
  {
    return _identifier_.is_unique();
  }

  bool face_info::has_face() const
  {
    return _face_.has_data();
  }

  bool face_info::has_positioning() const
  {
    return _positioning_.is_valid();
  }

  bool face_info::has_label() const
  {
    return ! _label_.empty();
  }

  void face_info::set_identifier(const face_identifier & fid_)
  {
    _identifier_ = fid_;
    return;
  }

  const face_identifier & face_info::get_identifier() const
  {
    return _identifier_;
  }

  face_identifier & face_info::grab_identifier()
  {
    return _identifier_;
  }

  void face_info::reset_identifier()
  {
    _identifier_.reset();
    return;
  }

  void face_info::set_face(const face_handle_type & face_handle_)
  {
    _face_ = face_handle_;
    return;
  }

  void face_info::reset_face()
  {
    _face_.reset();
    return;
  }

  void face_info::set_face_ref(i_shape_2d * face_ptr_)
  {
    _face_.reset(face_ptr_);
    return;
  }

  const i_shape_2d & face_info::get_face_ref() const
  {
    return _face_.get();
  }

  i_shape_2d & face_info::grab_face_ref()
  {
    return _face_.grab();
  }

  const placement & face_info::get_positioning() const
  {
    return _positioning_;
  }

  placement & face_info::grab_positioning()
  {
    return _positioning_;
  }

  void face_info::set_identity_positioning()
  {
    _positioning_.set_identity();
    return;
  }

  void face_info::reset_positioning()
  {
    _positioning_.reset();
    return;
  }

  const std::string & face_info::get_label() const
  {
    return _label_;
  }

  void face_info::set_label(const std::string & l_)
  {
    _label_ = l_;
    return;
  }

  void face_info::reset_label()
  {
    _label_.clear();
    return;
  }

  void face_info::reset()
  {
    reset_face();
    reset_identifier();
    reset_positioning();
    reset_label();
    return;
  }

  void face_info::tree_dump(std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_,
                            bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Face identifier : ";
      if (! has_identifier()) {
        out_ << "<none>";
      } else {
        out_ << _identifier_;
      }
      out_ << std::endl;
    }

    {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Face : ";
      if (! has_face()) {
        out_ << "<none>";
      } else {
        out_ << "'" << _face_.get().get_shape_name() << "'";
        if (_face_.get().is_valid()) {
          out_ << " (valid)";
        } else {
          out_ << " (invalid)";
        }
      }
      out_ << std::endl;
    }

    {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Positioning : ";
      if (! has_positioning()) {
        out_ << "<none>";
      }
      out_ << std::endl;
      if (has_positioning()) {
        std::ostringstream indent2;
        indent2 << indent_ << datatools::i_tree_dumpable::skip_tag;
        _positioning_.tree_dump(out_, "", indent2.str());
      }
    }

    {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Face label : ";
      if (has_label()) {
        out_ << "'" << _label_ << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
    }

    return;
  }

  void print_face_infos(const face_info_collection_type & finfos_,
                        std::ostream & out_,
                        const std::string & title_,
                        const std::string & indent_)
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    int count = 0;
    for (face_info_collection_type::const_iterator i = finfos_.begin();
         i != finfos_.end();
         i++) {
      const face_info & finfo = *i;
      out_ << indent_;
      std::ostringstream indent2;
      indent2 << indent_;
      face_info_collection_type::const_iterator j = i;
      if (++j == finfos_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
        indent2 << datatools::i_tree_dumpable::last_skip_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
        indent2 << datatools::i_tree_dumpable::skip_tag;
      }
      out_ << "Face #" << count << " : " << std::endl;
      finfo.tree_dump(out_, "", indent2.str());
      count++;
    }
    return;
  }

} // end of namespace geomtools
