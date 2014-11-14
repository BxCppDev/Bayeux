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
      double pK = 1. + conveK;
      double pL = pK + conveL;
      double pM = pL + conveM;
      double pp = pM + convp;
      double p = pp * prng();
      bool debug = false;
      if (debug) std::cerr << "genbb::decay0::decay0_nucltransKLM: Egamma=" << Egamma << std::endl;
      if (p <= 1.) {
        if (debug) std::cerr << "genbb::decay0::decay0_nucltransKLM: " << "gamma" << std::endl;
        decay0_gamma(prng,event,Egamma,tclev,thlev,tdlev);
      } else if (p <= pK) {
        decay0_electron(prng,event,Egamma-EbindeK,tclev,thlev,tdlev);
        // decay0_gamma(prng,event,EbindeK,tdlev,0.,tdlev); //  for real time
        decay0_gamma(prng,event,EbindeK,0.,0.,tdlev); //  for time shift
        if (debug) std::cerr << "genbb::decay0::decay0_nucltransKLM: " << "eK+gamma" << std::endl;
      } else if (p <= pL) {
        decay0_electron(prng,event,Egamma-EbindeL,tclev,thlev,tdlev);
        // decay0_gamma(prng,event,EbindeL,tdlev,0.,tdlev); // for real time
        decay0_gamma(prng,event,EbindeL,0.,0.,tdlev); //  for time shift
        if (debug) std::cerr << "genbb::decay0::decay0_nucltransKLM: " << "eL+gamma" << std::endl;
      } else if (p <= pM) {
        decay0_electron(prng,event,Egamma-EbindeM,tclev,thlev,tdlev);
        // decay0_gamma(prng,event,EbindeM,tdlev,0.,tdlev); // for real time
        decay0_gamma(prng,event,EbindeM,0.,0.,tdlev); //  for time shift
        if (debug) std::cerr << "genbb::decay0::decay0_nucltransKLM: " << "eM+gamma" << std::endl;
      } else {
        decay0_pair(prng,event,Egamma-2.*emass,tclev,thlev,tdlev);
        if (debug) std::cerr << "genbb::decay0::decay0_nucltransKLM: " << "pair" << std::endl;
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
