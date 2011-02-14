c Sn116low.f 
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
	subroutine Sn116low(levelkeV)
c Subroutine describes the deexcitation process in Sn116 nucleus
c after 2b-decay of Cd116 to ground and excited 0+ and 2+ levels
c of Sn116 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Sn116low(levelkeV)
c Input : levelkeV - energy of Sn116 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  - 1294 keV,
c                    0+(1)  - 1757 keV,
c                    0+(2)  - 2027 keV,
c                    2+(2)  - 2112 keV,
c                    2+(3)  - 2225 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 28.06.1993, 22.10.1995.
	tclev=0.
	if(levelkev.eq.2225) go to  2225
	if(levelkev.eq.2112) go to  2112
	if(levelkev.eq.2027) go to  2027
	if(levelkev.eq.1757) go to  1757
	if(levelkev.eq.1294) go to  1294
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
2225	thlev=0.
	p=100.*rnd1(d)
	if(p.le.37.) go to 22251
	             go to 22252
22251	call nucltransK(2.225,0.029,2.7e-4,3.4e-4,tclev,thlev,tdlev)
	return
22252	call nucltransK(0.932,0.029,1.5e-3,0.,tclev,thlev,tdlev)
	go to 1294
c-----------------------------------------------------------------------
2112	thlev=0.
	p=100.*rnd1(d)
	if(p.le.54.9) go to 21121
	if(p.le.96.9) go to 21122
	              go to 21123
21121	call nucltransK(2.112,0.029,3.1e-4,2.7e-4,tclev,thlev,tdlev)
	return
21122	call nucltransK(0.819,0.029,2.6e-3,0.,tclev,thlev,tdlev)
	go to 1294
21123	call nucltransK(0.355,0.029,1.8e-2,0.,tclev,thlev,tdlev)
	go to 1757
c-----------------------------------------------------------------------
2027	thlev=0.
	call nucltransK(0.733,0.029,2.7e-3,0.,tclev,thlev,tdlev)
	go to 1294
c-----------------------------------------------------------------------
1757	thlev=0.
	p=100.*rnd1(d)
	if(p.le.0.29) go to 17571
	              go to 17572
17571	call electron(1.757-0.029,tclev,thlev,tdlev) ! only ec
	call gamma(0.029,0.,0.,tdlev)
	return
17572	call nucltransK(0.463,0.029,9.0e-3,0.,tclev,thlev,tdlev)
	go to 1294
c-----------------------------------------------------------------------
1294	thlev=0.36e-12
	call nucltransK(1.294,0.029,7.5e-4,0.5e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Sn116: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Sn116low.f
c
c Local Variables: --
c mode: fortran --
c End: --
