/* skip_module.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-19
 * Last modified : 2013-02-15
 * 
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A skip data record processing module.
 * 
 * History: 
 * 
 */

#ifndef DPP_SKIP_MODULE_H_
#define DPP_SKIP_MODULE_H_ 1

#include <dpp/base_module.h>
#include <dpp/module_macros.h>

namespace dpp {

  /// \brief A data processing module to skip some data records
  class skip_module : public base_module
  //DPP_MODULE_CLASS_DECLARE(skip_module)
  {
  public:
        
    enum mode_type
    {
      SKIP_MODE_INVALID  = 0,
      SKIP_MODE_INTERVAL = 1,
      MODE_DEFAULT = SKIP_MODE_INTERVAL
    };

    /// \brief Internal class of the skip_module class
    struct module_entry
    {
      std::string        label;
      module_handle_type handle;
    };

  public:

    int get_first () const;

    int get_last () const;

    bool is_inverted () const;

    void set_first_last (int a_first,
                         int a_last,
                         bool a_inverted = false);

    void set_first_number (int a_first,
                           int a_number,
                           bool a_inverted = false);

    // Constructor :
    skip_module (int a_debug_level = NO_DEBUG); 

    // Destructor :
    virtual ~skip_module ();

    /** The module interface can also be declared with the macro :
     *   \code
     *   DPP_MODULE_INTERFACE (skip_module);
     *   \endcode
     */
    // Initialization method :
    virtual void initialize (const datatools::properties &,
                             datatools::service_manager &,
                             module_handle_dict_type &);

    // Termination method :
    virtual void reset ();

    // Event processing method :
    virtual int process (datatools::things &);  

  protected:

    void _set_defaults ();

  private:

    module_entry       _module_;   //!< The module
    int                _counter_;
    int                _first_;
    int                _last_;
    bool               _inverted_;

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(skip_module);

  };

}  // end of namespace dpp

#endif // DPP_SKIP_MODULE_H_

// end of skip_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
