
#include <sstream>
#include <stdexcept>

#include <genbb_help/decay0/Xe130low.h>

#include <mygsl/rng.h>
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

namespace genbb
{
  namespace decay0
  {

    // Xe130low.f 
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

    void Xe130low (mygsl::rng & prng_, genbb::primary_event & event_,
                   int levelkev)
    {
      //double t;
      double tdlev;
      double p;
      double tclev;
      double thlev;
      // Subroutine describes the deexcitation process in Xe130 nucleus
      // after 2b-decay of Te130 to ground 0+ and excited 2+ levels
      // of Xe130 ("Table of Isotopes", 7th ed., 1978).
      // Call : call Xe130low(levelkev)
      // Input : levelkev - energy of Xe130 level (integer in keV) occupied
      // initially
      // 0+(gs) - 0 keV,
      // 2+(1) - 536 keV,
      // 2+(2) - 1122 keV,
      // 0+(1) - 1794 keV.
      // Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
      // VIT, 20.06.1996
      tclev = 0.;
      if (levelkev == 1794)
        goto label_1794;
      if (levelkev == 1122)
        goto label_1122;
      if (levelkev == 536)
        goto label_536;
      if (levelkev == 0)
        goto label_10000;
      goto label_20000;
    label_1794:thlev = 0.;
      p = 100. * prng_ ();
      if (p <= 1.0)
        goto label_17941;     // E0 to gs; ToI-1978
      if (p <= 86.3)
        goto label_17942;
      goto label_17943;
    label_17941:
      p = 100. * prng_ ();
      if (p <= 12.7)
        { /* CARE */            // e0.exe
          decay0_pair (prng_, event_, 0.772, tclev, thlev, tdlev);      // conversion to pair
          /* CARE */
        }
      else 
        {
          decay0_electron (prng_, event_, 1.759, tclev, thlev, tdlev);  // conversion to electron
          decay0_gamma (prng_, event_, 0.035, 0., 0., tdlev);
        } /* CARE */ ;
      return;
    label_17942:
      decay0_nucltransK (prng_, event_, 1.258, 0.035, 1.0e-3,
                         1.5e-5, tclev, thlev, tdlev);
      goto label_536;
    label_17943:
      decay0_nucltransK (prng_, event_, 0.672, 0.035, 4.1e-3,
                         0., tclev, thlev, tdlev);
      goto label_1122;
    label_1122:
      thlev = 0.;
      p = 100. * prng_ ();
      if (p <= 13.3)
        goto label_11221;
      goto label_11222;
    label_11221:
      decay0_nucltransK (prng_, event_, 1.122, 0.035, 1.3e-3,
                         9.0e-7, tclev, thlev, tdlev);
      return;
    label_11222:
      decay0_nucltransK (prng_, event_, 0.586, 0.035, 7.5e-3,
                         0., tclev, thlev, tdlev);
      goto label_536;
    label_536:
      thlev = 7.0e-12;
      decay0_nucltransK (prng_, event_, 0.536, 0.035, 7.4e-3, 0., tclev,
                         thlev, tdlev);
      return;
    label_10000:
      return;
    label_20000:
      {
        std::ostringstream message;
        message << "genbb::decay0::Xe130low: Wrong energy level El="
                << levelkev << " [keV] !";
        throw std::logic_error (message.str());
      }
      return;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 
// end of Xe130low.cc
// Local Variables: --
// mode: c++ --
// End: --
