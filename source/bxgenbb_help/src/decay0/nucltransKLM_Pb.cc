// nucltransKLM_Pb.cc 

#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>

#include <genbb_help/decay0/nucltransKLM_Pb.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/electron.h>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/pair.h>

namespace genbb {
  namespace decay0 {

    void decay0_nucltransKLM_Pb(mygsl::rng & prng, primary_event & event,
                                double Egamma,double EbindeK,double conveK,
                                double EbindeL,double conveL,
                                double EbindeM,double conveM,
                                double convp,double tclev,double thlev,
                                double & tdlev)
    {
      static double emass = decay0_emass();
      double p=(1.+conveK+conveL+conveM+convp)*prng();
      if (p <= 1.) {
        decay0_gamma(prng,event,Egamma,tclev,thlev,tdlev);
      }
      else if (p <= 1.+conveK) {
        decay0_electron(prng,event,Egamma-EbindeK,tclev,thlev,tdlev);
        // decay0_gamma(Eprng,event,bindeK,tdlev,0.,tdlev)             // for real time
        //         decay0_gamma(prng,event,EbindeK,0.,0.,tdlev)                   // for time shift
        // emission of two X rays with E(X1)+E(X2)=EbindeK instead of one 
        // with E(X)=EbindeK; probabilities are from ToI'98, Appendix F.5.a (Table 7a).
        // VIT, 4.02.2009.
        double p1=100.*prng();
        // 73.9% - E(X1)=74 keV, 21.3% - E(X1)=85 keV
        if (p1 <= 73.9) {
          decay0_gamma(prng,event,0.074,0.,0.,tdlev);                
          decay0_gamma(prng,event,0.014,0.,0.,tdlev);
        }
        else if (p1 <= 95.2) {
          decay0_gamma(prng,event,0.085,0.,0.,tdlev);                
          decay0_gamma(prng,event,0.003,0.,0.,tdlev);                
        }
        // in 4.8% few low energy particles are emitted; they are neglected
      }                     
      else if (p <= 1.+conveK+conveL) {
        decay0_electron(prng,event,Egamma-EbindeL,tclev,thlev,tdlev);
        //         decay0_gamma(prng,event,EbindeL,tdlev,0.,tdlev);                // for real time
        decay0_gamma(prng,event,EbindeL,0.,0.,tdlev);                   // for time shift
      }
      else if (p <= 1.+conveK+conveL+conveM) {
        decay0_electron(prng,event,Egamma-EbindeM,tclev,thlev,tdlev);
        //         decay0_gamma(prng,event,EbindeM,tdlev,0.,tdlev)           ;     // for real time
        decay0_gamma(prng,event,EbindeM,0.,0.,tdlev);                   // for time shift
      }
      else
        {
          decay0_pair(prng,event,Egamma-2.*emass,tclev,thlev,tdlev);
        }
      return;
    }
  } // end of namespace decay0 
} // end of namespace genbb 
// 
// end of nucltransKLM_Pb.cc
//
// Local Variables: --
// mode: c++ --
// End: --
