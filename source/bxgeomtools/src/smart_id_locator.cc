// -*- mode: c++ ; -*-
/* smart_id_locator.cc
 */

#include <geomtools/smart_id_locator.h>

#include <geomtools/geom_map.h>

#include <sstream>
#include <stdexcept>

#include <datatools/utils/ioutils.h>

namespace geomtools {

  using namespace std;

  void smart_id_locator::set_debug (bool d_)
  {
    _debug_ = d_;
    return;
  }

  bool smart_id_locator::is_debug () const
  {
    return _debug_;
  }

  const list<const geom_info *> & smart_id_locator::get_ginfos () const
  {
    return _ginfos_;
  }

  void smart_id_locator::_construct ()
  {
    for (geom_info_dict_t::const_iterator i
           = _gmap_->get_geom_infos ().begin ();
         i != _gmap_->get_geom_infos ().end ();
         i++)
      {
        uint32_t gtype = i->first.get_type ();
        bool valid = false;

        // search matching type IDs:
        if (_type_ != geom_id::INVALID_TYPE)
          {
            if (gtype == _type_)
              {
                valid = true;
              }
          }
        else if (_idsel_.is_initialized ())
          {
            if (_idsel_.match (i->first))
              {
                valid = true;
              }
          }
        else
          {
            ostringstream message;
            message << "geomtools::smart_id_locator::_construct: "
                    << "Missing criterion for building the smart list of IDs !";
            throw logic_error (message.str ());
          }

        if (valid)
          {
            const geom_info * ginfo = &(i->second);
            _ginfos_.push_back (ginfo);
          }
      }

    if (_type_ != geom_id::INVALID_TYPE)
      {
        if (is_debug ())
          {
            clog << datatools::utils::io::debug
                 << "geomtools::smart_id_locator::_construct: "
                 << "Number of entries with type (" << _type_ << ") = "
                 << _ginfos_.size ()
                 << endl;
          }
      }
    if (_idsel_.is_initialized ())
      {
        if (is_debug ())
          {
            clog << datatools::utils::io::debug
                 << "geomtools::smart_id_locator::_construct: "
                 << "Number of entries selected by the ID selector = "
                 << _ginfos_.size ()
                 << endl;
          }
      }
    return;
  }

  void smart_id_locator::set_gmap (const geom_map & gmap_)
  {
    if (is_initialized ())
      {
        ostringstream message;
        message << "geomtools::smart_id_locator::set_gmap: Operation prohibited ! "
                << "Locator is already initialized !";
        throw logic_error (message.str ());
      }
    _gmap_ = &gmap_;
    return;
  }

  bool smart_id_locator::is_initialized () const
  {
    return _mode_ != MODE_INVALID;
  }

  void smart_id_locator::reset ()
  {
    if (! is_initialized ())
      {
        ostringstream message;
        message << "geomtools::smart_id_locator::reset: "
                << "Locator is not initialized !";
        throw logic_error (message.str ());
      }
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _idsel_.reset ();
    _last_found_ = 0;
    _ginfos_.clear ();
    _initialized_ = false;
    return;
  }

  void smart_id_locator::initialize (const string & rules_,
                                     int mode_)
  {
    if (is_initialized ())
      {
        ostringstream message;
        message << "geomtools::smart_id_locator::initialize: "
                << "Locator is already initialized !";
        throw logic_error (message.str ());
      }
    if (_gmap_ == 0)
      {
        ostringstream message;
        message << "geomtools::smart_id_locator::initialize: "
                << "Missing geoemtry map ! Use the 'set_gmap' method before !";
        throw logic_error (message.str ());
      }
    _idsel_.set_id_mgr (_gmap_->get_id_manager ());
    try
      {
        _idsel_.initialize (rules_);
      }
    catch (exception & x)
      {
        ostringstream message;
        message << "geomtools::smart_id_locator::initialize: "
                << "ID selector failed: " << x.what ();
        throw logic_error (message.str ());
      }

    _mode_ = mode_;
    _last_found_ = 0;

    _construct ();
    _initialized_ = true;

    return;
  }

  void smart_id_locator::initialize (uint32_t type_,
                                     int mode_)
  {
    if (is_initialized ())
      {
        ostringstream message;
        message << "geomtools::smart_id_locator::initialize: "
                << "Locator is already initialized !";
        throw logic_error (message.str ());
      }

    if (type_ == geom_id::INVALID_TYPE)
      {
        ostringstream message;
        message << "geomtools::smart_id_locator::_construct: "
                << "Invalid geometry info type (" << type_ << ")";
        throw logic_error (message.str ());
      }

    _mode_ = mode_;
    _type_ = type_;
    _last_found_ = 0;

    _construct ();
    _initialized_ = true;

    return;
  }

  // ctor:
  smart_id_locator::smart_id_locator ()
  {
    _debug_ = false;
    _initialized_ = false;
    _gmap_ = 0;
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _last_found_ = 0;
    return;
  }

  // ctor:
  smart_id_locator::smart_id_locator (const geom_map & gmap_)
  {
    _debug_ = false;
    _initialized_ = false;
    _gmap_ = 0;
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _last_found_ = 0;

    set_gmap (gmap_);
    return;
  }

  // ctor:
  smart_id_locator::smart_id_locator (const geom_map & gmap_,
                                      uint32_t type_,
                                      int mode_)
  {
    _debug_ = false;
    _initialized_ = false;
    _gmap_ = 0;
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _last_found_ = 0;

    set_gmap (gmap_);
    initialize (type_, mode_);
    return;
  }

  // ctor:
  smart_id_locator::smart_id_locator (const geom_map & gmap_,
                                      const string & selection_rules_,
                                      int mode_)
  {
    _debug_ = false;
    _initialized_ = false;
    _gmap_ = 0;
    _mode_ = MODE_INVALID;
    _type_ = geom_id::INVALID_TYPE;
    _last_found_ = 0;

    set_gmap (gmap_);
    initialize (selection_rules_, mode_);
    return;
  }

  // dtor:
  smart_id_locator::~smart_id_locator ()
  {
    if (is_initialized ())
      {
        reset ();
      }
    return;
  }

  bool smart_id_locator::validate_id (const geom_id & id_) const
  {
    return _gmap_->validate_id (id_);
  }

  const geom_info & smart_id_locator::get_geom_info (const geom_id & id_) const
  {
    return _gmap_->get_geom_info (id_);
  }

  const geom_id & smart_id_locator::get_geom_id (const vector_3d & world_position_,
                                                 int type_,
                                                 double tolerance_) const
  {

    if (_mode_ & MODE_LAST)
      {
        // first search if one matched the last found object :
        if (_last_found_ != 0)
          {
            bool located = false;
            located = geom_map::check_inside (*_last_found_,
                                              world_position_,
                                              tolerance_);
            if (located)
              {
                if (is_debug ())
                  {
                    clog << datatools::utils::io::debug
                         << "geomtools::smart_id_locator::get_geom_id: "
                         << "Last found entry match ("
                         << _last_found_->get_id () << ")"
                         << endl;
                  }
                return _last_found_->get_id ();
              }
          }
      }

    for (list<const geom_info *>::const_iterator i
           = _ginfos_.begin ();
         i != _ginfos_.end ();
         i++)
      {
        const geom_info * ginfo = *i;
        if (_last_found_ != 0)
          {
            // no need to check twice the last found location:
            if (ginfo == _last_found_)
              {
                continue;
              }
          }
        bool located = false;
        located = geom_map::check_inside (*ginfo,
                                          world_position_,
                                          tolerance_);
        if (located)
          {
            if (_mode_ & MODE_LAST)
              {
                smart_id_locator * non_const_this = const_cast<smart_id_locator *> (this);
                non_const_this->_last_found_ = ginfo;
              }
            if (is_debug ())
              {
                clog << datatools::utils::io::debug
                     << "geomtools::smart_id_locator::get_geom_id: "
                     << "New entry match ("
                     << ginfo->get_id () << ")"
                     << endl;
              }
            return ginfo->get_id ();
          }
      }

    //
    return _gmap_->get_invalid_geom_id ();
  }

  void smart_id_locator::dump (ostream & out_) const
  {
    out_ << "geomtools::smart_id_locator::dump: " << endl;
    out_ << "|-- Debug:       " << _debug_ << endl;
    out_ << "|-- Initialized: " << _initialized_ << endl;
    out_ << "|-- Mode:        " << _mode_ << endl;
    if (_type_ != geom_id::INVALID_TYPE)
      {
        out_ << "|-- Type:        " << _type_ << endl;
      }
    else
      {
        out_ << "|-- ID selector: " << _idsel_.is_initialized () << endl;
      }
    out_ << "|-- Geom. map:   " << _gmap_ << endl;
    out_ << "|-- Geom. infos: " << _ginfos_.size () << " items" << endl;
    out_ << "`-- Last found:  ";
    if (_last_found_ != 0)
      {
        out_ << *_last_found_;
      }
    else
      {
        out_ << "<none>";
      }
    out_ << endl;
    return;
  }

} // end of namespace geomtools

// end of smart_id_locator.cc
