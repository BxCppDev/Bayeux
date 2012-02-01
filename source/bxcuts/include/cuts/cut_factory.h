/* cut_factory.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-09
 * Last modified : 2011-06-09
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
 *   A factory class for event record processing cut.
 * 
 * History: 
 * 
 */

#ifndef __cuts_cut_factory_h
#define __cuts_cut_factory_h 1

#include <cuts/cut_tools.h>

#include <datatools/utils/i_tree_dump.h>

namespace cuts {

  class i_cut;

  using namespace std;

  class cut_factory : public datatools::utils::i_tree_dumpable
  {
  public: 
        
    enum flag_type 
      {
        blank      = 0x0,
        debug      = 0x1,
        no_preload = 0x2,
        verbose    = 0x4
      };

    bool is_locked () const;
        
    void set_locked (bool a_locked);

    bool is_debug () const;
        
    void set_debug (bool a_debug);

    bool is_verbose () const;
        
    void set_verbose (bool a_verbose);
        
    // ctor:
    cut_factory (unsigned int a_flag = blank);
        
    // dtor:
    virtual ~cut_factory ();
 
    void do_register (const cut_creator_type & a_cut_creator, 
                      const string & a_cut_id = "");
    
    // Main cut factory function :
    virtual cut_handle_type create_cut (const string & a_cut_id,
                                        const datatools::utils::properties & a_cut_configuration,
                                        datatools::service::service_manager & a_service_manager, 
                                        cut_handle_dict_type & a_cut_dict);
        
    const cut_creator_dict_type & get_creators () const;   

    cut_creator_dict_type & get_creators ();   

    virtual void tree_dump (ostream & a_out         = clog, 
                            const string & a_title  = "",
                            const string & a_indent = "",
                            bool a_inherit          = false) const;

  public: 

    static bool g_devel;

  private: 

    bool _debug_;
    bool _verbose_;
    bool _locked_;

  protected:

    cut_creator_dict_type _creators;

  };

}  // end of namespace cuts

#endif // __cuts_cut_factory_h

// end of cut_factory.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
