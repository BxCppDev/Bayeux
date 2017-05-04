
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/At214.h>
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

    // At214.f 
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

    void At214(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double palpha;
      double tclev;
      double thlev;
      double thnuc;
      // Model for scheme of At214 decay (NDS 99(2003)649 and ENSDF at NNDC site
      // on 9.12.2007).
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 9.12.2007.
      thnuc=558.e-9;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      palpha=100.*prng_();
      if (palpha <= 0.32) goto label_563;
      if (palpha <= 0.90) goto label_348;
      if (palpha <= 1.05) goto label_320;
      goto label_10000;
    label_563  :
      decay0_alpha(prng_, event_, 8.267,0.,0.,t);
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.563,0.091,8.6e-2,0.,tclev,thlev,tdlev);
      return;
    label_348  :
      decay0_alpha(prng_, event_, 8.478,0.,0.,t);
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.348,0.091,7.9e-2,0.,tclev,thlev,tdlev);
      return;
    label_320  :
      decay0_alpha(prng_, event_, 8.505,0.,0.,t);
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.320,0.091,3.9e-1,0.,tclev,thlev,tdlev);
      return;
    label_10000  :
      decay0_alpha(prng_, event_, 8.819,0.,0.,t);
      return;
    }
    // end of At214.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of At214.cc
// Local Variables: --
// mode: c++ --
// End: --
