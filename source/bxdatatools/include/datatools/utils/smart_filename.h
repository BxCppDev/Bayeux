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

    //using   namespace std;

    class smart_filename
    {
    public: 

      typedef std::vector<std::string> list_type;
      typedef list_type::const_iterator const_iterator;
 
      bool is_debug () const;
      
      void set_debug (bool);

      int get_mode () const;

      bool is_initialized () const;

      bool is_single () const;

      bool is_list () const;
 
      bool is_incremental () const;
      
      bool is_valid () const;

      size_t size () const;

      const_iterator begin () const;

      const_iterator end () const;

      const std::string & operator[] (int a_index) const;

      // single mode :
      void set (const std::string & a_filename);

      // list mode :
      void add (const std::string & a_filename);

      void reset ();    

    protected:  
     
      void _set_mode (int);
      
      // list:
      void _add_list (const std::string & a_filename);

      void _set_list_allow_duplication (bool);

      // incremental/list:
      void _set_current_index (int);

    public: 

      static const std::string MODE_SINGLE_LABEL;
      static const std::string MODE_LIST_LABEL;
      static const std::string MODE_INCREMENTAL_LABEL;

      enum mode_t
        {
          MODE_INVALID     =  0,
          MODE_SINGLE      =  1,
          MODE_LIST        =  2,
          MODE_INCREMENTAL =  4
        };

      // ctor:
      smart_filename ();
      
      // dtor:
      virtual ~smart_filename ();
    
      static void make_single (smart_filename & a_smart_filename,
                               const std::string & a_filename);
      
      static void make_list (smart_filename & a_smart_filename,
                             bool a_allow_duplication = false);

      static void make_list (smart_filename & a_smart_filename,
                             const std::string & a_list_file, 
                             bool a_allow_duplication = false);

   
      static void make_incremental (smart_filename & a_smart_filename,
                                    const std::string & a_path, 
                                    const std::string & a_prefix,
                                    const std::string & a_extension,
                                    int a_stopping_index,
                                    int a_starting_index = 0,
                                    int a_increment_index = 1);

      void initialize (const properties & a_config);

      void print_list_of_filenames (std::ostream & a_out) const;

      void store_list_of_filenames (const std::string & a_list_filename, 
                                    bool a_append = true) const;

      void dump (std::ostream & a_out = std::clog) const;

    private: 

      bool      _debug_;
      uint32_t  _mode_;
      list_type _list_;
      bool      _list_allow_duplication_;
     
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
