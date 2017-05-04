c GENBBsub.f 
c
c This file was extracted from the 'decay0' program by V.I. Tretyak
c
c Copyright 1995-2011 V.I. Tretyak
c
c This program is free software; you can redistribute it and/or modify
c it under the terms of the GNU General Public License as published by
c the Free Software Foundation; either version 3 of the License, or (at
c your option) any later version.
c 
c This program is distributed in the hope that it will be useful, but
c WITHOUT ANY WARRANTY; without even the implied warranty of
c MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
c General Public License for more details.
c 
c You should have received a copy of the GNU General Public License
c along with this program; if not, write to the Free Software
c Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
c 
c
	subroutine GENBBsub(i2bbs,chnuclide,ilevel,modebb,istart,ier)
c
c Subroutine GENBBsub generates the events of decay of natural
c radioactive nuclides and various modes of double beta decay.
c GENBB units: energy and moment - MeV and MeV/c; time - sec.
c Energy release in double beta decay - in accordance with
c G.Audi and A.H.Wapstra, Nucl. Phys. A 595(1995)409.
c
c Call  : character chnuclide*16,chfile*256,chart*4,chdspin*4
c         common/genbbpar/nevents,ievstart,irndmst,iwrfile,chfile
c         common/currentev/icurrent
c         common/enrange/ebb1,ebb2,toallevents,levelE,chdspin
c         common/artificial/nartparts,    chart(10),
c      +                    artemin(10),  artemax(10),
c      +                    arttmin(10),  arttmax(10),
c      +                    artfmin(10),  artfmax(10),
c      +                    artQb(10),    artZd(10),
c      +                    arttdelay(10),artthalf(10)
c      +                    nartnpg(10)
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call GENBBsub(i2bbs,chnuclide,ilevel,modebb,istart,ier)
c
c Input : i2bbs     - i2bbs=1 - to generate 2b-decay,
c                     i2bbs=2 - to generate decay of background nuclide
c                               or calibration source;
c         chnuclide - character*16 name of nuclide:
c                     for i2bbs=1 - Ca48,
c                                   Ni58,
c                                   Zn64,  Zn70,
c                                   Ge76,
c                                   Se74,  Se82,
c                                   Sr84,
c                                   Zr94,  Zr96,
c                                   Mo92,  Mo100,
c                                   Ru96,  Ru104,
c                                   Cd106, Cd108, Cd114, Cd116,
c                                   Sn112, Sn122, Sn124,
c                                   Te120, Te128, Te130,
c                                   Xe136,
c                                   Ce136, Ce138, Ce142,
c                                   Nd148, Nd150,
c                                   Dy156, Dy158,
c                                   W180,  W186,
c                                   Pt190, Pt198,
c                                   Bi214 (for Bi214+At214),
c                                   Pb214 (for Pb214+Po214),
c                                   Po218 (for Po218+Rn218+Po214),
c                                   Rn222 (for Rn222+Ra222+Rn218+Po214);
c                     for i2bbs=2 - Ac228,
c                                   Am241,
c                                   Ar39,
c                                   Ar42,
c                                   As79 (for As79+Se79m),
c                                   Bi207 (for Bi207+Pb207m),
c                                   Bi208,
c                                   Bi210,
c                                   Bi212 (for Bi212+Po212),
c                                   Bi214 (for Bi214+Po214),
c                                   Ca48 (for Ca48+Sc48),
c                                   C14,
c                                   Cd113,
c                                   Co60,
c                                   Cs136,
c                                   Cs137 (for Cs137+Ba137m),
c                                   Eu147,
c                                   Eu152,
c                                   Eu154,
c                                   Gd146,
c                                   Hf182,
c                                   I126,
c                                   I133,
c                                   I134,
c                                   I135,
c                                   K40,
c                                   K42,
c                                   Kr81,
c                                   Kr85,
c                                   Mn54,
c                                   Na22,
c                                   P32,
c                                   Pa234m,
c                                   Pb210,
c                                   Pb211,
c                                   Pb212,
c                                   Pb214,
c                                   Ra228,
c                                   Rb87,
c                                   Rh106,
c                                   Sb125,
c                                   Sb126,
c                                   Sb133,
c                                   Sr90,
c                                   Ta182,
c                                   Te133,
c                                   Te133m,
c                                   Te134,
c                                   Th234,
c                                   Tl207,
c                                   Tl208,
c                                   Xe129m,
c                                   Xe131m,
c                                   Xe133,
c                                   Xe135,
c                                   Y88,
c                                   Y90,
c                                   Zn95,
c                                   Zr96 (for Zr96+Nb96),
c                                   Art (for artificial event),
c                                   Com (for Compton effect),
c                                   Mol (for Moller scattering),
c                                   Pai (for e+e- pair creation from
c                                        external gammas);
c         ilevel    - (for i2bbs=1 only) level of daughter nucleus occupied
c                     in 2b decay (0 - ground state, 1 - first excited 0+ or
c                     2+ level, 2 - second excited 0+ or 2+ level etc.;
c                     excited levels with other spin than 0+ and 2+ should be
c                     omitted):
c                     for Ca48-Ti48   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.984 MeV},
c                                       2. 2+ (2)  {2.421 MeV},
c                     for Ni58-Fe58   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.811 MeV},
c                                       2. 2+ (2)  {1.675 MeV},
c                     for Zn64-Ni64   - 0. 0+ (gs)     {0 MeV},
c                     for Zn70-Ge70   - 0. 0+ (gs)     {0 MeV},
c                     for Ge76-Se76   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.559 MeV},
c                                       2. 0+ (1)  {1.122 MeV},
c                                       3. 2+ (2)  {1.216 MeV},
c                     for Se74-Ge74   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.596 MeV},
c                                       2. 2+ (2)  {1.204 MeV},
c                     for Se82-Kr82   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.776 MeV},
c                                       2. 2+ (2)  {1.475 MeV},
c                     for Sr84-Kr84   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.882 MeV},
c                     for Zr94-Mo94   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.871 MeV},
c                     for Zr96-Mo96   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.778 MeV},
c                                       2. 0+ (1)  {1.148 MeV},
c                                       3. 2+ (2)  {1.498 MeV},
c                                       4. 2+ (3)  {1.626 MeV},
c                                       5. 2+ (4)  {2.096 MeV},
c                                       6. 2+ (5)  {2.426 MeV},
c                                       7. 0+ (2)  {2.623 MeV},
c                                       8. 2+ (6)  {2.700 MeV},
c                                       9. 2+?(7)  {2.713 MeV},
c                     for Mo92-Zr92   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.934 MeV},
c                                       2. 0+ (1)  {1.383 MeV},
c                     for Mo100-Ru100 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.540 MeV},
c                                       2. 0+ (1)  {1.130 MeV},
c                                       3. 2+ (2)  {1.362 MeV},
c                                       4. 0+ (2)  {1.741 MeV},
c                     for Ru96-Mo96   - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.778 MeV},
c                                       2. 0+ (1)  {1.148 MeV},
c                                       3. 2+ (2)  {1.498 MeV},
c                                       4. 2+ (3)  {1.626 MeV},
c                                       5. 2+ (4)  {2.096 MeV},
c                                       6. 2+ (5)  {2.426 MeV},
c                                       7. 0+ (2)  {2.623 MeV},
c                                       8. 2+ (6)  {2.700 MeV},
c                                       9. 2+?(7)  {2.713 MeV},
c                     for Ru104-Pd104 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.556 MeV},
c                     for Cd106-Pd106 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.512 MeV},
c                                       2. 2+ (2)  {1.128 MeV},
c                                       3. 0+ (1)  {1.134 MeV},
c                                       4. 2+ (3)  {1.562 MeV},
c                                       5. 0+ (2)  {1.706 MeV},
c                     for Cd108-Pd108 - 0. 0+ (gs)     {0 MeV},
c                     for Cd114-Sn114 - 0. 0+ (gs)     {0 MeV},
c                     for Cd116-Sn116 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {1.294 MeV},
c                                       2. 0+ (1)  {1.757 MeV},
c                                       3. 0+ (2)  {2.027 MeV},
c                                       4. 2+ (2)  {2.112 MeV},
c                                       5. 2+ (3)  {2.225 MeV},
c                     for Sn112-Cd112 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.618 MeV},
c                                       2. 0+ (1)  {1.224 MeV},
c                                       3. 2+ (2)  {1.312 MeV},
c                                       4. 0+ (2)  {1.433 MeV},
c                                       5. 2+ (3)  {1.469 MeV},
c                                       6. 0+ (3)  {1.871 MeV},
c                     for Sn122-Te122 - 0. 0+ (gs)     {0 MeV},
c                     for Sn124-Te124 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.603 MeV},
c                                       2. 2+ (2)  {1.326 MeV},
c                                       3. 0+ (1)  {1.657 MeV},
c                                       4. 0+ (2)  {1.883 MeV},
c                                       5. 2+ (3)  {2.039 MeV},
c                                       6. 2+ (4)  {2.092 MeV},
c                                       7. 0+ (3)  {2.153 MeV},
c                                       8. 2+ (5)  {2.182 MeV},
c                     for Te120-Sn120 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {1.171 MeV},
c                     for Te128-Xe128 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.443 MeV},
c                     for Te130-Xe130 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.536 MeV},
c                                       2. 2+ (2)  {1.122 MeV},
c                                       3. 0+ (1)  {1.794 MeV},
c                     for Xe136-Ba136 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.819 MeV},
c                                       2. 2+ (2)  {1.551 MeV},
c                                       3. 0+ (1)  {1.579 MeV},
c                                       4. 2+ (3)  (2.080 MeV},
c                                       5. 2+ (4)  {2.129 MeV},
c                                       6. 0+ (2)  {2.141 MeV},
c                                       7. 2+ (5)  {2.223 MeV},
c                                       8. 0+ (3)  {2.315 MeV},
c                                       9. 2+ (6)  {2.400 MeV},
c                     for Ce136-Ba136 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.819 MeV},
c                                       2. 2+ (2)  {1.551 MeV},
c                                       3. 0+ (1)  {1.579 MeV},
c                                       4. 2+ (3)  (2.080 MeV},
c                                       5. 2+ (4)  {2.129 MeV},
c                                       6. 0+ (2)  {2.141 MeV},
c                                       7. 2+ (5)  {2.223 MeV},
c                                       8. 0+ (3)  {2.315 MeV},
c                                       9. 2+ (6)  {2.400 MeV},
c                     for Ce138-Ba138 - 0. 0+ (gs)     {0 MeV},
c                     for Ce142-Nd142 - 0. 0+ (gs)     {0 MeV},
c                     for Nd148-Sm148 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.550 MeV},
c                                       2. 2+ (2)  {1.455 MeV},
c                     for Nd150-Sm150 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.334 MeV},
c                                       2. 0+ (1)  {0.740 MeV},
c                                       3. 2+ (2)  {1.046 MeV},
c                                       4. 2+ (3)  {1.194 MeV},
c                                       5. 0+ (2)  {1.256 MeV};
c                     for Dy156-Gd156 - 0. 0+ (gs)     {0 MeV}'
c                                       1. 2+ (1)  {0.089 MeV}'
c                                       2. 0+ (1)  {1.050 MeV}'
c                                       3. 2+ (2)  {1.129 MeV}'
c                                       4. 2+ (3)  {1.154 MeV}'
c                                       5. 0+ (2)  {1.168 MeV}'
c                                       6. 2+ (4)  {1.258 MeV}'
c                                       7. 0+ (3)  {1.715 MeV}'
c                                       8. 2+ (5)  {1.771 MeV}'
c                                       9. 2+ (6)  {1.828 MeV}'
c                                      10. 0+ (4)  {1.851 MeV}'
c                                      11. 2+ (7)  {1.915 MeV}'
c                                      12. 1-      {1.946 MeV}'
c                                      13. 0-      {1.952 MeV}'
c                                      14. 0+ (5)  {1.989 MeV}'
c                                      15. 2+ (8)  {2.004 MeV}'
c                     for Dy158-Gd158 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.080 MeV},
c                                       2. 4+ (1)  {0.261 MeV};
c                     for W180-Hf180  - 0. 0+ (gs)     {0 MeV};
c                     for W186-Os186  - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.137 MeV};
c                     for Pt190-Os190 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.187 MeV},
c                                       2. 2+ (2)  {0.558 MeV},
c                                       3. 0+ (1)  {0.912 MeV},
c                                       4. 2+ (3)  {1.115 MeV},
c                                       5. 0+ (2)  {1.382 MeV};
c                     for Pt198-Hg198 - 0. 0+ (gs)     {0 MeV},
c                                       1. 2+ (1)  {0.412 MeV};
c                     for Bi214-At214 - 0. 1- (gs)     {0 MeV};
c                     for Pb214-Po214 - 0. 0+ (gs)     {0 MeV};
c                     for Po218-Rn218 - 0. 0+ (gs)     {0 MeV};
c                     for Rn222-Ra222 - 0. 0+ (gs)     {0 MeV};
c         modebb    - (for i2bbs=1 only) mode of 2b decay:
c                      1. 0nubb(mn)         0+ -> 0+     {2n},
c                      2. 0nubb(rhc-lambda) 0+ -> 0+     {2n},
c                      3. 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*},
c                      4. 2nubb             0+ -> 0+     {2n},
c                      5. 0nubbM1           0+ -> 0+     {2n}
c                         Majoron with spectral index SI=1
c                         (old M of Gelmini-Roncadelli),
c                      6. 0nubbM3           0+ -> 0+     {2n}
c                         Majoron with SI=3 (vector M, double M, charged M),
c                      7. 0nubb(rhc-lambda) 0+ -> 2+     {2n},
c                      8. 2nubb             0+ -> 2+     {2n}, {N*},
c                      9. 0nuKb+            0+ -> 0+, 2+,
c                     10. 2nuKb+            0+ -> 0+, 2+,
c                     11. 0nu2K             0+ -> 0+, 2+,
c                     12. 2nu2K             0+ -> 0+, 2+,
c                     13. 0nubbM7           0+ -> 0+     {2n}
c                         Majoron with SI=7,
c                     14. 0nubbM2           0+ -> 0+     {2n}
c                         Majoron with SI=2 (bulk M of Mohapatra)
c                     15. 2nubb             0+ -> 0+ with bosonic neutrinos,
c                     16. 2nubb             0+ -> 2+ with bosonic neutrinos,
c                     17. 0nubb(rhc-eta)    0+ -> 0+ simplified expression,
c                     18. 0nubb(rhc-eta)    0+ -> 0+ with specified NMEs;
c         istart    - -1 - to check consistency of input data and fill
c                          working arrays without generation of events;
c                      0 - the same but one event will be generated;
c                      1 - to generate all subsequent events. So, for
c                          the first call to GENBBsub istart must be
c                          =0 or -1;
c         nevents   - number of events to generate; it will be used only
c                     for information in GENBB.report file and in output
c                     file with generated events (if asked) - so, one
c                     has to call GENBBsub one time for one decay event;
c         ievstart  - number of first event;
c         irndmst   - (only for ievstart > 1) initial random integer
c                     for RDMIN;
c         iwrfile   - iwrfile=0 - do not write generated events in file,
c                     iwrfile=1 - to write generated events in file;
c         chfile    - (only for iwrfile=1) character*40 name of file;
c         icurrent  - current number of event; if last event, file
c                     GENBB.report is created;
c         ebb1,ebb2 - (only for i2bbs=1) left and right energy limits for
c                     sum of energies of emitted e-/e+; other events will
c                     be thrown away (for modebb = 4,5,6,8,10 and 13);
c
c
c                     For artificial event (i2bbs=2 and chnuclide='Art')
c                     (emission of up to 10 beta, e+e- pairs and all
c                     GEANT particles in region of energies and angles
c                     with time delay and halflife):
c
c         nartparts   - number of parts of artificial event (up to 10);
c         chart       - character*4 array of identifiers:
c                       GP - emission of GEANT particle,
c                       Pi - emission of internal conversion e+/e- pair,
c                       Be - beta decay;
c         artemin,    - arrays of min and max values for particles kinetic
c         artemax       energies (MeV);
c         arttmin,    - arrays of min and max values for particles teta
c         arttmax       angles (degree);
c         artfmin,    - arrays of min and max values for particles phi
c         artfmax       angles (degree);
c         artQb       - (for chart='Be') array of Qbeta values (MeV),
c                       (for chart='Pi') array of pair kinetic energy (MeV);
c         artZd       - (for chart='Be') array of atomic numbers of daughter
c                       nucleus (artZd>0 for beta- and artZd<0 for beta+ decay);
c         arttdelay   - array of time delays: minimum time between current
c                       part of event and previous one (sec);
c         artthalf    - array of half-lives of current part of events (sec);
c         nartnpg     - array of GEANT number particles:
c                        1 - gamma         2 - positron     3 - electron
c                        4 - neutrino      5 - muon+        6 - muon-
c                        7 - pion0         8 - pion+        9 - pion-
c                       10 - kaon0 long   11 - kaon+       12 - kaon-
c                       13 - neutron      14 - proton      15 - antiproton
c                       16 - kaon0 short  17 - eta         18 - lambda
c                       19 - sigma+       20 - sigma0      21 - sigma-
c                       22 - xi0          23 - xi-         24 - omega
c                       25 - antineutron  26 - antilambda  27 - antisigma-
c                       28 - antisigma0   29 - antisigma+  30 - antixi0
c                       31 - antixi+      32 - antiomega+  45 - deuteron
c                       46 - tritium      47 - alpha       48 - geantino
c                       49 - He3          50 - Cerenkov.
c
c                     For chart = 'GP' particle's energy will be uniformly
c                     distributed in the range [artemin,artemax]; particle
c                     will be emitted isotropically in the region of space
c                     determined by [arttmin,arttmax], [artfmin,artfmax].
c
c                     For chart = 'Be', energy will be sampled in [0,artQb] in
c                     accordance with beta decay energy spectrum; beta particle
c                     will be emitted isotropically in full space.
c
c                     For chart = 'Pi', energy of pair is fixed to artQb and
c                     divided half to half for e- and e+; e- and e+ are
c                     emitted in the same direction; this direction is
c                     distributed isotropically in full space.
c
c
c                     For generation of events of Compton effect (i2bbs=2 and
c                     chnuclide='Com'):
c
c         artemin(1), - min and max values for kinetic energy of initial
c         artemax(1)    gamma quantum (MeV);
c         arttmin(1), - min and max values for teta angle of initial particle
c         arttmax(1)    (degree);
c         artfmin(1), - min and max values for phi angle of initial particle
c         artfmax(1)    (degree);
c
c
c                     For generation of events of Moller scattering (i2bbs=2
c                     and chnuclide='Mol'):
c
c         artemin(1), - min and max values for kinetic energy of initial
c         artemax(1)    electron (MeV);
c         arttmin(1), - min and max values for teta angle of initial particle
c         arttmax(1)    (degree);
c         artfmin(1), - min and max values for phi angle of initial particle
c         artfmax(1)    (degree);
c         artQb(1)    - lower energy threshold for emitted delta rays (MeV);
c
c
c                     For generation of events of e+e- pair creation from
c                     external gamma quanta: (i2bbs=2 and chnuclide='Pai'):
c
c         artemin(1), - min and max values for kinetic energy of initial
c         artemax(1)    gamma quantum (MeV);
c         arttmin(1), - min and max values for teta angle of initial particle
c         arttmax(1)    (degree);
c         artfmin(1), - min and max values for phi angle of initial particle
c         artfmax(1)    (degree);
c         artZd(1)    - atomic number Z of target;
c
c Output: ier                   - ier=0/1 - everything is correct/incorrect in
c                                 input parameters;
c         tevst                 - time of event's start (sec);
c         npfull                - full number of emitted particles in event;
c         npgeant(1-npfull)     - array of GEANT numbers for particle identi-
c                                 fication (1 for gamma, 2 for e+, 3 for e-,
c                                 5 for muon+, 6 for muon-, 47 for alpha);
c         pmoment(1-3,1-npfull) - array of x,y,z components of particle momentum
c                                 (MeV/c);
c         ptime(1-npfull)       - array of time shift from previous time to
c                                 calculate when particle was emitted (sec);
c         toallevents           - (only for i2bbs=1 and restricted range for sum
c                                 of energies of emitted e-/e+) coefficient to
c                                 recalculate number of bb events in full range
c                                 of energies:
c                                 full_number=generated_number*toallevents
c                                 (for modebb = 4,5,6,8,10 and 13);
c         levelE                - (only for i2bbs=1) energy (in keV) of level of
c                                 daughter 2b nuclide;
c         chdspin               - (only for i2bbs=1) character*4 spin and parity of
c                                 the level of daughter nuclide in 2b decay.
c
c V.I.Tretyak, 04.12.1995.
c
	character chfile*256,chnuclide*16,chn*16,chart*4,chdspin*4
	character chmodebb*45
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	common/genbbpar/nevents,ievstart,irndmst,iwrfile,chfile
        common/const/pi,emass,datamass(50)
	common/enrange/ebb1,ebb2,toallevents,levelE,chdspin
	common/currentev/icurrent
	common/slate/isl(40)
	common/artificial/nartparts,    chart(10),
     +                    artemin(10),  artemax(10),
     +                    arttmin(10),  arttmax(10),
     +                    artfmin(10),  artfmax(10),
     +                    artQb(10),    artZd(10),
     +                    arttdelay(10),artthalf(10),
     +                    nartnpg(10)
        save Qbb,EK,Zdbb,Adbb,istartbb
	common/eta_nme/chi_GTw,chi_Fw,chip_GT,chip_F,chip_T,
     +                 chip_P,chip_R
	ier=0
	if(istart.eq.1) go to 1000
	chn=chnuclide
	if(i2bbs.eq.1) then
	   if((chn(1:2).eq.'Ca'.or.chn(1:2).eq.'CA'.or.chn(1:2).eq.'ca')
     +        .and.chn(3:4).eq.'48') then
	      chnuclide='Ca48'
	      Qbb=4.272
	      Zdbb=22.
	      Adbb=48.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'GENBBsub: illegal Ti48 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=984
	      if(ilevel.eq.2) levelE=2421
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2) itrans02=2
	   elseif((chn(1:2).eq.'Ni'.or.chn(1:2).eq.'NI'.or.
     +        chn(1:2).eq.'ni').and.chn(3:4).eq.'58') then
	      chnuclide='Ni58'
	      Qbb=1.926
	      Zdbb=-26.
	      Adbb=58.
	      EK=0.007
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'GENBBsub: illegal Fe58 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=811
	      if(ilevel.eq.2) levelE=1675
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2) itrans02=2
	   elseif((chn(1:2).eq.'Zn'.or.chn(1:2).eq.'ZN'.or.
     +        chn(1:2).eq.'zn').and.chn(3:4).eq.'64') then
	      chnuclide='Zn64'
	      Qbb=1.096
	      Zdbb=-28.
	      Adbb=64.
	      EK=0.008
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Ni64 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Zn'.or.chn(1:2).eq.'ZN'.or.
     +        chn(1:2).eq.'zn').and.chn(3:4).eq.'70') then
	      chnuclide='Zn70'
	      Qbb=1.001
	      Zdbb=32.
	      Adbb=70.
	      EK=0.
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Ge70 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Ge'.or.chn(1:2).eq.'GE'.or.
     +        chn(1:2).eq.'ge').and.chn(3:4).eq.'76') then
	      chnuclide='Ge76'
	      Qbb=2.039
	      Zdbb=34.
	      Adbb=76.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.3) then
	         print *,'GENBBsub: illegal Se76 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=559
	      if(ilevel.eq.2) levelE=1122
	      if(ilevel.eq.3) levelE=1216
	      if(ilevel.eq.0.or.ilevel.eq.2) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.3) itrans02=2
	   elseif((chn(1:2).eq.'Se'.or.chn(1:2).eq.'SE'.or.
     +        chn(1:2).eq.'se').and.chn(3:4).eq.'74') then
	      chnuclide='Se74'
	      Qbb=1.209
	      Zdbb=-32.
	      Adbb=74.
	      EK=0.011
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'GENBBsub: illegal Ge74 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=596
	      if(ilevel.eq.2) levelE=1204
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2) itrans02=2
	   elseif((chn(1:2).eq.'Se'.or.chn(1:2).eq.'SE'.or.
     +        chn(1:2).eq.'se').and.chn(3:4).eq.'82') then
	      chnuclide='Se82'
	      Qbb=2.995
	      Zdbb=36.
	      Adbb=82.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'GENBBsub: illegal Kr82 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=776
	      if(ilevel.eq.2) levelE=1475
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2) itrans02=2
	   elseif((chn(1:2).eq.'Sr'.or.chn(1:2).eq.'SR'.or.
     +        chn(1:2).eq.'sr').and.chn(3:4).eq.'84') then
	      chnuclide='Sr84'
	      Qbb=1.787
	      Zdbb=-36.
	      Adbb=84.
	      EK=0.014
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'GENBBsub: illegal Kr84 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=882
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	   elseif((chn(1:2).eq.'Zr'.or.chn(1:2).eq.'ZR'.or.
     +        chn(1:2).eq.'zr').and.chn(3:4).eq.'94') then
	      chnuclide='Zr94'
	      Qbb=1.144
	      Zdbb=42.
	      Adbb=94.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'GENBBsub: illegal Mo94 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=871
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	   elseif((chn(1:2).eq.'Zr'.or.chn(1:2).eq.'ZR'.or.
     +        chn(1:2).eq.'zr').and.chn(3:4).eq.'96') then
	      chnuclide='Zr96'
	      Qbb=3.350
	      Zdbb=42.
	      Adbb=96.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.9) then
	         print *,'GENBBsub: illegal Mo96 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=778
	      if(ilevel.eq.2) levelE=1148
	      if(ilevel.eq.3) levelE=1498
	      if(ilevel.eq.4) levelE=1626
	      if(ilevel.eq.5) levelE=2096
	      if(ilevel.eq.6) levelE=2426
	      if(ilevel.eq.7) levelE=2623
	      if(ilevel.eq.8) levelE=2700
	      if(ilevel.eq.9) levelE=2713
	      if(ilevel.eq.0.or.ilevel.eq.2.or.ilevel.eq.7) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.3.or.ilevel.eq.4.or.
     +           ilevel.eq.5.or.ilevel.eq.6.or.ilevel.eq.8.or.
c we suppose here that 2713 keV level is 2+
     +           ilevel.eq.9) itrans02=2
	   elseif((chn(1:2).eq.'Mo'.or.chn(1:2).eq.'MO'.or.
     +        chn(1:2).eq.'mo').and.chn(3:4).eq.'92') then
	      chnuclide='Mo92'
	      Qbb=1.649
	      Zdbb=-40.
	      Adbb=92.
	      EK=0.018
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'GENBBsub: illegal Zr92 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=934
	      if(ilevel.eq.2) levelE=1383
	      if(ilevel.eq.0.or.ilevel.eq.2) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	   elseif((chn(1:2).eq.'Mo'.or.chn(1:2).eq.'MO'.or.
     +        chn(1:2).eq.'mo').and.chn(3:5).eq.'100') then
	      chnuclide='Mo100'
	      Qbb=3.034
	      Zdbb=44.
	      Adbb=100.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.4) then
	         print *,'GENBBsub: illegal Ru100 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=540
	      if(ilevel.eq.2) levelE=1130
	      if(ilevel.eq.3) levelE=1362
	      if(ilevel.eq.4) levelE=1741
	      if(ilevel.eq.0.or.ilevel.eq.2.or.ilevel.eq.4) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.3) itrans02=2
	   elseif((chn(1:2).eq.'Ru'.or.chn(1:2).eq.'RU'.or.
     +        chn(1:2).eq.'ru').and.chn(3:4).eq.'96') then
	      chnuclide='Ru96'
	      Qbb=2.718
	      Zdbb=-42.
	      Adbb=96.
	      EK=0.020
	      if(ilevel.lt.0.or.ilevel.gt.9) then
	         print *,'GENBBsub: illegal Mo96 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=778
	      if(ilevel.eq.2) levelE=1148
	      if(ilevel.eq.3) levelE=1498
	      if(ilevel.eq.4) levelE=1626
	      if(ilevel.eq.5) levelE=2096
	      if(ilevel.eq.6) levelE=2426
	      if(ilevel.eq.7) levelE=2623
	      if(ilevel.eq.8) levelE=2700
	      if(ilevel.eq.9) levelE=2713
	      if(ilevel.eq.0.or.ilevel.eq.2.or.ilevel.eq.7) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.3.or.ilevel.eq.4.or.
     +           ilevel.eq.5.or.ilevel.eq.6.or.ilevel.eq.8.or.
c we suppose here that 2713 keV level is 2+
     +           ilevel.eq.9) itrans02=2
	      if(ilevel.eq.8) EK=0.003
	      if(ilevel.eq.9) EK=0.002
	   elseif((chn(1:2).eq.'Ru'.or.chn(1:2).eq.'RU'.or.
     +        chn(1:2).eq.'ru').and.chn(3:5).eq.'104') then
	      chnuclide='Ru104'
	      Qbb=1.301
	      Zdbb=46.
	      Adbb=104.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'GENBBsub: illegal Pd104 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=556
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +        chn(1:2).eq.'cd').and.chn(3:5).eq.'106') then
	      chnuclide='Cd106'
	      Qbb=2.771
	      Zdbb=-46.
	      Adbb=106.
              EK=0.024
	      if(ilevel.lt.0.or.ilevel.gt.5) then
	         print *,'GENBBsub: illegal Pd106 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=512
	      if(ilevel.eq.2) levelE=1128
	      if(ilevel.eq.3) levelE=1134
	      if(ilevel.eq.4) levelE=1562
	      if(ilevel.eq.5) levelE=1706
	      if(ilevel.eq.0.or.ilevel.eq.3.or.ilevel.eq.5)
     +           itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2.or.ilevel.eq.4)
     +           itrans02=2
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +        chn(1:2).eq.'cd').and.chn(3:5).eq.'108') then
	      chnuclide='Cd108'
	      Qbb=0.269
	      Zdbb=-46.
	      Adbb=108.
	      EK=0.024
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Pd108 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +        chn(1:2).eq.'cd').and.chn(3:5).eq.'114') then
	      chnuclide='Cd114'
	      Qbb=0.536
	      Zdbb=50.
	      Adbb=114.
	      EK=0.
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Sn114 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +        chn(1:2).eq.'cd').and.chn(3:5).eq.'116') then
	      chnuclide='Cd116'
	      Qbb=2.805
	      Zdbb=50.
	      Adbb=116.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.5) then
	         print *,'GENBBsub: illegal Sn116 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=1294
	      if(ilevel.eq.2) levelE=1757
	      if(ilevel.eq.3) levelE=2027
	      if(ilevel.eq.4) levelE=2112
	      if(ilevel.eq.5) levelE=2225
	      if(ilevel.eq.0.or.ilevel.eq.2.or.ilevel.eq.3) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.4.or.ilevel.eq.5) itrans02=2
	   elseif((chn(1:2).eq.'Sn'.or.chn(1:2).eq.'SN'.or.
     +        chn(1:2).eq.'sn').and.chn(3:5).eq.'112') then
	      chnuclide='Sn112'
	      Qbb=1.919
	      Zdbb=-48.
	      Adbb=112.
	      EK=0.027
	      if(ilevel.lt.0.or.ilevel.gt.6) then
	         print *,'GENBBsub: illegal Cd112 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=618
	      if(ilevel.eq.2) levelE=1224
	      if(ilevel.eq.3) levelE=1312
	      if(ilevel.eq.4) levelE=1433
	      if(ilevel.eq.5) levelE=1469
	      if(ilevel.eq.6) levelE=1871
	      if(ilevel.eq.0.or.ilevel.eq.2.or.ilevel.eq.4.or.
     +           ilevel.eq.6) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.3.or.ilevel.eq.5) itrans02=2
	   elseif((chn(1:2).eq.'Sn'.or.chn(1:2).eq.'SN'.or.
     +        chn(1:2).eq.'sn').and.chn(3:5).eq.'122') then
	      chnuclide='Sn122'
	      Qbb=0.368
	      Zdbb=52.
	      Adbb=122.
	      EK=0.
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Te122 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Sn'.or.chn(1:2).eq.'SN'.or.
     +        chn(1:2).eq.'sn').and.chn(3:5).eq.'124') then
	      chnuclide='Sn124'
	      Qbb=2.288
	      Zdbb=52.
	      Adbb=124.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.8) then
	         print *,'GENBBsub: illegal Te124 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=603
	      if(ilevel.eq.2) levelE=1326
	      if(ilevel.eq.3) levelE=1657
	      if(ilevel.eq.4) levelE=1883
	      if(ilevel.eq.5) levelE=2039
	      if(ilevel.eq.6) levelE=2092
	      if(ilevel.eq.7) levelE=2153
	      if(ilevel.eq.8) levelE=2182
	      if(ilevel.eq.0.or.ilevel.eq.3.or.ilevel.eq.4.or.
     +           ilevel.eq.7) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2.or.ilevel.eq.5.or.
     +           ilevel.eq.6.or.ilevel.eq.8) itrans02=2
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +        chn(1:2).eq.'te').and.chn(3:5).eq.'120') then
	      chnuclide='Te120'
	      Qbb=1.698
	      Zdbb=-50.
	      Adbb=120.
	      EK=0.029
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'GENBBsub: illegal Sn120 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=1171
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +        chn(1:2).eq.'te').and.chn(3:5).eq.'128') then
	      chnuclide='Te128'
	      Qbb=0.867
	      Zdbb=54.
	      Adbb=128.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'GENBBsub: illegal Xe128 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=443
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +        chn(1:2).eq.'te').and.chn(3:5).eq.'130') then
	      chnuclide='Te130'
	      Qbb=2.529
	      Zdbb=54.
	      Adbb=130.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.3) then
	         print *,'GENBBsub: illegal Xe130 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=536
	      if(ilevel.eq.2) levelE=1122
	      if(ilevel.eq.3) levelE=1794
	      if(ilevel.eq.0.or.ilevel.eq.3) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2) itrans02=2
	   elseif((chn(1:2).eq.'Xe'.or.chn(1:2).eq.'XE'.or.
     +        chn(1:2).eq.'xe').and.chn(3:5).eq.'136') then
	      chnuclide='Xe136'
	      Qbb=2.468
	      Zdbb=56.
	      Adbb=136.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.9) then
	         print *,'GENBBsub: illegal Ba136 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=819
	      if(ilevel.eq.2) levelE=1551
	      if(ilevel.eq.3) levelE=1579
	      if(ilevel.eq.4) levelE=2080
	      if(ilevel.eq.5) levelE=2129
	      if(ilevel.eq.6) levelE=2141
	      if(ilevel.eq.7) levelE=2223
	      if(ilevel.eq.8) levelE=2315
	      if(ilevel.eq.9) levelE=2400
	      if(ilevel.eq.0.or.ilevel.eq.3.or.ilevel.eq.6.or.ilevel
     +           .eq.8) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2.or.ilevel.eq.4.or.ilevel
     +           .eq.5.or.ilevel.eq.7.or.ilevel.eq.9) itrans02=2
	   elseif((chn(1:2).eq.'Ce'.or.chn(1:2).eq.'CE'.or.
     +        chn(1:2).eq.'ce').and.chn(3:5).eq.'136') then
	      chnuclide='Ce136'
	      Qbb=2.419
	      Zdbb=-56.
	      Adbb=136.
              EK=0.037
	      if(ilevel.lt.0.or.ilevel.gt.9) then
	         print *,'GENBBsub: illegal Ba136 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=819
	      if(ilevel.eq.2) levelE=1551
	      if(ilevel.eq.3) levelE=1579
	      if(ilevel.eq.4) levelE=2080
	      if(ilevel.eq.5) levelE=2129
	      if(ilevel.eq.6) levelE=2141
	      if(ilevel.eq.7) levelE=2223
	      if(ilevel.eq.8) levelE=2315
	      if(ilevel.eq.9) levelE=2400
	      if(ilevel.eq.0.or.ilevel.eq.3.or.ilevel.eq.6.or.ilevel
     +           .eq.8) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2.or.ilevel.eq.4.or.ilevel
     +           .eq.5.or.ilevel.eq.7.or.ilevel.eq.9) itrans02=2
c only 2L capture is allowed energetically for 2400 keV level
	      if(ilevel.eq.9) EK=0.006
	   elseif((chn(1:2).eq.'Ce'.or.chn(1:2).eq.'CE'.or.
     +        chn(1:2).eq.'ce').and.chn(3:5).eq.'138') then
	      chnuclide='Ce138'
	      Qbb=0.693
	      Zdbb=-56.
	      Adbb=138.
	      EK=0.037
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Ba138 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Ce'.or.chn(1:2).eq.'CE'.or.
     +        chn(1:2).eq.'ce').and.chn(3:5).eq.'142') then
	      chnuclide='Ce142'
	      Qbb=1.417
	      Zdbb=60.
	      Adbb=142.
	      EK=0.
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Nd142 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Nd'.or.chn(1:2).eq.'ND'.or.
     +        chn(1:2).eq.'nd').and.chn(3:5).eq.'148') then
	      chnuclide='Nd148'
	      Qbb=1.929
	      Zdbb=62.
	      Adbb=148.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'GENBBsub: illegal Sm148 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=550
	      if(ilevel.eq.2) levelE=1455
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2) itrans02=2
	   elseif((chn(1:2).eq.'Nd'.or.chn(1:2).eq.'ND'.or.
     +        chn(1:2).eq.'nd').and.chn(3:5).eq.'150') then
	      chnuclide='Nd150'
	      Qbb=3.367
	      Zdbb=62.
	      Adbb=150.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.5) then
	         print *,'GENBBsub: illegal Sm150 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=334
	      if(ilevel.eq.2) levelE=740
	      if(ilevel.eq.3) levelE=1046
	      if(ilevel.eq.4) levelE=1194
	      if(ilevel.eq.5) levelE=1256
	      if(ilevel.eq.0.or.ilevel.eq.2.or.ilevel.eq.5) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.3.or.ilevel.eq.4) itrans02=2

ccccc

	   elseif((chn(1:2).eq.'Dy'.or.chn(1:2).eq.'DY'.or.
     +        chn(1:2).eq.'dy').and.chn(3:5).eq.'156') then
	      chnuclide='Dy156'
	      Qbb=2.012
	      Zdbb=-64.
	      Adbb=156.
              EK=0.050
	      if(ilevel.lt.0.or.ilevel.gt.15) then
	         print *,'GENBBsub: illegal Gd156 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq. 0) levelE=0
	      if(ilevel.eq. 1) levelE=89
	      if(ilevel.eq. 2) levelE=1050
	      if(ilevel.eq. 3) levelE=1129
	      if(ilevel.eq. 4) levelE=1154
	      if(ilevel.eq. 5) levelE=1168
	      if(ilevel.eq. 6) levelE=1258
	      if(ilevel.eq. 7) levelE=1715
	      if(ilevel.eq. 8) levelE=1771
	      if(ilevel.eq. 9) levelE=1828
	      if(ilevel.eq.10) levelE=1851
	      if(ilevel.eq.11) levelE=1915
	      if(ilevel.eq.12) levelE=1946
	      if(ilevel.eq.13) levelE=1952
	      if(ilevel.eq.14) levelE=1989
	      if(ilevel.eq.15) levelE=2004
	      if(ilevel.eq.0  .or. ilevel.eq.2  .or. ilevel.eq.5 .or.
     +         ilevel.eq.7  .or. ilevel.eq.10 .or. ilevel.eq.14) 
     +         itrans02=0
	      if(ilevel.eq.1  .or. ilevel.eq.3  .or. ilevel.eq.4 .or.
     +         ilevel.eq.6  .or. ilevel.eq.8  .or. ilevel.eq.9 .or.
     +         ilevel.eq.11 .or. ilevel.eq.15) itrans02=2
	      if(ilevel.ge.11 .and. ilevel.le.13) EK=0.029
	      if(ilevel.eq.14) EK=0.008
	      if(ilevel.eq.15) EK=0.004
	   elseif((chn(1:2).eq.'Dy'.or.chn(1:2).eq.'DY'.or.
     +        chn(1:2).eq.'dy').and.chn(3:5).eq.'158') then
	      chnuclide='Dy158'
	      Qbb=0.285
	      Zdbb=-64.
	      Adbb=156.
              EK=0.050
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'GENBBsub: illegal Gd158 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=80
	      if(ilevel.eq.2) levelE=261
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	      if(ilevel.eq.2) EK=0.008
	   elseif((chn(1:1).eq.'W'.or.chn(1:1).eq.'w').and.
     +            chn(2:4).eq.'180') then
	      chnuclide='W180'
	      Qbb=0.144
	      Zdbb=-72.
	      Adbb=180.
	      EK=0.065
	      if(ilevel.lt.0.or.ilevel.gt.0) then
	         print *,'GENBBsub: illegal Hf180 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:1).eq.'W'.or.chn(1:1).eq.'w').and.
     +            chn(2:4).eq.'186') then
	      chnuclide='W186'
	      Qbb=0.490
	      Zdbb=76.
	      Adbb=186.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'GENBBsub: illegal Os186 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=137
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	   elseif((chn(1:2).eq.'Pt'.or.chn(1:2).eq.'PT'.or.
     +        chn(1:2).eq.'pt').and.chn(3:5).eq.'190') then
	      chnuclide='Pt190'
	      Qbb=1.383
	      Zdbb=-76.
	      Adbb=190.
	      EK=0.074
	      if(ilevel.lt.0.or.ilevel.gt.5) then
	         print *,'GENBBsub: illegal Os190 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=187
	      if(ilevel.eq.2) levelE=558
	      if(ilevel.eq.3) levelE=912
	      if(ilevel.eq.4) levelE=1115
	      if(ilevel.eq.5) levelE=1382
	      if(ilevel.eq.0.or.ilevel.eq.3.or.ilevel.eq.5) itrans02=0
	      if(ilevel.eq.1.or.ilevel.eq.2.or.ilevel.eq.4) itrans02=2
c for decay to 1382 keV level, electron captures from N or higher atomic shells are possible
	      if(ilevel.eq.5) EK=0.
	   elseif((chn(1:2).eq.'Pt'.or.chn(1:2).eq.'PT'.or.
     +        chn(1:2).eq.'pt').and.chn(3:5).eq.'198') then
	      chnuclide='Pt198'
	      Qbb=1.047
	      Zdbb=80.
	      Adbb=198.
	      EK=0.
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'GENBBsub: illegal Hg198 level ',ilevel
	         ier=1
	         return
	      endif
	      if(ilevel.eq.0) levelE=0
	      if(ilevel.eq.1) levelE=412
	      if(ilevel.eq.0) itrans02=0
	      if(ilevel.eq.1) itrans02=2
	   elseif((chn(1:2).eq.'Bi'.or.chn(1:2).eq.'BI'.or.
     +        chn(1:2).eq.'bi').and.chn(3:5).eq.'214') then
	      chnuclide='Bi214'
	      Qbb=2.180
	      Zdbb=85.
	      Adbb=214.
	      EK=0.
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal At214 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Pb'.or.chn(1:2).eq.'PB'.or.
     +        chn(1:2).eq.'pb').and.chn(3:5).eq.'214') then
	      chnuclide='Pb214'
	      Qbb=4.289
	      Zdbb=84.
	      Adbb=214.
	      EK=0.
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Po214 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Po'.or.chn(1:2).eq.'PO'.or.
     +        chn(1:2).eq.'po').and.chn(3:5).eq.'218') then
	      chnuclide='Po218'
	      Qbb=3.141
	      Zdbb=86.
	      Adbb=218.
	      EK=0.
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Rn218 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   elseif((chn(1:2).eq.'Rn'.or.chn(1:2).eq.'RN'.or.
     +        chn(1:2).eq.'rn').and.chn(3:5).eq.'222') then
	      chnuclide='Rn222'
	      Qbb=2.052
	      Zdbb=88.
	      Adbb=222.
	      EK=0.
	      if(ilevel.ne.0) then
	         print *,'GENBBsub: illegal Ra222 level ',ilevel
	         ier=1
	         return
	      endif
	      levelE=0
	      itrans02=0
	   else
	      print *,'unknown double beta nuclide'
	      ier=1
	      return
	   endif
	   if(itrans02.eq.0) chdspin='0+'
	   if(itrans02.eq.2) chdspin='2+'
	   if(itrans02.eq.0 .and. chnuclide.eq.'Bi214') chdspin='1-'
	   if(modebb.lt.1.or.modebb.gt.18) then
	      print *,'unknown bb mode'
	      ier=1
	      return
	   else
	      if(modebb.eq.1)  chmodebb='0nubb(mn) 0+ -> 0+     {2n}'
	      if(modebb.eq.2)  
     +           chmodebb='0nubb(rhc-lambda) 0+ -> 0+     {2n}'
	      if(modebb.eq.3)  
     +           chmodebb='0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*}'
	      if(modebb.eq.4)  chmodebb='2nubb     0+ -> 0+     {2n}'
	      if(modebb.eq.5)  chmodebb='0nubbM1   0+ -> 0+     {2n}'
	      if(modebb.eq.6)  chmodebb='0nubbM3   0+ -> 0+     {2n}'
	      if(modebb.eq.7)  
     +           chmodebb='0nubb(rhc-lambda) 0+ -> 2+     {2n}'
	      if(modebb.eq.8)
     +           chmodebb='2nubb     0+ -> 2+     {2n}, {N*}'
	      if(modebb.eq.9)  chmodebb='0nuKb+    0+ -> 0+, 2+'
	      if(modebb.eq.10) chmodebb='2nuKb+    0+ -> 0+, 2+'
	      if(modebb.eq.11) chmodebb='0nu2K     0+ -> 0+, 2+'
	      if(modebb.eq.12) chmodebb='2nu2K     0+ -> 0+, 2+'
	      if(modebb.eq.13) chmodebb='0nubbM7   0+ -> 0+     {2n}'
	      if(modebb.eq.14) chmodebb='0nubbM2   0+ -> 0+     {2n}'
	      if(modebb.eq.15) 
     +           chmodebb='2nubb     0+ -> 0+  bosonic nu'
	      if(modebb.eq.16) 
     +           chmodebb='2nubb     0+ -> 2+  bosonic nu'
	      if(modebb.eq.17) 
     +           chmodebb='0nubb(rhc-eta) 0+ -> 0+ {2n} simplified'
	      if(modebb.eq.18) 
     +           chmodebb='0nubb(rhc-eta) 0+ -> 0+ {2n} with NMEs'
	   endif
c checking the consistency of data: (1) energy
	   El=levelE/1000.
	   if(Zdbb.ge.0.)                   e0=Qbb
	   if(Zdbb.lt.0.)                   e0=Qbb-4.*emass
	   if(modebb.eq. 9.or.modebb.eq.10) e0=Qbb-EK-2.*emass
	   if(modebb.eq.11.or.modebb.eq.12) e0=Qbb-2.*EK
	   if(e0.le.El) then
	      print *,'not enough energy for transition to this level:'
	      print *,'full energy release and Elevel = ',e0,El
	      ier=1
	   return
	   endif
c (2) spin of level and mode of decay
	   m=modebb
	   if(itrans02.eq.0. and.
     +        (m.eq. 1.or.m.eq. 2.or.m.eq. 3.or.m.eq. 4.or.m.eq. 5.or.
     +         m.eq. 6.or.m.eq. 9.or.m.eq.10.or.m.eq.11.or.m.eq.12.or.
     +         m.eq.13.or.m.eq.14.or.m.eq.15.or.m.eq.17.or.m.eq.18)) 
     +         go to 2
	   if(itrans02.eq.2. and.
     +        (m.eq. 3.or.m.eq. 7.or.m.eq. 8.or.m.eq. 9.or.m.eq.10.or.
     +         m.eq.11.or.m.eq.12.or.m.eq.16)) go to 2
	   print 1,chmodebb,chdspin
1	   format(' decay mode and spin of daughter nucleus level'/
     +            ' are inconsistent: ',a,'  ',a)
	   ier=1
	   return
c (3) nuclide and mode of decay
c2	   if(Zdbb.ge.0..and.modebb.eq. 9.or.modebb.eq.10.or.
c     +                       modebb.eq.11.or.modebb.eq.12) then
2	   if(Zdbb.ge.0..and.(modebb.eq. 9.or.modebb.eq.10.or.
     +                       modebb.eq.11.or.modebb.eq.12)) then
            print *,'decay mode and nuclide are inconsistent:'
	      print 3,chmodebb,chnuclide
3	      format(' ',a,'  ',a)
	      ier=1
	      return
	   endif
	endif
	if(i2bbs.eq.2) then
	   if((chn(1:2).eq.'Ac'.or.chn(1:2).eq.'AC'.or.chn(1:2).eq.'ac')
     +        .and.chn(3:5).eq.'228') then
	      chnuclide='Ac228'
	   elseif((chn(1:2).eq.'Am'.or.chn(1:2).eq.'AM'.or.
     +             chn(1:2).eq.'am').and.chn(3:5).eq.'241') then
	      chnuclide='Am241'
	   elseif((chn(1:2).eq.'Ar'.or.chn(1:2).eq.'AR'.or.
     +             chn(1:2).eq.'ar').and.chn(3:4).eq.'39') then
	      chnuclide='Ar39'
	   elseif((chn(1:2).eq.'Ar'.or.chn(1:2).eq.'AR'.or.
     +             chn(1:2).eq.'ar').and.chn(3:4).eq.'42') then
	      chnuclide='Ar42'
	   elseif((chn(1:2).eq.'As'.or.chn(1:2).eq.'AS'.or.
     +             chn(1:2).eq.'as').and.chn(3:4).eq.'79') then
	      chnuclide='As79'
	   elseif((chn(1:2).eq.'Bi'.or.chn(1:2).eq.'BI'.or.
     +             chn(1:2).eq.'bi').and.chn(3:5).eq.'207') then
	      chnuclide='Bi207'
	   elseif((chn(1:2).eq.'Bi'.or.chn(1:2).eq.'BI'.or.
     +             chn(1:2).eq.'bi').and.chn(3:5).eq.'208') then
	      chnuclide='Bi208'
	   elseif((chn(1:2).eq.'Bi'.or.chn(1:2).eq.'BI'.or.
     +             chn(1:2).eq.'bi').and.chn(3:5).eq.'210') then
	      chnuclide='Bi210'
	   elseif((chn(1:2).eq.'Bi'.or.chn(1:2).eq.'BI'.or.
     +             chn(1:2).eq.'bi').and.chn(3:5).eq.'212') then
	      chnuclide='Bi212'
	   elseif((chn(1:2).eq.'Bi'.or.chn(1:2).eq.'BI'.or.
     +             chn(1:2).eq.'bi').and.chn(3:5).eq.'214') then
	      chnuclide='Bi214'
	   elseif((chn(1:1).eq.'C'.or.chn(1:1).eq.'c')
     +                              .and.chn(2:3).eq.'14') then
	      chnuclide='C14'
	   elseif((chn(1:2).eq.'Ca'.or.chn(1:2).eq.'CA'.or.
     +             chn(1:2).eq.'ca').and.chn(3:4).eq.'48') then
	      chnuclide='Ca48'
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +             chn(1:2).eq.'cd').and.chn(3:5).eq.'113') then
	      chnuclide='Cd113'
	   elseif((chn(1:2).eq.'Co'.or.chn(1:2).eq.'CO'.or.
     +             chn(1:2).eq.'co').and.chn(3:4).eq.'60') then
	      chnuclide='Co60'
	   elseif((chn(1:2).eq.'Cs'.or.chn(1:2).eq.'CS'.or.
     +             chn(1:2).eq.'cs').and.chn(3:5).eq.'136') then
	      chnuclide='Cs136'
	   elseif((chn(1:2).eq.'Cs'.or.chn(1:2).eq.'CS'.or.
     +             chn(1:2).eq.'cs').and.chn(3:5).eq.'137') then
	      chnuclide='Cs137'
	   elseif((chn(1:2).eq.'Eu'.or.chn(1:2).eq.'EU'.or.
     +             chn(1:2).eq.'eu').and.chn(3:5).eq.'147') then
	      chnuclide='Eu147'
	   elseif((chn(1:2).eq.'Eu'.or.chn(1:2).eq.'EU'.or.
     +             chn(1:2).eq.'eu').and.chn(3:5).eq.'152') then
	      chnuclide='Eu152'
	   elseif((chn(1:2).eq.'Eu'.or.chn(1:2).eq.'EU'.or.
     +             chn(1:2).eq.'eu').and.chn(3:5).eq.'154') then
	      chnuclide='Eu154'
	   elseif((chn(1:2).eq.'Gd'.or.chn(1:2).eq.'GD'.or.
     +             chn(1:2).eq.'gd').and.chn(3:5).eq.'146') then
	      chnuclide='Gd146'
	   elseif((chn(1:2).eq.'Hf'.or.chn(1:2).eq.'HF'.or.
     +             chn(1:2).eq.'hf').and.chn(3:5).eq.'182') then
	      chnuclide='Hf182'
	   elseif((chn(1:1).eq.'I'.or.chn(1:1).eq.'i')
     +        .and.chn(2:4).eq.'126') then
	      chnuclide='I126'
	   elseif((chn(1:1).eq.'I'.or.chn(1:1).eq.'i')
     +        .and.chn(2:4).eq.'133') then
	      chnuclide='I133'
	   elseif((chn(1:1).eq.'I'.or.chn(1:1).eq.'i')
     +        .and.chn(2:4).eq.'134') then
	      chnuclide='I134'
	   elseif((chn(1:1).eq.'I'.or.chn(1:1).eq.'i')
     +        .and.chn(2:4).eq.'135') then
	      chnuclide='I135'
	   elseif((chn(1:1).eq.'K'.or.chn(1:1).eq.'k')
     +        .and.chn(2:3).eq.'40') then
	      chnuclide='K40'
	   elseif((chn(1:1).eq.'K'.or.chn(1:1).eq.'k')
     +        .and.chn(2:3).eq.'42') then
	      chnuclide='K42'
	   elseif((chn(1:2).eq.'Kr'.or.chn(1:2).eq.'KR'.or.
     +             chn(1:2).eq.'kr').and.chn(3:4).eq.'81') then
	      chnuclide='Kr81'
	   elseif((chn(1:2).eq.'Kr'.or.chn(1:2).eq.'KR'.or.
     +             chn(1:2).eq.'kr').and.chn(3:4).eq.'85') then
	      chnuclide='Kr85'
	   elseif((chn(1:2).eq.'Mn'.or.chn(1:2).eq.'MN'.or.
     +             chn(1:2).eq.'mn').and.chn(3:4).eq.'54') then
	      chnuclide='Mn54'
	   elseif((chn(1:2).eq.'Na'.or.chn(1:2).eq.'NA'.or.
     +             chn(1:2).eq.'na').and.chn(3:4).eq.'22') then
	      chnuclide='Na22'
	   elseif((chn(1:1).eq.'P'.or.chn(1:1).eq.'p')
     +                              .and.chn(2:3).eq.'32') then
	      chnuclide='P32'
	   elseif((chn(1:2).eq.'Pa'.or.chn(1:2).eq.'PA'.or.
     +             chn(1:2).eq.'pa').and.chn(3:6).eq.'234m') then
	      chnuclide='Pa234m'
	   elseif((chn(1:2).eq.'Pb'.or.chn(1:2).eq.'PB'.or.
     +             chn(1:2).eq.'pb').and.chn(3:5).eq.'210') then
	      chnuclide='Pb210'
	   elseif((chn(1:2).eq.'Pb'.or.chn(1:2).eq.'PB'.or.
     +             chn(1:2).eq.'pb').and.chn(3:5).eq.'211') then
	      chnuclide='Pb211'
	   elseif((chn(1:2).eq.'Pb'.or.chn(1:2).eq.'PB'.or.
     +             chn(1:2).eq.'pb').and.chn(3:5).eq.'212') then
	      chnuclide='Pb212'
	   elseif((chn(1:2).eq.'Pb'.or.chn(1:2).eq.'PB'.or.
     +             chn(1:2).eq.'pb').and.chn(3:5).eq.'214') then
	      chnuclide='Pb214'
	   elseif((chn(1:2).eq.'Ra'.or.chn(1:2).eq.'RA'.or.
     +             chn(1:2).eq.'ra').and.chn(3:5).eq.'228') then
	      chnuclide='Ra228'
	   elseif((chn(1:2).eq.'Rb'.or.chn(1:2).eq.'RB'.or.
     +             chn(1:2).eq.'rb').and.chn(3:4).eq.'87') then
	      chnuclide='Rb87'
	   elseif((chn(1:2).eq.'Rh'.or.chn(1:2).eq.'RH'.or.
     +             chn(1:2).eq.'rh').and.chn(3:5).eq.'106') then
	      chnuclide='Rh106'
	   elseif((chn(1:2).eq.'Sb'.or.chn(1:2).eq.'SB'.or.
     +             chn(1:2).eq.'sb').and.chn(3:5).eq.'125') then
	      chnuclide='Sb125'
	   elseif((chn(1:2).eq.'Sb'.or.chn(1:2).eq.'SB'.or.
     +             chn(1:2).eq.'sb').and.chn(3:5).eq.'126') then
	      chnuclide='Sb126'
	   elseif((chn(1:2).eq.'Sb'.or.chn(1:2).eq.'SB'.or.
     +             chn(1:2).eq.'sb').and.chn(3:5).eq.'133') then
	      chnuclide='Sb133'
	   elseif((chn(1:2).eq.'Sr'.or.chn(1:2).eq.'SR'.or.
     +             chn(1:2).eq.'sr').and.chn(3:4).eq.'90') then
	      chnuclide='Sr90'
	   elseif((chn(1:2).eq.'Ta'.or.chn(1:2).eq.'TA'.or.
     +             chn(1:2).eq.'ta').and.chn(3:5).eq.'182') then
	      chnuclide='Ta182'
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +             chn(1:2).eq.'te').and.chn(3:6).eq.'133m') then
	      chnuclide='Te133m'
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +             chn(1:2).eq.'te').and.chn(3:5).eq.'133') then
	      chnuclide='Te133'
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +             chn(1:2).eq.'te').and.chn(3:5).eq.'134') then
	      chnuclide='Te134'
	   elseif((chn(1:2).eq.'Th'.or.chn(1:2).eq.'TH'.or.
     +             chn(1:2).eq.'th').and.chn(3:5).eq.'234') then
	      chnuclide='Th234'
	   elseif((chn(1:2).eq.'Tl'.or.chn(1:2).eq.'TL'.or.
     +             chn(1:2).eq.'tl').and.chn(3:5).eq.'207') then
	      chnuclide='Tl207'
	   elseif((chn(1:2).eq.'Tl'.or.chn(1:2).eq.'TL'.or.
     +             chn(1:2).eq.'tl').and.chn(3:5).eq.'208') then
	      chnuclide='Tl208'
	   elseif((chn(1:2).eq.'Xe'.or.chn(1:2).eq.'XE'.or.
     +             chn(1:2).eq.'xe').and.chn(3:6).eq.'129m') then
	      chnuclide='Xe129m'
	   elseif((chn(1:2).eq.'Xe'.or.chn(1:2).eq.'XE'.or.
     +             chn(1:2).eq.'xe').and.chn(3:6).eq.'131m') then
	      chnuclide='Xe131m'
	   elseif((chn(1:2).eq.'Xe'.or.chn(1:2).eq.'XE'.or.
     +             chn(1:2).eq.'xe').and.chn(3:5).eq.'133') then
	      chnuclide='Xe133'
	   elseif((chn(1:2).eq.'Xe'.or.chn(1:2).eq.'XE'.or.
     +             chn(1:2).eq.'xe').and.chn(3:5).eq.'135') then
	      chnuclide='Xe135'
	   elseif((chn(1:1).eq.'Y'.or.chn(1:1).eq.'y')
     +        .and.chn(2:3).eq.'88') then
	      chnuclide='Y88'
	   elseif((chn(1:1).eq.'Y'.or.chn(1:1).eq.'y')
     +        .and.chn(2:3).eq.'90') then
	      chnuclide='Y90'
	   elseif((chn(1:2).eq.'Zn'.or.chn(1:2).eq.'ZN'.or.
     +             chn(1:2).eq.'zn').and.chn(3:4).eq.'65') then
	      chnuclide='Zn65'
	   elseif((chn(1:2).eq.'Zr'.or.chn(1:2).eq.'ZR'.or.
     +             chn(1:2).eq.'zr').and.chn(3:4).eq.'96') then
	      chnuclide='Zr96'
	   elseif(chn(1:3).eq.'Art'.or.chn(1:3).eq.'ART'.or.
     +        chn(1:3).eq.'art') then
	      chnuclide='Artificial'
	      nartparts=min0(10,nartparts)
              do i=1,nartparts
	         chn=chart(i)
                 chart(i)='  '
	         if(chn(1:1).eq.'B' .or.chn(1:1).eq.'b')  chart(i)='Be'
  	         if(chn(1:1).eq.'G' .or.chn(1:1).eq.'g')  chart(i)='GP'
	         if(chn(1:1).eq.'P' .or.chn(1:1).eq.'p')  chart(i)='Pi'
	         if(chart(i).eq.'  ') then
	            print *,'unknown particle in artificial event'
	            ier=1
	            return
	         elseif(chart(i).eq.'Be') then
	            artemin(i)=artQb(i)
	            artemax(i)=artQb(i)
	            arttmin(i)=0.
	            arttmax(i)=pi
	            artfmin(i)=0.
	            artfmax(i)=2.*pi
	         elseif(chart(i).eq.'Pi') then
	            artemin(i)=artQb(i)
	            artemax(i)=artQb(i)
	            arttmin(i)=0.
	            arttmax(i)=pi
	            artfmin(i)=0.
	            artfmax(i)=2.*pi
	         else
	            arttmin(i)=arttmin(i)/180.*pi
	            arttmax(i)=arttmax(i)/180.*pi
	            artfmin(i)=artfmin(i)/180.*pi
	            artfmax(i)=artfmax(i)/180.*pi
	         endif
	      enddo
	   elseif(chn(1:3).eq.'Com'.or.chn(1:3).eq.'COM'.or.
     +        chn(1:3).eq.'com') then
	      chnuclide='Compton'
	      arttmin(1)=arttmin(1)/180.*pi
	      arttmax(1)=arttmax(1)/180.*pi
	      artfmin(1)=artfmin(1)/180.*pi
	      artfmax(1)=artfmax(1)/180.*pi
	   elseif(chn(1:3).eq.'Mol'.or.chn(1:3).eq.'MOL'.or.
     +        chn(1:3).eq.'mol') then
	      chnuclide='Moller'
	      arttmin(1)=arttmin(1)/180.*pi
	      arttmax(1)=arttmax(1)/180.*pi
	      artfmin(1)=artfmin(1)/180.*pi
	      artfmax(1)=artfmax(1)/180.*pi
	   elseif(chn(1:3).eq.'Pai'.or.chn(1:3).eq.'PAI'.or.
     +        chn(1:3).eq.'pai') then
	      chnuclide='E+E- external'
	      arttmin(1)=arttmin(1)/180.*pi
	      arttmax(1)=arttmax(1)/180.*pi
	      artfmin(1)=artfmin(1)/180.*pi
	      artfmax(1)=artfmax(1)/180.*pi
	   else
	      print *,'unknown background & source nuclide'
	      ier=1
	      return
	   endif
	endif
	if(ievstart.le.0) ievstart=1
	if(ievstart.eq.1) irndmst=0
	if(ievstart.ne.1) idummy=0 ! call rdmin(irndmst)
	if(iwrfile.eq.0) chfile='no file'
	if(chfile.ne.'no file') then
	   ! call datime(id,it)
	   open(unit=77,file=chfile,status='new')
	   write(77,8) chfile
8	   format(' DECAY0 generated file: ',a)
	   write(77,9) isl(3),isl(2),isl(1),isl(4),isl(5),isl(6)
9	   format(/'  date and hour         : ',
     +            2x,i2,'.',i2,'.',i4,4x,i2,':',i2,':',i2)
	   write(77,*) ' initial random number : ',
     +                 irndmst
	   write(77,*) '   '
c	   write(77,*) '#@seed=',irndmst
c	   write(77,*) '#@nevents=',nevents
c	   write(77,*) '#@type=',i2bbs
c	   write(77,*) '#@nuclide=',chnuclide
	   if(i2bbs.eq.1) then
	      write(77,10) chnuclide
10	      format('  event type: ',a)
	      write(77,*) '             ',chmodebb
c	      write(77,*) '#@modebb=',modebb
	      if(modebb.eq.18) then
	         write(77,*) '             NMEs: ',
     +                     'GTw,Fw,''GT,''F,''T,''P,''R ='
	         write(77,*) chi_GTw,chi_Fw,chip_GT,chip_F,
     +                     chip_T,chip_P,chip_R
	      endif
	      write(77,11) chdspin,levelE/1000.
11	      format(14x,'level, Elevel (MeV) =  ',a,'  ',g13.5,' MeV')
	      if(toallevents.gt.1.) then
	         write(77,*) '   '
	         write(77,*) 'range for sum of energies of emitted ',
     +                       'e-/e+ (MeV)   : ',ebb1,ebb2
		 write(77,*) '#@energy_min=',ebb1
		 write(77,*) '#@energy_max=',ebb2
		 write(77,*) '#@toallevents=',toallevents
	         allevents=anint(nevents*toallevents)
	         write(77,*) 'corresponding number of events in full ',
     +                       'energy range: ',allevents
	      endif
	   end if
	   if(i2bbs.eq.2) then
	      write(77,10) chnuclide
	      if(chnuclide.eq.'Artificial') then
	         write(77,*) ' number of parts in artificial event',
     +                        '             = ',nartparts
	         do i=1,nartparts
	            if(chart(i).eq.'GP') then
	               if(nartnpg(i).eq. 1) chn='Gamma        '
	               if(nartnpg(i).eq. 2) chn='Positron     '
	               if(nartnpg(i).eq. 3) chn='Electron     '
	               if(nartnpg(i).eq. 4) chn='Neutrino     '
	               if(nartnpg(i).eq. 5) chn='Muon+        '
	               if(nartnpg(i).eq. 6) chn='Muon-        '
	               if(nartnpg(i).eq. 7) chn='Pion0        '
	               if(nartnpg(i).eq. 8) chn='Pion+        '
	               if(nartnpg(i).eq. 9) chn='Pion-        '
	               if(nartnpg(i).eq.10) chn='Kaon0 long   '
	               if(nartnpg(i).eq.11) chn='Kaon+        '
	               if(nartnpg(i).eq.12) chn='Kaon-        '
	               if(nartnpg(i).eq.13) chn='Neutron      '
	               if(nartnpg(i).eq.14) chn='Proton       '
	               if(nartnpg(i).eq.15) chn='Antiproton   '
	               if(nartnpg(i).eq.16) chn='Kaon0 short  '
	               if(nartnpg(i).eq.17) chn='Eta          '
	               if(nartnpg(i).eq.18) chn='Lambda       '
	               if(nartnpg(i).eq.19) chn='Sigma+       '
	               if(nartnpg(i).eq.20) chn='Sigma0       '
	               if(nartnpg(i).eq.21) chn='Sigma-       '
	               if(nartnpg(i).eq.22) chn='Xi0          '
	               if(nartnpg(i).eq.23) chn='Xi-          '
	               if(nartnpg(i).eq.24) chn='Omega        '
	               if(nartnpg(i).eq.25) chn='Antineutron  '
	               if(nartnpg(i).eq.26) chn='Antilambda   '
	               if(nartnpg(i).eq.27) chn='Antisigma-   '
	               if(nartnpg(i).eq.28) chn='Antisigma0   '
	               if(nartnpg(i).eq.29) chn='Antisigma+   '
	               if(nartnpg(i).eq.30) chn='Antixi0      '
	               if(nartnpg(i).eq.31) chn='Antixi+      '
	               if(nartnpg(i).eq.32) chn='Antiomega+   '
	               if(nartnpg(i).eq.45) chn='Deuteron     '
	               if(nartnpg(i).eq.46) chn='Tritium      '
	               if(nartnpg(i).eq.47) chn='Alpha        '
	               if(nartnpg(i).eq.48) chn='Geantino     '
	               if(nartnpg(i).eq.49) chn='He3          '
	               if(nartnpg(i).eq.50) chn='Cerenkov     '
	            endif
	            if(chart(i).eq.'Be')     chn='Beta         '
	            if(chart(i).eq.'Pi')     chn='E+E- internal'
	            write(77,12) i,chn,artemin(i),artemax(i)
12	            format(2x,i2,1x,a13,1x,'min and max E',18x,'= ',
     +                     2g16.7)
	            if(chart(i).eq.'Be') write(77,13) artZd(i)
13	            format(19x,'Z of daughter nucleus          = ',
     +                     g16.7)
	            write(77,14) arttmin(i),arttmax(i),artfmin(i),
     +                           artfmax(i)
14	            format(19x,'min and max teta and phi [rad] = ',
     +                     4g16.7)
	            write(77,15) arttdelay(i),artthalf(i)
15	            format(19x,'time of delay and halflife     = ',
     +                     2g16.7)
	         enddo
	      endif
	      if(chnuclide.eq.'Compton') then
	         write(77,16) artemin(1),artemax(1)
16	         format(4x,'initial gamma: min and max E',18x,'= ',
     +                  2g16.7)
	         write(77,17) arttmin(1),arttmax(1),artfmin(1),
     +                        artfmax(1)
17	         format(19x,'min and max teta and phi [rad] = ',
     +                  4g16.7)
	      endif
	      if(chnuclide.eq.'Moller') then
	         write(77,18) artemin(1),artemax(1)
18	         format(4x,'initial e-: min and max E',18x,'= ',
     +                  2g16.7)
	         write(77,19) arttmin(1),arttmax(1),artfmin(1),
     +                        artfmax(1)
19	         format(16x,'min and max teta and phi [rad] = ',
     +                  4g16.7)
	         write(77,20) artQb(1)
20	         format(16x,'energy threshold for delta ray ='
     +                  ,g16.7)
	      endif
	      if(chnuclide.eq.'E+E- external') then
	         write(77,21) artemin(1),artemax(1)
21	         format(4x,'initial gamma: min and max E',18x,'= ',
     +                  2g16.7)
	         write(77,22) arttmin(1),arttmax(1),artfmin(1),
     +                        artfmax(1)
22	         format(19x,'min and max teta and phi [rad] = ',
     +                  4g16.7)
	         write(77,23) artZd(1)
23	         format(19x,'atomic number of target        ='
     +                  ,g16.7)
	      endif
	   endif
	   write(77,*) '   '
	   write(77,*) 'Format of data:'
	   write(77,*) ' for each event    - event''s number,'
	   write(77,*) '                     time of event''s start, '
	   write(77,*) '                     number of emitted',
     +                 ' particles;'
	   write(77,*) ' for each particle - GEANT number of particle,'
	   write(77,*) '                     ',
     +                 'x,y,z components of momentum,'
	   write(77,*) '                     ',
     +                 'time shift from previous time'
	   write(77,*) '   '
	   write(77,*) 'Time - in sec, momentum - in MeV/c'
	   write(77,*) '   '
	   write(77,*) 'First event and full number of events:'
	   write(77,*) ievstart,nevents
	   write(77,*) '   '
	endif
	istartbb=0
	if(istart.eq.-1) then
	   if(i2bbs.eq.1)
     +        call bb(modebb,Qbb,levelE/1000.,EK,Zdbb,Adbb,istartbb)
	   istart=1
	   return
	endif
	istart=1
1000	npfull=0
	if(i2bbs.eq.1) then
	   tevst=0.
	   call bb(modebb,Qbb,levelE/1000.,EK,Zdbb,Adbb,istartbb)
	   if(chnuclide.eq.'Ca48')    call  Ti48low(levelE)
	   if(chnuclide.eq.'Ni58')    call  Fe58low(levelE)
	   if(chnuclide.eq.'Ge76')    call  Se76low(levelE)
	   if(chnuclide.eq.'Zn64')    call  Ni64low(levelE)
	   if(chnuclide.eq.'Zn70')    call  Ge70low(levelE)
	   if(chnuclide.eq.'Se74')    call  Ge74low(levelE)
	   if(chnuclide.eq.'Se82')    call  Kr82low(levelE)
	   if(chnuclide.eq.'Sr84')    call  Kr84low(levelE)
	   if(chnuclide.eq.'Zr94')    call  Mo94low(levelE)
	   if(chnuclide.eq.'Zr96')    call  Mo96low(levelE)
	   if(chnuclide.eq.'Mo92')    call  Zr92low(levelE)
	   if(chnuclide.eq.'Mo100')   call Ru100low(levelE)
	   if(chnuclide.eq.'Ru96')    call  Mo96low(levelE)
	   if(chnuclide.eq.'Ru104')   call Pd104low(levelE)
	   if(chnuclide.eq.'Cd106')   call Pd106low(levelE)
	   if(chnuclide.eq.'Cd108')   call Pd108low(levelE)
	   if(chnuclide.eq.'Cd114')   call Sn114low(levelE)
	   if(chnuclide.eq.'Cd116')   call Sn116low(levelE)
	   if(chnuclide.eq.'Sn112')   call Cd112low(levelE)
	   if(chnuclide.eq.'Sn122')   call Te122low(levelE)
	   if(chnuclide.eq.'Sn124')   call Te124low(levelE)
	   if(chnuclide.eq.'Te120')   call Sn120low(levelE)
	   if(chnuclide.eq.'Te128')   call Xe128low(levelE)
	   if(chnuclide.eq.'Te130')   call Xe130low(levelE)
	   if(chnuclide.eq.'Xe136')   call Ba136low(levelE)
	   if(chnuclide.eq.'Ce136')   call Ba136low(levelE)
	   if(chnuclide.eq.'Ce138')   call Ba138low(levelE)
	   if(chnuclide.eq.'Ce142')   call Nd142low(levelE)
	   if(chnuclide.eq.'Nd148')   call Sm148low(levelE)
	   if(chnuclide.eq.'Nd150')   call Sm150low(levelE)
	   if(chnuclide.eq.'Dy156')   call Gd156low(levelE)
	   if(chnuclide.eq.'Dy158')   call Gd158low(levelE)
	   if(chnuclide.eq.'W180')    call Hf180low(levelE)
	   if(chnuclide.eq.'W186')    call Os186low(levelE)
	   if(chnuclide.eq.'Pt190')   call Os190low(levelE)
	   if(chnuclide.eq.'Pt198')   call Hg198low(levelE)
	   if(chnuclide.eq.'Bi214')   then
	      call At214low(0)
	      npfull0=npfull
	      call at214(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	   endif
	   if(chnuclide.eq.'Pb214')   then
	      call Po214low(0)
	      npfull0=npfull
	      call po214(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	   endif
	   if(chnuclide.eq.'Po218')   then
	      call Rn218low(0)
	      npfull0=npfull
	      call rn218(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	      npfull0=npfull
	      call po214(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	   endif
	   if(chnuclide.eq.'Rn222')   then
	      call Ra222low(0)
	      npfull0=npfull
	      call ra222(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	      npfull0=npfull
	      call rn218(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	      npfull0=npfull
	      call po214(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	   endif
	end if
	if(i2bbs.eq.2) then
	   if(chnuclide.eq.'Ac228')   call ac228(0.,tdnuc)
	   if(chnuclide.eq.'Am241')   call am241(0.,tdnuc)
	   if(chnuclide.eq.'Ar39')    call ar39(0.,tdnuc)
	   if(chnuclide.eq.'Ar42')    call ar42(0.,tdnuc)
	   if(chnuclide.eq.'As79')    call as79(0.,tdnuc)
	   if(chnuclide.eq.'Bi207')   call bi207(0.,tdnuc)
	   if(chnuclide.eq.'Bi208')   call bi208(0.,tdnuc)
	   if(chnuclide.eq.'Bi210')   call bi210(0.,tdnuc)
	   if(chnuclide.eq.'Bi212')   then
	      call bi212(0.,tdnuc)
	      npfull0=npfull
	      if(npgeant(1).ne.47) then
	         call po212(0.,tdnuc1)
	         ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	      endif
	   endif
	   if(chnuclide.eq.'Bi214')   then
	      call bi214(0.,tdnuc)
	      npfull0=npfull
	      if(npgeant(1).ne.47) then
	         call po214(0.,tdnuc1)
	         ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	      endif
	   endif
	   if(chnuclide.eq.'Ca48')   then
	      call ca48(0.,tdnuc)
	      npfull0=npfull
	      call sc48(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	   endif
	   if(chnuclide.eq.'C14')     call c14(0.,tdnuc)
	   if(chnuclide.eq.'Cd113')   call cd113(0.,tdnuc)
	   if(chnuclide.eq.'Co60')    call co60(0.,tdnuc)
	   if(chnuclide.eq.'Cs136')   call cs136(0.,tdnuc)
	   if(chnuclide.eq.'Cs137')   call cs137(0.,tdnuc)
	   if(chnuclide.eq.'Eu147')   call eu147(0.,tdnuc)
	   if(chnuclide.eq.'Eu152')   call eu152(0.,tdnuc)
	   if(chnuclide.eq.'Eu154')   call eu154(0.,tdnuc)
	   if(chnuclide.eq.'Gd146')   call gd146(0.,tdnuc)
	   if(chnuclide.eq.'Hf182')   call hf182(0.,tdnuc)
	   if(chnuclide.eq.'I126')    call i126(0.,tdnuc)
	   if(chnuclide.eq.'I133')    call i133(0.,tdnuc)
	   if(chnuclide.eq.'I134')    call i134(0.,tdnuc)
	   if(chnuclide.eq.'I135')    call i135(0.,tdnuc)
	   if(chnuclide.eq.'K40')     call k40(0.,tdnuc)
	   if(chnuclide.eq.'K42')     call k42(0.,tdnuc)
	   if(chnuclide.eq.'Kr81')    call kr81(0.,tdnuc)
	   if(chnuclide.eq.'Kr85')    call kr85(0.,tdnuc)
	   if(chnuclide.eq.'Mn54')    call mn54(0.,tdnuc)
	   if(chnuclide.eq.'Na22')    call na22(0.,tdnuc)
	   if(chnuclide.eq.'P32')     call p32(0.,tdnuc)
	   if(chnuclide.eq.'Pa234m')  call pa234m(0.,tdnuc)
	   if(chnuclide.eq.'Pb210')   call pb210(0.,tdnuc)
	   if(chnuclide.eq.'Pb211')   call pb211(0.,tdnuc)
	   if(chnuclide.eq.'Pb212')   call pb212(0.,tdnuc)
	   if(chnuclide.eq.'Pb214')   call pb214(0.,tdnuc)
	   if(chnuclide.eq.'Ra228')   call ra228(0.,tdnuc)
	   if(chnuclide.eq.'Rb87')    call rb87(0.,tdnuc)
	   if(chnuclide.eq.'Rh106')   call rh106(0.,tdnuc)
	   if(chnuclide.eq.'Sb125')   call sb125(0.,tdnuc)
	   if(chnuclide.eq.'Sb126')   call sb126(0.,tdnuc)
	   if(chnuclide.eq.'Sb133')   call sb133(0.,tdnuc)
	   if(chnuclide.eq.'Sr90')    call sr90(0.,tdnuc)
	   if(chnuclide.eq.'Ta182')   call ta182(0.,tdnuc)
	   if(chnuclide.eq.'Te133')   call te133(0.,tdnuc)
	   if(chnuclide.eq.'Te133m')  call te133m(0.,tdnuc)
	   if(chnuclide.eq.'Te134')   call te134(0.,tdnuc)
	   if(chnuclide.eq.'Th234')   call th234(0.,tdnuc)
	   if(chnuclide.eq.'Tl207')   call tl207(0.,tdnuc)
	   if(chnuclide.eq.'Tl208')   call tl208(0.,tdnuc)
	   if(chnuclide.eq.'Xe129m')  call xe129m(0.,tdnuc)
	   if(chnuclide.eq.'Xe131m')  call xe131m(0.,tdnuc)
	   if(chnuclide.eq.'Xe133')   call xe133(0.,tdnuc)
	   if(chnuclide.eq.'Xe135')   call xe135(0.,tdnuc)
	   if(chnuclide.eq.'Y88')     call y88(0.,tdnuc)
	   if(chnuclide.eq.'Y90')     call y90(0.,tdnuc)
	   if(chnuclide.eq.'Zn65')    call zn65(0.,tdnuc)
	   if(chnuclide.eq.'Zr96')    then
	      call zr96(0.,tdnuc)
	      npfull0=npfull
	      call nb96(0.,tdnuc1)
	      ptime(npfull0+1)=ptime(npfull0+1)+tdnuc1
	   endif
	   tevst=tdnuc
	   if(chnuclide.eq.'Artificial') then
	      tevst=0.
	      do j=1,nartparts
	         if(chart(j).eq.'Be') then
	            call beta(artQb(j),artZd(j),arttdelay(j),
     +                      artthalf(j),t)
	         elseif(chart(j).eq.'Pi') then
	            call pair(artQb(j),arttdelay(j),artthalf(j),t)
	         else
	            np=nartnpg(j)
                    call particle(np,artemin(j),artemax(j),arttmin(j),
     +                            arttmax(j),artfmin(j),artfmax(j),
     +                            arttdelay(j),artthalf(j),t)
	         endif
	      enddo
	   endif
	   if(chnuclide.eq.'Compton') then
	      tevst=0.
	      call compton(artemin(1),artemax(1),arttmin(1),arttmax(1),
     +                     artfmin(1),artfmax(1))
	   endif
	   if(chnuclide.eq.'Moller') then
	      tevst=0.
	      call moller(artemin(1),artemax(1),arttmin(1),arttmax(1),
     +                    artfmin(1),artfmax(1),artQb(1))
	   endif
	   if(chnuclide.eq.'E+E- external') then
	      tevst=0.
	      call pairext(artemin(1),artemax(1),arttmin(1),arttmax(1),
     +                     artfmin(1),artfmax(1),artZd(1))
	   endif
	endif
	if(chfile.ne.'no file') then
	   write(77,6) icurrent,tevst,npfull
6	   format(i8,g13.6,i4)
	   do j=1,npfull
	      write(77,7) npgeant(j),(pmoment(k,j),k=1,3),ptime(j)
7	      format(i3,3g14.6,g13.6)
	   enddo
	endif
	if(icurrent.ne.nevents) return
	idummy=0 ! call rdmout(irndmfin)
	return
	end
c
c end of GENBBsub.f
c
c Local Variables: --
c mode: fortran --
c End: --
