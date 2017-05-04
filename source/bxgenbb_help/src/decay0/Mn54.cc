
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Mn54.h>
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

    // Mn54.f 
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

    void Mn54(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Mn54 decay ("Table of Isotopes", 8th ed., 1996 + NDS 50(1987)255).
      // Accuracy in description of: decay branches - 0.001%,
      // : deexcitation process - 0.001%.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 16.04.1998.
      // VIT, 1.04.2007, updated to NDS 107(2006)1393.
      thnuc=2.696717E+07;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      // 100% EC to Cr54
      decay0_gamma(prng_, event_, 0.006,0.,0.,t);
      thlev=7.9e-12;
      decay0_nucltransK(prng_, event_, 0.835,0.006,2.4e-4,0.,tclev,thlev,tdlev);
      return;
    }
    // end of Mn54.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Mn54.cc
// Local Variables: --
// mode: c++ --
// End: --
