
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Tl207.h>
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

    // Tl207.f 
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

    void Tl207(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double p;
      double pbeta;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Tl207 decay ("Table of Isotopes", 7th ed., 1978).
      // Three-figured labels correspond to energies of Pb207
      // excited levels in keV.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 14.08.1992, 22.10.1995
      // VIT, 30.10.2006 (update to NDS 70(1993)315 and correction to the beta shape).
      thnuc=286.2;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pbeta=100.*prng_();
      if (pbeta <= 0.268) goto label_898;  // 0.268%
      goto label_10000;  // 99.732%
    label_898  :
      decay0_beta(prng_, event_, 0.529,82.,0.,0.,t);
      thlev=0.115e-12;
      p=100.*prng_();
      if (p <= 99.29) goto label_89801;  // 99.29%
      goto label_89802;  // 0.71%
    label_89801  :
      decay0_nucltransK(prng_, event_, 0.898,0.088,2.5e-2,0.,tclev,thlev,tdlev);
      return;
    label_89802  :
      decay0_nucltransK(prng_, event_, 0.328,0.088,3.5e-1,0.,tclev,thlev,tdlev);
      goto label_57000;
    label_57000  :
      thlev=130.5e-12;
      decay0_nucltransK(prng_, event_, 0.570,0.088,2.2e-2,0.,tclev,thlev,tdlev);
      return;
      // change to forbidden spectrum with std::experimental correction from
      // J.M.Trischuk et al., NPA 90(1967)33 and H.Daniel, RMP 40(1968)659
      // cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), w=e/emass+1.
    label_10000  :
      decay0_beta1(prng_, event_, 1.427,82.,0.,0.,t,0.,0.024,0.,0.);
      return;
    }
    // end of Tl207.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Tl207.cc
// Local Variables: --
// mode: c++ --
// End: --
