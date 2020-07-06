#ifndef GENBB_DECAY0_BB_H_
#define GENBB_DECAY0_BB_H_ 1

// Standard library:
#include <string>
#include <fstream>

namespace mygsl{
  class rng;
}

namespace genbb {

  class primary_event;

  namespace decay0 {

    /// \brief Energy range information for DBD generation
    struct enrange
    {
      double      ebb1; ///< Left E sum limit for modes with continuous distribution of sum of e-/e+ energies (4,5,6,8,10 and 13), in which events will be generated (MeV)
      double      ebb2; ///< Right E sum limit for modes with continuous distribution of sum of e-/e+ energies (4,5,6,8,10 and 13), in which events will be generated (MeV)
      double      toallevents; /** Coefficient to calculate the corresponding
                                *  number of bb events in full range of energies:
                                *  full_number=generated_number*toallevents
                                *  (for modes 4,5,6,8,10 and 13).
                                */
      int         levelE;   ///< keV
      int         itrans02; ///< Flag for spin
      std::string chdspin;  ///< Spin string

      enrange();
      void reset();
    };

    struct genbbpar
    {
      int nevents;
      int ievstart;
      int irndmst;
      int iwrfile;
      std::string chfile;
      int icurrent; // From common/currentev/
      std::ofstream fout;

      genbbpar();
      void reset();
    };

    /// \brief Mode of double beta (- or +) decay:
    // [SI - spectral index]
    enum modebb_type
      {
        MODEBB_UNDEF            = 0,  ///< Undefined
        MODEBB_0NUBB_0_2N       = 1,  ///< 0nubb with neutrino mass, 0+ -> 0+,      2n
        MODEBB_1                = MODEBB_0NUBB_0_2N,
        MODEBB_0NUBB_RHCL_0_2N  = 2,  ///< 0nubb with rhc-lambda,    0+ -> 0+,      2n
        MODEBB_2                = MODEBB_0NUBB_RHCL_0_2N,
        MODEBB_0NUBB_RHCL_02_N  = 3,  ///< 0nubb with rhc-lambda,    0+ -> 0+, 2+,  N*
        MODEBB_3                = MODEBB_0NUBB_RHCL_02_N,
        MODEBB_2NUBB_0_2N       = 4,  ///< 2nubb,                    0+ -> 0+,      2n
        MODEBB_4                = MODEBB_2NUBB_0_2N,
        MODEBB_0NUBB_MAJ1_0_2N  = 5,  ///< 0nuM1bb (Majoron, SI=1),  0+ -> 0+,      2n
        MODEBB_5                = MODEBB_0NUBB_MAJ1_0_2N,
        MODEBB_0NUBB_MAJ3_0_2N  = 6,  ///< 0nuM3bb (Majoron, SI=3),  0+ -> 0+,      2n
        MODEBB_6                = MODEBB_0NUBB_MAJ3_0_2N,
        MODEBB_0NUBB_RHCL_2_2N  = 7,  ///< 0nubb with rhc-lambda,    0+ -> 2+,      2n
        MODEBB_7                = MODEBB_0NUBB_RHCL_2_2N,
        MODEBB_2NUBB_2_2N       = 8,  ///< 2nubb,                    0+ -> 2+,      2n, N*
        MODEBB_8                = MODEBB_2NUBB_2_2N,
        MODEBB_0NUKB_02         = 9,  ///< 0nuKb+                    0+ -> 0+, 2+
        MODEBB_9                = MODEBB_0NUKB_02,
        MODEBB_2NUKB_02         = 10, ///< 2nuKb+                    0+ -> 0+, 2+
        MODEBB_10               = MODEBB_2NUKB_02,
        MODEBB_0NUKK_02         = 11, ///< 0nu2K                     0+ -> 0+, 2+
        MODEBB_11               = MODEBB_0NUKK_02,
        MODEBB_2NUKK_02         = 12, ///< 2nu2K                     0+ -> 0+, 2+
        MODEBB_12               = MODEBB_2NUKK_02,
        MODEBB_0NUBB_MAJ7_0_2N  = 13, ///< 0nuM7bb (Majoron, SI=7)   0+ -> 0+,      2n
        MODEBB_13               = MODEBB_0NUBB_MAJ7_0_2N,
        MODEBB_0NUBB_MAJ2_0_2N  = 14, ///< 0nuM2bb (Majoron, SI=2)   0+ -> 0+,      2n
        MODEBB_14               = MODEBB_0NUBB_MAJ2_0_2N,
        MODEBB_2NUBB_BOSE_0     = 15, ///< 2nubb with bosonic nu's   0+ -> 0+
        MODEBB_15               = MODEBB_2NUBB_BOSE_0,
        MODEBB_2NUBB_BOSE_2     = 16, ///< 2nubb with bosonic nu's   0+ -> 2+
        MODEBB_16               = MODEBB_2NUBB_BOSE_2,
        MODEBB_0NUBB_RHCE_0     = 17, ///< 0nubb with rhc-eta        0+ -> 0+       simplified expression
        MODEBB_17               = MODEBB_0NUBB_RHCE_0,
        MODEBB_0NUBB_RHCE_0_NME = 18, ///< 0nubb with rhc-eta        0+ -> 0+       with specific NMEs
        MODEBB_18               = MODEBB_0NUBB_RHCE_0_NME,
        MODEBB_MIN              = MODEBB_1,
        MODEBB_MAX              = MODEBB_18
      };

    std::string decay0_dbd_mode_label(int);

    bool decay0_supports_esum_limits(int modebb_);

    struct denrange
    {
      double dens;
      double denf;
      int    mode;
      denrange();
      void reset();
    };

    struct helpbb
    {
      double Zd;
      double Ad;
      double e0;
      double e1;
      helpbb();
      void reset();
    };

    struct eta_nme
    {
      double chi_GTw;
      double chi_Fw;
      double chip_GT;
      double chip_F;
      double chip_T;
      double chip_P;
      double chip_R;
      eta_nme();
      void reset();
    };

    struct bbpars : public enrange , public denrange, public helpbb, public eta_nme
    {
      static const unsigned int SPSIZE = 4300;
      int    modebb;   /// Mode of double beta (- or +) decay (from 1 to 18)
      double Qbb;      /// Energy release in double beta process: difference between masses of parent and daughter atoms (MeV);
      double Edlevel;  /// Energy of level of daughter nucleus to which transition occured (MeV);
      double EK;       /// Binding energy of electron on K shell of parent atom (MeV) (for modebb=10 and 11);
      double Zdbb;     /// Atomic number of daughter nucleus (Z>0 for b-b- and Z<0 for b+b+ and eb+ processes);
      double Adbb;     /// Mass number of daughter nucleus
      int    istartbb; /// Initialization flag must be =0 for first call of bb for a given mode;
      double spthe1[SPSIZE];
      double spthe2[SPSIZE];
      double spmax;
      bbpars();
      void reset();
    };

    /// \brief Subroutine for sampling the energies and angles of electrons in various modes of double beta decay without Primakoff-Rosen approximation.
    //
    // Output: - time of event's start (sec) (as supposed, =0 here);
    //         - time shift from previous time to calculate
    //           when particle was emitted (sec)
    //           (as supposed, =0 here);
    //         - toallevents
    // V.I.Tretyak, 26.11.1995; 17.02.2009.
    void decay0_bb(mygsl::rng & prng, primary_event & event, void *params);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_BB_H_
//
// Local Variables: --
// mode: c++ --
// End: --
