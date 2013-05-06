c Th234.f 
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
	subroutine Th234(tcnuc,tdnuc)
c Scheme of Th234 decay to Pa234m (not Pa234!) in accordance with NDS 
c 108(2007)681 and ENSDF database at NNDC site on 8.08.2007.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 8.08.2007.
	thnuc=2.08224e6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 1.501) go to 187         !  1.501%
	if(pbeta.le. 1.516) go to 177         !  0.015%
	if(pbeta.le. 7.921) go to 167         !  6.405%
	if(pbeta.le.21.933) go to 166         ! 14.012%
	                    go to  74         ! 78.067%
c-----------------------------------------------------------------------
187	call beta(0.086,91.,0.,0.,t)
18700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.17.0) go to 18701          !  17.0%
	if(p.le.21.7) go to 18702          !   4.7%
	              go to 18703          !  78.3%
18701	call nucltransK(0.113,0.021,2.3e-1,0.,tclev,thlev,tdlev)
	go to 74000
18702	call nucltransK(0.083,0.021,2.0e-1,0.,tclev,thlev,tdlev)
	go to 10300
18703	call nucltransK(0.020,0.005,2.4e2,0.,tclev,thlev,tdlev)
	go to 16700
c-----------------------------------------------------------------------
177	call beta(0.096,91.,0.,0.,t)
17700	thlev=0.
	call nucltransK(0.103,0.021,3.8e0,0.,tclev,thlev,tdlev)
	go to 74000
c-----------------------------------------------------------------------
167	call beta(0.106,91.,0.,0.,t)
16700	thlev=0.55e-9
	p=100.*rnd1(d)
	if(p.le.32.1) go to 16701          !  32.1%
	              go to 16702          !  67.9%
16701	call nucltransK(0.093,0.021,1.5e-1,0.,tclev,thlev,tdlev)
	go to 74000
16702	call nucltransK(0.063,0.021,4.1e-1,0.,tclev,thlev,tdlev)
	go to 10300
c-----------------------------------------------------------------------
166	call beta(0.106,91.,0.,0.,t)
16600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.97.0) go to 16601          !  97.0%
	              go to 16602          !   3.0%
16601	call nucltransK(0.092,0.021,5.3e0,0.,tclev,thlev,tdlev)
	go to 74000
16602	call nucltransK(0.062,0.021,2.5e1,0.,tclev,thlev,tdlev)
	go to 10300
c-----------------------------------------------------------------------
10300	thlev=0.
	call nucltransK(0.029,0.021,4.4e3,0.,tclev,thlev,tdlev)
	go to 74000
c-----------------------------------------------------------------------
74	call beta(0.199,91.,0.,0.,t)
c below is creation of Pa234m with T1/2=1.159 m which mainly beta^- decays
C to U234 (IT to Pa234 is only 0.16%); decay of Pa234m should be generated 
c independently
74000	return 
c-----------------------------------------------------------------------
	end
c
c end of Th234.f
c
c Local Variables: --
c mode: fortran --
c End: --
