
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Kr85.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/alpha.h>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/electron.h>
#include <genbb_help/decay0/positron.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/pair.h>
#include <genbb_help/decay0/nucltransK.h>
#include <genbb_help/decay0/nucltransKL.h>
#include <genbb_help/decay0/nucltransKLM.h>
#include <genbb_help/decay0/nucltransKLM_Pb.h>
#include <genbb_help/decay0/beta.h>
#include <genbb_help/decay0/beta1.h>
#include <genbb_help/decay0/beta2.h>
#include <genbb_help/decay0/beta_1fu.h>
#include <genbb_help/decay0/PbAtShell.h>

namespace genbb {
  namespace decay0 {

    // Kr85.f 
    // This file was extracted from the 'decay0' program by V.I. Tretyak
    // Copyright 1995-2011 V.I. Tretyak
    // This program is free software
    // it under the terms of the GNU General Public License as published by
    // the Free Software Foundation
    // your option) any later version.
    // 
    // This program is distributed in the hope that it will be useful, but
    // WITHOUT ANY WARRANTY
    // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    // General Public License for more details.
    // 
    // You should have received a copy of the GNU General Public License
    // along with this program
    // Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
    // 

    void Kr85(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double p;
      double pdecay;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Kr85 decay (NDS 62(1991)271 and ENSDF at NNDC site on 9.12.2007).
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 9.12.2007.
      thnuc=3.394243e8;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pdecay=100.*prng_();
      if (pdecay <= 0.434) goto label_514;
      goto label_10000;
    label_514  :
      decay0_beta(prng_, event_, 0.173,37.,0.,0.,t);
    label_514000  :
      thlev=1.015e-6;
      p=100.*prng_();
      if (p <= 99.99947) goto label_51401;
      goto label_51402;
    label_51401  :
      decay0_nucltransKLM(prng_, event_, 0.514,0.014,6.3e-3,0.002,7.1e-4,0.000,2.3e-4,
			  0.,tclev,thlev,tdlev);
      return;
    label_51402  :
      decay0_nucltransKLM(prng_, event_, 0.363,0.014,2.9e-2,0.002,3.9e-3,0.000,1.3e-3,
			  0.,tclev,thlev,tdlev);
      goto label_15100;
    label_15100  :
      thlev=0.71e-9;
      decay0_nucltransKLM(prng_, event_, 0.151,0.014,4.3e-2,0.002,4.8e-3,0.000,3.4e-4,
			  0.,tclev,thlev,tdlev);
      return;
      // 1st forbidden unique beta decay
    label_10000  :
      decay0_beta2(prng_, event_, 0.687,37.,0.,0.,t,1,1.,0.,0.,0.);
      return;
    }
    // end of Kr85.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Kr85.cc
// Local Variables: --
// mode: c++ --
// End: --
