c Sn120low.f 
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
	subroutine Sn120low(levelkeV)
c Subroutine describes the deexcitation process in Sn120 nucleus
c after 2b-decay of Te120 to ground 0+ and excited 2+ levels
c of Sn120 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Sn120low(levelkeV)
c Input : levelkeV - energy of Sn120 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  - 1171 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 13.02.2004.
	tclev=0.
	if(levelkev.eq.1171) go to  1171
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1171	thlev=0.642e-12
	call nucltransK(1.171,0.029,8.0e-4,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Sn120: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Sn120low.f
c
c Local Variables: --
c mode: fortran --
c End: --
