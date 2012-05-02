/* smart_filename.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-12
 * Last modified : 2011-09-12
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
 * Description: 
 *
 *   Automated incremental filename
 * 
 * History: 
 * 
 */

#ifndef __datatools__datatools__utils__smart_filename_h
#define __datatools__datatools__utils__smart_filename_h 1

#include <iostream>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

#include <datatools/utils/properties.h>

namespace datatools {

  namespace utils {

    class smart_filename
    {
    public: 

      typedef std::vector<std::string>  list_type;
      typedef list_type::const_iterator const_iterator;
 
      bool is_debug () const;
      
      void set_debug (bool);

      int get_mode () const;

      bool is_initialized () const;

      bool is_single () const;

      bool is_list () const;
 
      bool is_incremental () const;
      
      bool is_valid () const;

      bool is_ranged () const;

      bool is_expand_path () const;

      size_t size () const;

      size_t current_size () const;

      const_iterator begin () const;

      const_iterator end () const;

      const std::string & operator[] (int a_index) const;

      const std::string & get_filename (int a_index) const;

      bool has_filename (const std::string & filename_, 
                         bool expand_path_ = true) const;

      // single mode :
      void set (const std::string & a_filename);

      // single mode :
      void set_single (const std::string & a_filename);

      // list mode :
      void add (const std::string & a_filename);

      // list mode :
      void add_to_list (const std::string & a_filename);

      void reset ();  

      void build_incremental_filename (int increment_index_, std::string & filename_) const;

    protected:  
     
      void _set_mode (int);
      
      // list:
      void _add_list (const std::string & a_filename);

      void _set_list_allow_duplication (bool);

      // incremental/list:
      void _set_current_index (int);

    public: 

      static const int         MODE_INCREMENTAL_UNRANGED          = -1;
      static const int         MODE_INCREMENTAL_DEFAULT_START     =  0;
      static const int         MODE_INCREMENTAL_DEFAULT_INCREMENT =  1;
      static const std::string MODE_SINGLE_LABEL;
      static const std::string MODE_LIST_LABEL;
      static const std::string MODE_INCREMENTAL_LABEL;

      enum mode_t
        {
          MODE_INVALID     =  0,
          MODE_SINGLE      =  0x1,
          MODE_LIST        =  0x2,
          MODE_INCREMENTAL =  0x4
        };

      // ctor:
      smart_filename ();
      
      // dtor:
      virtual ~smart_filename ();
    
      static void make_single (smart_filename & a_smart_filename,
                               const std::string & a_filename,
                               bool a_expand_path = true);
      
      static void make_list (smart_filename & a_smart_filename,
                             bool a_allow_duplication = false,
                             bool a_expand_path = true);

      static void make_list (smart_filename & a_smart_filename,
                             const std::string & a_list_file, 
                             bool a_allow_duplication = false,
                             bool a_expand_path = true);

   
      static void make_incremental (smart_filename & a_smart_filename,
                                    const std::string & a_path, 
                                    const std::string & a_prefix,
                                    const std::string & a_extension,
                                    int a_stopping_index,
                                    int a_starting_index = 0,
                                    int a_increment_index = 1,
                                    const std::string & a_suffix = "",
                                    int a_incremental_index_ndigit = 0,
                                    bool a_expand_path = true);
    
      static void make_unranged_incremental (smart_filename & a_smart_filename,
                                             const std::string & a_path, 
                                             const std::string & a_prefix,
                                             const std::string & a_extension,
                                             int a_starting_index = 0,
                                             int a_increment_index = 1,
                                             const std::string & a_suffix = "",
                                             int a_incremental_index_ndigit = 0,
                                             bool a_expand_path = true);
 
      void initialize (const properties & a_config);

      void print_list_of_filenames (std::ostream & a_out = std::clog) const;

      void store_list_of_filenames (const std::string & a_list_filename, 
                                    bool a_append = true) const;

      void dump (std::ostream & a_out = std::clog) const;

    private: 

      bool        _debug_;
      uint32_t    _mode_;
      bool        _expand_path_;
      list_type   _list_;
      bool        _list_allow_duplication_;
      bool        _ranged_;
      std::string _incremental_path_;
      std::string _incremental_prefix_;
      std::string _incremental_suffix_;
      std::string _incremental_extension_;
      int32_t     _incremental_starting_index_;
      int32_t     _incremental_stopping_index_;
      int32_t     _incremental_increment_;
      uint32_t    _incremental_index_ndigit_;

    };

  }  // end of namespace utils

}  // end of namespace datatools

#endif // __datatools__datatools__utils__smart_filename_h

// end of smart_filename.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
