
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Ra228.h>
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

    // Ra228.f
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

    void Ra228(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double p;
      double pbeta;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Ra228 decay in accordance with NDS 80(1997)723 and
      // ENSDF database at NNDC site on 8.08.2007.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 8.08.2007.
      thnuc=1.814512e8;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pbeta=100.*prng_();
      if (pbeta <= 30.) goto label_331;  // 30%
      if (pbeta <= 50.) goto label_202;  // 20%
      if (pbeta <= 90.) goto label_67;  // 40%
      goto label_63;  // 10%
    label_331  :
      decay0_beta(prng_, event_, 0.0128,89.,0.,0.,t);
      ////label_33100  :
      thlev=0.;
      p=100.*prng_();
      if (p <= 50.) goto label_33101;  // 50%
      goto label_33102;  // 50%
    label_33101  :
      decay0_nucltransK(prng_, event_, 0.0264,0.0198,2.1e2,0.,tclev,thlev,tdlev);
      goto label_67000;
    label_33102  :
      decay0_nucltransK(prng_, event_, 0.0128,0.0050,8.7e0,0.,tclev,thlev,tdlev);
      goto label_20200;
    label_202  :
      decay0_beta(prng_, event_, 0.0257,89.,0.,0.,t);
    label_20200  :
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.0135,0.0050,6.1e0,0.,tclev,thlev,tdlev);
      goto label_67000;
    label_67  :
      decay0_beta(prng_, event_, 0.0392,89.,0.,0.,t);
    label_67000  :
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.0067,0.0050,1.6e6,0.,tclev,thlev,tdlev);
      return;
    label_63  :
      decay0_beta(prng_, event_, 0.0396,89.,0.,0.,t);
      ////label_63000  :
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.0063,0.0050,7.1e6,0.,tclev,thlev,tdlev);
      return;
    }
    // end of Ra228.f




  } // end of namespace decay0
} // end of namespace genbb

// end of Ra228.cc
// Local Variables: --
// mode: c++ --
// End: --
