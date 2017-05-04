c Rh106.f 
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
	subroutine Rh106(tcnuc,tdnuc)
c Approximate scheme of 106Rh decay ("Table of Isotopes", 7th ed., 1978)
c (beta decays to excited levels of 106Pd not higher 2.002 MeV,
c 99.32% of decay).
c Three-figured labels correspond to energies of 106Pd excited levels
c in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 17.12.1995.
	thnuc=29.80
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.45) go to  2002
	if(pbeta.le. 0.52) go to  1707
	if(pbeta.le. 2.32) go to  1562
	if(pbeta.le.12.32) go to  1134
	if(pbeta.le.19.32) go to   512
	                   go to 10000
c-----------------------------------------------------------------------
2002	call beta(1.539,46.,0.,0.,t)
20020	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.4) go to 20021
	if(p.le.98.6) go to 20022
	              go to 20023
20021	call nucltransK(1.490,0.024,4.0e-4,0.3e-4,tclev,thlev,tdlev)
	go to 51200
20022	call nucltransK(0.874,0.024,1.3e-3,0.,tclev,thlev,tdlev)
	go to 11280
20023	call nucltransK(0.440,0.024,8.5e-3,0.,tclev,thlev,tdlev)
	go to 15620
c-----------------------------------------------------------------------
1707	call beta(1.834,46.,0.,0.,t)
17070	thlev=0.
	p=100.*rnd1(d)
	if(p.le.89.0) go to 17071
	              go to 17072
17071	call nucltransK(1.195,0.024,7.5e-4,0.,tclev,thlev,tdlev)
	go to 51200
17072	call nucltransK(0.578,0.024,3.7e-3,0.,tclev,thlev,tdlev)
	go to 11280
c-----------------------------------------------------------------------
1562	call beta(1.979,46.,0.,0.,t)
15620	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 9.1) go to 15621
	if(p.le.95.6) go to 15622
	if(p.le.96.8) go to 15623
	              go to 15624
15621	call nucltransK(1.562,0.024,3.5e-4,0.4e-4,tclev,thlev,tdlev)
	return
15622	call nucltransK(1.051,0.024,8.5e-4,0.,tclev,thlev,tdlev)
	go to 51200
15623	call nucltransK(0.434,0.024,8.5e-3,0.,tclev,thlev,tdlev)
	go to 11280
15624	call nucltransK(0.428,0.024,8.5e-3,0.,tclev,thlev,tdlev)
	go to 11340
c-----------------------------------------------------------------------
1134	call beta(2.407,46.,0.,0.,t)
11340	thlev=7.0e-12
	call nucltransK(0.622,0.024,3.3e-3,0.,tclev,thlev,tdlev)
	go to 51200
c-----------------------------------------------------------------------
11280	thlev=3.2e-12
	p=100.*rnd1(d)
	if(p.le.34.) go to 11281
	             go to 11282
11281	call nucltransK(1.128,0.024,7.0e-4,0.,tclev,thlev,tdlev)
	return
11282	call nucltransK(0.616,0.024,3.0e-3,0.,tclev,thlev,tdlev)
	go to 51200
c-----------------------------------------------------------------------
512	call beta(3.029,46.,0.,0.,t)
51200	thlev=11.0e-12
	call nucltransK(0.512,0.024,5.5e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call beta(3.541,46.,0.,0.,t)
	return
	end
c
c end of Rh106.f
c
c Local Variables: --
c mode: fortran --
c End: --
