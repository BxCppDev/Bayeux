// -*- mode: c++ ; -*-
/* base_io.cc
 */

#include <brio/detail/base_io.h>

#include <stdexcept>
#include <sstream>

#include <boost/archive/codecvt_null.hpp>
#include <boost/filesystem.hpp>

#include <datatools/exception.h>

#include <TFile.h>

namespace brio {
  namespace detail {

    using namespace std;

    const string base_io::PBA_LABEL  = "pba";
    const string base_io::TEXT_LABEL = "text";

    void base_io::set_logging_priority(datatools::logger::priority p)
    {
      _logging_priority_ = p;
    }

    datatools::logger::priority base_io::get_logging_priority() const
    {
      return _logging_priority_;
    }

    // static :
    int base_io::get_format (const string & format_str_)
    {
      if (format_str_ == PBA_LABEL) return FORMAT_PBA;
      if (format_str_ == TEXT_LABEL) return FORMAT_TEXT;
      DT_THROW_IF(true,logic_error,"Invalid format label '" << format_str_ << "' !");
    }

    int base_io::get_format () const
    {
      return _format_;
    }

    void base_io::set_format (int format_)
    {
      DT_THROW_IF (is_opened (),std::logic_error,"Operation prohibited; file is opened !");
      _format_ = format_;
      if (is_format_text ()) {
        if (_default_locale == 0) {
          _default_locale = new std::locale (std::locale::classic (),
                                             new boost::archive::codecvt_null<char>);
        }
        if (_locale == 0) {
          if (_rw == RW_READ) {
            _locale = new std::locale (*_default_locale,
                                       new boost::math::nonfinite_num_get<char>);
          }
          if (_rw == RW_WRITE) {
            _locale = new std::locale (*_default_locale,
                                       new boost::math::nonfinite_num_put<char>);
          }
        }
      }
      return;
    }

    void base_io::set_format (const string & format_str_)
    {
      set_format (base_io::get_format (format_str_));
      return;
    }

    bool base_io::is_debug () const
    {
      return _logging_priority_ >= datatools::logger::PRIO_DEBUG;
    }

    void base_io::set_debug (bool new_value_)
    {
      if (new_value_) _logging_priority_ = datatools::logger::PRIO_DEBUG;
      else _logging_priority_ = datatools::logger::PRIO_FATAL;
      return;
    }

    bool base_io::is_verbose () const
    {
      return _logging_priority_ >= datatools::logger::PRIO_NOTICE;
    }

    void base_io::set_verbose (bool new_value_)
    {
      if (new_value_) _logging_priority_ = datatools::logger::PRIO_NOTICE;
      else _logging_priority_ = datatools::logger::PRIO_FATAL;
      return;
    }

    bool base_io::is_reading () const
    {
      return _rw == RW_READ;
    }

    bool base_io::is_writing () const
    {
      return _rw == RW_WRITE;
    }

    bool base_io::is_format_pba () const
    {
      return _format_ == FORMAT_PBA;
    }

    bool base_io::is_format_text () const
    {
      return _format_ == FORMAT_TEXT;
    }

    bool base_io::is_opened () const
    {
      return (_file != 0) && _file->IsOpen ();
    }

    void base_io::close ()
    {
      DT_LOG_TRACE(get_logging_priority(),"Entering...");
      DT_THROW_IF (! is_opened (), std::logic_error, "Not opened !");
      if (_file != 0) {
        _file->cd ();
        if (is_writing ()) _file->Write ();
        _file->Close ();
      }
      base_io::_reset ();
      DT_LOG_TRACE(get_logging_priority(),"Exiting.");
      return;
    }

    void base_io::open (const string & filename_)
    {
      DT_LOG_TRACE(get_logging_priority(),"Entering with filename '" << filename_ << "'");
      DT_THROW_IF (is_opened (),std::logic_error,"Already opened !");
      if (_format_ == FORMAT_UNDEFINED) {
        DT_LOG_NOTICE(get_logging_priority(),"Guessing the archive format from the filename !");
        string file_extension = boost::filesystem::extension (filename_);
        if (file_extension == store_info::TRIO_FILE_EXTENSION) {
          set_format (FORMAT_TEXT);
          DT_LOG_NOTICE(get_logging_priority(),"Using '" << TEXT_LABEL << "' archive format !");
        } else {
          set_format (FORMAT_PBA);
          DT_LOG_NOTICE(get_logging_priority(),"Using '" << PBA_LABEL << "' archive format !");
        }
      }
      _at_open (filename_);
      DT_LOG_TRACE(get_logging_priority(),"Exiting.");
      return;
    }

    bool base_io::has_automatic_store () const
    {
      return has_store (store_info::AUTOMATIC_STORE_LABEL);
    }

    void base_io::select_automatic_store ()
    {
      DT_THROW_IF (! has_automatic_store (),
                   std::logic_error,
                   "No automatic store (with label '"
                   << store_info::AUTOMATIC_STORE_LABEL << "') is available !");
      select_store (store_info::AUTOMATIC_STORE_LABEL);
      return;
    }

    bool base_io::has_store (const string & label_) const
    {
      // accelerator:
      if (_current_store != 0) {
        if (_current_store->label == label_) return true;
      }
      // else search the dictionnary:
      store_info_dict_type::const_iterator found = _store_infos.find (label_);
      return found != _store_infos.end ();
    }

    bool base_io::has_store_with_serial_tag (const string & label_, const string & serial_tag_) const
    {
      store_info_dict_type::const_iterator found = _store_infos.find (label_);
      if (found == _store_infos.end ()) return false;
      const store_info & the_si = found->second;
      if (the_si.has_dedicated_serialization_tag ()) {
        if (the_si.get_serialization_tag () == serial_tag_) return true;
      }
      return false;
    }

    bool base_io::has_mixed_store (const string & label_) const
    {
      store_info_dict_type::const_iterator found = _store_infos.find (label_);
      if (found == _store_infos.end ()) return false;
      const store_info & the_si = found->second;
      if (! the_si.has_dedicated_serialization_tag ()) {
        return true;
      }
      return false;
    }

    const string & base_io::get_serialization_tag (const string & label_) const
    {
      DT_THROW_IF (! is_opened (),std::logic_error,"Operation prohibited; file is not opened !");
      const store_info * si = _get_store_info (label_);
      DT_THROW_IF (si == 0,std::logic_error,"No store with label '" << label_ << "' !");
      return si->serialization_tag;
    }

    int64_t base_io::get_number_of_entries (const string & label_) const
    {
      DT_THROW_IF (! is_opened (),std::logic_error,"Operation prohibited; file is not opened !");
      const store_info * si = _get_store_info (label_);
      DT_THROW_IF (si == 0,std::logic_error,"No store with label '" << label_ << "' !");
      return si->number_of_entries;
    }

    int64_t base_io::get_current_entry (const string & label_) const
    {
      DT_THROW_IF (! is_opened (),std::logic_error,"Operation prohibited; file is not opened !");
      const store_info * si = _get_store_info (label_);
      DT_THROW_IF (si == 0,std::logic_error,"No store with label '" << label_ << "' !");
      return si->current_entry;
    }

    void base_io::unselect_store ()
    {
      _current_store = 0;
    }

    void base_io::select_store (const string & label_)
    {
      DT_THROW_IF (! is_opened (),std::logic_error,"Operation prohibited; file is not opened !");
      store_info * si = _get_store_info (label_);
      DT_THROW_IF (si == 0,std::logic_error,"No store with label '" << label_ << "' nor default store available !");
      _current_store = si;
      return;
    }

    void base_io::_set_default ()
    {
      _logging_priority_ = datatools::logger::PRIO_FATAL;
      _format_ = FORMAT_UNDEFINED;
      _file = 0;
      _filename= "";
      _current_store = 0;
      _locale = 0;
      _default_locale = 0;
      return;
    }

    // ctor:
    base_io::base_io (int rw_)
    {
      _rw = rw_;
      _format_ = FORMAT_UNDEFINED;
      base_io::_set_default ();
      _default_locale = 0;
      _locale = 0;
      return;
    }

    // ctor:
    base_io::base_io (int rw_, datatools::logger::priority p_)
    {
      _rw = rw_;
      _format_ = FORMAT_UNDEFINED;
      base_io::_set_default ();
      set_logging_priority(p_);
      _default_locale = 0;
      _locale = 0;
      return;
    }

    // ctor:
    base_io::base_io (int rw_, int format_ , datatools::logger::priority p_)
    {
      _rw = rw_;
      _format_ = FORMAT_UNDEFINED;
      base_io::_set_default ();
      set_logging_priority(p_);
      _default_locale = 0;
      _locale = 0;
      set_format (format_);
      return;
    }

    // dtor:
    base_io::~base_io ()
    {
      reset();
      return;
    }

    void base_io::reset()
    {
      if (_locale != 0) {
        delete _locale;
        _locale = 0;
      }
      if (_default_locale != 0) {
        delete _default_locale;
        _default_locale = 0;
      }
      base_io::_reset ();
    }

    void base_io::_reset ()
    {
      if (_file != 0) {
        if (_file->IsOpen ()) {
          // 2012-12-19, FM: explicit write
          if (is_writing ()) _file->Write ();
          _file->Close ();
        }
        // 2012-12-19, FM: explicit delete the TFile
        delete _file;
        _file = 0;
      }
      if (! _filename.empty ()) {
        _filename.clear ();
      }
      _store_infos.clear();
      base_io::_set_default ();
      return;
    }


    void base_io::get_list_of_stores (list<string> & list_) const
    {
      list_.clear ();
      for (store_info_dict_type::const_iterator i = _store_infos.begin ();
           i != _store_infos.end ();
           i++) {
        list_.push_back (i->first);
      }
      return;
    }

    store_info * base_io::_get_store_info (const string & label_)
    {
      if (label_.empty ()) {
        // if label is not specified, use the current store if it exists:
        if (_current_store != 0) {
          return _current_store;
        }
      } else {
        // first we check the current store:
        if ((_current_store != 0) && (_current_store->label == label_)) {
          return _current_store;
        }
        // then we search from the stores' dictionnary :
        store_info_dict_type::iterator found = _store_infos.find (label_);
        if (found != _store_infos.end ()) {
          return &(found->second);
        }
      }
      return 0;
    }

    const store_info * base_io::_get_store_info (const string & label_) const
    {
      const store_info * c_ptr_si = 0;
      base_io * ptr_io = const_cast<base_io *> (this);
      store_info * ptr_si = ptr_io->_get_store_info (label_);
      c_ptr_si = const_cast<store_info *> (ptr_si);
      return c_ptr_si;
    }

    void base_io::tree_dump (std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
    {
      using namespace datatools;
      string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty ()) {
        out_ << indent << title_ << endl;
      }

      out_ << indent << i_tree_dumpable::tag << "Logging : " << _logging_priority_  << endl;

      // 2011-06-16 FM: restored
      out_ << indent << i_tree_dumpable::tag << "Format: '"
           << (_format_ == FORMAT_PBA ? base_io::PBA_LABEL : base_io::TEXT_LABEL)
           << "'" << endl;

      out_ << indent << i_tree_dumpable::tag
           << "Opened: " << is_opened () << endl;
      if (is_opened ()) {
        out_ << indent << i_tree_dumpable::skip_tag
             << i_tree_dumpable::tag
             << "Filename: '" << _filename << "'" << endl;
        out_ << indent << i_tree_dumpable::skip_tag
             << i_tree_dumpable::last_tag
             << "File: " << hex << _file << dec << " (ROOT TFile)" << endl;
      }

      out_ << indent << i_tree_dumpable::tag << "Stores: ";
      if (_store_infos.size () > 0) {
        out_ << _store_infos.size ();
      } else {
        out_ << "<none>";
      }
      out_ << endl;
      for (store_info_dict_type::const_iterator i = _store_infos.begin ();
           i != _store_infos.end ();
           i++) {
        const string & label = i->first;
        const store_info & si = i->second;
        store_info_dict_type::const_iterator j = i;
        j++;
        string tag = "|-- ";
        string skip_tag = "|   ";

        if (j ==  _store_infos.end ()) {
          tag = "`-- ";
          skip_tag = "    ";
        }
        out_ << indent << "|   " << tag << "Store label: '" << si.label << "' : " << endl;
        out_ << indent << "|   " << skip_tag << "|-- " << "Serialization tag = ";
        if (si.has_dedicated_serialization_tag ()) {
          out_ << indent << "'" << si.get_serialization_tag () << "'";
        } else {
          out_ << "<mixed>";
        }
        out_ << endl;
        out_ << indent << "|   " << skip_tag << "|-- " << "Number of entries = " << si.number_of_entries << " " << endl;
        out_ << indent << "|   " << skip_tag << "`-- " << "Current entry     = ";
        if (si.current_entry < 0) {
          out_ << "<rewind>";
        } else if (si.current_entry >= si.number_of_entries) {
          out_ << "<unwind>";
        } else {
          out_ << '#' << si.current_entry << " in [ 0 : " << (si.number_of_entries - 1) << " ]";
        }
        out_ << endl;
      }

      {
        out_ << indent << i_tree_dumpable::inherit_tag (inherit_) << "Current store: ";
        if (_current_store == 0) {
          out_ << "<none>";
        } else {
          out_ << "'" << _current_store->label << "'";
        }
        out_ << endl;
      }

      return;
    }

  } // end of namespace detail
} // end of namespace brio

// end of base_io.cc
