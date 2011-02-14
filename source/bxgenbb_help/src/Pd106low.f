c Pd106low.f 
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
	subroutine Pd106low(levelkeV)
c Subroutine describes the deexcitation process in Pd106 nucleus
c after 2b-decay of Cd106 to ground and excited 0+ and 2+ levels
c of Pd106 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Pd106low(levelkeV)
c Input : levelkeV - energy of Pd106 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  512 keV,
c                    2+(2)  - 1128 keV,
c                    0+(1)  - 1134 keV,
c                    2+(3)  - 1562 keV,
c                    0+(2)  - 1706 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 30.11.1995; 3.04.2006; 14.11.2006.
	tclev=0.
c	if(levelkev.eq.2741) go to  2741
	if(levelkev.eq.1706) go to  1706
	if(levelkev.eq.1562) go to  1562
	if(levelkev.eq.1134) go to  1134
	if(levelkev.eq.1128) go to  1128
	if(levelkev.eq. 512) go to   512
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
2741	thlev=0.
	p=100.*rnd1(d)
	if(p.le.66.2) go to 27411
	              go to 27412
27411	call nucltransK(2.741,0.024,1.4e-4,6.6e-4,tclev,thlev,tdlev)
	return
27412	call nucltransK(2.230,0.024,2.1e-4,3.9e-4,tclev,thlev,tdlev)
	go to 512
c-----------------------------------------------------------------------
1706	thlev=0.
	p=100.*rnd1(d)
	if(p.le.87.2) go to 17061
	              go to 17062
17061	call nucltransK(1.195,0.024,6.9e-4,6.7e-6,tclev,thlev,tdlev)
	go to 512
17062	call nucltransK(0.578,0.024,4.0e-3,0.,tclev,thlev,tdlev)
	go to 1128
c-----------------------------------------------------------------------
1562	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 9.0) go to 15621
	if(p.le.95.0) go to 15622
	if(p.le.96.1) go to 15623
	              go to 15624
15621	call nucltransK(1.562,0.024,3.9e-4,1.1e-4,tclev,thlev,tdlev)
	return
15622	call nucltransK(1.050,0.024,1.0e-3,0.,tclev,thlev,tdlev)
	go to 512
15623	call nucltransK(0.434,0.024,7.7e-3,0.,tclev,thlev,tdlev)
	go to 1128
15624	call nucltransK(0.428,0.024,9.5e-3,0.,tclev,thlev,tdlev)
	go to 1134
c-----------------------------------------------------------------------
1134	thlev=6.8e-12
	p=100.*rnd1(d)
	if(p.le.5.7e-2) go to 11341
	                go to 11342
11341	call electron(1.110,tclev,thlev,tdlev) ! E0 transition
	call gamma(0.024,0.,0.,tdlev)
	return
11342	call nucltransK(0.622,0.024,3.2e-3,0.,tclev,thlev,tdlev)
	go to 512
c-----------------------------------------------------------------------
1128	thlev=3.12e-12
	p=100.*rnd1(d)
	if(p.le.35.0) go to 11281
	              go to 11282
11281	call nucltransK(1.128,0.024,7.7e-4,0.,tclev,thlev,tdlev)
	return
11282	call nucltransK(0.616,0.024,3.4e-3,0.,tclev,thlev,tdlev)
	go to 512
c-----------------------------------------------------------------------
512	thlev=12.1e-12
	call nucltransK(0.512,0.024,5.6e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Pd106: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Pd106low.f
c
c Local Variables: --
c mode: fortran --
c End: --
