// -*- mode: c++; -*- 
/* i_genbb.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 
 * 
 * License: 
 * 
 * Description: 
 *   GENBB generator abstract class
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__i_genbb_h
#define __genbb_help__i_genbb_h 1

#include <genbb_help/primary_particle.h>
#include <genbb_help/primary_event.h>

namespace genbb {

  class i_genbb
  {
  public:

    i_genbb ();

    virtual ~i_genbb ();

    virtual bool has_next () = 0;

    virtual void load_next (primary_event & event_, 
			    bool compute_classification_ = true);

  protected:

    virtual void _load_next (primary_event & event_, 
			    bool compute_classification_) = 0;

  };

} // end of namespace genbb

#endif // __genbb_help__i_genbb_h

// end of i_genbb.h
