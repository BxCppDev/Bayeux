
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Fe58low.h>
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

    // Fe58low.f 
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

    void Fe58low(mygsl::rng & prng_, genbb::primary_event & event_, int levelkev)
    {
      //double t;
      double tdlev;
      double p;
      double tclev;
      double thlev;
      // Subroutine describes the deexcitation process in Fe58 nucleus
      // after 2b-decay of Ni58 to ground and excited 0+ and 2+ levels
      // of Fe58 ("Table of Isotopes", 7th ed., 1978).
      // Call : call Fe58low(levelkev)
      // Input : levelkev - energy of Fe58 level (integer in keV) occupied
      // initially
      // 0+(gs) - 0 keV,
      // 2+(1) - 811 keV,
      // 2+(2) - 1675 keV.
      // Output: // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 30.11.1995.
      tclev=0.;
      if (levelkev == 1675) goto label_1675;
      if (levelkev ==  811) goto label_811;
      if (levelkev ==  0) goto label_10000;
      goto label_20000;
    label_1675  :
      thlev=0.;
      p=100.*prng_();
      if (p <= 43.) goto label_16751;
      goto label_16752;
    label_16751  :
      decay0_nucltransK(prng_, event_, 1.675,0.007,1.0e-4,0.6e-4,tclev,thlev,tdlev);
      return;
    label_16752  :
      decay0_nucltransK(prng_, event_, 0.864,0.007,3.0e-4,0.,tclev,thlev,tdlev);
      goto label_811;
    label_811  :
      thlev=9.0e-12;
      decay0_nucltransK(prng_, event_, 0.811,0.007,5.0e-4,0.,tclev,thlev,tdlev);
      return;
    label_10000  :
      return;
    label_20000  :
      // print *,'Fe58: wrong level [keV] ',levelkev
      return;
    }
    // end of Fe58low.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Fe58low.cc
// Local Variables: --
// mode: c++ --
// End: --
