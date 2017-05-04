
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Zr96.h>
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

    // Zr96.f 
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

    void Zr96(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Zr96 beta decay.
      // It is supposed that decay goes to the first excited level (5+) of Nb96
      // (E_exc=44 keV) with T1/2=2.4e20 y in accordance with: H.Heiskanen et al.,
      // JPG 34(2007)837. Transition to the ground state (6+) is suppressed by
      // factor of ~1e-9, to the excited 4+ (E_exc=146 keV) - by 1e-2.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 07.05.1998.
      // VIT, 18.08.2007: update to NDS 68(1993)165 and ENSDF at NNDC site.
      thnuc=7.574e27;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      decay0_beta(prng_, event_, 0.117,41.,0.,0.,t);
      tclev=0.;
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.044,0.019,2.4e0,0.,tclev,thlev,tdlev);
      return;
    }
    // end of Zr96.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Zr96.cc
// Local Variables: --
// mode: c++ --
// End: --
