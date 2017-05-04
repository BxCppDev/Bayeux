
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Na22.h>
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

    // Na22.f
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

    void Na22(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double pdecay;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Na22 decay ("Table of Isotopes", 7th ed., 1978).
      // Accuracy in description of: decay branches - 0.001%,
      // : deexcitation process - 0.001%.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 26.07.1993, 22.10.1995.
      // VIT, 12.11.2006 (updated to NDS 106(2005)1 and change to beta spectra with
      // std::experimental corrections).
      // VIT, 26.08.2007 (corrected beta shapes).
      thnuc=8.2132717e+7;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pdecay=100.*prng_();
      if (pdecay <= 99.944) goto label_1275;
      goto label_10000;
    label_1275  :
      thlev=3.63e-12;
      if (pdecay <= 9.618) {
        decay0_gamma(prng_, event_, 0.001,0.,0.,t);
      }
      // if (pdecay > 9.618) decay0_beta(prng_, event_, 0.545,-10.,0.,0.,t)
      // std::experimental correction to the shape of the beta spectrum from:
      // H.Daniel, RMP 40(1968)659 and H.Wenninger et al., NPA 109(1968)561
      if (pdecay > 9.618) {
        decay0_beta1(prng_, event_, 0.545,-10.,0.,0.,t,1.e-3,0.,0.,0.);
      }
      decay0_nucltransK(prng_, event_, 1.275,0.001,6.8e-6,2.1e-5,tclev,thlev,tdlev);
      return;
      // change to 2nd unique shape in accordance with H.Daniel, RMP 40(1968)659
    label_10000  :
      decay0_beta2(prng_, event_, 1.820,-10.,0.,0.,t,2,3.333333,1.,0.,0.);
      return;
    }
    // end of Na22.f




  } // end of namespace decay0
} // end of namespace genbb

// end of Na22.cc
// Local Variables: --
// mode: c++ --
// End: --
