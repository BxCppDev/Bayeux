c block_data.f 
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
	block data
	common/const/pi,emass,datamass(50)
	common/bj69plog/plog69(48)

	data pi/3.1415927/
	data emass/0.51099906/

c Particle masses (MeV) - in accordance with GEANT 3.21 manual of October,
c 1994:
c        1 - gamma         2 - positron     3 - electron
c        4 - neutrino      5 - muon+        6 - muon-
c        7 - pion0         8 - pion+        9 - pion-
c       10 - kaon0 long   11 - kaon+       12 - kaon-
c       13 - neutron      14 - proton      15 - antiproton
c       16 - kaon0 short  17 - eta         18 - lambda
c       19 - sigma+       20 - sigma0      21 - sigma-
c       22 - xi0          23 - xi-         24 - omega
c       25 - antineutron  26 - antilambda  27 - antisigma-
c       28 - antisigma0   29 - antisigma+  30 - antixi0
c       31 - antixi+      32 - antiomega+  45 - deuteron
c       46 - tritium      47 - alpha       48 - geantino
c       49 - He3          50 - Cerenkov
	data datamass/
     +     0.,         0.51099906,   0.51099906, 0.,      105.658389, !  1 -  5
     +   105.658389, 134.9764,     139.5700,   139.5700,  497.672,    !  6 - 10
     +   493.677,    493.677,      939.56563,  938.27231, 938.27231,  ! 11 - 15
     +   497.672,    547.45,      1115.684,   1189.37,   1192.55,     ! 16 - 20
     +  1197.436,   1314.9,       1321.32,    1672.45,    939.56563,  ! 21 - 25
     +  1115.684,   1189.37,      1192.55,    1197.436,  1314.9,      ! 26 - 30
     +  1321.32,    1672.45,         0.,         0.,        0.,       ! 31 - 35
     +     0.,         0.,           0.,         0.,        0.,       ! 35 - 40
     +     0.,         0.,           0.,         0.,     1875.613,    ! 41 - 45
     +  2809.25,    3727.417,        0.,      2809.23,      0./       ! 46 - 50

c Values of natural logarithm of the standard values of momentum 
c (in units m_e*c) from: 
c H.Behrens, J.Janecke, "Numerical tables for beta-decay and 
c electron capture", Berlin, Springer-Verlag, 1969.
c Range of momenta correspond to kinetic energy range from 2.55 keV to 25.0 MeV.
	data plog69/
c p      0.1         0.2         0.3         0.4         0.5
     +  -2.302585,  -1.609438,  -1.203973,  -0.9162907, -0.6931472,
c p      0.6         0.7         0.8         0.9         1.0
     +  -0.5108256, -0.3566750, -0.2231435, -0.1053605,  0.0000000,
c p      1.2         1.4         1.6         1.8         2.0
     +   0.1823216,  0.3364722,  0.4700036,  0.5877866,  0.6931472,
c p      2.2         2.4         2.6         2.8         3.0
     +   0.7884574,  0.8754688,  0.9555114,  1.029619,   1.098612,
c p      3.2         3.4         3.6         3.8         4.0
     +   1.163151,   1.223776,   1.280934,   1.335001,   1.386294,
c p      4.5         5.0         5.5         6.0         6.5
     +   1.504077,   1.609438,   1.704748,   1.791759,   1.871802,
c p      7.0         7.5         8.0         9.0        10.0
     +   1.945910,   2.014903,   2.079442,   2.197225,   2.302585,
c       11.0        12.0        13.0        14.0        16.0
     +   2.397895,   2.484907,   2.564949,   2.639057,   2.772589,
c       18.0        20.0        25.0        30.0        35.0
     +   2.890372,   2.995732,   3.218876,   3.401197,  3.555348,
c       40.0        45.0        50.0
     +   3.688879,   3.806663,   3.912023/

	end
c
c end of block_data.f
c
c Local Variables: --
c mode: fortran --
c End: --
