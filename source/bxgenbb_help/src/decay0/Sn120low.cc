
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Sn120low.h>
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

    // Sn120low.f 
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

    void Sn120low(mygsl::rng & prng_, genbb::primary_event & event_, int levelkev)
    {
      //double t;
      double tdlev;
      double tclev;
      double thlev;
      // Subroutine describes the deexcitation process in Sn120 nucleus
      // after 2b-decay of Te120 to ground 0+ and excited 2+ levels
      // of Sn120 ("Table of Isotopes", 7th ed., 1978).
      // Call : call Sn120low(levelkev)
      // Input : levelkev - energy of Sn120 level (integer in keV) occupied
      // initially
      // 0+(gs) - 0 keV,
      // 2+(1) - 1171 keV.
      // Output: // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 13.02.2004.
      tclev=0.;
      if (levelkev == 1171) goto label_1171;
      if (levelkev ==  0) goto label_10000;
      goto label_20000;
    label_1171  :
      thlev=0.642e-12;
      decay0_nucltransK(prng_, event_, 1.171,0.029,8.0e-4,0.,tclev,thlev,tdlev);
      return;
    label_10000  :
      return;
    label_20000  :
      // print *,'Sn120: wrong level [keV] ',levelkev
      return;
    }
    // end of Sn120low.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Sn120low.cc
// Local Variables: --
// mode: c++ --
// End: --
