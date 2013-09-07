// Pa231.cc
// Copyright 2013, P. Loaiza
//
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

#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Pa231.h>
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

    void Pa231(mygsl::rng & prng_, genbb::primary_event & event_,
               double tcnuc, double & tdnuc)
    {
      datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;

      // Models for scheme of Pa231 decay:
      //  - http://www.nucleide.org/DDEP_WG/Nuclides/Pa-231_tables.pdf
      //  - http://www.nucleide.org/DDEP_WG/Nuclides/Pa-231_com.pdf
      //  - ENSDF at NNDC site on 11.07.2013
      //  - http://xdb.lbl.gov/Section1/Table_1-1c.htm

      // Halflife time of Pa231
      const double thnuc = 1.03e12; // 32670 y
      tdnuc = tcnuc - thnuc/std::log(2.)*std::log(prng_());

      // Nuclear transition parameters
      double thlev = 0.0; /// input- level halflife (sec).
      double tclev = 0.0; /// input - time of creation of level from which
                          /// particle will be emitted (sec);
      double tdlev;       /// output - time of decay of level (sec)
      double t;           /// output - time of decay
      const double AcElectronBindingEnergyK = 0.106855; /// binding energy of electron (MeV) on K-shell
      const double AcElectronBindingEnergyL = 0.018264; /// binding energy of electron (MeV) on L-shel
      const double AcElectronBindingEnergyM = 0.004031; /// binding energy of electron (MeV) on M-shell

      // Probabilities
      double pgamma;
      const double palpha = 100.0 * prng_();
      if (palpha <= 0.0008) {
        // Alpha 4533.0 keV
        decay0_alpha(prng_, event_, 4.5330, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4533.0 keV with t = " << t << " s");
        goto level_537000;
      } else if (palpha <= 0.0029) {
        // Alpha 4415.6 keV
        decay0_alpha(prng_, event_, 4.4156, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4415.6 keV with t = " << t << " s");
        goto level_656000;
      } else if (palpha <= 0.0061) {
        // Alpha 4761.2 keV
        decay0_alpha(prng_, event_, 4.7612, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4761.2 keV with t = " << t << " s");
        goto level_304000;
      } else if (palpha <= 0.0097) {
        // Alpha 4507.6 keV
        decay0_alpha(prng_, event_, 4.5076, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4507.6 keV with t = " << t << " s");
        goto level_562000;
      } else if (palpha <= 0.0177) {
        // Alpha 4568.1 keV
        decay0_alpha(prng_, event_, 4.5681, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4568.1 keV with t = " << t << " s");
        goto level_501000;
      } else if (palpha <= 0.0327) {
        // Alpha 4599.6 keV
        decay0_alpha(prng_, event_, 4.5996, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4599.6 keV with t = " << t << " s");
        goto level_469000;
      } else if (palpha <= 0.0727) {
        // Alpha 4794.1 keV
        decay0_alpha(prng_, event_, 4.7941, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4794.1 keV with t = " << t << " s");
        goto level_271000;
      } else if (palpha <= 0.1231) {
        // Alpha 4633.0 keV
        decay0_alpha(prng_, event_, 4.6330, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4633.0 keV with t = " << t << " s");
        goto level_435000;
      } else if (palpha <= 0.2011) {
        // Alpha 4630.3 keV
        decay0_alpha(prng_, event_, 4.6303, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4630.3 keV with t = " << t << " s");
        goto level_437000;
      } else if (palpha <= 0.2811) {
        // Alpha 4642.5 keV
        decay0_alpha(prng_, event_, 4.6425, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4642.5 keV with t = " << t << " s");
        goto level_425000;
      } else if (palpha <= 0.6811) {
        // Alpha 4977.6 keV
        decay0_alpha(prng_, event_, 4.9776, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4977.6 keV with t = " << t << " s");
        goto level_84000;
      } else if (palpha <= 1.8811) {
        // Alpha 4712.3 keV
        decay0_alpha(prng_, event_, 4.7123, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4712.3 keV with t = " << t << " s");
        goto level_354000;
      } else if (palpha <= 3.2811) {
        // Alpha 4853.5 keV
        decay0_alpha(prng_, event_, 4.8535, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4853.5 keV with t = " << t << " s");
        goto level_210000;
      } else if (palpha <= 4.8811) {
        // Alpha 4987.8 keV
        decay0_alpha(prng_, event_, 4.9878, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4987.8 keV with t = " << t << " s");
        goto level_74000;
      } else if (palpha <= 6.6811) {
        // Alpha 4680.1 keV
        decay0_alpha(prng_, event_, 4.6801, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4680.1 keV with t = " << t << " s");
        goto level_387000;
      } else if (palpha <= 9.4811) {
        // Alpha 5033.8 keV
        decay0_alpha(prng_, event_, 5.0338, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 5033.8 keV with t = " << t << " s");
        goto level_27000;
      } else if (palpha <= 12.3811) {
        // Alpha 4936.0 keV
        decay0_alpha(prng_, event_, 4.9360, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4936.0 keV with t = " << t << " s");
        goto level_126000;
      } else if (palpha <= 20.7811) {
        // Alpha 4736.3 keV
        decay0_alpha(prng_, event_, 4.7363, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4736.3 keV with t = " << t << " s");
        goto level_330000;
      } else if (palpha <= 32.4811) {
        // Alpha 5060.7 keV
        decay0_alpha(prng_, event_, 5.0607, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 5060.7 keV with t = " << t << " s");
        goto level_0000;
      } else if (palpha <= 52.4811) {
        // Alpha 5031.2 keV
        decay0_alpha(prng_, event_, 5.0312, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 5031.2 keV with t = " << t << " s");
        goto level_29000;
      } else if (palpha <= 74.9811) {
        // Alpha 4952.6 keV
        decay0_alpha(prng_, event_, 4.9526, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 4952.6 keV with t = " << t << " s");
        goto level_109000;
      } else if (palpha <= 100.2811) {
        // Alpha 5015.1 keV
        decay0_alpha(prng_, event_, 5.0151, 0., 0., t);
        DT_LOG_DEBUG(local_priority, "alpha 5015.1 keV with t = " << t << " s");
        goto level_46000;
      }

      // Ac227 excited states

    level_656000 :
      // Level 656.4 keV
      DT_LOG_DEBUG (local_priority, "level 656.4 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 14.62)  {
        decay0_gamma(prng_, event_, 582.3, tclev, thlev, tdlev);
        goto level_74000;
      } else if (pgamma <= 37.26) {
        decay0_gamma(prng_, event_, 571.9, tclev, thlev, tdlev);
        goto level_84000;
      } else if (pgamma <= 60.84) {
        decay0_gamma(prng_, event_, 610.1, tclev, thlev, tdlev);
        goto level_46000;
      } else {
        decay0_gamma(prng_, event_, 546.5, tclev, thlev, tdlev);
        goto level_109000;
      }

    level_562000 :
      // Level 562.8 keV
      DT_LOG_DEBUG (local_priority, "level 562.8 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 16.80) {
        decay0_gamma(prng_, event_, 535.43, tclev,thlev,tdlev);
        goto level_27000;
      } else if (pgamma <= 54.54) {
        decay0_gamma(prng_, event_, 516.45, tclev,thlev,tdlev);
        goto level_46000;
      } else {
        decay0_gamma(prng_, event_, 488.66, tclev,thlev,tdlev);
        goto level_74000;
      }

    level_537000 :
      // Level 537.0 keV
      DT_LOG_DEBUG (local_priority, "level 537.0 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 47.37) {
        decay0_gamma(prng_, event_, 509.63, tclev,thlev,tdlev);
        goto level_27000;
      } else {
        decay0_gamma(prng_, event_, 490.65, tclev,thlev,tdlev);
        goto level_46000;
      }

    level_501000 :
      // Level 501.28 keV
      DT_LOG_DEBUG (local_priority, "level 501.28 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 47.95) {
        decay0_gamma(prng_, event_, 427.14, tclev,thlev,tdlev);
        goto level_74000;
      } else {
        decay0_gamma(prng_, event_, 501.28, tclev,thlev,tdlev);
        goto level_0000;
      }

    level_469000 :
      // Level 469.24 keV
      DT_LOG_DEBUG (local_priority, "level 469.24 keV");
      thlev = 0.0;
      decay0_gamma(prng_, event_, 384.69, tclev,thlev,tdlev);
      goto level_84000;

    level_437000 :
      // Level 437.96 keV
      DT_LOG_DEBUG (local_priority, "level 437.96 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 7.75) {
        decay0_nucltransKLM(prng_, event_, 410.590,
                            AcElectronBindingEnergyK, 0.01482,
                            AcElectronBindingEnergyL, 0.00264,
                            AcElectronBindingEnergyM, 0.000628,
                            0.,tclev,thlev,tdlev);
        goto level_27000;
      } else if (pgamma <= 17.2) {
        decay0_nucltransKLM(prng_, event_, 50.730,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 18.8,
                            AcElectronBindingEnergyM, 4.52,
                            0.,tclev,thlev,tdlev);
        goto level_387000;
      } else if (pgamma <= 36.57) {
        decay0_gamma(prng_, event_, 437.960, tclev,thlev,tdlev);
        goto level_0000;
      } else if (pgamma <= 65.54) {
        decay0_nucltransKLM(prng_, event_, 391.610,
                            AcElectronBindingEnergyK, 0.01636,
                            AcElectronBindingEnergyL, 0.00293,
                            AcElectronBindingEnergyM, 0.000697,
                            0.,tclev,thlev,tdlev);
        goto level_46000;
      } else {
        decay0_gamma(prng_, event_, 363.820, tclev,thlev,tdlev);
        goto level_74000;
      }

    level_435000 :
      // Level 435.19 keV
      DT_LOG_DEBUG (local_priority, "level 435.19 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 7.63) {
        decay0_gamma(prng_, event_, 435.190, tclev,thlev,tdlev);
        goto level_0000;
      } else {
        decay0_nucltransKLM(prng_, event_, 407.820,
                            AcElectronBindingEnergyK, 0.269,
                            AcElectronBindingEnergyL, 0.0496,
                            AcElectronBindingEnergyM, 0.01187,
                            0.,tclev,thlev,tdlev);
        goto level_27000;
      }

    level_425000 :
      // Level 425.59 keV
      DT_LOG_DEBUG (local_priority, "level 425.59 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 3.51) {
        thlev = 0.0;
        decay0_nucltransKLM(prng_, event_, 395.610,
                            AcElectronBindingEnergyK, 0.01601,
                            AcElectronBindingEnergyL, 0.00286,
                            AcElectronBindingEnergyM, 0.000682,
                            0.,tclev,thlev,tdlev);
        goto level_29000;
      } else if (pgamma <= 7.86) {
        decay0_nucltransKLM(prng_, event_, 351.450,
                            AcElectronBindingEnergyK, 0.0206,
                            AcElectronBindingEnergyL, 0.00373,
                            AcElectronBindingEnergyM, 0.000891,
                            0.,tclev,thlev,tdlev);
        goto level_74000;
      } else if (pgamma <= 22.62) {
        decay0_gamma(prng_, event_, 398.220, tclev,thlev,tdlev);
        goto level_27000;
      } else {
        decay0_nucltransKLM(prng_, event_, 379.240,
                            AcElectronBindingEnergyK, 0.25,
                            AcElectronBindingEnergyL, 0.052,
                            AcElectronBindingEnergyM, 0.0125,
                            0.,tclev,thlev,tdlev);
        goto level_46000;
      }

    level_387000 :
      // Level 387.23 keV
      DT_LOG_DEBUG (local_priority, "level 387.23 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 0.03) {
        decay0_nucltransKLM(prng_, event_, 387.230,
                            AcElectronBindingEnergyK, 0.0430,
                            AcElectronBindingEnergyL, 0.0254,
                            AcElectronBindingEnergyM, 0.00667,
                            0.,tclev,thlev,tdlev);
        goto level_0000;
      } else if (pgamma <= 0.53) {
        decay0_nucltransKLM(prng_, event_, 57.190,
                          AcElectronBindingEnergyK, 0.0,
                          AcElectronBindingEnergyL, 108.5,
                          AcElectronBindingEnergyM, 29.6,
                          0.,tclev,thlev,tdlev);
        goto level_330000;
      } else if (pgamma <= 1.46) {
        decay0_gamma(prng_, event_, 359.860, tclev,thlev,tdlev);
        goto level_27000;
      } else if (pgamma <= 8.87) {
        decay0_nucltransKLM(prng_, event_, 277.290,
                            AcElectronBindingEnergyK, 0.4,
                            AcElectronBindingEnergyL, 0.11,
                            AcElectronBindingEnergyM, 0.03,
                            0.,tclev,thlev,tdlev);
        goto level_109000;
      } else if (pgamma <= 19.63) {
        decay0_nucltransKLM(prng_, event_, 313.090,
                            AcElectronBindingEnergyK, 0.22,
                            AcElectronBindingEnergyL, 0.070,
                            AcElectronBindingEnergyM, 0.0177,
                            0.,tclev,thlev,tdlev);
        goto level_74000;
      } else if (pgamma <= 37.95) {
        decay0_nucltransKLM(prng_, event_, 357.250,
                            AcElectronBindingEnergyK, 0.34,
                            AcElectronBindingEnergyL, 0.066,
                            AcElectronBindingEnergyM, 0.0159,
                            0.,tclev,thlev,tdlev);
        goto level_29000;
      } else if (pgamma <= 57.25) {
        decay0_nucltransKLM(prng_, event_, 340.880,
                            AcElectronBindingEnergyK, 0.081,
                            AcElectronBindingEnergyL, 0.020,
                            AcElectronBindingEnergyM, 0.0050,
                            0.,tclev,thlev,tdlev);
        goto level_46000;
      } else if (pgamma <= 77.1) {
        decay0_nucltransKLM(prng_, event_, 260.370,
                          AcElectronBindingEnergyK, 0.37,
                          AcElectronBindingEnergyL, 0.133,
                          AcElectronBindingEnergyM, 0.0340,
                          0.,tclev,thlev,tdlev);
        goto level_126000;
      } else {
        decay0_nucltransKLM(prng_, event_, 302.680,
                            AcElectronBindingEnergyK, 0.0285,
                            AcElectronBindingEnergyL, 0.00527,
                            AcElectronBindingEnergyM, 0.001260,
                            0.,tclev,thlev,tdlev);
        goto level_84000;
      }

    level_354000 :
      // Level 354.5 keV
      DT_LOG_DEBUG (local_priority, "level 354.5 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 3.57) {
        decay0_nucltransKLM(prng_, event_, 24.460,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 161.3,
                            AcElectronBindingEnergyM, 39.0,
                            0.,tclev,thlev,tdlev);
        goto level_330000;
      } else if (pgamma <= 29.88) {
        decay0_nucltransKLM(prng_, event_, 327.130,
                            AcElectronBindingEnergyK, 0.0240,
                            AcElectronBindingEnergyL, 0.00440,
                            AcElectronBindingEnergyM, 0.001050,
                            0.,tclev,thlev,tdlev);
        goto level_27000;
      } else {
        decay0_nucltransKLM(prng_, event_, 354.500,
                            AcElectronBindingEnergyK, 0.0855,
                            AcElectronBindingEnergyL, 0.0386,
                            AcElectronBindingEnergyM, 0.01003,
                            0.,tclev,thlev,tdlev);
        goto level_0000;
      }

    level_330000 :
      // Level 330.04 keV
      DT_LOG_DEBUG (local_priority, "level 330.04 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 0.06) {
        decay0_nucltransKLM(prng_, event_, 56.900,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 28,
                            AcElectronBindingEnergyM, 7.1,
                            0.,tclev,thlev,tdlev);
        goto level_273000;
      } else if (pgamma <= 0.15) {
        decay0_nucltransKLM(prng_, event_, 245.490,
                            AcElectronBindingEnergyK, 3.70,
                            AcElectronBindingEnergyL, 1.143,
                            AcElectronBindingEnergyM, 0.293,
                            0.,tclev,thlev,tdlev);
        goto level_84000;
      } else if (pgamma <= 1.5) {
        decay0_nucltransKLM(prng_, event_, 255.900,
                            AcElectronBindingEnergyK, 0.0992,
                            AcElectronBindingEnergyL, 0.1216,
                            AcElectronBindingEnergyM, 0.0327,
                            0.,tclev,thlev,tdlev);
        goto level_74000;
      } else if (pgamma <= 18.85) {
        decay0_nucltransKLM(prng_, event_, 330.040,
                            AcElectronBindingEnergyK, 0.430,
                            AcElectronBindingEnergyL, 0.0836,
                            AcElectronBindingEnergyM, 0.0202,
                            0.,tclev,thlev,tdlev);
        goto level_0000;
      } else if (pgamma <= 39.9) {
        decay0_nucltransKLM(prng_, event_, 283.690,
                            AcElectronBindingEnergyK, 0.0329,
                            AcElectronBindingEnergyL, 0.00614,
                            AcElectronBindingEnergyM, 0.001468,
                            0.,tclev,thlev,tdlev);
        goto level_46000;
      } else if (pgamma <= 69.25) {
        decay0_nucltransKLM(prng_, event_, 302.670,
                            AcElectronBindingEnergyK, 0.0285,
                            AcElectronBindingEnergyL, 0.00527,
                            AcElectronBindingEnergyM, 0.001260,
                            0.,tclev,thlev,tdlev);
        goto level_27000;
      } else {
        decay0_nucltransKLM(prng_, event_, 300.060,
                            AcElectronBindingEnergyK, 0.613,
                            AcElectronBindingEnergyL, 0.1146,
                            AcElectronBindingEnergyM, 0.0275,
                            0.,tclev,thlev,tdlev);
        goto level_29000;
      }

    level_304000 :
      // Level 304.73 keV
      DT_LOG_DEBUG (local_priority, "level 304.73 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 46.88) {
        decay0_gamma(prng_, event_, 258.380, tclev,thlev,tdlev);
        goto level_46000;
      } else {
        decay0_gamma(prng_, event_, 230.590, tclev,thlev,tdlev);
        goto level_74000;
      }

    level_273000 :
      // Level 273.14 keV
      DT_LOG_DEBUG (local_priority, "level 273.14 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 2.75) {
        decay0_gamma(prng_, event_, 199.000, tclev,thlev,tdlev);
        goto level_74000;
      } else if (pgamma <= 13.77) {
        decay0_nucltransKLM(prng_, event_, 245.770,
                            AcElectronBindingEnergyK, 0.0455,
                            AcElectronBindingEnergyL, 0.00867,
                            AcElectronBindingEnergyM, 0.00208,
                            0.,tclev,thlev,tdlev);
        goto level_27000;
      } else if (pgamma <= 46.83) {
        decay0_nucltransKLM(prng_, event_, 243.160,
                            AcElectronBindingEnergyK, 0.56,
                            AcElectronBindingEnergyL, 0.176,
                            AcElectronBindingEnergyM, 0.0445,
                            0.,tclev,thlev,tdlev);
        goto level_29000;
      } else {
        decay0_nucltransKLM(prng_, event_, 273.140,
                            AcElectronBindingEnergyK, 0.57,
                            AcElectronBindingEnergyL, 0.131,
                            AcElectronBindingEnergyM, 0.0323,
                            0.,tclev,thlev,tdlev);
        goto level_0000;
      }

    level_271000 :
      // Level 271.29 keV
      DT_LOG_DEBUG (local_priority, "level 271.29 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 27.71) {
        decay0_nucltransKLM(prng_, event_, 144.430,
                            AcElectronBindingEnergyK, 0.263,
                            AcElectronBindingEnergyL, 1.407,
                            AcElectronBindingEnergyM, 0.384,
                            0.,tclev,thlev,tdlev);
        goto level_126000;
      } else {
        decay0_nucltransKLM(prng_, event_, 72.580,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 6.59,
                            AcElectronBindingEnergyM, 1.582,
                            0.,tclev,thlev,tdlev);
        goto level_198000;
      }

    level_210000 :
      // Level 210.78 keV
      DT_LOG_DEBUG (local_priority, "level 210.78 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 17.52) {
        decay0_nucltransKLM(prng_, event_, 23.460,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 182,
                            AcElectronBindingEnergyM, 44.1,
                            0.,tclev,thlev,tdlev);
        goto level_187000;
      } else {
        decay0_nucltransKLM(prng_, event_, 100.840,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 7.30,
                            AcElectronBindingEnergyM, 2.00,
                            0.,tclev,thlev,tdlev);
        goto level_109000;
      }

    level_198000 :
      // Level 198.71 keV
      DT_LOG_DEBUG (local_priority, "level 198.71 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 30.65) {
        decay0_nucltransKLM(prng_, event_, 71.850,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 6.79,
                            AcElectronBindingEnergyM, 1.630,
                            0.,tclev,thlev,tdlev);
        goto level_126000;
      } else {
        decay0_nucltransKLM(prng_, event_, 124.570,
                            AcElectronBindingEnergyK, 0.285,
                            AcElectronBindingEnergyL, 2.75,
                            AcElectronBindingEnergyM, 0.752,
                            0.,tclev,thlev,tdlev);
        goto level_74000;
      }

    level_187000 :
      // Level 187.32 keV
      DT_LOG_DEBUG (local_priority, "level 187.32 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 6.21) {
        decay0_nucltransKLM(prng_, event_, 60.460,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 0.327,
                            AcElectronBindingEnergyM, 0.0800,
                            0.,tclev,thlev,tdlev);
        goto level_126000;
      } else if (pgamma <= 28.48) {
        decay0_nucltransKLM(prng_, event_, 102.770,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 6.69,
                            AcElectronBindingEnergyM, 1.83,
                            0.,tclev,thlev,tdlev);
        goto level_84000;
      } else {
        decay0_nucltransKLM(prng_, event_, 77.380,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 5.47,
                            AcElectronBindingEnergyM, 1.313,
                            0.,tclev,thlev,tdlev);
        goto level_109000;
      }

    level_126000 :
      // Level 126.86 keV
      DT_LOG_DEBUG (local_priority, "level 126.86 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 47.50) {
        decay0_nucltransKLM(prng_, event_, 52.720,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 16.81,
                            AcElectronBindingEnergyM, 4.03,
                            0.,tclev,thlev,tdlev);
        goto level_74000;
      } else {
        decay0_nucltransKLM(prng_, event_, 96.880,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 8.81,
                            AcElectronBindingEnergyM, 2.41,
                            0.,tclev,thlev,tdlev);
        goto level_29000;
      }

    level_109000 :
      // Level 109.94 keV
      DT_LOG_DEBUG (local_priority, "level 109.94 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 10.46) {
        decay0_nucltransKLM(prng_, event_, 35.800,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 1.313,
                            AcElectronBindingEnergyM, 0.327,
                            0.,tclev,thlev,tdlev);
        goto level_74000;
      } else if (pgamma <= 39.07) {
        decay0_nucltransKLM(prng_, event_, 63.590,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 65.1,
                            AcElectronBindingEnergyM, 17.8,
                            0.,tclev,thlev,tdlev);
        goto level_46000;
      } else {
        decay0_nucltransKLM(prng_, event_, 25.390,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 144.6,
                            AcElectronBindingEnergyM, 34.9,
                            0.,tclev,thlev,tdlev);
        goto level_84000;
      }

    level_84000 :
      // Level 84.55 keV
      DT_LOG_DEBUG (local_priority, "level 84.55 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 12.65) {
        decay0_nucltransKLM(prng_, event_, 57.180,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 108.6,
                            AcElectronBindingEnergyM, 29.6,
                            0.,tclev,thlev,tdlev);
        goto level_27000;
      } else if (pgamma <= 41.22) {
        decay0_nucltransKLM(prng_, event_, 54.570,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 0.430,
                            AcElectronBindingEnergyM, 0.1053,
                            0.,tclev,thlev,tdlev);
        goto level_29000;
      } else {
        decay0_nucltransKLM(prng_, event_, 38.200,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 66,
                            AcElectronBindingEnergyM, 17,
                            0.,tclev,thlev,tdlev);
        goto level_46000;
      }

    level_74000 :
      // Level 74.14 keV
      DT_LOG_DEBUG (local_priority, "level 74.14 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 28.85) {
        decay0_nucltransKLM(prng_, event_, 74.140,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 31.2,
                            AcElectronBindingEnergyM, 8.53,
                            0.,tclev,thlev,tdlev);
        goto level_0000;
      } else {
        decay0_nucltransKLM(prng_, event_, 44.160,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 28.3,
                            AcElectronBindingEnergyM, 6.79,
                            0.,tclev,thlev,tdlev);
        goto level_29000;
      }

    level_46000 :
      // Level 46.35 keV
      DT_LOG_DEBUG (local_priority, "level 46.35 keV");
      thlev = 0.0;
      pgamma = 100. * prng_ ();
      if (pgamma <= 24.33) {
        decay0_nucltransKLM(prng_, event_, 46.350,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 0.663,
                            AcElectronBindingEnergyM, 0.1634,
                            0.,tclev,thlev,tdlev);
        goto level_0000;
      } else if (pgamma <= 52.63) {
        decay0_nucltransKLM(prng_, event_, 16.370,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 5.06,
                            AcElectronBindingEnergyM, 2.68,
                            0.,tclev,thlev,tdlev);
        goto level_29000;
      } else {
        decay0_nucltransKLM(prng_, event_, 18.980,
                            AcElectronBindingEnergyK, 0.0,
                            AcElectronBindingEnergyL, 2.35,
                            AcElectronBindingEnergyM, 82.7,
                            0.,tclev,thlev,tdlev);
        goto level_27000;
      }

    level_29000 :
      // Level 29.98 keV
      DT_LOG_DEBUG (local_priority, "level 29.98 keV");
      thlev = 0.0;
      decay0_nucltransKLM(prng_, event_, 29.980,
                          AcElectronBindingEnergyK, 0.0,
                          AcElectronBindingEnergyL, 202,
                          AcElectronBindingEnergyM, 52,
                          0.,tclev,thlev,tdlev);
      goto level_0000;

    level_27000 :
      // Level 27.37 keV
      DT_LOG_DEBUG (local_priority, "level 27.37 keV");
      thlev = 38.3e-9;
      decay0_nucltransKLM(prng_, event_, 27.370,
                          AcElectronBindingEnergyK, 0.0,
                          AcElectronBindingEnergyL, 3.3,
                          AcElectronBindingEnergyM, 0.87,
                          0.,tclev,thlev,tdlev);
      goto level_0000;

    level_0000 :
      // Ground state
      DT_LOG_DEBUG (local_priority, "Ground state");
      return;
    }

  } // end of namespace decay0
} // end of namespace genbb

// end of Pa231.cc
// Local Variables: --
// mode: c++ --
// End: --
