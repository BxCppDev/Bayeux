/* smart_filename.cc
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 *
 * 
 * 
 */

#include <datatools/utils/smart_filename.h>

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <sstream>
#include <fstream>
#include <stdexcept>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <datatools/utils/utils.h>
#include <datatools/utils/ioutils.h>

namespace datatools {

  namespace utils {

    using namespace std;

    const string smart_filename::MODE_SINGLE_LABEL      =  "single";
    const string smart_filename::MODE_LIST_LABEL        =  "list";
    const string smart_filename::MODE_INCREMENTAL_LABEL =  "incremental";
  
    bool
    smart_filename::is_debug () const
    {
      return _debug_;
    }
  
    void
    smart_filename::set_debug (bool a_new_value)
    {
      _debug_ = a_new_value;
      return;
    }

    int
    smart_filename::get_mode () const
    {
      return _mode_;
    }

    bool smart_filename::is_initialized () const
    {
      return (_mode_ != MODE_INVALID);
    }

    bool smart_filename::is_expand_path () const
    {
      return _expand_path_;
    }

    bool smart_filename::is_single () const
    {
      return (_mode_ == MODE_SINGLE);
    }

    bool smart_filename::is_list () const
    {
      return (_mode_ == MODE_LIST);
    }

    bool smart_filename::is_incremental () const
    {
      return (_mode_ == MODE_INCREMENTAL);
    }
     
    bool smart_filename::is_ranged () const
    {
      return _ranged_;
    }
     
    bool smart_filename::is_valid () const
    {
      if (! is_initialized ()) return false;
      if (is_ranged ()) 
        {
          return _list_.size () > 0;
        }
      return true;
    }
 
    size_t smart_filename::current_size () const
    {
      return _list_.size ();
    }

    bool smart_filename::has_filename (const std::string & filename_, 
                                       bool expand_path_) const
    {
      std::string filename = filename_;
      if (expand_path_)
        {
          datatools::utils::fetch_path_with_env (filename);
        }
      if (! is_initialized ()) 
        {
          return false;
        }
      if (is_ranged () || _list_.size () > 0)
        {
          return find (_list_.begin (), _list_.end (), filename) != _list_.end ();
        }
      return false;
    }
 
    size_t smart_filename::size () const
    {
      if (! is_ranged ())
        {
          throw logic_error ("datatools::utils::smart_filename::size: List of filenames is not ranged ! Size is not known in advance !");         
        }
      return _list_.size ();
    }

    smart_filename::const_iterator smart_filename::begin () const
    {
      return _list_.begin ();
    }

    smart_filename::const_iterator smart_filename::end () const
    {
      if (! _ranged_)
        {
          throw logic_error ("datatools::utils::smart_filename::end: List of filenames is not ranged (incremental mode with no stopping index) !");
        }
      return _list_.end ();
    }

    const std::string & smart_filename::operator[] (int a_index) const
    {
      return get_filename (a_index);
    }

    const std::string & smart_filename::get_filename (int a_index) const
    {
      if (! is_valid ())
        {
          throw logic_error ("datatools::utils::smart_filename::get_filename: Not valid !");
        }
      if (a_index < 0)
        {
          throw range_error ("datatools::utils::smart_filename::get_filename: Index is not valid !");
        }
      if (is_incremental () && ! is_ranged () && (a_index >= _list_.size ()))
        {
          smart_filename * mutable_this = const_cast<smart_filename*>(this);
          for (int i = _list_.size (); i <= a_index; i++)
            {
              string filename;
              int file_number = _incremental_starting_index_ +  _incremental_increment_ * a_index;
              build_incremental_filename (file_number, filename);
              // std::clog << "DEVEL: " << "datatools::utils::smart_filename::operator[]: " 
              //           << "Adding file '" << filename << "'..."
              //           << std::endl;
              mutable_this->_add_list (filename);
            }
        }
      else
        {
          if (a_index < 0 || a_index > _list_.size ())
            {
              throw range_error ("datatools::utils::smart_filename::get_filename: Index is not valid !");
            }
        }
      return _list_[a_index];
    }

    void
    smart_filename::_set_mode (int a_new_value)
    {
      _mode_ = a_new_value;
      if (_mode_ == MODE_SINGLE)
        {
          _ranged_ = true;
        }
      else if (_mode_ == MODE_LIST)
        {
          _ranged_ = true;
        }
      else if (_mode_ == MODE_INCREMENTAL)
        {
          _ranged_ = false;
          if (_incremental_stopping_index_ > MODE_INCREMENTAL_UNRANGED)
            {
              _ranged_ = true;
            }
        }
      else 
        {
          _ranged_ = false;
        }
      return;
    }

    void
    smart_filename::set (const string & a_new_value)
    {
      if (! is_single ())
        {
          throw logic_error ("datatools::utils::smart_filename::set: Not using 'single' mode !");
        }
      _list_.clear ();
      _add_list (a_new_value);
      return;
    }
     
    void 
    smart_filename::set_single (const std::string & a_filename)
    {
      set (a_filename);
      return;
    }
  
    void smart_filename::reset ()
    {
      _mode_ = MODE_INVALID;
      _ranged_ = false;
      _expand_path_ = true;
      _incremental_starting_index_ = 0;
      _incremental_stopping_index_ = MODE_INCREMENTAL_UNRANGED;
      _incremental_increment_      = MODE_INCREMENTAL_DEFAULT_INCREMENT;
      _incremental_prefix_ = "";
      _incremental_suffix_ = "";
      _incremental_extension_ = "";
      _incremental_path_ = "";
      _incremental_index_ndigit_ = 0;
      _list_.clear ();
      _list_allow_duplication_ = false;
      return;
    }
    
    void smart_filename::_set_list_allow_duplication (bool a_new_value)
    {
      _list_allow_duplication_ = a_new_value;
      return;
    }
  
    void smart_filename::_add_list (const string & a_filename)
    {
      if (a_filename.empty ())
        {
          throw logic_error ("datatools::utils::smart_filename::_add_list: Missing filename !");
        }
      if (is_single () && (_list_.size () > 0))
        {
          throw logic_error ("datatools::utils::smart_filename::_add_list: Cannot add a filename ('single' mode) !");
        }
      string filename = a_filename;
      if (is_expand_path ())
        {
          datatools::utils::fetch_path_with_env (filename);
        }
      if ((_list_.size () > 0) && ! _list_allow_duplication_)
        { 
          if (std::find (_list_.begin (), _list_.end (), filename) != _list_.end ())
            {
              ostringstream message;
              message << "datatools::utils::smart_filename::_add_list: "
                      << "Duplication error: filename '" 
                      << filename << "' is already in the list !";
              throw logic_error (message.str());              
            }
        }
      _list_.push_back (filename);
      return;
    }
      
    void smart_filename::add (const string & a_filename)
    {
      if (! is_list ())
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::add: "
                  << "Not using 'list' mode !";
          throw logic_error (message.str());                      
        }
      _add_list (a_filename);
      return;
    }
    
    void smart_filename::add_to_list (const std::string & a_filename)
    {
      add (a_filename);
      return;
    }
     
    // Constructor:
    smart_filename::smart_filename ()
    {
      _debug_  = false;
      reset ();
      return;
    }
  
    // Destructor:
    smart_filename::~smart_filename ()
    {
      reset ();
      return;
    }

    // static 
    void smart_filename::make_single (smart_filename & a_smart_filename,
                                      const string & a_filename,
                                      bool a_expand_path)
    {
      a_smart_filename.reset ();
      a_smart_filename._ranged_ = true;
      a_smart_filename._set_mode (MODE_SINGLE);
      a_smart_filename._expand_path_ = a_expand_path;
      a_smart_filename._add_list (a_filename);
      return;
    }
      
    void smart_filename::make_list (smart_filename & a_smart_filename,
                                    bool a_allow_duplication,
                                    bool a_expand_path)
    {
      a_smart_filename.reset ();
      a_smart_filename._ranged_ = true;
      a_smart_filename._set_mode (MODE_LIST);
      a_smart_filename._set_list_allow_duplication (a_allow_duplication);
      a_smart_filename._expand_path_ = a_expand_path;
      return;
    }

    void smart_filename::make_list (smart_filename & a_smart_filename,
                                    const string & a_list_filename, 
                                    bool a_allow_duplication,
                                    bool a_expand_path)
    {
      a_smart_filename.reset ();
      a_smart_filename._ranged_ = true;
      a_smart_filename._set_mode (MODE_LIST);
      a_smart_filename._set_list_allow_duplication (a_allow_duplication);
      a_smart_filename._expand_path_ = a_expand_path;
      string list_filename = a_list_filename;
      datatools::utils::fetch_path_with_env (list_filename);
      if (! boost::filesystem::exists (list_filename))
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::make_list: "
                  << "File '" 
                  << a_list_filename << "' does not exists !";
          throw logic_error (message.str());                      
        }
      if (boost::filesystem::is_directory (list_filename))
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::make_list: "
                  << "Path '" 
                  << list_filename << "' is a directory !";
          throw logic_error (message.str());                      
        }
      ostringstream message;
      message << "datatools::utils::smart_filename::make_list: "
              << "Reading file '" 
              << list_filename << "'...";
      clog << datatools::utils::io::notice << message.str () << endl; 
      ifstream inlist (list_filename.c_str ());
      while (inlist)
        {
          string line;
          getline (inlist, line);
          if (! line.empty())
            {
              istringstream iss (line);
              string word;
              iss >> word;
              // skipping blank lines :
              if (! word.empty ())
                {
                  // comment line :
                  if (word[0] == '#')
                    {
                      continue;
                    }
                  else
                    {
                      // add a filename in the list :
                      a_smart_filename.add (word);
                    }
                }
            }
          if (inlist.eof ()) break;
        }
      return;
    }
    
    void smart_filename::make_unranged_incremental (smart_filename & a_smart_filename,
                                                    const string & a_path, 
                                                    const string & a_prefix,
                                                    const string & a_extension,
                                                    int a_starting_index,
                                                    int a_increment_index,
                                                    const string & a_suffix,
                                                    int a_incremental_index_ndigit,
                                                    bool a_expand_path)
    {
      make_incremental (a_smart_filename, a_path, a_prefix, a_extension, 
                        MODE_INCREMENTAL_UNRANGED, a_starting_index, a_increment_index, 
                        a_suffix, a_incremental_index_ndigit, a_expand_path);
      return;
    }
    
    void smart_filename::make_incremental (smart_filename & a_smart_filename,
                                           const string & a_path, 
                                           const string & a_prefix,
                                           const string & a_extension,
                                           int a_stopping_index,
                                           int a_starting_index,
                                           int a_increment_index,
                                           const string & a_suffix,
                                           int a_incremental_index_ndigit,
                                           bool a_expand_path)
    {
      /*
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: path = " << a_path << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: prefix = " << a_prefix << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: extension = " << a_extension << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: stopping index = " << a_stopping_index << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: starting index = " << a_starting_index << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: increment index = " << a_increment_index << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: suffix = " << a_suffix << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: ndigit = " << a_incremental_index_ndigit << std::endl;
      */
      a_smart_filename.reset ();
      a_smart_filename._set_mode (MODE_INCREMENTAL);
      a_smart_filename._ranged_ = false;
      a_smart_filename._expand_path_ = a_expand_path;
      if (a_prefix.empty ())
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::make_incremental: "
                  << "Missing prefix !";
          throw logic_error (message.str());              
        }
      a_smart_filename._incremental_path_ = a_path;
      a_smart_filename._incremental_prefix_ = a_prefix;
      a_smart_filename._incremental_extension_ = a_extension;
      a_smart_filename._incremental_suffix_ = a_suffix;
      a_smart_filename._incremental_index_ndigit_ = a_incremental_index_ndigit;
      if (a_increment_index == 0)
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::make_incremental: "
                  << "Invalid null increment !";
          throw logic_error (message.str());              
        }
      string path;
      if (! a_path.empty ())
        {
          path = a_smart_filename._incremental_path_;
          if (a_smart_filename.is_expand_path ())
            {
              datatools::utils::fetch_path_with_env (path);
              a_smart_filename._incremental_path_ = path;
            }
        } 
      a_smart_filename._incremental_starting_index_ = a_starting_index;
      a_smart_filename._incremental_stopping_index_ = a_stopping_index;
      a_smart_filename._incremental_increment_ = a_increment_index;
      a_smart_filename._ranged_ = false;   
      if (a_smart_filename._incremental_stopping_index_ <= MODE_INCREMENTAL_UNRANGED)
        {
          a_smart_filename._incremental_stopping_index_ = MODE_INCREMENTAL_UNRANGED;
          if (a_smart_filename._incremental_increment_ < 0)
            {
              a_smart_filename._incremental_stopping_index_ = 0;
              a_smart_filename._ranged_ = true;                 
            }
        }
      else
        {
          a_smart_filename._ranged_ = true;         
        }
      /*
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: starting index  = " << a_smart_filename._incremental_starting_index_ << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: stopping index  = " << a_smart_filename._incremental_stopping_index_ << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: increment index = " << a_smart_filename._incremental_increment_ << std::endl;
      std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: ranged          = " << a_smart_filename._ranged_ << std::endl;
      */
      if (a_smart_filename._incremental_stopping_index_ > MODE_INCREMENTAL_UNRANGED)
        {
          if (a_smart_filename._incremental_stopping_index_ < a_smart_filename._incremental_starting_index_)
            {
              if (a_smart_filename._incremental_increment_ >= 0)
                {
                  ostringstream message;
                  message << "datatools::utils::smart_filename::make_incremental: "
                          << "Invalid increment rule (start=" << a_smart_filename._incremental_starting_index_
                          << ",stop=" << a_smart_filename._incremental_stopping_index_ << ",increment=" 
                          << a_smart_filename._incremental_increment_ << ") !";
                  throw logic_error (message.str());              
                }
            }
        }

      int index = a_smart_filename._incremental_starting_index_;
      int direction = +1;
      if (a_smart_filename._incremental_increment_ < 0) direction = -1;
      if (a_smart_filename.is_ranged ())
        {
          while ((direction * index) <= (direction * a_smart_filename._incremental_stopping_index_))
            {
              //std::clog << "DEVEL: datatools::utils::smart_filename::make_incremental: index=" << index << std::endl;
              std::string filename;
              a_smart_filename.build_incremental_filename (index, filename);
              a_smart_filename._add_list (filename);
              // increment:
              index += a_smart_filename._incremental_increment_;
            }
        }
      a_smart_filename.dump (std::clog);
      return;
    }

    void smart_filename::build_incremental_filename (int increment_index_, std::string & filename_) const
    {
      ostringstream filename_ss;
      if (! _incremental_path_.empty ())
        {
          filename_ss << _incremental_path_;
          if (_incremental_path_[_incremental_path_.length () - 1] != '/')
            {
              filename_ss << '/';
            }
        }
      filename_ss << _incremental_prefix_;
      if (_incremental_index_ndigit_ > 0)
        {
          filename_ss << std::dec << std::setfill ('0') 
                      << std::setw (_incremental_index_ndigit_);
        }
      filename_ss << increment_index_;    
      if (! _incremental_suffix_.empty ())
        {
          filename_ss << _incremental_suffix_;
        }
      if (! _incremental_extension_.empty ())
        {
          if (_incremental_extension_[0] != '.')
            {
              filename_ss << '.';           
            }
          filename_ss << _incremental_extension_;           
        }
      filename_ = filename_ss.str ();
      return;
    }

    void smart_filename::print_list_of_filenames (ostream & a_out) const
    {
      if (! is_initialized ())
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::print_list_of_filenames: "
                  << "Smart file is not initialized !";
          throw logic_error (message.str());                      
        }
      for (vector<string>::const_iterator i = _list_.begin ();
           i != _list_.end ();
           i++)
        {
          a_out << *i << endl;
        }
      return;
    }

    void smart_filename::store_list_of_filenames (const string & a_list_filename, 
                                                  bool a_append) const
    {
      ofstream fout;
      string list_filename = a_list_filename;
      datatools::utils::fetch_path_with_env (list_filename);
      if (a_append)
        {
          fout.open (list_filename.c_str (), ios::app);
        }
      else
        {
          fout.open (list_filename.c_str ());
        }
      if (! fout)
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::store_list_of_filenames: "
                  << "Cannot open file '" << list_filename << "' !";
          throw logic_error (message.str());                      
        }
      if (! a_append)
        {
          fout << "# a list of filenames : " << endl;
        }
      print_list_of_filenames (fout);
      return;
    }

    void smart_filename::dump (ostream & a_out) const
    {
      a_out << "smart_filename::dump : " << endl;
      a_out << "|-- Debug       : " << _debug_ << endl;
      if (! is_initialized ())
        {
          a_out << "`-- Initialized : '" << "No" << "'" << endl;
        }
      else  
        {
          a_out << "|-- Initialized : '" << "Yes" << "'" << endl;
        }
      string mode_str = "";
      if (is_single ()) mode_str = MODE_SINGLE_LABEL;
      if (is_list ()) mode_str = MODE_LIST_LABEL;
      if (is_incremental ()) mode_str = MODE_INCREMENTAL_LABEL;
      a_out << "|-- Mode        : '" << mode_str << "'" << endl;
      a_out << "|-- Ranged      : " << _ranged_ << endl;
      if (! is_valid ())
        {
          a_out << "`-- Valid       : '" << "No" << "'" << endl;
        }
      else  
        {
          a_out << "|-- Valid       : '" << "Yes" << "'" << endl;
          a_out << "`-- Current list [" << _list_.size () << ']' << endl;
          for (int i = 0; i < (int) _list_.size (); i++)
            {
              a_out << "    ";
              int j = i;
              j++;
              if (j != _list_.size ()) a_out << "|-- ";
              else a_out << "`-- ";
              a_out << "Filename '" << _list_[i] << "'" << endl;
            }
        }   
      return;
    }

    void smart_filename::initialize (const properties & a_config)
    {
      if (a_config.has_flag ("debug"))
        {
          set_debug (true);
        }
      
      bool expand_path = true;
      if (a_config.has_flag ("no_expand_path"))
        {
          expand_path = false;
        }

      if (! a_config.has_key ("mode"))
        {
          throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'mode' key !");
        }
      
      string mode_str = a_config.fetch_string ("mode");
      
      if (mode_str == MODE_SINGLE_LABEL)
        {
          if (! a_config.has_key ("single.filename"))
            {
              throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'single.filename' key !");
            }
          string single_filename = a_config.fetch_string ("single.filename");
          smart_filename::make_single (*this, single_filename, expand_path);
        }
      else if (mode_str == MODE_LIST_LABEL)
        {
          bool allow_duplicate = false;
          if (a_config.has_flag ("list.duplicate"))
            {
              allow_duplicate = true;
            }
          if (a_config.has_key ("list.file"))
            {
              string list_file = a_config.fetch_string ("list.file");
              smart_filename::make_list (*this, list_file, allow_duplicate, expand_path);
            }
          else
            {
              vector<string> list_vec;
              if (! a_config.has_key ("list.filenames"))
                {
                  throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'list.filenames' key !");
                }
              smart_filename::make_list (*this, allow_duplicate, expand_path);
              a_config.fetch ("list.filenames", list_vec);
              for (int i = 0; i < (int) list_vec.size (); i++)
                {
                  this->add (list_vec[i]);
                }
            }         
        }
      else if (mode_str == MODE_INCREMENTAL_LABEL)
        {
          string   incremental_prefix = a_config.fetch_string ("incremental.prefix");
          string   incremental_suffix = "";
          string   incremental_path;
          string   incremental_extension;
          int      incremental_stopping  = MODE_INCREMENTAL_UNRANGED; 
          int      incremental_starting  = MODE_INCREMENTAL_DEFAULT_START; 
          int      incremental_increment = MODE_INCREMENTAL_DEFAULT_INCREMENT; 
          uint32_t incremental_index_ndigit = 0; 
          if (! a_config.has_key ("incremental.prefix"))
            {
              throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'incremental.prefix' key !");
            }
          if (a_config.has_key ("incremental.suffix"))
            {
              incremental_suffix = a_config.fetch_string ("incremental.suffix");
            }
          if (a_config.has_key ("incremental.path"))
            {
              incremental_path = a_config.fetch_string ("incremental.path");
            }
          if (a_config.has_key ("incremental.extension"))
            {
              incremental_extension = a_config.fetch_string ("incremental.extension");
            }
          if (a_config.has_key ("incremental.increment"))
            {
              incremental_increment = a_config.fetch_integer ("incremental.increment");
              if (incremental_increment == 0)
                {
                  throw logic_error ("datatools::utils::smart_filename::initialize: Invalid null increment value !");
                }
            }
          if (a_config.has_key ("incremental.stop"))
            {
              incremental_stopping = a_config.fetch_integer ("incremental.stop");
              if (incremental_stopping < -1)
                {
                  throw logic_error ("datatools::utils::smart_filename::initialize: Invalid negative stopping value !");
                }
            }
          else
            {
              // 2012-05-02 FM : allow unranged list of incremented filenames.
              // if (incremental_increment > 0)
              //   {
              //     throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'incremental.stop' key !");
              //   }
            }
          if (a_config.has_key ("incremental.start"))
            {
              incremental_starting = a_config.fetch_integer ("incremental.start");
              if (incremental_starting < 0)
                {
                  throw logic_error ("datatools::utils::smart_filename::initialize: Invalid negative starting value !");
                }
            }
          else
            {
              if (incremental_increment < 0)
                {
                  throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'incremental.start' key !");
                }
            }

          smart_filename::make_incremental (*this, 
                                            incremental_path, 
                                            incremental_prefix, 
                                            incremental_extension,
                                            incremental_stopping,
                                            incremental_starting,
                                            incremental_increment,
                                            incremental_suffix,
                                            incremental_index_ndigit,
                                            expand_path);

        }
      else
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::initialize: "
                  << "Invalid 'mode' key ('" 
                  << mode_str << "') !";
          throw logic_error (message.str());              
        }
        
      return;
    }
 
  }  // end of namespace utils
  
}  // end of namespace datatools

// end of smart_filename.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
