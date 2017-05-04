
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/dshelp1.h>
#include <genbb_help/decay0/bb.h>
#include <genbb_help/decay0/dshelp2.h>
#include <genbb_help/decay0/dgmlt2.h>

namespace genbb {
  namespace decay0 {
  
    void decay0_dshelp1(int m_, 
                        const double * du1_,
                        double * df1_,
                        double * d_el_,
                        void * params_)
    {
      const bbpars * pars = static_cast<const bbpars *>(params_);
      const double & dens = pars->dens;
      const double & denf = pars->denf;
      for (int i = 0; i < m_; i++) {
        d_el_[0] = du1_[i];
        double d = std::max(0., dens - d_el_[0]);
        df1_[i] = decay0_dgmlt2(decay0_dshelp2, d, denf - d_el_[0],
                                16, 8 , d_el_, params_);
      }
      
      return;
    }

  } // end of namespace decay0 
} // end of namespace genbb 
