c Os190low.f 
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
	subroutine Os190low(levelkeV)
c Subroutine describes the deexcitation process in Os190 nucleus
c after eb+/2e decay of Pt190 to ground and excited 0+ and 2+ levels
c of Os190 (in accordance with NDS 99(2003)275 and NNDC on 22.07.2010).
c Level (0,1,2)+ at 1382 keV is supposed to be 0+.
c Call  : call Os190low(levelkeV)
c Input : levelkeV - energy of Os190 level (integer in keV) occupied
c	             initially; the following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  187 keV,
c                    2+(2)  -  558 keV,
c                    0+(1)  -  912 keV,
c                    2+(3)  - 1115 keV,
c                    0+(2)  - 1382 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 30.07.2010.
	tclev=0.
	if(levelkev.eq.1382) go to  1382
	if(levelkev.eq.1115) go to  1115
	if(levelkev.eq. 912) go to   912
	if(levelkev.eq. 558) go to   558
	if(levelkev.eq. 187) go to   187
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1382	thlev=0.
	call nucltransK(1.196,0.074,3.5e-3,4.0e-6,tclev,thlev,tdlev)
	go to 187
c-----------------------------------------------------------------------
1115	thlev=0.
	p=100.*rnd1(d)
	if(p.le.28.8) go to 11151
	if(p.le.79.3) go to 11152
	if(p.le.97.0) go to 11153
	              go to 11154
11151	call nucltransK(1.115,0.074,4.0e-3,3.5e-7,tclev,thlev,tdlev)
	return
11152	call nucltransK(0.928,0.074,5.8e-3,0.,tclev,thlev,tdlev)
	go to 187
11153	call nucltransK(0.359,0.074,5.4e-2,0.,tclev,thlev,tdlev)
	go to 756
11154	call nucltransK(0.203,0.074,3.2e-1,0.,tclev,thlev,tdlev)
	go to 912
c-----------------------------------------------------------------------
912	thlev=15e-12
	p=100.*rnd1(d)
	if(p.le.77.3) go to 91201
	              go to 91202
91201	call nucltransK(0.725,0.074,9.7e-3,0.,tclev,thlev,tdlev)
	go to 187
91202	call nucltransK(0.354,0.074,6.0e-2,0.,tclev,thlev,tdlev)
	go to 558
c-----------------------------------------------------------------------
756	thlev=0.
	p=100.*rnd1(d)
	if(p.le.87.1) go to 75601
	if(p.le.91.7) go to 75602
	              go to 75603
75601	call nucltransK(0.569,0.074,1.7e-2,0.,tclev,thlev,tdlev)
	go to 187
75602	call nucltransK(0.208,0.074,3.0e-1,0.,tclev,thlev,tdlev)
	go to 548
75603	call nucltransK(0.198,0.074,3.5e-1,0.,tclev,thlev,tdlev)
	go to 558
c-----------------------------------------------------------------------
558	thlev=15.2e-12
	p=100.*rnd1(d)
	if(p.le.56.7) go to 55801
	              go to 55802
55801	call nucltransK(0.558,0.074,1.8e-2,0.,tclev,thlev,tdlev)
	return
55802	call nucltransK(0.371,0.074,5.1e-2,0.,tclev,thlev,tdlev)
	go to 187
c-----------------------------------------------------------------------
548	thlev=12.8e-12
	call nucltransK(0.361,0.074,5.4e-2,0.,tclev,thlev,tdlev)
	go to 187
c-----------------------------------------------------------------------
187	thlev=375e-12
	call nucltransK(0.187,0.074,4.25e-1,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Os190: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Os190low.f
c
c Local Variables: --
c mode: fortran --
c End: --
