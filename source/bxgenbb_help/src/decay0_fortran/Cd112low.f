c Cd112low.f 
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
	subroutine Cd112low(levelkeV)
c Subroutine describes the deexcitation process in Cd112 nucleus
c after 2b-decay of Sn112 to ground 0+ and excited 2+ levels
c of Cd112 (NNDC site on 22.12.2008 and NDS 79(1996)639).
c Call  : call Cd112low(levelkeV)
c Input : levelkeV - energy of Cd112 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  618 keV,
c                    0+(1)  - 1224 keV,
c                    2+(2)  - 1312 keV,
c                    0+(2)  - 1433 keV,
c                    2+(3)  - 1469 keV,
c                    0+(3)  - 1871 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 23.12.2008.
	tclev=0.
	if(levelkev.eq.1871) go to  1871
	if(levelkev.eq.1469) go to  1469
	if(levelkev.eq.1433) go to  1433
	if(levelkev.eq.1312) go to  1312
	if(levelkev.eq.1224) go to  1224
	if(levelkev.eq. 618) go to   618
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1871	thlev=0.
	p=100.*rnd1(d)
	if(p.le.86.91) go to 18711
	if(p.le.89.88) go to 18712
	               go to 18713
18711	call nucltransK(1.253,0.027,7.2e-4,1.5e-5,tclev,thlev,tdlev)
	go to 618
18712	call nucltransK(0.559,0.027,4.9e-3,0.,tclev,thlev,tdlev)
	go to 1312
18713	call nucltransK(0.402,0.027,1.3e-2,0.,tclev,thlev,tdlev)
	go to 1469
c-----------------------------------------------------------------------
1469	thlev=2.7e-12
	p=100.*rnd1(d)
	if(p.le.36.98) go to 14691
	if(p.le.99.14) go to 14692
	               go to 14693
14691	call nucltransK(1.469,0.027,5.8e-4,7.1e-5,tclev,thlev,tdlev)
	return
14692	call nucltransK(0.851,0.027,1.8e-3,0.,tclev,thlev,tdlev)
	go to 618
14693	call nucltransK(0.245,0.027,6.4e-2,0.,tclev,thlev,tdlev)
	go to 1224
c-----------------------------------------------------------------------
1433	thlev=1.9e-9
	p=100.*rnd1(d)
	if(p.le. 0.66) go to 14331
	if(p.le.39.36) go to 14332
	if(p.le.60.61) go to 14333
	               go to 14334
14331	p=100.*rnd1(d)
	if(p.le.3.8) then                         ! e0.exe
	   call pair(0.411,tclev,thlev,tdlev)     ! conversion to pair
	else
	   call electron(1.406,tclev,thlev,tdlev) ! conversion to electron
	   call gamma(0.027,0.,0.,tdlev)
	endif
        return
14332	call nucltransK(0.816,0.027,1.8e-3,0.,tclev,thlev,tdlev)
	go to 618
14333	call electron(0.182,tclev,thlev,tdlev)    ! E0 conversion to electron
	call gamma(0.027,0.,0.,tdlev)
	go to 1224
14334	call nucltransK(0.121,0.027,7.6e-1,0.,tclev,thlev,tdlev)
	go to 1312
c-----------------------------------------------------------------------
1312	thlev=2.0e-12
	p=100.*rnd1(d)
	if(p.le.26.59) go to 13121
	               go to 13122
13121	call nucltransK(1.312,0.027,6.6e-4,2.6e-5,tclev,thlev,tdlev)
	return
13122	call nucltransK(0.695,0.027,2.8e-3,0.,tclev,thlev,tdlev)
	go to 618
c-----------------------------------------------------------------------
1224	thlev=4.2e-12
	p=100.*rnd1(d)
	if(p.le. 0.12) go to 12241
	               go to 12242
12241	p=100.*rnd1(d)
	if(p.le.0.4) then                         ! e0.exe
	   call pair(0.202,tclev,thlev,tdlev)     ! conversion to pair
	else
	   call electron(1.197,tclev,thlev,tdlev) ! conversion to electron
	   call gamma(0.027,0.,0.,tdlev)
	endif
        return
12242	call nucltransK(0.607,0.027,3.9e-3,0.,tclev,thlev,tdlev)
	go to 618
c-----------------------------------------------------------------------
618	thlev=6.51e-12
	call nucltransK(0.618,0.027,3.7e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Cd112: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Cd112low.f
c
c Local Variables: --
c mode: fortran --
c End: --
