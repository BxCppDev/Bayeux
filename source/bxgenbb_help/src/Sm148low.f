c Sm148low.f 
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
	subroutine Sm148low(levelkeV)
c Subroutine describes the deexcitation process in Sm148 nucleus
c after 2b-decay of Nd148 to ground and excited 0+ and 2+ levels
c of Sm148 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Sm148low(levelkeV)
c Input : levelkeV - energy of Sm148 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  550 keV,
c                    2+(2)  - 1455 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 30.11.1995.
	tclev=0.
	if(levelkev.eq.1455) go to  1455
	if(levelkev.eq. 550) go to   550
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1455	thlev=0.6e-12
	p=100.*rnd1(d)
	if(p.le.42.) go to 14551
	             go to 14552
14551	call nucltransK(1.455,0.047,1.1e-3,0.3e-4,tclev,thlev,tdlev)
	return
14552	call nucltransK(0.904,0.047,2.8e-3,0.,tclev,thlev,tdlev)
	go to 550
c-----------------------------------------------------------------------
550	thlev=7.3e-12
	call nucltransK(0.550,0.047,9.0e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Sm148: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Sm148low.f
c
c Local Variables: --
c mode: fortran --
c End: --
