/* datatools/utils/library_loader.h */
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-30
 * Last modified : 2011-06-30
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
 * Description: 
 *
 *  A shared library loader facility (base on kwsys).
 *
 */

#ifndef __datatools__utils__library_loader_h
#define __datatools__utils__library_loader_h 1

#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <list>

#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>

#include <datatools/utils/handle.h>
#include <datatools/utils/bit_mask.h>

#include <kwsys/DynamicLoader.hxx>

namespace datatools {
  
  namespace utils {

    using namespace std;
    
    struct library_entry_type;
    
    typedef datatools::utils::handle<library_entry_type> handle_library_entry_type;
    typedef map<string, handle_library_entry_type>       handle_library_entry_dict_type;
    typedef list<handle_library_entry_type>              handle_library_entry_stack_type;

    typedef kwsys::DynamicLoader::SymbolPointer symbol_ptr;

    struct library_entry_type
    {
      string name;
      string directory;
      string filename;
      string full_path;
      string version;
      bool   autoload;
      int    status;
      kwsys::DynamicLoader::LibraryHandle handle;

      library_entry_type (const string & a_name = "",
                          const string & a_directory = "",
                          const string & a_filename = "",
                          const string & a_full_path = "",
                          const string & a_version = "",
                          bool a_autoload = true);

      virtual ~library_entry_type ();

      void print (ostream & a_out = clog, const string & a_indent = "") const;

    };

    class library_loader : boost::noncopyable 
    {

    public:

      static bool g_devel;
      static bool g_test;
 
      enum flag_type
        {
          debug              = bit_mask::bit00,
          allow_unregistered = bit_mask::bit01,
          auto_all           = bit_mask::bit02,
          auto_none          = bit_mask::bit03,
          test               = bit_mask::bit10
        };

    private:

      uint32_t  flags_; //!< Not used yet
      string    config_filename_;
      handle_library_entry_stack_type stacked_libraries_;
      handle_library_entry_dict_type  libraries_;

    public:

      bool is_debug () const;

      void set_debug (bool);

      void set_allow_unregistered (bool);

      bool allowing_unregistered () const;

      bool is_test () const;

      library_loader (uint32_t the_flags = 0, 
                      const string & dll_config_file = "");

      virtual ~library_loader ();

      bool has (const string & a_lib_name) const;

      bool is_loaded (const string & a_lib_name) const;

      int registration (const string & a_lib_name,
                        const string & a_lib_directory = "",
                        const string & a_lib_filename = "",
                        const string & a_lib_full_path = "",
                        const string & a_lib_version = "",
                        bool           a_autoload = false);

      int load (const string & a_lib_name, 
                const string & a_lib_directory= "", 
                const string & a_lib_filename = "",
                const string & a_lib_full_path = "",
                const string & a_lib_version = "");

      int close (const string & a_lib_name);

      int close_all ();

      void print (ostream & a_out = clog) const;

      symbol_ptr get_symbol_address (const string & a_lib_name,
                                     const string & a_symbol);

    protected:

      void init_ ();

    };

  } // end of namespace utils

} // end of namespace datatools

#endif // __datatools__utils__library_loader_h

/* end of datatools/utils/library_loader.h */
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
