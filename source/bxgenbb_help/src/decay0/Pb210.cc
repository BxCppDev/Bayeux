
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Pb210.h>
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

    // Pb210.f
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

    void Pb210(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double pbeta;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Pb210 decay in accordance with NDS 99(2003)649 and ENSDF at
      // the NNDC site on 6.08.2007.
      // Labels correspond to energies of Bi210 excited levels in keV.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // Beta decays are 1st forbidden (dJ^(dPi)=0^-,1^-)
      // is supposen.
      // VIT, 6.08.2007.
      thnuc=7.0056e8;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pbeta=100.*prng_();
      if (pbeta <= 84.) goto label_47;
      goto label_10000;
    label_47  :
      decay0_beta(prng_, event_, 0.0170,83.,0.,0.,t);
      ////label_47000  :
      thlev=0.;
      decay0_nucltransKLM(prng_, event_, 0.0465,0.016,14.2,0.004,3.36,0.001,1.14,
                          0.,tclev,thlev,tdlev);
      return;
    label_10000  :
      decay0_beta(prng_, event_, 0.0635,83.,0.,0.,t);
      return;
    }
    // end of Pb210.f




  } // end of namespace decay0
} // end of namespace genbb

// end of Pb210.cc
// Local Variables: --
// mode: c++ --
// End: --
