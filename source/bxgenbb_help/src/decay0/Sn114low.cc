
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Sn114low.h>
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

    // Sn114low.f
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

    void Sn114low(mygsl::rng & /* prng_ */,
                  genbb::primary_event & /* event_ */,
                  int levelkev)
    {
      //double t;
      //double tdlev;
      ////double tclev;
      // Subroutine describes the deexcitation process in Sn114 nucleus
      // after 2b-decay of Cd114 to ground and excited 0+ and 2+ levels
      // of Sn114 ("Table of Isotopes", 7th ed., 1978).
      // Call : call Sn114low(levelkev)
      // Input : levelkev - energy of Sn114 level (integer in keV) occupied
      // initially
      // 0+(gs) - 0 keV.
      // Output: // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 30.11.1995.
      ////tclev=0.;
      if (levelkev ==  0) goto label_10000;
      goto label_20000;
    label_10000  :
      return;
    label_20000  :
      // print *,'Sn114: wrong level [keV] ',levelkev
      return;
    }
    // end of Sn114low.f




  } // end of namespace decay0
} // end of namespace genbb

// end of Sn114low.cc
// Local Variables: --
// mode: c++ --
// End: --
