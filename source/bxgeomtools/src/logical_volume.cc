// -*- mode: c++ ; -*-
/* logical_volume.cc
 */

#include <geomtools/logical_volume.h>

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <geomtools/i_placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/i_model.h>

namespace geomtools {

  using namespace std;

  const string logical_volume::HAS_REPLICA_FLAG = "__has_replica";

  bool logical_volume::is_locked () const
  {
    return _locked_;
  }

  void logical_volume::lock ()
  {
    _at_lock_ ();
    _locked_ = true;
    return;
  }

  void logical_volume::unlock ()
  {
    _locked_ = false;
    return;
  }

  bool logical_volume::has_name() const
  {
    return ! _name_.empty();
  }

  const string & logical_volume::get_name () const
  {
    return _name_;
  }

  void logical_volume::set_name (const string & name_)
  {
    _name_ = name_;
    return;
  }

  const datatools::properties & logical_volume::get_parameters () const
  {
    return _parameters_;
  }

  datatools::properties & logical_volume::grab_parameters ()
  {
    return _parameters_;
  }

  bool logical_volume::has_shape () const
  {
    return _shape_ != 0;
  }

  void logical_volume::_clear_shape_ ()
  {
    if (_shape_ != 0) {
      if (_own_shape_) {
        delete _shape_;
      }
      _shape_ = 0;
      _own_shape_ = false;
    }
    return;
  }

  void logical_volume::set_shape (const i_shape_3d & shape_)
  {
    _clear_shape_ ();
    _own_shape_ = false;
    _shape_ = &shape_;
    return;
  }

  void logical_volume::set_shape (const i_shape_3d * shape_)
  {
    _clear_shape_ ();
    if (shape_ != 0) {
      _own_shape_ = true;
      _shape_ = shape_;
    }
    return;
  }

  const i_shape_3d & logical_volume::get_shape () const
  {
    DT_THROW_IF (! _shape_, std::logic_error, "Missing shape for logical '" << get_name () << "' !");
    return *_shape_;
  }

  bool logical_volume::has_effective_shape() const
  {
    return _effective_shape_ != 0;
  }

  void logical_volume::set_effective_shape (const i_shape_3d & shape_)
  {
    _effective_shape_ = &shape_;
  }

  const i_shape_3d & logical_volume::get_effective_shape () const
  {
    DT_THROW_IF (! _effective_shape_, std::logic_error, "Missing effective shape for logical '" << get_name () << "' !");
    return *_effective_shape_;
  }

  bool logical_volume::has_geometry_model() const
  {
    return _geo_model_ != 0;
  }

  void logical_volume::set_geometry_model (const i_model & model_)
  {
    _geo_model_ = &model_;
  }

  const i_model & logical_volume::get_geometry_model () const
  {
    DT_THROW_IF (! _geo_model_, std::logic_error, "Missing geometry model for logical '" << get_name () << "' !");
    return *_geo_model_;
  }

  bool logical_volume::has_material_ref () const
  {
    return ! _material_ref_.empty();
    /*
    if (material::has_key (_parameters_, material::constants::instance ().MATERIAL_REF_PROPERTY)) {
      return (_parameters_.is_string  (material::make_key (material::constants::instance ().MATERIAL_REF_PROPERTY)));
    }
    return false;
    */
  }

  string logical_volume::get_material_ref () const
  {
    return _material_ref_;
    /*
    if (has_material_ref ()) {
      return (_parameters_.fetch_string  (material::make_key (material::constants::instance ().MATERIAL_REF_PROPERTY)));
    }
    return material::constants::instance ().MATERIAL_REF_UNKNOWN;
    */
  }

  void logical_volume::set_material_ref (const string & material_ref_)
  {
    _material_ref_ = material_ref_;
    /*
    string mr = material_ref_;
    if (mr.empty ()) {
      mr = material::constants::instance ().MATERIAL_REF_DEFAULT;
    }
    _parameters_.update (material::make_key (material::constants::instance ().MATERIAL_REF_PROPERTY), material_ref_);
    */
    return;
  }

  bool logical_volume::has_effective_material_ref () const
  {
    return ! _effective_material_ref_.empty();
  }

  string logical_volume::get_effective_material_ref () const
  {
    return _effective_material_ref_;
  }

  void logical_volume::set_effective_material_ref (const string & effective_material_ref_)
  {
    _effective_material_ref_ = effective_material_ref_;
    return;
  }

  void logical_volume::_init_defaults_ ()
  {
    _locked_ = false;
    _own_shape_ = false;
    _shape_ = 0;
    _geo_model_ = 0;
    _effective_shape_ = 0;
    _abstract_ = false;
    return;
  }

  logical_volume::logical_volume ()
  {
    _init_defaults_ ();
    return;
  }

  logical_volume::logical_volume (const string & name_)
  {
    _init_defaults_ ();
    set_name (name_);
    return;
  }

  logical_volume::logical_volume (const string & name_,
                                  const i_shape_3d & shape_)
  {
    _init_defaults_ ();
    set_name (name_);
    set_shape (shape_);
    return;
  }

  logical_volume::logical_volume (const string & name_,
                                  const i_shape_3d * shape_)
  {
    _init_defaults_ ();
    set_name (name_);
    set_shape (shape_);
    return;
  }

  logical_volume::~logical_volume ()
  {
    _locked_ = false;
    _clear_shape_ ();
    return;
  }

  bool logical_volume::has_physical (const string & name_) const
  {
    return (_physicals_.find (name_) != _physicals_.end ());
  }

  const logical_volume::physicals_col_type &
  logical_volume::get_physicals () const
  {
    return _physicals_;
  }

  const logical_volume::physicals_col_type &
  logical_volume::get_real_physicals () const
  {
    return _real_physicals_;
  }

  const physical_volume & logical_volume::get_physical (const string & name_) const
  {
    physicals_col_type::const_iterator found = _physicals_.find (name_);
    DT_THROW_IF(found == _physicals_.end (), std::logic_error, "Name '" << name_ << "' is not used !");
    return *(found->second);
  }

  void logical_volume::add_physical (const physical_volume & phys_,
                                     const string & name_)
  {
    DT_THROW_IF (_physicals_.find (name_) != _physicals_.end (),
                 std::logic_error,
                 "Name '" << name_ << "' is already used !");
    string name;
    if (name_.empty ()) {
      name = phys_.get_name ();
    } else {
      name = name_;
    }
    DT_THROW_IF (name.empty (), std::logic_error,  "Missing physical's name !");
    DT_THROW_IF (_parameters_.has_flag (HAS_REPLICA_FLAG), std::logic_error,
                 "Cannot add more physical volume for a 'replica' already exists !");
    if (phys_.get_placement ().is_replica ()) {
      DT_THROW_IF (_physicals_.size () > 0, std::logic_error,
                   "Cannot add a 'replica' physical volume for other physicals already exist !");
      _parameters_.store_flag (HAS_REPLICA_FLAG);
    }
    _physicals_[name] = &phys_;
    return;
  }

  bool logical_volume::is_replica () const
  {
    if (_physicals_.size () != 1) return false;
    const physical_volume & phys = *(_physicals_.begin ()->second);
    const i_placement & pl = phys.get_placement ();
    return pl.is_replica ();
    // alternative:
    //return _parameters_.has_flag (HAS_REPLICA_FLAG);
  }

  void logical_volume::set_abstract (bool a_)
  {
    _abstract_ = a_;
    return;
  }

  bool logical_volume::is_abstract () const
  {
    return _abstract_;
  }

  void logical_volume::_at_lock_ ()
  {
    if (! has_material_ref ()) {
      _abstract_ = true;
    }
    _compute_real_physicals_ ();
    return;
  }

  void logical_volume::_compute_real_physicals_ ()
  {
    // Not implemented
  }

  void logical_volume::tree_dump (ostream & out_,
                                  const string & title_,
                                  const string & indent_,
                                  bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) {
      out_ << indent << title_ << endl;
    }
    out_ << indent <<  datatools::i_tree_dumpable::tag
         << "Name       : '" << _name_ << "'" << endl;

    out_ << indent <<  datatools::i_tree_dumpable::tag
         << "Material   : '" << _material_ref_ << "'" << endl;

    out_ << indent <<  datatools::i_tree_dumpable::tag
         << "Locked     : " << (_locked_? "Yes": "No") << endl;

    out_ << indent <<  datatools::i_tree_dumpable::tag
         << "Abstract   : " << (_abstract_? "Yes": "No") << endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Parameters : ";
      if (_parameters_.size () == 0) {
        out_ << "<empty>";
      }
      out_ << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _parameters_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    {
      out_ << indent <<  datatools::i_tree_dumpable::tag
           << "Shape : ";
      if (has_shape ()) {
        out_ << "'" << _shape_->get_shape_name () << "' "
             << (_own_shape_? "(owned)": "(not owned)");
      } else {
        out_ << "<no shape>";
      }
      out_ << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _shape_->tree_dump (out_,"",indent_oss.str ());
      }
    }

    if (has_effective_shape()) {
      out_ << indent <<  datatools::i_tree_dumpable::tag
           << "Effective shape : '"
           << get_effective_shape().get_shape_name () << "'"
           << endl;
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::skip_tag;
      get_effective_shape().tree_dump (out_,"",indent_oss.str ());
    }

    if (has_effective_material_ref()) {
      out_ << indent <<  datatools::i_tree_dumpable::tag
           << "Effective material : '"
           << get_effective_material_ref() << "'"
           << endl;
    }

    if (has_geometry_model()) {
      out_ << indent <<  datatools::i_tree_dumpable::tag
           << "Geometry model : '"
           << _geo_model_->get_name ()  << "'"
           << endl;
    }

    {
      out_ << indent <<  datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Physicals : ";
      if (_physicals_.size()) {
        out_ << _physicals_.size();
      } else {
        out_ << "none";
      }
      out_ << endl;
      if (_physicals_.size ()) {
        for (physicals_col_type::const_iterator i = _physicals_.begin ();
             i != _physicals_.end ();
             i++) {
          physicals_col_type::const_iterator j = i;
          j++;
          out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
          if (j == _physicals_.end ()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << '"' << i->first << '"' << std::endl;
        }
      }
    }

    return;
  }

} // end of namespace geomtools

// end of logical_volume.cc
