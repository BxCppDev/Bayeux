// -*- mode: c++ ; -*-
/* sensitive_hit.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2010-05-26
 * Last modified : 2013-06-03
 *
 * License:
 *
 * Description:
 *   GEANT4 sensitive hit implementation
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_SENSITIVE_HIT_H_
#define MCTOOLS_G4_SENSITIVE_HIT_H_ 1

#include <G4VHit.hh>

#include <mctools/base_step_hit.h>

namespace mctools {

  namespace g4 {

    class sensitive_hit : public G4VHit
    {
    public:

      const mctools::base_step_hit & get_hit_data () const;

      mctools::base_step_hit & grab_hit_data ();

      sensitive_hit ();

      virtual ~sensitive_hit ();

      void reset ();

    private:

      mctools::base_step_hit _hit_data_; //!< Basic MC step hit data

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_SENSITIVE_HIT_H_

// end of sensitive_hit.h
