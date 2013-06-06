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

    /*
    //typedef G4THitsCollection<sensitive_hit> sensitive_hits_collection;

    //#ifdef SNG4_SENSITIVE_HIT_OPTMEM
    extern G4Allocator<sensitive_hit> g_sensitive_hit_allocator;

    inline void * sensitive_hit::operator new (size_t a_size)
    {
      void * a_hit;
      a_hit = (void *) g_sensitive_hit_allocator.MallocSingle ();
      return (a_hit);
    }

    inline void sensitive_hit::operator delete (void * a_hit)
    {
      g_sensitive_hit_allocator.FreeSingle ((sensitive_hit *) a_hit);
      return;
    }
    //#endif // SNG4_SENSITIVE_HIT_OPTMEM
    */

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_SENSITIVE_HIT_H_

// end of sensitive_hit.h
