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
#include <list>

#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>

#include <datatools/datatools_config.h>
#include <datatools/utils/handle.h>
#include <datatools/utils/bit_mask.h>

#include <datatools_sys/DynamicLoader.hxx>

namespace datatools {

  namespace utils {

    struct library_entry_type;

    typedef datatools::utils::handle<library_entry_type>      handle_library_entry_type;
    typedef std::map<std::string, handle_library_entry_type>  handle_library_entry_dict_type;
    typedef std::list<handle_library_entry_type>              handle_library_entry_stack_type;

    typedef DATATOOLS_SYS_NAMESPACE::DynamicLoader::SymbolPointer symbol_ptr;

    struct library_entry_type
    {
      std::string name;
      std::string directory;
      std::string filename;
      std::string full_path;
      std::string version;
      bool   autoload;
      int    status;
      DATATOOLS_SYS_NAMESPACE::DynamicLoader::LibraryHandle handle;

      library_entry_type (const std::string & name_      = "",
                          const std::string & directory_ = "",
                          const std::string & filename_  = "",
                          const std::string & full_path_ = "",
                          const std::string & version_   = "",
                          bool autoload_                 = true);

      virtual ~library_entry_type ();

      void print (std::ostream & out_ = std::clog, const std::string & indent_ = "") const;

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

      uint32_t    _flags_; //!< Not used yet
      std::string _config_filename_;
      handle_library_entry_stack_type _stacked_libraries_;
      handle_library_entry_dict_type  _libraries_;

    public:

      bool is_debug () const;

      void set_debug (bool);

      void set_allow_unregistered (bool);

      bool allowing_unregistered () const;

      bool is_test () const;

      library_loader (uint32_t flags_ = 0,
                      const std::string & dll_config_file_ = "");

      virtual ~library_loader ();

      bool has (const std::string & lib_name_) const;

      bool is_loaded (const std::string & lib_name_) const;

      int registration (const std::string & lib_name_,
                        const std::string & lib_directory_ = "",
                        const std::string & lib_filename_  = "",
                        const std::string & lib_full_path_ = "",
                        const std::string & lib_version_   = "",
                        bool                autoload_      = false);

      int load (const std::string & lib_name_,
                const std::string & lib_directory_ = "",
                const std::string & lib_filename_  = "",
                const std::string & lib_full_path_ = "",
                const std::string & lib_version_   = "");

      int close (const std::string & lib_name_);

      int close_all ();

      void print (std::ostream & out_ = std::clog) const;

      symbol_ptr get_symbol_address (const std::string & lib_name_,
                                     const std::string & symbol_);

    protected:

      void _init ();

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
