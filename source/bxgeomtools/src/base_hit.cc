// base_hit.cc

// Ourselves:
#include <geomtools/base_hit.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/ioutils.h>

namespace geomtools {

  // Serial tag for datatools serializatable interface :
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_hit, "geomtools::base_hit")

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION (base_hit, "geomtools::base_hit/__system__")
  GEOMTOOLS_HIT_REGISTRATION_IMPLEMENT(base_hit, "geomtools::base_hit")
  
  const int32_t base_hit::INVALID_HIT_ID;

  base_hit & base_hit::i_measurement::operator()(base_hit & a_hit)
  {
    do_measurement(a_hit);
    return a_hit;
  }

  base_hit & base_hit::measure(i_measurement & a_measurement)
  {
    return a_measurement(*this);
  }

  void base_hit::_store_set(uint32_t bit_mask_)
  {
    _store |= bit_mask_;
    return;
  }

  void base_hit::_store_unset(uint32_t bit_mask_)
  {
    _store &= ~bit_mask_;
    return;
  }

  bool base_hit::_store_check(uint32_t bit_mask_) const
  {
    return _store & bit_mask_;
  }

  bool base_hit::has_hit_id() const
  {
    return _hit_id_ > INVALID_HIT_ID;
  }

  int32_t base_hit::get_hit_id() const
  {
    return _hit_id_;
  }

  void base_hit::set_hit_id(int32_t a_hit_id)
  {
    if (a_hit_id > INVALID_HIT_ID) {
      _store_set(STORE_HIT_ID);
      _hit_id_ = a_hit_id;
    } else {
      base_hit::invalidate_hit_id();
    }
    return;
  }

  void base_hit::invalidate_hit_id()
  {
    _hit_id_ = INVALID_HIT_ID;
    _store_unset(STORE_HIT_ID);
    return;
  }

  bool base_hit::has_geom_id() const
  {
    return _geom_id_.is_valid();
  }

  void base_hit::invalidate_geom_id()
  {
    _geom_id_.invalidate();
    _store_unset(STORE_GEOM_ID);
    return;
  }

  const geomtools::geom_id & base_hit::get_geom_id() const
  {
    return _geom_id_;
  }

  geomtools::geom_id & base_hit::grab_geom_id()
  {
    _store_set(STORE_GEOM_ID);
    return _geom_id_;
  }

  void base_hit::set_geom_id(const geomtools::geom_id & a_gid)
  {
    _store_set(STORE_GEOM_ID);
    _geom_id_ = a_gid;
    return;
  }

  // Check if there are stored auxiliary properties
  bool base_hit::has_auxiliaries() const
  {
    return(_store & STORE_AUXILIARIES) && ! _auxiliaries_.empty();
  }

  const datatools::properties &
  base_hit::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties &
  base_hit::grab_auxiliaries()
  {
    _store_set(STORE_AUXILIARIES);
    return _auxiliaries_;
  }

  void base_hit::set_auxiliaries(const datatools::properties & a_)
  {
    _store_set(STORE_AUXILIARIES);
    _auxiliaries_ = a_;
  }

  void base_hit::invalidate_auxiliaries()
  {
    _auxiliaries_.clear();
    _store_unset(STORE_AUXILIARIES);
    return;
  }

  base_hit::base_hit()
  {
    _store = STORE_NOTHING;
    _hit_id_ = INVALID_HIT_ID;
    _geom_id_.invalidate();
    return;
  }

  base_hit::~base_hit()
  {
    this->base_hit::invalidate();
    return;
  }

  bool base_hit::is_valid() const
  {
    // Should we consider only the need for a valid hit id and let the possibility for an
    // un-initialized geom ID ?
    return(_hit_id_ != INVALID_HIT_ID) && _geom_id_.is_valid();
    // return(_hit_id_ != INVALID_HIT_ID);
  }

  void base_hit::reset()
  {
    this->base_hit::invalidate();
    return;
  }

  void base_hit::invalidate()
  {
    invalidate_hit_id();
    invalidate_geom_id();
    invalidate_auxiliaries();
    return;
  }

  void base_hit::clear()
  {
    this->base_hit::reset();
    return;
  }

  void base_hit::print_tree(std::ostream & out_,
                            const boost::property_tree::ptree & options_) const
  {
    datatools::i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    bool no_auxiliaries_list  = options_.get<bool>("no_list_auxiliaries", false);

    const std::string & indent = popts.indent;
    if (! popts.title.empty ()) {
      out_ << indent << popts.title << std::endl;
    }

    out_ << indent << tag
          << "Store       : " << datatools::io::to_binary(_store) << std::endl;

    out_ << indent << tag
          << "Hit ID      : ";
    if (has_hit_id()) {
      out_ << _hit_id_;
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;
    
    out_ << indent << tag
          << "Geometry ID : ";
    if (_geom_id_.is_valid()) {
      out_ << _geom_id_;
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << inherit_tag(popts.inherit)
          << "Auxiliaries : ";
    if (_auxiliaries_.empty()) {
      out_ << "<empty>";
    } else {
      out_ << _auxiliaries_.size() << " item(s)";
    }
    out_ << std::endl;
    if (!no_auxiliaries_list) {
      if (!_auxiliaries_.empty()) {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << inherit_skip_tag(popts.inherit) ;
        _auxiliaries_.tree_dump(out_, "", indent_oss.str());
      }
    }
    return;
  }

  void base_hit::tree_dump(std::ostream & a_out,
                           const std::string & a_title,
                           const std::string & a_indent,
                           bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty()) {
      indent = a_indent;
    }
    if ( ! a_title.empty() ) {
      a_out << indent << a_title << std::endl;
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Store       : " << datatools::io::to_binary(_store) << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Hit ID      : ";
    if (has_hit_id()) {
      a_out << _hit_id_;
    } else {
      a_out << "<none>";
    }
    a_out << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Geometry ID : ";
    if (_geom_id_.is_valid()) {
      a_out << _geom_id_;
    } else {
      a_out << "<none>";
    }
    a_out << std::endl;

    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Auxiliaries : ";
    if (_auxiliaries_.empty()) {
      a_out << "<empty>";
    }
    a_out << std::endl;
    if (!_auxiliaries_.empty()) {
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(a_inherit) ;
      _auxiliaries_.tree_dump(a_out, "", indent_oss.str());
    }
    return;
  }

  void base_hit::dump() const
  {
    tree_dump(std::clog, "geomtools::base_hit");
    return;
  }

} // end of namespace geomtools
