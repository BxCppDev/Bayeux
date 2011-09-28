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
     
    bool smart_filename::is_valid () const
    {
      return is_initialized () && (_list_.size () > 0);
    }
 
    size_t smart_filename::size () const
    {
      return _list_.size ();
    }

    smart_filename::const_iterator smart_filename::begin () const
    {
      return _list_.begin ();
    }

    smart_filename::const_iterator smart_filename::end () const
    {
      return _list_.end ();
    }

    const string & smart_filename::operator[] (int a_index) const
    {
      if (! is_valid ())
        {
          throw logic_error ("datatools::utils::smart_filename::operator[]: Not valid !");
        }
      return _list_.at (a_index);
    }

    void
    smart_filename::_set_mode (int a_new_value)
    {
      _mode_ = a_new_value;
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
  
    void smart_filename::reset ()
    {
      _mode_ = MODE_INVALID;
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
      datatools::utils::fetch_path_with_env (filename);
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
     
    // ctor:
    smart_filename::smart_filename ()
    {
      _debug_ = false;
      reset ();
      return;
    }
  
    // dtor:
    smart_filename::~smart_filename ()
    {
      reset ();
      return;
    }

    // static 
    void smart_filename::make_single (smart_filename & a_smart_filename,
                                      const string & a_filename)
    {
      a_smart_filename.reset ();
      a_smart_filename._set_mode (MODE_SINGLE);
      a_smart_filename._add_list (a_filename);
      return;
    }
      
    void smart_filename::make_list (smart_filename & a_smart_filename,
                                    bool a_allow_duplication)
    {
      a_smart_filename.reset ();
      a_smart_filename._set_mode (MODE_LIST);
      a_smart_filename._set_list_allow_duplication (a_allow_duplication);
      return;
    }

    void smart_filename::make_list (smart_filename & a_smart_filename,
                                    const string & a_list_filename, 
                                    bool a_allow_duplication)
    {
      a_smart_filename.reset ();
      a_smart_filename._set_mode (MODE_LIST);
      a_smart_filename._set_list_allow_duplication (a_allow_duplication);
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
    
    void smart_filename::make_incremental (smart_filename & a_smart_filename,
                                           const string & a_path, 
                                           const string & a_prefix,
                                           const string & a_extension,
                                           int a_stopping_index,
                                           int a_starting_index,
                                           int a_increment_index)
    {
      a_smart_filename.reset ();
      a_smart_filename._set_mode (MODE_INCREMENTAL);
      if (a_prefix.empty ())
        {
          ostringstream message;
          message << "datatools::utils::smart_filename::make_incremental: "
                  << "Missing prefix !";
          throw logic_error (message.str());              
        }
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
          path = a_path;
          datatools::utils::fetch_path_with_env (path);
        }
      if (a_stopping_index < a_starting_index)
        {
          if (a_increment_index >= 0)
            {
              ostringstream message;
              message << "datatools::utils::smart_filename::make_incremental: "
                      << "Invalid increment rule (start=" << a_starting_index
                      << ",stop=" << a_stopping_index << ",increment=" 
                      << a_increment_index << ") !";
              throw logic_error (message.str());              
            }
        }
      int index = a_starting_index;
      int direction = +1;
      if (a_increment_index < 0) direction = -1;
      while ((direction * index) <= (direction * a_stopping_index))
        {
          ostringstream filename_ss;
          if (! path.empty ())
            {
              filename_ss << path;
              if (path[path.length () - 1] != '/')
                {
                  filename_ss << '/';
                }
            }
          filename_ss << a_prefix;
          filename_ss << index;
          if (! a_extension.empty ())
            {
              if (a_extension[0] != '.')
                {
                  filename_ss << '.';           
                }
              filename_ss << a_extension;           
            }
          a_smart_filename._add_list (filename_ss.str ());
          // increment:
          index += a_increment_index;
        }
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
      a_out << "|-- Debug    : " << _debug_ << endl;
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
      a_out << "|-- Mode     : '" << mode_str << "'" << endl;
      if (! is_valid ())
        {
          a_out << "`-- Valid : '" << "No" << "'" << endl;
        }
      else  
        {
          a_out << "|-- Valid : '" << "Yes" << "'" << endl;
          a_out << "`-- List [" << _list_.size () << ']' << endl;
          for (vector<string>::const_iterator i = _list_.begin ();
               i != _list_.end ();
               i++)
            {
              a_out << "    ";
              vector<string>::const_iterator j = i;
              j++;
              if (j != _list_.end ()) a_out << "|-- ";
              else a_out << "`-- ";
              a_out << "Filename '" << *i << "'" << endl;
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
          smart_filename::make_single (*this, single_filename);
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
              smart_filename::make_list (*this, list_file, allow_duplicate);
            }
          else
            {
              vector<string> list_vec;
              if (! a_config.has_key ("list.filenames"))
                {
                  throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'list.filenames' key !");
                }
              smart_filename::make_list (*this, allow_duplicate);
              a_config.fetch ("list.filenames", list_vec);
              for (int i = 0; i < (int) list_vec.size (); i++)
                {
                  this->add (list_vec[i]);
                }
            }         
        }
      else if (mode_str == MODE_INCREMENTAL_LABEL)
        {
          if (! a_config.has_key ("incremental.prefix"))
            {
              throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'incremental.prefix' key !");
            }
          string incremental_prefix = a_config.fetch_string ("incremental.prefix");
          string incremental_path;
          string incremental_extension;
          if (a_config.has_key ("incremental.path"))
            {
              incremental_path = a_config.fetch_string ("incremental.path");
            }
          if (a_config.has_key ("incremental.extension"))
            {
              incremental_extension = a_config.fetch_string ("incremental.extension");
            }
          int incremental_stopping  = 0; 
          int incremental_starting  = 0; 
          int incremental_increment = 1; 
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
              if (incremental_stopping < 0)
                {
                  throw logic_error ("datatools::utils::smart_filename::initialize: Invalid negative stopping value !");
                }
            }
          else
            {
              if (incremental_increment > 0)
                {
                  throw logic_error ("datatools::utils::smart_filename::initialize: Missing 'incremental.stop' key !");
                }
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
                                            incremental_increment);

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
