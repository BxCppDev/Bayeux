c Xe130low.f 
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
	subroutine Xe130low(levelkeV)
c Subroutine describes the deexcitation process in Xe130 nucleus
c after 2b-decay of Te130 to ground 0+ and excited 2+ levels
c of Xe130 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Xe130low(levelkeV)
c Input : levelkeV - energy of Xe130 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  536 keV,
c                    2+(2)  - 1122 keV,
c                    0+(1)  - 1794 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 20.06.1996; 3.04.2006.
	tclev=0.
	if(levelkev.eq.1794) go to  1794
	if(levelkev.eq.1122) go to  1122
	if(levelkev.eq. 536) go to   536
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1794	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.0) go to 17941 ! E0 to gs; ToI-1978
	if(p.le.86.3) go to 17942
	              go to 17943
17941	p=100.*rnd1(d)
	if(p.le.12.7) then                        ! e0.exe
	   call pair(0.772,tclev,thlev,tdlev)     ! conversion to pair
	else
	   call electron(1.759,tclev,thlev,tdlev) ! conversion to electron
	   call gamma(0.035,0.,0.,tdlev)
	endif
        return
17942	call nucltransK(1.258,0.035,1.0e-3,1.5e-5,tclev,thlev,tdlev)
	go to 536
17943	call nucltransK(0.672,0.035,4.1e-3,0.,tclev,thlev,tdlev)
	go to 1122
c-----------------------------------------------------------------------
1122	thlev=0.
	p=100.*rnd1(d)
	if(p.le.13.3) go to 11221 
	              go to 11222
11221	call nucltransK(1.122,0.035,1.3e-3,9.0e-7,tclev,thlev,tdlev)
	return
11222	call nucltransK(0.586,0.035,7.5e-3,0.,tclev,thlev,tdlev)
	go to 536
c-----------------------------------------------------------------------
536	thlev=7.0e-12
	call nucltransK(0.536,0.035,7.4e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Xe130: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Xe130low.f
c
c Local Variables: --
c mode: fortran --
c End: --
