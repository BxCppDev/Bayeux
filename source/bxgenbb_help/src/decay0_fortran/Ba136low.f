c Ba136low.f 
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
	subroutine Ba136low(levelkeV)
c Subroutine describes the deexcitation process in Ba136 nucleus
c after 2b-decay of Xe136 or Ce136 to ground and excited 0+ and 2+ levels
c of Ba136 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Ba136low(levelkeV)
c Input : levelkeV - energy of Ba136 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  819 keV,
c                    2+(2)  - 1551 keV,
c                    0+(1)  - 1579 keV,
c                    2+(3)  - 2080 keV,
c                    2+(4)  - 2129 keV,
c                    0+(2)  - 2141 keV,
c                    2+(5)  - 2223 keV,
c                    0+(3)  - 2315 keV,
c                    2+(6)  - 2400 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 28.06.1993, 22.10.1995.
c VIT, 10.12.2008: updated to NDS 95(2002)837 
c (E0 transitions from 2141 and 1579 keV levels are neglected); 
c levels 2080, 2129, 2141, 2223, 2315 and 2400 keV were added.
	tclev=0.
	if(levelkev.eq.2400) go to  2400
	if(levelkev.eq.2315) go to  2315
	if(levelkev.eq.2223) go to  2223
	if(levelkev.eq.2141) go to  2141
	if(levelkev.eq.2129) go to  2129
	if(levelkev.eq.2080) go to  2080
	if(levelkev.eq.1579) go to  1579
	if(levelkev.eq.1551) go to  1551
	if(levelkev.eq. 819) go to   819
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
2400	thlev=0.
	call nucltransK(1.581,0.037,1.0e-3,1.1e-4,tclev,thlev,tdlev)
	go to 819
c-----------------------------------------------------------------------
2315	thlev=0.
	call nucltransK(1.497,0.037,8.7e-4,7.7e-5,tclev,thlev,tdlev)
	go to 819
c-----------------------------------------------------------------------
2223	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 4.3) go to 22231
	if(p.le.51.9) go to 22232
	              go to 22233
22231	call nucltransK(2.223,0.037,7.8e-4,4.0e-4,tclev,thlev,tdlev)
	return
22232	call nucltransK(1.404,0.037,9.6e-4,4.8e-5,tclev,thlev,tdlev)
	go to 819
22233	call nucltransK(0.672,0.037,6.5e-3,0.,tclev,thlev,tdlev)
	go to 1551
c-----------------------------------------------------------------------
2141	thlev=0.
	call nucltransK(1.323,0.037,1.0e-3,2.6e-5,tclev,thlev,tdlev)
	go to 819
c-----------------------------------------------------------------------
2129	thlev=0.051e-12
	p=100.*rnd1(d)
	if(p.le.33.3) go to 21291
	              go to 21292
21291	call nucltransK(2.129,0.037,7.7e-4,3.6e-4,tclev,thlev,tdlev)
	return
21292	call nucltransK(1.310,0.037,1.4e-3,2.3e-5,tclev,thlev,tdlev)
	go to 819
c-----------------------------------------------------------------------
2080	thlev=0.6e-12
	p=100.*rnd1(d)
	if(p.le.35.4) go to 20801
	if(p.le.93.8) go to 20802
	              go to 20803
20801	call nucltransK(2.080,0.037,7.6e-4,3.3e-4,tclev,thlev,tdlev)
	return
20802	call nucltransK(1.262,0.037,1.3e-3,1.4e-5,tclev,thlev,tdlev)
	go to 819
20803	call nucltransK(0.529,0.037,1.0e-2,0.,tclev,thlev,tdlev)
	go to 1551
c-----------------------------------------------------------------------
1579	thlev=0.
	call nucltransK(0.761,0.037,3.4e-3,0.,tclev,thlev,tdlev)
	go to 819
c-----------------------------------------------------------------------
1551	thlev=1.01e-12
	p=100.*rnd1(d)
	if(p.le.52.1) go to 15511
	              go to 15512
15511	call nucltransK(1.551,0.037,8.4e-4,9.7e-5,tclev,thlev,tdlev)
	return
15512	call nucltransK(0.733,0.037,4.5e-3,0.,tclev,thlev,tdlev)
	go to 819
c-----------------------------------------------------------------------
819	thlev=1.93e-12
	call nucltransK(0.819,0.037,2.9e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Ba136: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Ba136low.f
c
c Local Variables: --
c mode: fortran --
c End: --
