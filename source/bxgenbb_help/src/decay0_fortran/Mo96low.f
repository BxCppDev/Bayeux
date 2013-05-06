c Mo96low.f 
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
	subroutine Mo96low(levelkeV)
c Subroutine describes the deexcitation process in Mo96 nucleus
c after 2b- decay of Zr96 and 2b+/eb+/2e decay of Ru96 to ground and 
c excited 0+ and 2+ levels c of Mo96 ("Table of Isotopes", 7th ed., 1978).
c VIT, 20.05.2009: four levels (2096, 2426, 2623 and 2700 keV) are added 
c and information is updated in accordance with NNDC site on 19.05.2009 and 
c NDS 109(2008)2501.
c Call  : call Mo96low(levelkeV)
c Input : levelkeV - energy of Mo96 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  778 keV,
c                    0+(1)  - 1148 keV,
c                    2+(2)  - 1498 keV,
c                    2+(3)  - 1626 keV,
c                    2+(4)  - 2096 keV,
c                    2+(5)  - 2426 keV,
c                    0+(2)  - 2623 keV,
c                    2+(6)  - 2700 keV,
c                    2+(7)  - 2713 keV (in fact spin and parity of this level
c                             are unknown).
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 28.06.1993, 22.10.1995, 20.05.2009.
	tclev=0.
	if(levelkev.eq.2713) go to  2713
	if(levelkev.eq.2700) go to  2700
	if(levelkev.eq.2623) go to  2623
	if(levelkev.eq.2426) go to  2426
	if(levelkev.eq.2096) go to  2096
	if(levelkev.eq.1626) go to  1626
	if(levelkev.eq.1498) go to  1498
	if(levelkev.eq.1148) go to  1148
	if(levelkev.eq. 778) go to   778
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
2713	thlev=0.
c 2+ to 6+ E4 transition is supposed
27131	call nucltransK(0.272,0.020,5.9e-1,0.,tclev,thlev,tdlev)
	go to 2441
c-----------------------------------------------------------------------
2700	thlev=0.103e-12
	p=100.*rnd1(d)
	if(p.le. 3.04) go to 27001
	if(p.le.12.12) go to 27002
	if(p.le.53.40) go to 27003
	if(p.le.86.67) go to 27004
	               go to 27005
27001	call nucltransK(0.160,0.020,6.9e-2,0.,tclev,thlev,tdlev)
	go to 2540
27002	call nucltransK(1.074,0.020,6.7e-4,0.,tclev,thlev,tdlev)
	go to 1626
27003	call nucltransK(1.202,0.020,5.5e-4,0.,tclev,thlev,tdlev)
	go to 1498
27004	call nucltransK(1.922,0.020,2.1e-4,2.5e-4,tclev,thlev,tdlev)
	go to 778
27005	call nucltransK(2.701,0.020,1.1e-4,6.4e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
2623	thlev=0.6e-12
26231	call nucltransK(1.844,0.020,2.2e-4,2.3e-4,tclev,thlev,tdlev)
	go to 778
c-----------------------------------------------------------------------
2540	thlev=0.069e-12
	p=100.*rnd1(d)
	if(p.le. 8.17) go to 25401
	if(p.le.28.31) go to 25402
	               go to 25403
25401	call nucltransK(0.915,0.020,9.7e-4,0.,tclev,thlev,tdlev)
	go to 1626
25402	call nucltransK(1.043,0.020,7.3e-4,0.,tclev,thlev,tdlev)
	go to 1498
25403	call nucltransK(1.762,0.020,2.5e-4,1.7e-4,tclev,thlev,tdlev)
	go to 778
c-----------------------------------------------------------------------
2441	thlev=0.208e-12
24411	call nucltransK(0.813,0.020,1.3e-3,0.,tclev,thlev,tdlev)
	go to 1628
c-----------------------------------------------------------------------
2426	thlev=0.19e-12
	p=100.*rnd1(d)
	if(p.le. 2.50) go to 24261
	if(p.le.38.16) go to 24262
	if(p.le.42.07) go to 24263
	if(p.le.95.22) go to 24264
	               go to 24265
24261	call nucltransK(0.448,0.020,6.4e-3,0.,tclev,thlev,tdlev)
	go to 1978
24262	call nucltransK(0.800,0.020,1.3e-3,0.,tclev,thlev,tdlev)
	go to 1626
24263	call nucltransK(0.928,0.020,9.2e-4,0.,tclev,thlev,tdlev)
	go to 1498
24264	call nucltransK(1.648,0.020,2.9e-4,1.2e-4,tclev,thlev,tdlev)
	go to 778
24265	call nucltransK(2.426,0.020,1.4e-4,5.1e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
2096	thlev=0.097e-12
	p=100.*rnd1(d)
	if(p.le. 3.06) go to 20961
	if(p.le.98.55) go to 20962
	               go to 20963
20961	call nucltransK(0.948,0.020,8.7e-4,0.,tclev,thlev,tdlev)
	go to 1148
20962	call nucltransK(1.317,0.020,4.4e-4,2.5e-5,tclev,thlev,tdlev)
	go to 778
20963	call nucltransK(2.096,0.020,1.8e-4,3.5e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1978	thlev=2.29e-12
	p=100.*rnd1(d)
	if(p.le. 0.18) go to 19781
	if(p.le. 5.96) go to 19782
	if(p.le. 9.27) go to 19783
	if(p.le.30.34) go to 19784
	               go to 19785
19781	call nucltransK(0.109,0.020,2.0e-1,0.,tclev,thlev,tdlev)
	go to 1870
19782	call nucltransK(0.350,0.020,1.2e-2,0.,tclev,thlev,tdlev)
	go to 1628
19783	call nucltransK(0.353,0.020,1.2e-2,0.,tclev,thlev,tdlev)
	go to 1626
19784	call nucltransK(0.481,0.020,4.3e-3,0.,tclev,thlev,tdlev)
	go to 1498
19785	call nucltransK(1.200,0.020,5.4e-4,6.1e-6,tclev,thlev,tdlev)
	go to 778
c-----------------------------------------------------------------------
1870	thlev=6.4e-12
	p=100.*rnd1(d)
	if(p.le. 7.52) go to 18701
	if(p.le.12.22) go to 18702
	               go to 18703
18701	call nucltransK(0.241,0.020,2.4e-2,0.,tclev,thlev,tdlev)
	go to 1628
18702	call nucltransK(0.372,0.020,1.2e-2,0.,tclev,thlev,tdlev)
	go to 1498
18703	call nucltransK(1.091,0.020,6.4e-4,0.,tclev,thlev,tdlev)
	go to 778
c-----------------------------------------------------------------------
1628	thlev=1.2e-12
16281	call nucltransK(0.850,0.020,1.1e-3,0.,tclev,thlev,tdlev)
	go to 778
c-----------------------------------------------------------------------
1626	thlev=1.4e-12
	p=100.*rnd1(d)
	if(p.le. 8.47) go to 16261
	if(p.le.98.58) go to 16262
	               go to 16263
16261	call nucltransK(1.626,0.020,2.8e-4,1.3e-4,tclev,thlev,tdlev)
	return
16262	call nucltransK(0.848,0.020,1.2e-3,0.,tclev,thlev,tdlev)
	go to 778
16263	call nucltransK(0.128,0.020,1.3e-1,0.,tclev,thlev,tdlev)
	go to 1498
c-----------------------------------------------------------------------
1498	thlev=0.78e-12
	p=100.*rnd1(d)
	if(p.le.29.73) go to 14981
	               go to 14982
14981	call nucltransK(1.498,0.020,3.3e-4,8.3e-5,tclev,thlev,tdlev)
	return
14982	call nucltransK(0.720,0.020,1.7e-3,0.,tclev,thlev,tdlev)
	go to 778
c-----------------------------------------------------------------------
1148	thlev=61.e-12
11481	call nucltransK(0.370,0.020,1.2e-2,0.,tclev,thlev,tdlev)
	go to 778
c-----------------------------------------------------------------------
778	thlev=3.67e-12
	call nucltransK(0.778,0.020,1.4e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Mo96: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Mo96low.f
c
c Local Variables: --
c mode: fortran --
c End: --
