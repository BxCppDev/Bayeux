c Ti48low.f 
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
	subroutine Ti48low(levelkeV)
c Subroutine describes the deexcitation process in Ti48 nucleus
c after 2b-decay of Ca48 to ground and excited 0+ and 2+ levels
c of Ti48 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Ti48low(levelkeV)
c Input : levelkeV - energy of Ti48 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  984 keV,
c                    2+(2)  - 2421 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 19.07.1993, 22.10.1995.
	tclev=0.
	if(levelkev.eq.2421) go to  2421
	if(levelkev.eq. 984) go to   984
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
2421	thlev=24.e-15
	p=100.*rnd1(d)
	if(p.le.8.) go to 24211
	            go to 24212
24211	call nucltransK(2.421,0.005,1.5e-5,5.0e-4,tclev,thlev,tdlev)
	return
24212	call nucltransK(1.437,0.005,3.1e-5,1.8e-4,tclev,thlev,tdlev)
	go to 984
c-----------------------------------------------------------------------
984	thlev=4.3e-12
	call nucltransK(0.984,0.005,1.2e-4,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Ti48: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Ti48low.f
c
c Local Variables: --
c mode: fortran --
c End: --
