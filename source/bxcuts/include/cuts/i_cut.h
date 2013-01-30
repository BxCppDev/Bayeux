/* i_cut.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2012-06-08
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

#ifndef _CUTS_I_CUT_H
#define _CUTS_I_CUT_H 1

#include <ostream>
#include <string>

#include <cuts/cut_tools.h>
#include <datatools/bit_mask.h>

#include <boost/cstdlib.hpp>

#include <datatools/i_tree_dump.h>
#include <datatools/factory_macros.h>

/// Main datatools namespace (cuts depends on datatools)
namespace datatools {
    class properties;
    class service_manager;
  }

/// Main cuts namespace
namespace cuts {

  /// \brief The cut abstract base class
  class i_cut : public datatools::i_tree_dumpable
  {
  public:
        
    enum result_type
      {
        INAPPLICABLE = -1, //!< Returned by the 'process' method when applying some cut that makes no sense in the current context
        REJECTED     = 0, //!< Returned by the 'process' method when some cut is not passed 
        ACCEPTED     = 1 //!< Returned by the 'process' method when some cut is passed 
      };

  public: 

    bool is_debug () const;
    
    void set_debug (bool);

    int get_debug_level () const;
    
    void set_debug_level (int);
    
    const std::string & get_name () const;
 
    bool has_description () const;
   
    const std::string & get_description () const;
    
    void set_description (const std::string & a_description);

    const std::string & get_version () const;
   
    void set_version (const std::string & a_version);
 
    bool is_initialized () const;
  
    bool has_user_data () const;

    virtual void set_user_data (void *);

    virtual void unset_user_data ();
 
    int operator() ();
 
    void set_name (const std::string & a_name);
  
  protected:

    void _lock_guard (const std::string & where_, const std::string & what_ = "") const;

    void _set_name (const std::string & a_name);

    void _set_initialized (bool a_initialized);

    void * _get_user_data () const;

    virtual void _prepare_cut ();

    virtual void _finish_cut ();

    // Pure virtual selection method  (invoked by process)
    virtual int _accept () = 0; 

  public:
   
    virtual void initialize_simple ();
   
    virtual void initialize_standalone (const datatools::properties & a_config);

    virtual void initialize_with_service_only (const datatools::properties & a_config,
                                               datatools::service_manager & a_service_manager);

    virtual void initialize_without_service (const datatools::properties & a_config,
                                             cut_handle_dict_type & a_cut_dictionnary);


    /** The main initialization method (post-construction):
     * @param a_config the container of configuration parameters
     * @param a_cut_dictionnary a dictionnary of existing 'cuts' which can be used
     *        to build the current cut (used in the framework of a factory)
     * @param a_service_manager a manager for external services
     */

    virtual void initialize (const datatools::properties & a_config,
                             datatools::service_manager & a_service_manager,
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
    i_cut (const std::string & a_cut_name = "", 
           const std::string & a_cut_description = "", 
           const std::string & a_cut_version = "", 
           int           a_debug_level = 0);
    
    // dtor:
    virtual ~i_cut ();

    virtual void tree_dump (std::ostream & a_out         = std::clog, 
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

    void print (std::ostream & a_out = std::clog) const;
        

  public:

    static bool g_devel;

  private: 

    std::string _name;           //!< The name of the cut
    std::string _description;    //!< The description of the cut   
    std::string _version;        //!< The version of the cut

    //private:

    bool   _initialized_;    //!< The initialization flag
    int    _debug_level_;    //!< The debug level of the cut
    void * _user_data_;

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_cut);
    
  };
 
}  // end of namespace cuts

#include <cuts/cut_macros.h>

#endif // _CUTS_I_CUT_H

// end of i_cut.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
