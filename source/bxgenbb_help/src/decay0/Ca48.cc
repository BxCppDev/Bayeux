
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Ca48.h>
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

    // Ca48.f 
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

    void Ca48(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Ca48 beta decay.
      // It is supposed that decay goes to excited 5+ level of Sc48 (E_exc=131 keV)
      // with T1/2=1.1e21 y calculated in M.Aunola et al., Europhys. Lett. 46(1999)577
      // (transition to g.s. is suppressed by factor of ~1e-10
      // with E_exc=252 keV - by factor of ~1e-5).
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 07.05.1998
      thnuc=3.47e28;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      decay0_beta(prng_, event_, 0.151,21.,0.,0.,t);
      tclev=0.;
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.131,0.004,8.1e-3,0.,tclev,thlev,tdlev);
      return;
    }
    // end of Ca48.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Ca48.cc
// Local Variables: --
// mode: c++ --
// End: --
