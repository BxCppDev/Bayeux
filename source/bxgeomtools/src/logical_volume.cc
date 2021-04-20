// logical_volume.cc

// Ourselves:
#include <geomtools/logical_volume.h>

// Standard library:
#include <stdexcept>
#include <iostream>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <geomtools/i_placement.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/i_model.h>

namespace geomtools {

  using namespace std;

  // static
  const std::string & logical_volume::has_replica_flag()
  {
    static std::string token = "__has_replica";
    return token;
  }

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

  void logical_volume::set_logging_priority(datatools::logger::priority p)
  {
    _logging_priority_ = p;
  }

  datatools::logger::priority logical_volume::get_logging_priority() const
  {
    return _logging_priority_;
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
    return _shape_ != nullptr;
  }

  void logical_volume::_clear_shape_ ()
  {
    if (_shape_ != nullptr) {
      if (_own_shape_) {
        delete _shape_;
      }
      _shape_ = nullptr;
      _own_shape_ = false;
    }
    return;
  }

  void logical_volume::set_shape (const i_shape_3d & shape_)
  {
    DT_THROW_IF(! shape_.is_locked(),
                std::logic_error,
                "Shape '" << shape_.get_shape_name() << "' in logical volume '"
                << _name_ << "' is not locked!");
    _clear_shape_ ();
    _own_shape_ = false;
    _shape_ = &shape_;
    return;
  }

  void logical_volume::set_shape (const i_shape_3d * shape_)
  {
    DT_THROW_IF(! shape_->is_locked(),
                std::logic_error,
                "Shape '" << shape_->get_shape_name() << "' in logical volume '"
                << _name_ << "' is not locked!");
    _clear_shape_ ();
    if (shape_ != nullptr) {
      _own_shape_ = true;
      _shape_ = shape_;
    }
    return;
  }

  const i_shape_3d & logical_volume::get_shape () const
  {
    DT_THROW_IF (! _shape_, std::logic_error, "Missing shape for logical volume '" << get_name () << "' !");
    return *_shape_;
  }

  bool logical_volume::has_effective_shape() const
  {
    return _effective_shape_ != nullptr;
  }

  void logical_volume::set_effective_shape (const i_shape_3d & shape_)
  {
    _effective_shape_ = &shape_;
  }

  const i_shape_3d & logical_volume::get_effective_shape () const
  {
    DT_THROW_IF (_effective_shape_ == nullptr, std::logic_error, "Missing effective shape for logical volume '" << get_name () << "' !");
    return *_effective_shape_;
  }

  bool logical_volume::has_effective_relative_placement() const
  {
    return _effective_relative_placement_ != nullptr;
  }

  void logical_volume::set_effective_relative_placement (const placement & placement_)
  {
    _effective_relative_placement_ = &placement_;
  }

  const placement & logical_volume::get_effective_relative_placement () const
  {
    DT_THROW_IF (! _effective_relative_placement_, std::logic_error, "Missing effective placement for logical volume '" << get_name () << "' !");
    return *_effective_relative_placement_;
  }

  bool logical_volume::has_geometry_model() const
  {
    return _geo_model_ != nullptr;
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
    _name_.clear();
    _logging_priority_ = datatools::logger::PRIO_FATAL;
    _material_ref_.clear();
    _locked_ = false;
    _parameters_.clear();
    _own_shape_ = false;
    _shape_ = nullptr;
    _geo_model_ = nullptr;
    _effective_shape_ = nullptr;
    _effective_relative_placement_ = nullptr;
    _effective_material_ref_.clear();
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
    // 2013-06-17 FM : the following test should be considered in the future : inhibited for now
    // DT_THROW_IF (is_locked (),
    //              std::logic_error,
    //              "Logical volume '" << get_name() << "' is locked ! Cannot add daughter physical volume with name '" << name_ << "' !");
    DT_THROW_IF(_physicals_.find(name_) != _physicals_.end(),
                std::logic_error,
                "Name '" << name_ << "' is already used in logical volume '" << get_name()  << "' !");
    std::string name;
    if (name_.empty()) {
      name = phys_.get_name();
    } else {
      name = name_;
    }
    DT_THROW_IF(name.empty(), std::logic_error, "Missing physical's name in logical volume '" <<  get_name()  << "' !");
    DT_THROW_IF(_parameters_.has_flag(logical_volume::has_replica_flag()), std::logic_error,
                "Cannot add more physical volume for a 'replica' already exists in logical volume '" <<  get_name()  << "' !");
    if (phys_.get_placement().is_replica()) {
      DT_THROW_IF(_physicals_.size() > 0, std::logic_error,
                  "Cannot add a 'replica' physical volume for other physicals already exist in logical volume '" <<  get_name()  << "' !");
      _parameters_.store_flag(logical_volume::has_replica_flag());
    }
    _physicals_[name] = &phys_;
    return;
  }

  bool logical_volume::is_replica() const
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

  logical_volume::locate_result::locate_result()
  {
    reset();
    return;
  }

  void logical_volume::locate_result::reset()
  {
    _shape_domain_flags_ = SHAPE_DOMAIN_NONE;
    _daughter_name_.clear();
    _daughter_physical_ = 0;
    _daughter_placement_index_ = -1;
    geomtools::invalidate(_position_in_daughter_);
    return;
  }

  void logical_volume::locate_result::set_shape_domain_flags(uint32_t bits_)
  {
    _shape_domain_flags_ = bits_;
    return;
  }

  void logical_volume::locate_result::set_daughter_name(const std::string & name_)
  {
    _daughter_name_ = name_;
    return;
  }

  void logical_volume::locate_result::set_daughter(const physical_volume & pv_)
  {
    _daughter_physical_ = &pv_;
    return;
  }

  void logical_volume::locate_result::set_daughter_placement_index(int index_)
  {
    _daughter_placement_index_ = index_;
    return;
  }

  void logical_volume::locate_result::set_position_in_daughter(const vector_3d & pos_)
  {
    _position_in_daughter_ = pos_;
    return;
  }

  uint32_t logical_volume::locate_result::get_shape_domain_flags() const
  {
    return _shape_domain_flags_;
  }

  bool logical_volume::locate_result::has_daughter() const
  {
    return _daughter_physical_ != 0;
  }

  const std::string & logical_volume::locate_result::get_daughter_name() const
  {
    return _daughter_name_;
  }

  bool logical_volume::locate_result::has_daughter_placement_index() const
  {
    return _daughter_placement_index_ >= 0;
  }

  int logical_volume::locate_result::get_daughter_placement_index() const
  {
    return _daughter_placement_index_;
  }

  const physical_volume & logical_volume::locate_result::get_daughter() const
  {
    DT_THROW_IF(_daughter_physical_ == 0, std::logic_error, "Missing daughter physical!");
    return *_daughter_physical_;
  }

  bool logical_volume::locate_result::has_position_in_daughter() const
  {
    return geomtools::is_valid(_position_in_daughter_);
  }

  const vector_3d & logical_volume::locate_result::get_position_in_daughter() const
  {
    return _position_in_daughter_;
  }

  void logical_volume::locate_result::dump(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "Shape domain flags : " << _shape_domain_flags_ << std::endl;
    out_ << indent_ << "|-- " << "Daughter name      : '" << _daughter_name_ << "'" << std::endl;
    out_ << indent_ << "|-- " << "Daughter physical  : '"
         << (_daughter_physical_ ? _daughter_physical_->get_name() : "<none>") << "'" << std::endl;
    out_ << indent_ << "|-- " << "Daughter placement index : " << _daughter_placement_index_ << std::endl;
    out_ << indent_ << "`-- " << "Position in daughter : " << _position_in_daughter_ / CLHEP::mm << " mm" << std::endl;
  }

  uint32_t logical_volume::locate(const vector_3d & local_position_,
                                  bool ignore_daugthers_,
                                  double tolerance_, double daughter_tolerance_,
                                  locate_result * locate_res_) const
  {
    datatools::logger::priority logging = get_logging_priority();
    //logging = datatools::logger::PRIO_DEBUG;

    uint32_t bits = SHAPE_DOMAIN_NONE;
    bool outside_main_shape = false;
    bool on_main_shape_surface = false;
    DT_THROW_IF(!has_shape(),
                std::logic_error,
                "No shape in logical volume '" << get_name() << "' !");
    outside_main_shape = _shape_->is_outside(local_position_, tolerance_);
    if (outside_main_shape) {
      bits |= SHAPE_DOMAIN_OUTSIDE;
      DT_LOG_TRACE(logging, "SHAPE_DOMAIN_OUTSIDE: bits =" << bits);
    } else {
      // Case: !outside_main_shape
      face_identifier fid;
      _shape_->make_any_face(fid);
      on_main_shape_surface = _shape_->is_on_surface(local_position_,
                                                     fid,
                                                     tolerance_);
      if (on_main_shape_surface) {
        bits |= SHAPE_DOMAIN_ON_SURFACE;
        DT_LOG_TRACE(logging, "SHAPE_DOMAIN_ON_SURFACE: bits =" << bits);
      } else {
        // Case: ! on_main_shape_surface
        bits |= SHAPE_DOMAIN_INSIDE;
        if ( ! ignore_daugthers_) {
          // Loop on daughter volumes:
          for (physicals_col_type::const_iterator i = _physicals_.begin();
               i != _physicals_.end();
               i++) {
            const std::string & daughter_label = i->first;
            const physical_volume & daughter_phys = *(i->second);
            DT_LOG_DEBUG(logging, "Checking daughter volume '" << daughter_label
                         << "' in logical volume '" << get_name() << "'...");
            const i_placement & daughter_placement = daughter_phys.get_placement();
            DT_LOG_DEBUG(logging, "Placement has " << daughter_placement.get_number_of_items ()
                         << " items.");
            // Loop on all placements :
            for(size_t item = 0; item < daughter_placement.get_number_of_items (); item++) {
              vector_3d daughter_item_position;
              placement daughter_item_placement;
              daughter_placement.get_placement (item, daughter_item_placement);
              // Change from mother to child reference frame:
              daughter_item_placement.mother_to_child(local_position_, daughter_item_position);
              DT_THROW_IF(!daughter_phys.has_logical(),
                          std::logic_error,
                          "Daughter physical volume '" << daughter_label
                          << "' has no associated logical in logical volume '" << get_name() << "' !");
              DT_THROW_IF(!daughter_phys.get_logical ().has_shape(),
                          std::logic_error,
                          "Daughter logical volume '" << daughter_label
                          << "' has no associated shape in logical volume '" << get_name() << "' !");
              const i_shape_3d & daughter_shape = daughter_phys.get_logical ().get_shape ();
              bool daughter_outside_main_shape
                = daughter_shape.is_outside(daughter_item_position, daughter_tolerance_);
              if (! daughter_outside_main_shape) {
                face_identifier local_fid;
                daughter_shape.make_any_face(local_fid);
                bool on_daughter_shape_surface
                  = daughter_shape.is_on_surface(daughter_item_position,
                                                 local_fid,
                                                 daughter_tolerance_);
                if (on_daughter_shape_surface) {
                  bits |= SHAPE_DOMAIN_ON_DAUGHTER_SURFACE;
                  DT_LOG_TRACE(logging, "SHAPE_DOMAIN_ON_DAUGHTER_SURFACE: bits =" << bits);
                  if (locate_res_) {
                    locate_res_->set_daughter_name(daughter_label);
                    locate_res_->set_daughter(daughter_phys);
                    locate_res_->set_daughter_placement_index(item);
                    locate_res_->set_position_in_daughter(daughter_item_position);
                  }
                  break;
                } else {
                  bits |= SHAPE_DOMAIN_INSIDE_DAUGHTER;
                  DT_LOG_TRACE(logging, "SHAPE_DOMAIN_INSIDE_DAUGHTER: bits =" << bits);
                  if (locate_res_) {
                    locate_res_->set_daughter_name(daughter_label);
                    locate_res_->set_daughter(daughter_phys);
                    locate_res_->set_daughter_placement_index(item);
                    locate_res_->set_position_in_daughter(daughter_item_position);
                  }
                  break;
                }
              } // ! daughter_outside_main_shape
            } // end of loop on all placement
            // If some 'daughter' bit is already activated, there is
            // no need to continue to traverse the daughter volumes:
            if ((bits & SHAPE_DOMAIN_ON_DAUGHTER_SURFACE)
                || (bits & SHAPE_DOMAIN_INSIDE_DAUGHTER)) {
              break;
            }
          } // end of loop on daughter volumes.
        }
      } // end of !on_main_shape_surface
    }// end of !outside_main_shape

    if (locate_res_) {
      locate_res_->set_shape_domain_flags(bits);
    }
    DT_LOG_TRACE(logging, "Exiting: with bits =" << bits);
    return bits;
  }


  // static
  bool logical_volume::same(const logical_volume & log1_,
                            const logical_volume & log2_,
                            uint32_t /*mode_*/)
  {
    if (&log1_ == &log2_) return true;
    const i_shape_3d * sh1 = &log1_.get_shape ();
    const i_shape_3d * sh2 = &log2_.get_shape ();
    if (log1_.has_effective_shape()) {
      sh1 = &log1_.get_effective_shape ();
    }
    if (log2_.has_effective_shape()) {
      sh2 = &log2_.get_effective_shape ();
    }
    std::string mat1 = log1_.get_material_ref();
    std::string mat2 = log2_.get_material_ref();
    if (log1_.has_effective_material_ref()) {
      mat1 = log1_.get_effective_material_ref();
    }
    if (log2_.has_effective_material_ref()) {
      mat2 = log2_.get_effective_material_ref();
    }
    if (sh1 == sh2 && mat1 == mat2) {
      return true;
    }
    return false;
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

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority  : '" << datatools::logger::get_priority_label(_logging_priority_) << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Locked     : " << (_locked_? "Yes": "No") << std::endl;

    out_ << indent <<  datatools::i_tree_dumpable::tag
         << "Material   : '" << _material_ref_ << "'" << endl;

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

    if (has_effective_relative_placement()) {
      out_ << indent <<  datatools::i_tree_dumpable::tag
           << "Effective relative placement : '"
           << get_effective_relative_placement() << "'"
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
