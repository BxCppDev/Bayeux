c Kr85.f 
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
	subroutine Kr85(tcnuc,tdnuc)
c Scheme of Kr85 decay (NDS 62(1991)271 and ENSDF at NNDC site on 9.12.2007).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 9.12.2007.
	thnuc=3.394243e8
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.0.434) go to   514
	                    go to 10000
c-----------------------------------------------------------------------
514	call beta(0.173,37.,0.,0.,t)
514000	thlev=1.015e-6
	p=100.*rnd1(d)
	if(p.le.99.99947) go to 51401
	                  go to 51402
51401	call nucltransKLM(0.514,0.014,6.3e-3,0.002,7.1e-4,0.000,2.3e-4,
     +                    0.,tclev,thlev,tdlev)
	return
51402	call nucltransKLM(0.363,0.014,2.9e-2,0.002,3.9e-3,0.000,1.3e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 15100
c-----------------------------------------------------------------------
15100	thlev=0.71e-9
	call nucltransKLM(0.151,0.014,4.3e-2,0.002,4.8e-3,0.000,3.4e-4,
     +                    0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
c 1st forbidden unique beta decay
10000	call beta2(0.687,37.,0.,0.,t,1,1.,0.,0.,0.)
c-----------------------------------------------------------------------
	return
	end
c
c end of Kr85.f
c
c Local Variables: --
c mode: fortran --
c End: --
