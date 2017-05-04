
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Sc48.h>
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

    // Sc48.f
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

    void Sc48(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double p;
      double pbeta;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Sc48 decay ("Table of Isotopes", 8 ed., 1996 + NDS 68(1993)1).
      // Three-figured labels correspond to energies of 48Ti excited
      // levels in keV.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 7.05.1998
      thnuc=1.57212e5;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pbeta=100.*prng_();
      if (pbeta <= 10.02) goto label_3509;
      goto label_3333;
    label_3509  :
      decay0_beta(prng_, event_, 0.483,22.,0.,0.,t);
      ////    label_35090  :
      thlev=1.4e-12;
      p=100.*prng_();
      if (p <= 24.14) goto label_35091;
      goto label_35092;
    label_35091  :
      decay0_nucltransK(prng_, event_, 1.213,0.005,8.8e-5,0.1e-4,tclev,thlev,tdlev);
      goto label_22960;
    label_35092  :
      decay0_nucltransK(prng_, event_, 0.175,0.005,4.5e-3,0.,tclev,thlev,tdlev);
      goto label_33330;
    label_3333  :
      decay0_beta(prng_, event_, 0.659,22.,0.,0.,t);
    label_33330  :
      thlev=221.e-15;
      decay0_nucltransK(prng_, event_, 1.038,0.005,1.1e-4,0.,tclev,thlev,tdlev);
      goto label_22960;
    label_22960  :
      thlev=0.762e-12;
      decay0_nucltransK(prng_, event_, 1.312,0.005,9.7e-5,0.1e-4,tclev,thlev,tdlev);
      goto label_98400;
    label_98400  :
      thlev=4.04e-12;
      decay0_nucltransK(prng_, event_, 0.984,0.005,1.3e-4,0.,tclev,thlev,tdlev);
      return;
    }
    // end of Sc48.f




  } // end of namespace decay0
} // end of namespace genbb

// end of Sc48.cc
// Local Variables: --
// mode: c++ --
// End: --
