/* prng_state_manager.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#include <sstream>
#include <fstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

#include <mygsl/prng_state_manager.h>

namespace mygsl {

  using namespace std;

  const size_t prng_state_manager::record::DEFAULT_BUFFER_SIZE = 65536;

  prng_state_manager::record::record ()
  {
    state_buffer.reserve (DEFAULT_BUFFER_SIZE);
    return;
  }

  prng_state_manager::record::~record ()
  {
    return;
  }

  /*************************************/

  const int    prng_state_manager::INVALID_COUNTER_VALUE = -1;
  const string prng_state_manager::DEFAULT_FILENAME = "prng_states.data";

  bool prng_state_manager::has_counter () const
  {
    return _counter_ > INVALID_COUNTER_VALUE;
  }

  int prng_state_manager::get_counter () const
  {
    return _counter_;
  }

  void prng_state_manager::set_counter (int c_)
  {
    _counter_ = c_;
    if (_counter_ < INVALID_COUNTER_VALUE) {
      _counter_ = INVALID_COUNTER_VALUE;
    }
    return;
  }

  void prng_state_manager::reset_counter ()
  {
    _counter_ = INVALID_COUNTER_VALUE;
    return;
  }

  void prng_state_manager::increment_counter (unsigned int a_increment)
  {
    if (_counter_ == INVALID_COUNTER_VALUE) {
      _counter_ = 0;
    } else {
      _counter_ += a_increment;
    }
    return;
  }

  bool prng_state_manager::has_filename () const
  {
    return ! _filename_.empty ();
  }

  const string & prng_state_manager::get_filename () const
  {
    return _filename_;
  }

  void prng_state_manager::set_filename (const string & fn_)
  {
    _filename_ = fn_;
    return;
  }

  void prng_state_manager::reset_filename()
  {
    _filename_.clear ();
    return;
  }

  void prng_state_manager::clear ()
  {
    _dict_.clear ();
    return;
  }

  void prng_state_manager::reset ()
  {
    clear ();
    _counter_ = INVALID_COUNTER_VALUE;
    reset_filename();
    reset_error();
    return;
  }

  void prng_state_manager::reset_error()
  {
    _error_code_ = 0;
    return;
  }

  size_t prng_state_manager::size () const
  {
    return _dict_.size ();
  }

  bool prng_state_manager::empty () const
  {
    return _dict_.size () == 0;
  }

  void prng_state_manager::invalidate ()
  {
    for (dict_type::iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++) {
      record & rec = i->second;
      rec.state_buffer.clear ();
    }
    return;
  }

  void prng_state_manager::get_labels (vector<string> & labels_) const
  {
    labels_.clear ();
    labels_.reserve (_dict_.size ());
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++) {
      labels_.push_back (i->first);
    }
    return;
  }

  bool prng_state_manager::has_state (const string & label_) const
  {
    return _dict_.find (label_) != _dict_.end ();
  }

  const prng_state_manager::record & prng_state_manager::get_state (const string & label_) const
  {
    dict_type::const_iterator found = _dict_.find (label_);
    DT_THROW_IF (found == _dict_.end (),
                 logic_error,
                 "No PRNG's state record with label '" << label_ << "' is defined !");
    return found->second;
  }

  prng_state_manager::record & prng_state_manager::get_state (const string & label_)
  {
    dict_type::iterator found = _dict_.find (label_);
    DT_THROW_IF (found == _dict_.end (),
                 logic_error,
                 "No PRNG's state record with label '" << label_ << "' is defined !");
    return found->second;
  }

  void prng_state_manager::add_state (const string & label_, size_t sz_)
  {
    DT_THROW_IF (label_.empty (),logic_error,"Label '" << label_ << "' is empty !");
    DT_THROW_IF (_dict_.find (label_) != _dict_.end (),logic_error, "Label '" << label_ << "' is already used !");
    record rec;
    _dict_[label_] = rec;
    _dict_[label_].state_buffer.reserve (sz_);
    return;
  }

  bool prng_state_manager::error() const
  {
    return _error_code_ != 0;
  }

  void prng_state_manager::_force_error(int e) const
  {
    prng_state_manager * mutable_this = const_cast<prng_state_manager*>(this);
    mutable_this->_error_code_ = e;
    return;
  }

  // ctor:
  prng_state_manager::prng_state_manager ()
  {
    _error_code_ = 0;
    _counter_ = INVALID_COUNTER_VALUE;
    return;
  }

  // ctor:
  prng_state_manager::prng_state_manager (const string & fn_)
  {
    _error_code_ = 0;
    _counter_ = INVALID_COUNTER_VALUE;
    set_filename (fn_);
    return;
  }

  // dtor:
  prng_state_manager::~prng_state_manager ()
  {
    if (has_filename ()) {
      if (! error()) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,"Attempt to store the PRNG state records in file '"
                      << _filename_ << "' !");
        store ();
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,"Automated storage of the PRNG state records in file '"
                      << _filename_ << "' !");
      }
    }
    return;
  }

  void prng_state_manager::load (const string & filename_)
  {
    //bool devel = false;
    //devel = true;
    string fn;
    if (! filename_.empty ()) {
      fn = filename_;
    } else {
      DT_THROW_IF (_filename_.empty (),logic_error,"No file was specified to load the PRNG state records !");
      fn = _filename_;
    }
    datatools::fetch_path_with_env (fn);
    DT_THROW_IF (! boost::filesystem::exists (fn),runtime_error,"File '" << fn << "' does not exist !");
    ifstream ifs (fn.c_str ());
    DT_THROW_IF (! ifs,runtime_error,"Cannot open file '" << fn << "' to load the PRNG state records !");
    this->reset ();
    int counter;
    ifs >> counter;
    // if (devel)
    //   {
    //     clog << datatools::io::devel
    //          << "mygsl::prng_state_manager::load: "
    //          << "counter= " << counter << endl;
    //   }
    DT_THROW_IF (! ifs,runtime_error,"Cannot read the 'counter' value from the '" << fn << "' file !");
    set_counter (counter);
    size_t dict_sz;
    ifs >> dict_sz;
    // if (devel)
    //   {
    //     clog << datatools::io::devel
    //          << "mygsl::prng_state_manager::load: "
    //          << "dict_sz= " << dict_sz << endl;
    //   }
    DT_THROW_IF (! ifs,runtime_error,"Cannot read the dictionary size from the '" << fn << "' file !");
    ifs >> ws;
    for (size_t i = 0; i < dict_sz; i++) {
      string line;
      getline (ifs, line);
      // if (devel)
      //   {
      //     clog << datatools::io::devel
      //          << "mygsl::prng_state_manager::load: "
      //          << "line= '" << line << "'" << endl;
      //   }
      DT_THROW_IF (! ifs,runtime_error,"Cannot read a new PRNG state record from the '" << fn << "' file !");
      istringstream iss (line);
      string label;
      iss >> label;
      // if (devel)
      //   {
      //     clog << datatools::io::devel
      //          << "mygsl::prng_state_manager::load: "
      //          << "label= " << label << endl;
      //   }
      DT_THROW_IF (! iss,runtime_error,"Cannot read the PRNG's label from file '" << fn << "' file !");
      size_t state_sz;
      iss >> state_sz;
      // if (devel)
      //   {
      //     clog << datatools::io::devel
      //          << "mygsl::prng_state_manager::load: "
      //          << "state_sz= " << state_sz << endl;
      //   }
      DT_THROW_IF (! iss,runtime_error,"Cannot read the size of the PRNG internal state from file '" << fn << "' file !");
      add_state (label, state_sz);
      for (size_t i = 0; i < state_sz; i++) {
        unsigned int val;
        iss >> val;
        DT_THROW_IF (! iss,runtime_error,"Cannot read the PRNG internal state byte from file '" << fn << "' file !");
        unsigned char c = static_cast<unsigned char> (val);
        get_state (label).state_buffer.push_back (c);
      }
    }
    return;
  }

  void prng_state_manager::store (const string & filename_) const
  {
    DT_THROW_IF (error(), std::logic_error, "PRNg state manager has error !");
    string fn;
    if (! filename_.empty ()) {
      fn = filename_;
    } else {
      // if (_filename_.empty ()) {
      //   fn = "/tmp/prng_states.save";
      //   DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
      //                  "No file was specified to store the PRNG state records ! "
      //                  << "PRNG state saved in '" << fn << "' !");
      //   // th row logic_error (message.str ());
      // } else {
      fn = _filename_;
      // }
    }
    if (fn.empty ()) return;

    datatools::fetch_path_with_env (fn);
    DT_THROW_IF(fn.empty (), std::runtime_error, "Missing filename !");

    if (boost::filesystem::exists (fn)) {
      string backup_fn = fn + ".~backup~";
      if (boost::filesystem::exists (backup_fn)) {
        boost::filesystem::remove (backup_fn);
      }
      boost::filesystem::rename (fn, backup_fn);
    }

    {
      // Ensure the base directory exists:
      boost::filesystem::path fn_path(fn);
      //std::cerr << "DEVEL : fn_path = " << fn_path << std::endl;
      boost::filesystem::path fn_dir = fn_path.parent_path();
      //std::cerr << "DEVEL : fn_dir = " << fn_dir << std::endl;
      if (! fn_dir.empty()) {
        if (! boost::filesystem::exists(fn_dir)) {
          bool mkdir_success = false;
          std::string errormsg;
          try {
            mkdir_success = boost::filesystem::create_directories(fn_dir);
          }
          catch(exception & x) {
            mkdir_success = false;
            errormsg = x.what();
          }
          if (! mkdir_success) {
            _force_error(1);
            DT_THROW_IF(true, std::runtime_error,
                        "Cannot create directory " << fn_dir << " ! "
                        << errormsg);
          }
          DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                        "Directory " << fn_dir  << " has been created.");
        }
      }
    }

    ofstream ofs (fn.c_str ());
    if (! ofs) {
      _force_error(1);
      DT_THROW_IF (true, std::runtime_error,"Cannot open file '" << fn << "' to store the PRNG state records !");
    }
    ofs << get_counter () << endl;
    ofs << _dict_.size () << endl;
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++) {
      const record & rec = i->second;
      ofs << i->first << ' ';
      ofs << rec.state_buffer.size ();
      for (size_t i = 0; i < rec.state_buffer.size (); i++) {
        unsigned int b = static_cast<unsigned int> (rec.state_buffer[i]);
        ofs << ' ' << b;
      }
      ofs << endl;
    }
    return;
  }

  void prng_state_manager::dump (ostream & out_) const
  {
    out_ << "mygsl::prng_state_manager:" << endl;
    out_ << "|-- " << "Counter : " << _counter_ << endl;
    for (dict_type::const_iterator i = _dict_.begin ();
         i != _dict_.end ();
         i++) {
      out_ << "|-- ";
      out_ << "Generator ";
      out_.width (15) ;
      out_ << i->first << " : ";
      out_.width (10);
      const record & rec = i->second;
      if (rec.state_buffer.size () == 0) {
        out_ << "[no stored internal state]";
      } else {
        out_ << "internal state has " << rec.state_buffer.size () << " bytes";
      }
      out_ << endl;
    }
    out_ << "`-- " << "End" << endl;

    return;
  }

}  // end of namespace mygsl

// end of prng_state_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
