
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Bi208.h>
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

    // Bi208.f
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

    void Bi208(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double pdecay;
      double tclev;
      double thlev;
      double thnuc;
      double Egamma; // Manually added
      // Scheme of Bi208 decay (NDS 47(1986)797 + ToI-1978).
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 17.12.1995
      thnuc=1.161288E+13;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pdecay=100.*prng_();
      if (pdecay <= 43.6) Egamma=0.088  ;// EC-K 43.6%
      if (pdecay > 43.6 && pdecay <= 83.8) Egamma=0.016  ;// EC-L 40.2%
      if (pdecay > 83.8) Egamma=0.004  ;// EC-M 16.2%
      decay0_gamma(prng_, event_, Egamma,0.,0.,t);
    label_2615  :
      thlev=32.e-12;
      decay0_nucltransK(prng_, event_, 2.615,0.088,8.5e-3,4.3e-4,tclev,thlev,tdlev);
      return;
    }
    // end of Bi208.f




  } // end of namespace decay0
} // end of namespace genbb

// end of Bi208.cc
// Local Variables: --
// mode: c++ --
// End: --
