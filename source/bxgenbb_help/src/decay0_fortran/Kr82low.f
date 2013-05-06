c Kr82low.f 
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
	subroutine Kr82low(levelkeV)
c Subroutine describes the deexcitation process in Kr82 nucleus
c after 2b-decay of Se82 to ground and excited 0+ and 2+ levels
c of Kr82 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Kr82low(levelkeV)
c Input : levelkeV - energy of Kr82 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  776 keV,
c                    2+(2)  - 1475 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 28.06.1993, 22.10.1995.
	tclev=0.
	if(levelkev.eq.1475) go to  1475
	if(levelkev.eq. 776) go to   776
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1475	thlev=0.
	p=100.*rnd1(d)
	if(p.le.36.7) go to 14751
	              go to 14752
14751	call nucltransK(1.475,0.014,2.0e-4,0.5e-4,tclev,thlev,tdlev)
	return
14752	call nucltransK(0.698,0.014,1.3e-3,0.,tclev,thlev,tdlev)
	go to 776
c-----------------------------------------------------------------------
776	thlev=5.e-12
	call nucltransK(0.776,0.014,9.3e-4,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Kr82: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Kr82low.f
c
c Local Variables: --
c mode: fortran --
c End: --
