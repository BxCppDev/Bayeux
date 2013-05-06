c Sc48.f 
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
	subroutine Sc48(tcnuc,tdnuc)
c Scheme of Sc48 decay ("Table of Isotopes", 8 ed., 1996 + NDS 68(1993)1).
c Three-figured labels correspond to energies of 48Ti excited
c levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 7.05.1998; 13.08.2007 update to NDS 107(2006)1747.
	thnuc=1.57212e5
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.10.02) go to 3509
	                   go to 3333
c-----------------------------------------------------------------------
3509	call beta(0.483,22.,0.,0.,t)
35090	thlev=1.4e-12
	p=100.*rnd1(d)
	if(p.le.24.14) go to 35091
	               go to 35092
35091	call nucltransK(1.213,0.005,8.8e-5,0.1e-4,tclev,thlev,tdlev)
	go to 22960
35092	call nucltransK(0.175,0.005,4.5e-3,0.,tclev,thlev,tdlev)
	go to 33330
c-----------------------------------------------------------------------
3333	call beta(0.659,22.,0.,0.,t)
33330	thlev=221.e-15
	call nucltransK(1.038,0.005,1.1e-4,0.,tclev,thlev,tdlev)
	go to 22960
c-----------------------------------------------------------------------
22960	thlev=0.762e-12
	call nucltransK(1.312,0.005,9.7e-5,0.1e-4,tclev,thlev,tdlev)
	go to 98400
c-----------------------------------------------------------------------
98400	thlev=4.04e-12
	call nucltransK(0.984,0.005,1.3e-4,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Sc48.f
c
c Local Variables: --
c mode: fortran --
c End: --
