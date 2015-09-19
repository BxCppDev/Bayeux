/** \file smart_id_locator.cc */

// Ourselves:
#include <geomtools/smart_id_locator.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>

// This project:
#include <geomtools/geom_map.h>

namespace geomtools {

  datatools::logger::priority smart_id_locator::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void smart_id_locator::set_logging_priority(datatools::logger::priority p)
  {
    _logging_priority_ = p;
    return;
  }

  const std::list<const geom_info *> & smart_id_locator::get_ginfos() const
  {
    return _ginfos_;
  }

  void smart_id_locator::_construct()
  {
    for (geom_info_dict_type::const_iterator i
           = _gmap_->get_geom_infos().begin();
         i != _gmap_->get_geom_infos().end();
         i++) {
      uint32_t gtype = i->first.get_type();
      bool valid = false;

      // search matching type IDs:
      if (_type_ != geom_id::INVALID_TYPE) {
        if (gtype == _type_) {
          valid = true;
        }
      } else if (_idsel_.is_initialized()) {
        if (_idsel_.match(i->first)) {
          valid = true;
        }
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing criterion for building the smart list of IDs !");
      }

      if (valid) {
        const geom_info * ginfo = &(i->second);
        _ginfos_.push_back(ginfo);
      }
    }

    if (_type_ != geom_id::INVALID_TYPE) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "Number of entries with type (" << _type_ << ") = " << _ginfos_.size());
    }
    if (_idsel_.is_initialized()) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "Number of entries selected by the ID selector = " << _ginfos_.size());
    }
    return;
  }

  void smart_id_locator::set_gmap(const geom_map & gmap_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Locator is already initialized !");
    _gmap_ = &gmap_;
    return;
  }

  bool smart_id_locator::is_initialized() const
  {
    return _mode_ != MODE_INVALID;
  }

  void smart_id_locator::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Locator is not initialized !");
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _idsel_.reset();
    _last_found_ = 0;
    _ginfos_.clear();
    _initialized_ = false;
    return;
  }

  void smart_id_locator::initialize(const std::string & rules_,
                                    int mode_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Locator is already initialized !");
    DT_THROW_IF(_gmap_ == 0, std::logic_error, "Missing geometry map ! Use the 'set_gmap' method before !");
    _idsel_.set_id_mgr(_gmap_->get_id_manager());
    try {
      _idsel_.initialize(rules_);
    }
    catch (std::exception & x) {
      DT_THROW_IF(true, std::logic_error, "ID selector failed: " << x.what());
    }
    _mode_ = mode_;
    _last_found_ = 0;
    _construct();
    _initialized_ = true;
    return;
  }

  void smart_id_locator::initialize(uint32_t type_,
                                    int mode_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Locator is already initialized !");
    DT_THROW_IF(type_ == geom_id::INVALID_TYPE, std::logic_error,
                "Invalid geometry info type (" << type_ << ")");
    _mode_ = mode_;
    _type_ = type_;
    _last_found_ = 0;
    _construct();
    _initialized_ = true;
    return;
  }

  smart_id_locator::smart_id_locator()
  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    _initialized_ = false;
    _gmap_ = 0;
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _last_found_ = 0;
    return;
  }

  smart_id_locator::smart_id_locator(const geom_map & gmap_)
  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    _initialized_ = false;
    _gmap_ = 0;
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _last_found_ = 0;
    set_gmap(gmap_);
    return;
  }

  smart_id_locator::smart_id_locator(const geom_map & gmap_,
                                     uint32_t type_,
                                     int mode_)
  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    _initialized_ = false;
    _gmap_ = 0;
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _last_found_ = 0;
    set_gmap(gmap_);
    initialize(type_, mode_);
    return;
  }

  smart_id_locator::smart_id_locator(const geom_map & gmap_,
                                     const std::string & selection_rules_,
                                     int mode_)
  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    _initialized_ = false;
    _gmap_ = 0;
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _last_found_ = 0;
    set_gmap(gmap_);
    initialize(selection_rules_, mode_);
    return;
  }

  smart_id_locator::~smart_id_locator()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  bool smart_id_locator::validate_id(const geom_id & id_) const
  {
    return _gmap_->validate_id(id_);
  }

  const geom_info & smart_id_locator::get_geom_info(const geom_id & id_) const
  {
    return _gmap_->get_geom_info(id_);
  }

  const geom_id & smart_id_locator::get_geom_id(const vector_3d & world_position_,
                                                int /*type_*/,
                                                double tolerance_) const
  {
    // The reverse flag means that we check if a position IS INSIDE a volume
    // by computing if this position IS NOT OUTSIDE the volume (see the
    // geomtools::geom_map::check_inside method) :
    bool reverse = true;
    if (_mode_ & MODE_LAST) {
      // First search if one matched the last found object :
      if (_last_found_ != 0) {
        bool located = false;
        located = geom_map::check_inside(*_last_found_,
                                         world_position_,
                                         tolerance_,
                                         reverse);
        if (located) {
          DT_LOG_DEBUG(get_logging_priority(),
                       "Last found entry match (" << _last_found_->get_id() << ")");
          return _last_found_->get_id();
        }
      }
    }

    for (std::list<const geom_info *>::const_iterator i
           = _ginfos_.begin();
         i != _ginfos_.end();
         i++) {
      const geom_info * ginfo = *i;
      if (_last_found_ != 0) {
        // No need to check twice the last found location:
        if (ginfo == _last_found_) {
          continue;
        }
      }
      const bool located = geom_map::check_inside(*ginfo,
                                                  world_position_,
                                                  tolerance_,
                                                  reverse);
      if (located) {
        if (_mode_ & MODE_LAST) {
          smart_id_locator * non_const_this = const_cast<smart_id_locator *>(this);
          non_const_this->_last_found_ = ginfo;
        }
        DT_LOG_DEBUG(get_logging_priority(), "New entry match (" << ginfo->get_id() << ")");
        return ginfo->get_id();
      }
    }

    return _gmap_->get_invalid_geom_id();
  }

  void smart_id_locator::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) {
      indent = indent_;
    }
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }
    out_ << indent << datatools::i_tree_dumpable::tag << "Logging priority: " << datatools::logger::get_priority_label(_logging_priority_) << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Initialized: " << _initialized_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Mode:        " << _mode_ << std::endl;
    if (_type_ != geom_id::INVALID_TYPE) {
      out_ << indent << datatools::i_tree_dumpable::tag << "Type:        " << _type_ << std::endl;
    } else {
      out_ << indent << datatools::i_tree_dumpable::tag << "ID selector: " << _idsel_.is_initialized() << std::endl;
    }
    out_ << indent << datatools::i_tree_dumpable::tag << "Geom. map:   " << _gmap_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Geom. infos: " << _ginfos_.size() << " items" << std::endl;
    // for (std::list<const geom_info *>::const_iterator iginfo = _ginfos_.begin();
    //      iginfo != _ginfos_.end();
    //      iginfo++) {
    //   out_ << "|   " << "|-- " << **iginfo;
    //   out_ << std::endl;
    // }
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "Last found:  ";
    if (_last_found_ != 0) {
      out_ << *_last_found_;
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;
    return;
  }

  void smart_id_locator::dump(std::ostream & out_) const
  {
    tree_dump(out_,"geomtools::smart_id_locator::dump: ");
    return;
  }

  void smart_id_locator::print_infos(std::ostream & out_) const
  {
    for (std::list<const geom_info *>::const_iterator iginfo = _ginfos_.begin();
         iginfo != _ginfos_.end();
         iginfo++) {
      out_ << **iginfo << std::endl;
    }
    return;
  }

} // end of namespace geomtools
