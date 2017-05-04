
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Pd104low.h>
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

    // Pd104low.f 
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

    void Pd104low(mygsl::rng & prng_, genbb::primary_event & event_, int levelkev)
    {
      //double t;
      double tdlev;
      double tclev;
      double thlev;
      // Subroutine describes the deexcitation process in Pd104 nucleus
      // after 2b-decay of Ru104 to ground and excited 0+ and 2+ levels
      // of Pd104 (NNDC site on 19.05.2009).
      // Call : call Pd104low(levelkev)
      // Input : levelkev - energy of Pd104 level (integer in keV) occupied
      // initially
      // 0+(gs) - 0 keV,
      // 2+(1) - 556 keV.
      // Output: // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 19.05.2009.
      tclev=0.;
      if (levelkev == 556) goto label_556;
      if (levelkev ==  0) goto label_10000;
      goto label_20000;
    label_556  :
      thlev=9.9e-12;
      decay0_nucltransK(prng_, event_, 0.556,0.024,4.5e-3,0.,tclev,thlev,tdlev);
      return;
    label_10000  :
      return;
    label_20000  :
      // print *,'Pd104: wrong level [keV] ',levelkev
      return;
    }
    // end of Pd104low.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Pd104low.cc
// Local Variables: --
// mode: c++ --
// End: --
