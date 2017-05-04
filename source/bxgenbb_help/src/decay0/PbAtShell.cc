
#include <genbb_help/decay0/PbAtShell.h>

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

namespace genbb
{
  namespace decay0
  {

// PbAtShell.f
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

    void PbAtShell (mygsl::rng & prng_, genbb::primary_event & event_,
                    int KLMenergy, double tclev, double thlev, double &tdlev)
    {
      //double t;
      int Lhole;
      int Mhole;
      double p;
// Subroutine describes in some approximation the deexcitation process in
// atomic shell of Pb after creation of electron vacation in K, L or M shell.
// Call : call PbAtShell(KLMenergy,tclev,thlev,tdlev)
// Input : KLMenergy - binding energy of electron on atomic shell [integer
// in keV] where the hole was created
// tclev - time of hole creation (sec)
// thlev - hole halflife (sec).
// Output: tdlev - time of decay of hole (sec).
// It is supposed electron binding energy on Pb K-shell = 88 keV, on L-shell =
// 15 keV, on M-shell = 3 keV. So, following values of KLMenergy are allowed:
// 88 (hole in K-shell), 15 (in L-shell) and 3 (in M-shell).
// VIT, 7.07.1995, 22.10.1995.
        Lhole = 0;
        Mhole = 0;
      if (KLMenergy == 88)
        goto label_88;
      if (KLMenergy == 15)
        {
          Lhole = 1;
          goto label_15;
        }
      if (KLMenergy == 3)
        {
          Mhole = 1;
          goto label_3;
        }
      goto label_20000;
// K-shell
    label_88:
      p = 100. * prng_ ();
      if (p <= 22.)
        goto label_88001;
      goto label_88002;
    label_88001:
      decay0_gamma (prng_, event_, 0.085, tclev, thlev, tdlev); // X ray K-M
      Mhole = Mhole + 1;
      goto label_3;
    label_88002:
      p = 100. * prng_ ();
      if (p <= 96.)
        {
          decay0_gamma (prng_, event_, 0.073, tclev, thlev, tdlev);     // X ray K-L
        }
      else
        {
          decay0_electron (prng_, event_, 0.058, tclev, thlev, tdlev);  // Auger electron K-LL
          Lhole = Lhole + 1;
        }
      Lhole = Lhole + 1;
// L-shell
    label_15:
      for (int i = 1; i <= Lhole; i++)
        {
          //i = 1, Lhole;
          p = 100. * prng_ ();
          if (p <= 40.)
            {
              decay0_gamma (prng_, event_, 0.012, tclev, thlev, tdlev); // X ray L-M
            }
          else
            {
              decay0_electron (prng_, event_, 0.009, tclev, thlev, tdlev);      // Auger electron L-MM
              Mhole = Mhole + 1;
            }
          Mhole = Mhole + 1;
        }
// M-shell
    label_3:
      for (int i = 1; i <= Lhole; i++)
        {
          //i = 1, Mhole;
          decay0_gamma (prng_, event_, 0.003, tclev, thlev, tdlev);     // X ray M-inf
        }
      return;
    label_20000:
// print *,'Pb_At_Shell: wrong hole level [keV] ',KLMenergy
      return;
    }
// end of PbAtShell.f
// Local Variables: --
// mode: c++ --
// End: --

  }                             // end of namespace decay0
}                               // end of namespace genbb
