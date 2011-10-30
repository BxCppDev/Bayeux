/* i_cut.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2011-06-07
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
 *   Base cut.
 * 
 * History: 
 * 
 */

#ifndef __cuts__i_cut_h
#define __cuts__i_cut_h 1

#include <ostream>
#include <string>

#include <cuts/cut_tools.h>

#include <boost/cstdlib.hpp>

#include <datatools/utils/i_tree_dump.h>

namespace datatools {
  namespace utils {
    class properties;
  }}

namespace datatools {
  namespace service {
    class service_manager;
  }}

namespace cuts {

  using namespace std;

  class i_cut : public datatools::utils::i_tree_dumpable
  {
  public:
        
    enum result_type
      {
        INAPPLICABLE = -1, //!< Returned by the 'process' method when applying some cut makes no sense in the current context
        REJECTED     = 0, //!< Returned by the 'process' method when some cut is not passed 
        ACCEPTED     = 1 //!< Returned by the 'process' method when some cut is passed 
      };

  public: 

    bool is_debug () const;
    
    void set_debug (bool);

    int get_debug_level () const;
    
    void set_debug_level (int);
    
    const string & get_name () const;
    
    const string & get_description () const;
    
    void set_description (const string & a_description);

    const string & get_version () const;
   
    void set_version (const string & a_version);
 
    bool is_initialized () const;
  
    bool has_user_data () const;

    virtual void set_user_data (void *);

    virtual void unset_user_data ();
 
    int operator() ();
   
  protected:

    void _set_name (const string & a_name);

    void _set_initialized (bool a_initialized);

    void * _get_user_data () const;

    virtual void _prepare_cut ();

    virtual void _finish_cut ();

		// Pure virtual selection method  (invoked by process)
    virtual int _accept () = 0; 

  public:
   
    virtual void initialize_standalone (const datatools::utils::properties & a_config);

    virtual void initialize_with_service (const datatools::utils::properties & a_config,
                                          datatools::service::service_manager & a_service_manager);

    /** The main initialization method (post-construction):
     * @param a_config the container of configuration parameters
     * @param a_cut_dictionnary a dictionnary of existing 'cuts' which can be used
     *        to build the current cut (used in the framework of a factory)
     * @param a_service_manager a manager for external services
     */

    virtual void initialize (const datatools::utils::properties & a_config,
                             datatools::service::service_manager & a_service_manager,
                             cut_handle_dict_type & a_cut_dictionnary) = 0;
      
    /** The main cut processing method
     * @return the status 
     */
    virtual int process ();

    /** The main termination method :
     */
    virtual void reset ();
      
  public: 

    // ctor:
    i_cut (const string & a_cut_name, 
           const string & a_cut_description = "", 
           const string & a_cut_version = "", 
           int           a_debug_level = 0);
    
    // dtor:
    virtual ~i_cut ();

    virtual void tree_dump (ostream & a_out         = clog, 
                            const string & a_title  = "",
                            const string & a_indent = "",
                            bool a_inherit          = false) const;

    void print (ostream & a_out = clog) const;
        

  public:

    static bool g_devel;

  private: 

    string _name;           //!< The name of the cut
    
    string _description;    //!< The description of the cut
    
    string _version;        //!< The version of the cut

		//private:

    bool   _initialized_;    //!< The initialization flag

    int    _debug_level_;    //!< The debug level of the cut

    void * _user_data_;
    
  };
      
  /* Utility to enable auto-registering of a Cut class
   * in the global dictionary:
   *
   * The templatized class 'Cut' must inherits from 'i_cut':
   */
  template <class Cut>
  struct default_creator_registration
  {
  public:
    static i_cut * 
    g_default_cut_creator (const datatools::utils::properties & a_cut_config, 
                           datatools::service::service_manager & a_service_manager,
                           cut_handle_dict_type & a_cut_dictionary)
    {
      Cut * new_cut = new Cut; 
      new_cut->initialize (a_cut_config,
                           a_service_manager, 
                           a_cut_dictionary);
      return new_cut;
    }
        
    // ctor :
    default_creator_registration (const string & a_cut_id) 
    {
      cut_tools::get_cut_creator_db ().register_cut_creator (default_creator_registration<Cut>::g_default_cut_creator,
                                                             a_cut_id);
      return;
    }
        
  };
  
}  // end of namespace cuts

#endif // __cuts__i_cut_h

// end of i_cut.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
