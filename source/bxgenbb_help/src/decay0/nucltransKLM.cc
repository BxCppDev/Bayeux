// nucltransKLM.cc

#include <genbb_help/decay0/nucltransKLM.h>

#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/electron.h>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/pair.h>

namespace genbb {
  namespace decay0 {

    void decay0_nucltransKLM(mygsl::rng & prng, primary_event & event,
                             double Egamma,
                             double EbindeK,double conveK,
                             double EbindeL,double conveL,
                             double EbindeM,double conveM,
                             double convp,double tclev,double thlev,
                             double & tdlev)
    {
      static double emass = decay0_emass();
      double p=(1.+conveK+conveL+conveM+convp)*prng();
      // p=100000000.3; // XXX
      // std::cout << " ***   ";
      // std::cout.precision(8);
      // std::cout.width (9);
      // //std::cout.fill ('0');
      // std::cout.flags ( std::ios::left );
      // std::cout << p << "    " << std::endl;
      if (p <= 1.) {
        decay0_gamma(prng,event,Egamma,tclev,thlev,tdlev);
      }
      else if (p <= 1.+conveK) {
        decay0_electron(prng,event,Egamma-EbindeK,tclev,thlev,tdlev);
        //         decay0_gamma(prng,event,EbindeK,tdlev,0.,tdlev);                //  for real time
        decay0_gamma(prng,event,EbindeK,0.,0.,tdlev);                   //  for time shift
      }
      else if (p <= 1.+conveK+conveL) {
        decay0_electron(prng,event,Egamma-EbindeL,tclev,thlev,tdlev);
        //         decay0_gamma(prng,event,EbindeL,tdlev,0.,tdlev);                // for real time
        decay0_gamma(prng,event,EbindeL,0.,0.,tdlev);                   //  for time shift
      }
      else if (p <= 1.+conveK+conveL+conveM)
        {
          decay0_electron(prng,event,Egamma-EbindeM,tclev,thlev,tdlev);
          //         decay0_gamma(prng,event,EbindeM,tdlev,0.,tdlev);                // for real time
          decay0_gamma(prng,event,EbindeM,0.,0.,tdlev);                   //  for time shift
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
// end of nucltransKLM.cc
//
// Local Variables: --
// mode: c++ --
// End: --
