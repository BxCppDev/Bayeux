/* cut_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-22
 * Last modified : 2011-06-22
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
 *   A factory class for cut.
 * 
 * History: 
 * 
 */

#ifndef __cuts__cut_manager_h
#define __cuts__cut_manager_h 1

#include <string>

#include <boost/cstdint.hpp>

#include <datatools/utils/i_tree_dump.h>

#include <cuts/cut_tools.h>
#include <cuts/cut_factory.h>

// Forward declaration :
namespace datatools {
  namespace utils {
    class properties;
    class multi_properties;
  }
  namespace service {
    class service_manager;
  }
}

namespace cuts {
  
  class cut_manager : public datatools::utils::i_tree_dumpable

  {
  public:
    enum ctor_flag_type
      {
        BLANK             = 0x0,
        DEBUG             = 0x1,
        FACTORY_DEBUG     = 0x2,
        FACTORY_NOPRELOAD = 0x4,
        VERBOSE           = 0x8
      };
  
    bool is_debug () const;
        
    void set_debug (bool a_debug);
  
    bool is_verbose () const;
        
    void set_verbose (bool a_verbose);
 
    bool is_no_preload () const;

    bool has (const string & a_cut_name) const;
  
    void remove (const string & a_cut_name);

    i_cut & get (const string & a_cut_name);

    const i_cut & get (const string & a_cut_name) const;

    cut_handle_dict_type & get_cuts ();

    const cut_handle_dict_type & get_cuts () const;

    bool is_initialized () const;

    void initialize (const datatools::utils::properties & a_setup);

    void reset ();

    cut_manager (uint32_t the_flags = BLANK);

    virtual ~cut_manager ();

    const cut_factory & get_factory () const;

    bool has_service_manager () const;

    const datatools::service::service_manager & get_service_manager () const;

    datatools::service::service_manager & get_service_manager ();

    void set_service_manager (datatools::service::service_manager & a_service_manager);

    void install_service_manager (const datatools::utils::properties & a_service_manager_configuration);

    virtual void tree_dump (ostream & a_out         = clog, 
                            const string & a_title  = "",
                            const string & a_indent = "",
                            bool a_inherit          = false) const;

  protected:
        
    void load_cuts_ (const datatools::utils::multi_properties & a_cuts_config);

  private:

    bool                 initialized_; //!< Initialization status
    uint32_t             flags_;       //!< Some flags
    cut_factory        * factory_;     //!< Handle to the embedded cut factory
    cut_handle_dict_type cuts_;        //!< Dictionnary of cuts
    bool                                  service_manager_owner_; //!< Owner flag for the embedded service manager
    datatools::service::service_manager * service_manager_;       //!< Handle to the embedded service manager

  };

}  // end of namespace cuts

#endif // __cuts__cut_manager_h

// end of cut_fmanager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
