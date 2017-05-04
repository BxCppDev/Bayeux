c Gd156low.f 
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
	subroutine Gd156low(levelkeV)
c Subroutine describes the deexcitation process in Gd156 nucleus
c after 2b-decay of Dy156 to ground and excited 0+ and 2+ levels
c of Gd156 (NNDC site on 21.12.2010; NDS 99(2003)753).
c Call  : call Gd156low(levelkeV)
c Input : levelkeV - energy of Gd156 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -   89 keV,
c                    0+(1)  - 1050 keV,
c                    2+(2)  - 1129 keV,
c                    2+(3)  - 1154 keV,
c                    0+(2)  - 1168 keV,
c                    2+(4)  - 1258 keV,
c                    0+(3)  - 1715 keV,
c                    2+(5)  - 1771 keV,
c                    2+(6)  - 1828 keV,
c                    0+(4)  - 1851 keV,
c                    2+(7)  - 1915 keV,
c                    1-()   - 1946 keV,
c                    0-()   - 1952 keV,
c                    0+(5)  - 1989 keV,
c                    2+(8)  - 2004 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 23.12.2010.
	tclev=0.
	if(levelkev.eq.2004) go to  2004
	if(levelkev.eq.1989) go to  1989
	if(levelkev.eq.1952) go to  1952
	if(levelkev.eq.1946) go to  1946
	if(levelkev.eq.1915) go to  1915
	if(levelkev.eq.1851) go to  1851
	if(levelkev.eq.1828) go to  1828
	if(levelkev.eq.1771) go to  1771
	if(levelkev.eq.1715) go to  1715
	if(levelkev.eq.1258) go to  1258
	if(levelkev.eq.1168) go to  1168
	if(levelkev.eq.1154) go to  1154
	if(levelkev.eq.1129) go to  1129
	if(levelkev.eq.1050) go to  1050
	if(levelkev.eq.  89) go to    89
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
2004	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 6.1) go to 20041
	if(p.le.39.5) go to 20042
	if(p.le.53.8) go to 20043
	if(p.le.81.5) go to 20044
	if(p.le.94.1) go to 20045
	if(p.le.97.0) go to 20046
	              go to 20047
20041	call nucltransK(0.684,0.050,2.4e-3,0.,tclev,thlev,tdlev)
	go to 1320
20042	call nucltransK(0.728,0.050,2.1e-3,0.,tclev,thlev,tdlev)
	go to 1276
20043	call nucltransK(0.756,0.050,7.2e-3,0.,tclev,thlev,tdlev)
	go to 1248
20044	call nucltransK(0.761,0.050,1.9e-3,0.,tclev,thlev,tdlev)
	go to 1243
20045	call nucltransK(0.850,0.050,5.4e-3,0.,tclev,thlev,tdlev)
	go to 1154
20046	call nucltransK(0.874,0.050,6.5e-3,0.,tclev,thlev,tdlev)
	go to 1129
20047	call nucltransK(1.715,0.050,1.1e-3,0.,tclev,thlev,tdlev)
	go to 288
c-----------------------------------------------------------------------
1989	thlev=0.
	call nucltransK(1.900,0.050,7.9e-4,2.4e-4,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1952	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 5.5) go to 19521
	if(p.le. 9.3) go to 19522
	              go to 19523
19521	call nucltransK(0.586,0.050,1.7e-2,0.,tclev,thlev,tdlev)
	go to 1367
19522	call nucltransK(0.633,0.050,7.8e-3,0.,tclev,thlev,tdlev)
	go to 1320
19523	call nucltransK(0.710,0.050,1.1e-2,0.,tclev,thlev,tdlev)
	go to 1243
c-----------------------------------------------------------------------
1946	thlev=30.e-15
	p=100.*rnd1(d)
	if(p.le. 2.8) go to 19461
	if(p.le.60.7) go to 19462
	              go to 19463
19461	call nucltransK(0.688,0.050,2.4e-3,0.,tclev,thlev,tdlev)
	go to 1258
19462	call nucltransK(1.857,0.050,3.9e-4,4.7e-4,tclev,thlev,tdlev)
	go to 89
19463	call nucltransK(1.946,0.050,3.6e-4,5.4e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1915	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.64) go to 19151
	if(p.le. 4.06) go to 19152
	if(p.le.13.31) go to 19153
	if(p.le.17.90) go to 19154
	if(p.le.22.04) go to 19155
	if(p.le.26.03) go to 19156
	               go to 19157
19151	call nucltransK(0.376,0.050,9.2e-3,0.,tclev,thlev,tdlev)
	go to 1539
19152	call nucltransK(0.548,0.050,3.9e-3,0.,tclev,thlev,tdlev)
	go to 1367
19153	call nucltransK(0.639,0.050,2.8e-3,0.,tclev,thlev,tdlev)
	go to 1276
19154	call nucltransK(0.657,0.050,1.8e-2,0.,tclev,thlev,tdlev)
	go to 1258
19155	call nucltransK(0.667,0.050,1.0e-2,0.,tclev,thlev,tdlev)
	go to 1248
19156	call nucltransK(0.672,0.050,2.5e-3,0.,tclev,thlev,tdlev)
	go to 1243
19157	call nucltransK(1.826,0.050,1.1e-3,2.4e-4,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1851	thlev=0.
	p=100.*rnd1(d)
	if(p.le.13.2) go to 18511
	if(p.le.25.7) go to 18512
	if(p.le.34.6) go to 18513
	if(p.le.40.0) go to 18514
	if(p.le.99.7) go to 18515
	              go to 18516
18511	call nucltransK(0.485,0.050,5.1e-3,0.,tclev,thlev,tdlev)
	go to 1367
18512	call nucltransK(0.609,0.050,3.1e-3,0.,tclev,thlev,tdlev)
	go to 1243
18513	call nucltransK(0.697,0.050,6.2e-3,0.,tclev,thlev,tdlev)
	go to 1154
18514	call nucltransK(0.722,0.050,5.7e-3,0.,tclev,thlev,tdlev)
	go to 1129
18515	call nucltransK(1.763,0.050,9.0e-4,1.8e-4,tclev,thlev,tdlev)
	go to 89
18516	p=100.*rnd1(d)
	if(p.le.91.7) then                        ! e0.exe
	   call electron(1.801,tclev,thlev,tdlev) ! conversion to electron
	   call gamma(0.050,0.,0.,tdlev)
	else
	   call pair(0.829,tclev,thlev,tdlev)     ! conversion to pair
	endif
	return
c-----------------------------------------------------------------------
1828	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.28) go to 18281
	if(p.le. 3.37) go to 18282
	if(p.le.14.51) go to 18283
	if(p.le.28.57) go to 18284
	if(p.le.48.82) go to 18285
	if(p.le.71.88) go to 18286
	               go to 18287
18281	call nucltransK(0.366,0.050,3.3e-2,0.,tclev,thlev,tdlev)
	go to 1462
18282	call nucltransK(0.570,0.050,1.4e-2,0.,tclev,thlev,tdlev)
	go to 1258
18283	call nucltransK(0.580,0.050,1.4e-2,0.,tclev,thlev,tdlev)
	go to 1248
18284	call nucltransK(0.674,0.050,6.6e-3,0.,tclev,thlev,tdlev)
	go to 1154
18285	call nucltransK(0.698,0.050,9.0e-3,0.,tclev,thlev,tdlev)
	go to 1129
18286	call nucltransK(0.778,0.050,4.8e-3,0.,tclev,thlev,tdlev)
	go to 1050
18287	call nucltransK(1.739,0.050,1.3e-3,1.9e-4,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1771	thlev=0.42e-12
	p=100.*rnd1(d)
	if(p.le. 0.10) go to 17711
	if(p.le. 1.08) go to 17712
	if(p.le. 7.61) go to 17713
	if(p.le. 9.76) go to 17714
	if(p.le.10.50) go to 17715
	               go to 17716
17711	call nucltransK(0.232,0.050,3.1e-2,0.,tclev,thlev,tdlev)
	go to 1539
17712	call nucltransK(0.405,0.050,7.7e-3,0.,tclev,thlev,tdlev)
	go to 1367
17713	call nucltransK(0.495,0.050,4.8e-3,0.,tclev,thlev,tdlev)
	go to 1276
17714	call nucltransK(0.513,0.050,2.4e-2,0.,tclev,thlev,tdlev)
	go to 1258
17715	call nucltransK(0.529,0.050,4.2e-3,0.,tclev,thlev,tdlev)
	go to 1243
17716	call nucltransK(1.682,0.050,1.4e-3,1.6e-4,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1715	thlev=2.6e-12
	p=100.*rnd1(d)
	if(p.le. 6.97) go to 17151
	if(p.le.74.65) go to 17152
	if(p.le.74.72) go to 17153
	if(p.le.77.43) go to 17154
	if(p.le.77.54) go to 17155
	if(p.le.99.88) go to 17156
	               go to 17157
17151	call nucltransK(0.349,0.050,1.1e-2,0.,tclev,thlev,tdlev)
	go to 1367
17152	call nucltransK(0.473,0.050,5.4e-3,0.,tclev,thlev,tdlev)
	go to 1243
17153	call electron(0.497,tclev,thlev,tdlev) ! only conversion to electron
	call gamma(0.050,0.,0.,tdlev)
	go to 1168
17154	call nucltransK(0.586,0.050,9.4e-3,0.,tclev,thlev,tdlev)
	go to 1129
17155	call electron(0.616,tclev,thlev,tdlev) ! only conversion to electron
	call gamma(0.050,0.,0.,tdlev)
	go to 1050
17156	call nucltransK(1.625,0.050,1.1e-3,1.2e-4,tclev,thlev,tdlev)
	go to 89
17157	call electron(1.665,tclev,thlev,tdlev) ! only conversion to electron
	call gamma(0.050,0.,0.,tdlev)
	return
c-----------------------------------------------------------------------
1539	thlev=20.e-15
	p=100.*rnd1(d)
	if(p.le. 0.2) go to 15391
	if(p.le.51.6) go to 15392
	              go to 15393
15391	call nucltransK(0.385,0.050,8.6e-3,0.,tclev,thlev,tdlev)
	go to 1154
15392	call nucltransK(1.251,0.050,7.6e-4,0.5e-4,tclev,thlev,tdlev)
	go to 288
15393	call nucltransK(1.450,0.050,5.9e-4,1.7e-4,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1462	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.16) go to 14621
	if(p.le. 0.76) go to 14622
	if(p.le. 1.00) go to 14623
	if(p.le.27.32) go to 14624
	if(p.le.89.98) go to 14625
	               go to 14626
14621	call nucltransK(0.165,0.050,4.9e-1,0.,tclev,thlev,tdlev)
	go to 1298
14622	call nucltransK(0.204,0.050,2.1e-1,0.,tclev,thlev,tdlev)
	go to 1258
14623	call nucltransK(0.333,0.050,3.4e-2,0.,tclev,thlev,tdlev)
	go to 1129
14624	call nucltransK(0.878,0.050,3.7e-3,0.,tclev,thlev,tdlev)
	go to 585
14625	call nucltransK(1.174,0.050,3.2e-3,0.,tclev,thlev,tdlev)
	go to 288
14626	call nucltransK(1.373,0.050,1.5e-3,0.,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1367	thlev=17.e-15
	p=100.*rnd1(d)
	if(p.le. 0.05) go to 13671
	if(p.le.64.74) go to 13672
	               go to 13673
13671	call nucltransK(0.237,0.050,2.9e-2,0.,tclev,thlev,tdlev)
	go to 1129
13672	call nucltransK(1.278,0.050,7.3e-4,0.6e-4,tclev,thlev,tdlev)
	go to 89
13673	call nucltransK(1.367,0.050,6.5e-4,1.1e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1320	thlev=3.9e-12
	p=100.*rnd1(d)
	if(p.le. 0.22) go to 13201
	               go to 13202
13201	call nucltransK(0.190,0.050,5.2e-2,0.,tclev,thlev,tdlev)
	go to 1129
13202	call nucltransK(1.231,0.050,7.8e-4,0.,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1298	thlev=1.6e-12
	p=100.*rnd1(d)
	if(p.le. 0.07) go to 12981
	if(p.le. 0.84) go to 12982
	if(p.le. 6.41) go to 12983
	if(p.le.50.74) go to 12984
	               go to 12985
12981	call nucltransK(0.144,0.050,6.8e-1,0.,tclev,thlev,tdlev)
	go to 1154
12982	call nucltransK(0.168,0.050,4.0e-1,0.,tclev,thlev,tdlev)
	go to 1129
12983	call nucltransK(0.713,0.050,5.8e-3,0.,tclev,thlev,tdlev)
	go to 585
12984	call nucltransK(1.010,0.050,1.7e-2,0.,tclev,thlev,tdlev)
	go to 288
12985	call nucltransK(1.209,0.050,1.9e-3,0.,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1276	thlev=0.098e-12
	p=100.*rnd1(d)
	if(p.le.31.0) go to 12761
	              go to 12762
12761	call nucltransK(0.988,0.050,1.2e-3,0.,tclev,thlev,tdlev)
	go to 288
12762	call nucltransK(1.187,0.050,8.3e-4,0.,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1258	thlev=1.54e-12
	p=100.*rnd1(d)
	if(p.le.50.8) go to 12581
	if(p.le.86.8) go to 12582
	              go to 12583
12581	call nucltransK(0.970,0.050,3.0e-3,0.,tclev,thlev,tdlev)
	go to 288
12582	call nucltransK(1.169,0.050,3.1e-3,0.,tclev,thlev,tdlev)
	go to 89
12583	call nucltransK(1.258,0.050,1.7e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1248	thlev=0.58e-12
	p=100.*rnd1(d)
	if(p.le.21.3) go to 12481
	              go to 12482
12481	call nucltransK(0.960,0.050,3.0e-3,0.,tclev,thlev,tdlev)
	go to 288
12482	call nucltransK(1.159,0.050,2.1e-3,0.,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
1243	thlev=31.e-15
	p=100.*rnd1(d)
	if(p.le.50.7) go to 12431
	              go to 12432
12431	call nucltransK(1.154,0.050,8.8e-4,0.,tclev,thlev,tdlev)
	go to 89
12432	call nucltransK(1.243,0.050,7.7e-4,0.4e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1168	thlev=5.e-12
	p=100.*rnd1(d)
	if(p.le. 0.18) go to 11681
	if(p.le.99.95) go to 11682
	               go to 11683
11681	call electron(0.069,tclev,thlev,tdlev) ! only conversion to electron
	call gamma(0.050,0.,0.,tdlev)
	go to 1050
11682	call nucltransK(1.079,0.050,2.4e-3,0.,tclev,thlev,tdlev)
	go to 89
11683	call electron(1.118,tclev,thlev,tdlev) ! only conversion to electron
	call gamma(0.050,0.,0.,tdlev)
	return
c-----------------------------------------------------------------------
1154	thlev=0.568e-12
	p=100.*rnd1(d)
	if(p.le. 1.89) go to 11541
	if(p.le.51.90) go to 11542
	               go to 11543
11541	call nucltransK(0.866,0.050,3.8e-3,0.,tclev,thlev,tdlev)
	go to 288
11542	call nucltransK(1.065,0.050,2.4e-3,0.,tclev,thlev,tdlev)
	go to 89
11543	call nucltransK(1.154,0.050,2.1e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1129	thlev=1.59e-12
	p=100.*rnd1(d)
	if(p.le.24.3) go to 11291
	if(p.le.83.8) go to 11292
	              go to 11293
11291	call nucltransK(0.841,0.050,4.0e-3,0.,tclev,thlev,tdlev)
	go to 288
11292	call nucltransK(1.041,0.050,1.4e-2,0.,tclev,thlev,tdlev)
	go to 89
11293	call nucltransK(1.129,0.050,2.2e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1050	thlev=1.8e-12
	p=100.*rnd1(d)
	if(p.le.99.4) go to 10501
	              go to 10502
10501	call nucltransK(0.961,0.050,3.0e-3,0.,tclev,thlev,tdlev)
	go to 89
10502	call electron(1.000,tclev,thlev,tdlev) ! only conversion to electron
	call gamma(0.050,0.,0.,tdlev)
	return
c-----------------------------------------------------------------------
585	thlev=15.8e-12
	call nucltransK(0.297,0.050,6.3e-2,0.,tclev,thlev,tdlev)
	go to 288
c-----------------------------------------------------------------------
288	thlev=111.9e-12
	call nucltransK(0.199,0.050,2.3e-1,0.,tclev,thlev,tdlev)
	go to 89
c-----------------------------------------------------------------------
89	thlev=2.21e-9
	call nucltransK(0.089,0.050,3.93,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Gd156: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Gd156low.f
c
c Local Variables: --
c mode: fortran --
c End: --
