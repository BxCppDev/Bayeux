
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Sr90.h>
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

    // Sr90.f 
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

    void Sr90(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double thnuc;
      // Scheme of Sr90 decay ("Table of Isotopes", 7th ed., 1978).
      // Accuracy in description of: decay branches - 0.001%,
      // : deexcitation process - 0.001%.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // Slight update to NDS 82(1997)379.
      // VIT, 9.08.1993, 22.10.1995, 26.10.2006.
      thnuc=0.9085184E+09;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      // decay0_beta(prng_, event_, 0.546,39.,0.,0.,t)
      // Change from the allowed shape to the 1st forbidden unique with empirical
      // correction from: H.H.Hansen, Appl. Rad. Isot. 34(1983)1241
      decay0_beta_1fu(prng_, event_, 0.546,39.,0.,0.,t,0.,-0.032,0.,0.);
      return;
    }
    // end of Sr90.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Sr90.cc
// Local Variables: --
// mode: c++ --
// End: --
