// -*- mode: c++; -*- 
/* i_genbb.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 2011-02-13
 * 
 * License: 
 * Copyright 2007-2011 F. Mauger
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
 *   GENBB generator abstract class
 * 
 * History: 
 * 
 */

#ifndef GENBB_HELP_I_GENBB_H_
#define GENBB_HELP_I_GENBB_H_ 1

#include <genbb_help/detail/pg_tools.h>

#include <datatools/factory_macros.h>

namespace datatools {
  class properties;
  class service_manager;
}

namespace mygsl {
  class rng;
}

namespace genbb {

  class primary_event;

  /// \brief GENBB particle generator abstract base class 
  class i_genbb
  {
  public:

    /// Constructor
    i_genbb ();

    /// Destructor
    virtual ~i_genbb ();

    /// Load a new 'primary_event' object
    virtual void load_next (primary_event & event_, 
                            bool compute_classification_ = true);

    virtual bool can_external_random () const;

    virtual bool has_external_random () const;

    virtual void set_external_random (mygsl::rng &);

    virtual void reset_external_random ();

    mygsl::rng & grab_external_random ();
 
    const mygsl::rng & get_external_random () const;
   
    /// Simple initialization (no external resource)
    virtual void initialize_simple ();

    /// Initialization from a container of properties
    virtual void initialize_standalone (const datatools::properties & setup_);

    /// Initialization from a container of properties and a service manager
    virtual void initialize_with_service_only (const datatools::properties & setup_,
                                               datatools::service_manager & service_manager_);

    /// Initialization from a container of properties, a service manager and a dictionnary of vertex generators
    virtual void initialize_with_dictionary_only (const datatools::properties & setup_,
                                                  detail::pg_dict_type & dictionary_);

    /// Check initialization status
    virtual bool is_initialized () const = 0;

    /// Main initialization interface method
    virtual void initialize (const datatools::properties & setup_,
                             datatools::service_manager & service_manager_,
                             detail::pg_dict_type & dictionary_) = 0;

    /// Reset method
    virtual void reset () = 0;

    /// Returns true if the generator can provide one more generated event 
    virtual bool has_next () = 0;
  
  protected:

    /// Protected abstract interface to be invoked by the public 'load_next' method
    virtual void _load_next (primary_event & event_, 
                             bool compute_classification_) = 0;

  private:

    mygsl::rng * _external_random_; /// Handle to an external PRNG

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_genbb);

  };

} // end of namespace genbb

#include <genbb_help/genbb_macros.h>

#endif // GENBB_HELP_I_GENBB_H_

// end of i_genbb.h
