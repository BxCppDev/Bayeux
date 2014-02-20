
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Xe131m.h>
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

    // Xe131m.f 
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

    void Xe131m(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      //double t;
      double tdlev;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Xe129m decay (NDS 107(2006)2715 and ENSDF at NNDC site on
      // 13.11.2007).
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 13.11.2007.
      thnuc=1.022976e+6;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
    label_16400  :
      thlev=0.;
      decay0_nucltransKLM(prng_, event_, 0.164,0.035,31.60,0.005,14.75,0.001,4.15,
			  0.,tclev,thlev,tdlev);
      return;
    }
    // end of Xe131m.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Xe131m.cc
// Local Variables: --
// mode: c++ --
// End: --
