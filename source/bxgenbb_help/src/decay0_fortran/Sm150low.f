c Sm150low.f 
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
	subroutine Sm150low(levelkeV)
c Subroutine describes the deexcitation process in Sm150 nucleus
c after 2b-decay of Nd150 to ground and excited 0+ and 2+ levels
c of Sm150 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Sm150low(levelkeV)
c Input : levelkeV - energy of Sm150 level (integer in keV) occupied
c	             initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  334 keV,
c                    0+(1)  -  740 keV,
c                    2+(2)  - 1046 keV,
c                    2+(3)  - 1194 keV,
c                    0+(2)  - 1256 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 28.06.1993, 22.10.1995; 23.09.2008.
	tclev=0.
	if(levelkev.eq.1256) go to  1256
	if(levelkev.eq.1194) go to  1194
	if(levelkev.eq.1046) go to  1046
	if(levelkev.eq. 740) go to   740
	if(levelkev.eq. 334) go to   334
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1256	thlev=0.
	p=100.*rnd1(d)
	if(p.le.93.) go to 12561
	             go to 12562
12561	call nucltransK(0.922,0.047,2.6e-3,0.,tclev,thlev,tdlev)
	go to 334
12562	call nucltransK(0.210,0.047,1.7e-1,0.,tclev,thlev,tdlev)
	go to 1046
c-----------------------------------------------------------------------
1194	thlev=1.3e-12
	p=100.*rnd1(d)
	if(p.le.55.9) go to 11941
	if(p.le.96.9) go to 11942
	if(p.le.98.7) go to 11943
	              go to 11944
11941	call nucltransK(1.194,0.047,1.6e-3,0.1e-4,tclev,thlev,tdlev)
	return
11942	call nucltransK(0.860,0.047,3.2e-3,0.,tclev,thlev,tdlev)
	go to 334
11943	call nucltransK(0.453,0.047,1.5e-2,0.,tclev,thlev,tdlev)
	go to 740
11944	call nucltransK(0.420,0.047,1.9e-2,0.,tclev,thlev,tdlev)
	go to 773
c-----------------------------------------------------------------------
1046	thlev=0.7e-12
	p=100.*rnd1(d)
	if(p.le. 7.0) go to 10461
	if(p.le.94.3) go to 10462
	if(p.le.97.0) go to 10463
	              go to 10464
10461	call nucltransK(1.046,0.047,2.0e-3,0.,tclev,thlev,tdlev)
	return
10462	call nucltransK(0.712,0.047,7.6e-3,0.,tclev,thlev,tdlev)
	go to 334
10463	call nucltransK(0.306,0.047,4.9e-2,0.,tclev,thlev,tdlev)
	go to 740
10464	call nucltransK(0.273,0.047,7.0e-2,0.,tclev,thlev,tdlev)
	go to 773
c-----------------------------------------------------------------------
773	thlev=6.6e-12
	call nucltransK(0.439,0.047,1.7e-2,0.,tclev,thlev,tdlev)
	go to 334
c-----------------------------------------------------------------------
740	thlev=20.e-12
	p=100.*rnd1(d)
c the following line is corrected in accordance with ToI'1998 and Nucl.
c Data Sheets 75(1995)827 (real page is 856)
c VIT, 23.09.2008
c	if(p.le.92.) go to 74001
	if(p.le.1.33) go to 74001
	              go to 74002
74001	call electron(0.740-0.047,tclev,thlev,tdlev) ! only ec
	call gamma(0.047,0.,0.,tdlev)
	return
74002	call nucltransK(0.407,0.047,2.0e-2,0.,tclev,thlev,tdlev)
	go to 334
c-----------------------------------------------------------------------
334	thlev=48.5e-12
	call nucltransK(0.334,0.047,3.7e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Sm150: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Sm150low.f
c
c Local Variables: --
c mode: fortran --
c End: --
