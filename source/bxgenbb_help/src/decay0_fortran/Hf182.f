c Hf182.f 
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
	subroutine Hf182(tcnuc,tdnuc)
c Scheme of 182Hf decay ("Table of Isotopes", 7th ed., 1978).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 5.03.1996.
	thnuc=2.84011e14
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	go to 270
c-----------------------------------------------------------------------
270	call beta(0.160,73.,0.,0.,t)
	thlev=1.2e-9
	p=100.*rnd1(d)
	if(p.le.91.77) go to 27001
	if(p.le.92.00) go to 27002
	               go to 27003
27001	call nucltransK(0.270,0.067,3.1e-1,0.,tclev,thlev,tdlev)
	return
27002	call nucltransK(0.173,0.067,9.5e-2,0.,tclev,thlev,tdlev)
	go to 98000
27003	call nucltransK(0.156,0.067,1.5e-1,0.,tclev,thlev,tdlev)
	go to 11400
c-----------------------------------------------------------------------
11400	thlev=0.
	call nucltransK(0.114,0.067,4.5e-0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
98000	thlev=0.
	call nucltransK(0.098,0.067,5.0e-0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Hf182.f
c
c Local Variables: --
c mode: fortran --
c End: --
