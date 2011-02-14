c Pb214.f 
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
	subroutine Pb214(tcnuc,tdnuc)
c Scheme of 214Pb decay ("Table of Isotopes", 7th ed., 1978).
c Three-figured labels correspond to energies of 214Bi
c excited levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 11.08.1992, 22.10.1995.
c VIT, 4.01.2007: updated to NDS 76(1995)127, and conversion from K, L, M shells
c is introduced.
	thnuc=1608.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.034) go to   888   !  0.034%
	if(pbeta.le. 2.766) go to   839   !  2.732%
	if(pbeta.le. 2.787) go to   797   !  0.021%
	if(pbeta.le. 3.951) go to   534   !  1.164%
	if(pbeta.le.52.172) go to   352   ! 48.221%
	if(pbeta.le.93.787) go to   295   ! 41.615%
	                    go to 10000   !  6.213%
c-----------------------------------------------------------------------
888	call beta(0.135,83.,0.,0.,t)
	thlev=0.
	call nucltransK(0.511,0.091,6.0e-2,0.,tclev,thlev,tdlev)
	go to 37700
c-----------------------------------------------------------------------
839	call beta(0.184,83.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.21.3) go to 83901   ! 21.3%
	if(p.le.60.1) go to 83902   ! 38.8%
	if(p.le.72.9) go to 83903   ! 12.8%
	if(p.le.75.4) go to 83904   !  2.5%
	if(p.le.90.8) go to 83905   ! 15.4%
	if(p.le.98.8) go to 83906   !  8.0%
	              go to 83907   !  1.2%
83901	call nucltransKLM(0.839,0.091,2.99e-3,0.016,4.7e-4,
     +                    0.004,1.5e-4,0.,tclev,thlev,tdlev)
	return
83902	call nucltransKLM(0.786,0.091,3.38e-3,0.016,5.3e-4,
     +                    0.004,1.7e-4,0.,tclev,thlev,tdlev)
	go to 53000
83903	call nucltransKLM(0.580,0.091,6.06e-3,0.016,9.7e-4,
     +                    0.004,3.2e-4,0.,tclev,thlev,tdlev)
	go to 25900
83904	call nucltransKLM(0.544,0.091,6.90e-3,0.016,1.11e-3,
     +                    0.004,3.7e-4,0.,tclev,thlev,tdlev)
	go to 29500
83905	call nucltransKLM(0.487,0.091,8.65e-3,0.016,1.41e-3,
     +                    0.004,4.4e-4,0.,tclev,thlev,tdlev)
	go to 35200
83906	call nucltransKLM(0.462,0.091,9.64e-3,0.016,1.58e-3,
     +                    0.004,4.8e-4,0.,tclev,thlev,tdlev)
	go to 37700
83907	call nucltransKLM(0.305,0.091,2.40e-2,0.016,4.1e-3,
     +                    0.004,1.3e-3,0.,tclev,thlev,tdlev)
	go to 53400
c-----------------------------------------------------------------------
797	call beta(0.226,83.,0.,0.,t)
	thlev=0.
	call nucltransK(0.538,0.091,8.5e-3,0.,tclev,thlev,tdlev)
	go to 25900
c-----------------------------------------------------------------------
534	call beta(0.489,83.,0.,0.,t)
53400	thlev=0.
	p=100.*rnd1(d)
	if(p.le.16.3) go to 53401   ! 16.3%
	if(p.le.46.0) go to 53402   ! 29.7%
	              go to 53403   ! 54.0%
53401	call nucltransKL(0.534,0.091,5.0e-2,0.016,1.0e-2,
     +                   0.,tclev,thlev,tdlev)
	return
53402	call nucltransKLM(0.480,0.091,1.22e-1,0.016,1.9e-2,
     +                    0.004,6.0e-3,0.,tclev,thlev,tdlev)
	go to 53000
53403	call nucltransKLM(0.275,0.091,2.9e-1,0.016,7.3e-2,
     +                    0.004,2.4e-2,0.,tclev,thlev,tdlev)
	go to 25900
c-----------------------------------------------------------------------
37700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.26.2) go to 37701   ! 26.2%
	              go to 37702   ! 73.8%
37701	call nucltransK(0.324,0.091,2.1e-1,0.,tclev,thlev,tdlev)
	go to 53000
37702	call nucltransK(0.314,0.091,2.3e-1,0.,tclev,thlev,tdlev)
	go to 63000
c-----------------------------------------------------------------------
352	call beta(0.671,83.,0.,0.,t)
35200	thlev=0.
	call nucltransKLM(0.352,0.091,2.55e-1,0.016,4.41e-2,
     +                    0.004,1.38e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
295	call beta(0.728,83.,0.,0.,t)
29500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.67.10) go to 29501   ! 67.1%
	               go to 29502   ! 32.9%
29501	call nucltransKLM(0.295,0.091,3.8e-1,0.016,6.9e-2,
     +                    0.004,2.2e-2,0.,tclev,thlev,tdlev)
	return
29502	call nucltransKLM(0.242,0.091,7.13e-1,0.016,1.23e-1,
     +                    0.004,3.88e-2,0.,tclev,thlev,tdlev)
	go to 53000
c-----------------------------------------------------------------------
25900	thlev=0.
	p=100.*rnd1(d)
	if(p.le.81.4) go to 25901   ! 81.4%
	if(p.le.83.8) go to 25902   !  2.4%
	              go to 25903   ! 16.2%
25901	call nucltransKLM(0.259,0.091,5.92e-1,0.016,1.03e-1,
     +                    0.004,3.2e-2,0.,tclev,thlev,tdlev)
	return
25902	call nucltransKLM(0.206,0.091,1.12e+0,0.016,1.95e-1,
     +                    0.004,6.1e-2,0.,tclev,thlev,tdlev)
	go to 53000
25903	call nucltransKLM(0.196,0.091,1.28e+0,0.016,2.23e-1,
     +                    0.004,7.0e-2,0.,tclev,thlev,tdlev)
	go to 63000
c-----------------------------------------------------------------------
63000	thlev=0.
	call nucltransK(0.063,0.016,1.0e+1,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
53000	thlev=0.
	call nucltransKL(0.053,0.016,9.69e+0,0.004,3.05e+0,
     +                   0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call beta(1.023,83.,0.,0.,t)
	return
	end
c
c end of Pb214.f
c
c Local Variables: --
c mode: fortran --
c End: --
