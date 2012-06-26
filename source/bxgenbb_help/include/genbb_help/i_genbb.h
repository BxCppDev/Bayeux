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

#ifndef __genbb_help__i_genbb_h
#define __genbb_help__i_genbb_h 1

#include <genbb_help/primary_event.h>

namespace mygsl {
  class rng;
}

namespace genbb {

  /// GENBB generator abstract class 
  class i_genbb
  {
  public:

    /// Constructor
    i_genbb ();

    /// Destructor
    virtual ~i_genbb ();

    /// Returns true if the generator can provide one more generated event 
    virtual bool has_next () = 0;

    /// Load a new 'primary_event' object
    virtual void load_next (primary_event & event_, 
                            bool compute_classification_ = true);

    virtual bool can_external_random () const;

    virtual bool has_external_random () const;

    virtual void set_external_random (mygsl::rng &);

    virtual void reset_external_random ();

    mygsl::rng & grab_external_random ();
 
    const mygsl::rng & get_external_random () const;
 
  protected:

    /// Protected abstract interface to be invoked by the public 'load_next' method
    virtual void _load_next (primary_event & event_, 
                             bool compute_classification_) = 0;

 
  private:

    mygsl::rng * _external_random_; /// Handle to an external PRNG

  };

} // end of namespace genbb

#endif // __genbb_help__i_genbb_h

// end of i_genbb.h
