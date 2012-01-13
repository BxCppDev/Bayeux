c Te124low.f 
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
	subroutine Te124low(levelkeV)
c Subroutine describes the deexcitation process in Te124 nucleus
c after 2b-decay of Sn124 to ground 0+ and excited 2+ levels
c of Te124 (NNDC site on 22.12.2008 and NDS 109(2008)1655).
c Call  : call Te124low(levelkeV)
c Input : levelkeV - energy of Te124 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  603 keV,
c                    2+(2)  - 1326 keV,
c                    0+(1)  - 1657 keV,
c                    0+(2)  - 1883 keV,
c                    2+(3)  - 2039 keV,
c                    2+(4)  - 2092 keV,
c                    0+(3)  - 2153 keV,
c                    2+(5)  - 2182 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 23.12.2008.
	tclev=0.
	if(levelkev.eq.2182) go to  2182
	if(levelkev.eq.2153) go to  2153
	if(levelkev.eq.2092) go to  2092
	if(levelkev.eq.2039) go to  2039
	if(levelkev.eq.1883) go to  1883
	if(levelkev.eq.1657) go to  1657
	if(levelkev.eq.1326) go to  1326
	if(levelkev.eq. 603) go to   603
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
2182	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 9.53) go to 21821
	if(p.le.92.38) go to 21822
	               go to 21823
21821	call nucltransK(2.182,0.032,7.0e-4,3.9e-4,tclev,thlev,tdlev)
	return
21822	call nucltransK(1.580,0.032,7.7e-4,1.0e-4,tclev,thlev,tdlev)
	go to 603
21823	call nucltransK(0.857,0.032,2.3e-3,0.,tclev,thlev,tdlev)
	go to 1326
c-----------------------------------------------------------------------
2153	thlev=0.
	p=100.*rnd1(d)
	if(p.le.19.29) go to 21531
	               go to 21532
21531	call nucltransK(1.550,0.032,6.8e-4,1.0e-4,tclev,thlev,tdlev)
	go to 603
21532	call nucltransK(0.828,0.032,2.2e-3,0.,tclev,thlev,tdlev)
	go to 1326
c-----------------------------------------------------------------------
2092	thlev=0.28e-12
	p=100.*rnd1(d)
	if(p.le.97.97) go to 20921
	if(p.le.98.24) go to 20922
	               go to 20923
20921	call nucltransK(1.488,0.032,8.3e-4,7.1e-5,tclev,thlev,tdlev)
	go to 603
20922	call nucltransK(0.844,0.032,2.1e-3,0.,tclev,thlev,tdlev)
	go to 1248
20923	call nucltransK(0.766,0.032,3.0e-3,0.,tclev,thlev,tdlev)
	go to 1326
c-----------------------------------------------------------------------
2039	thlev=0.49e-12
	p=100.*rnd1(d)
	if(p.le.34.26) go to 20391
	if(p.le.94.57) go to 20392
	if(p.le.96.80) go to 20393
	if(p.le.99.04) go to 20394
	               go to 20395
20391	call nucltransK(2.039,0.032,6.7e-4,3.2e-4,tclev,thlev,tdlev)
	return
20392	call nucltransK(1.437,0.032,8.7e-4,5.5e-5,tclev,thlev,tdlev)
	go to 603
20393	call nucltransK(0.791,0.032,2.5e-3,0.,tclev,thlev,tdlev)
	go to 1248
20394	call nucltransK(0.714,0.032,4.0e-3,0.,tclev,thlev,tdlev)
	go to 1326
20395	call nucltransK(0.382,0.032,1.8e-2,0.,tclev,thlev,tdlev)
	go to 1657
c-----------------------------------------------------------------------
1883	thlev=0.76e-12
	p=100.*rnd1(d)
	if(p.le. 0.31) go to 18831
	if(p.le.99.93) go to 18832
	               go to 18833
18831	p=100.*rnd1(d)
	if(p.le.21.89) then                         ! e0.exe
	   call pair(0.861,tclev,thlev,tdlev)       ! conversion to pair
	else
	   call electron(1.851,tclev,thlev,tdlev)   ! conversion to electron
	   call gamma(0.032,0.,0.,tdlev)
	endif
        return
18832	call nucltransK(0.557,0.032,6.0e-3,0.,tclev,thlev,tdlev)
	go to 1326
18833	call electron(0.194,tclev,thlev,tdlev)      ! E0 conversion to electron
	call gamma(0.032,0.,0.,tdlev)
	go to 1657
c-----------------------------------------------------------------------
1657	thlev=0.55-12
	p=100.*rnd1(d)
	if(p.le. 0.02) go to 16571
	               go to 16572
16571	p=100.*rnd1(d)
	if(p.le.10.68) then                         ! e0.exe
	   call pair(0.636,tclev,thlev,tdlev)       ! conversion to pair
	else
	   call electron(1.626,tclev,thlev,tdlev)   ! conversion to electron
	   call gamma(0.032,0.,0.,tdlev)
	endif
        return
16572	call nucltransK(1.055,0.032,1.3e-3,0.,tclev,thlev,tdlev)
	go to 603
c-----------------------------------------------------------------------
1326	thlev=1.04e-12
	p=100.*rnd1(d)
	if(p.le.13.84) go to 13261
	               go to 13262
13261	call nucltransK(1.326,0.032,8.3e-4,2.8e-5,tclev,thlev,tdlev)
	return
13262	call nucltransK(0.723,0.032,3.1e-3,0.,tclev,thlev,tdlev)
	go to 603
c-----------------------------------------------------------------------
1248	thlev=1.4e-12
	call nucltransK(0.646,0.032,4.1e-3,0.,tclev,thlev,tdlev)
	go to 603
c-----------------------------------------------------------------------
603	thlev=6.2e-12
	call nucltransK(0.603,0.032,4.9e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Te124: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Te124low.f
c
c Local Variables: --
c mode: fortran --
c End: --
