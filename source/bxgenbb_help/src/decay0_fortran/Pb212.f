c Pb212.f 
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
	subroutine Pb212(tcnuc,tdnuc)
c Scheme of Pb212 decay ("Table of Isotopes", 7th ed., 1978).
c Three-figured labels correspond to energies of 212Bi excited
c levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 5.08.1992, 22.10.1995.
	thnuc=38304.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 5.) go to   415         !  5%
	if(pbeta.le.88.) go to   239         ! 83%
	                 go to 10000         ! 12%
c-----------------------------------------------------------------------
415	call beta(0.158,83.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.5) go to 41501          !  0.5%
	if(p.le.98.5) go to 41502          ! 98.0%
	              go to 41503          !  1.5%
41501	call nucltransK(0.415,0.091,0.24,0.,tclev,thlev,tdlev)
	return
41502	call nucltransK(0.300,0.091,0.55,0.,tclev,thlev,tdlev)
	go to 115
41503	call nucltransK(0.177,0.091,2.4,0.,tclev,thlev,tdlev)
	go to 23900
c-----------------------------------------------------------------------
239	call beta(0.334,83.,0.,0.,t)
23900	thlev=1.e-12
	call nucltransK(0.239,0.091,1.1,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
115	thlev=8.e-12
	call nucltransK(0.115,0.091,8.0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call beta(0.573,83.,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of Pb212.f
c
c Local Variables: --
c mode: fortran --
c End: --
