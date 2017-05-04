c Xe133.f 
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
	subroutine Xe133(tcnuc,tdnuc)
c Scheme of Xe133 decay ("Table of Isotopes", 8th ed., 1996).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 18.08.1997. Updated 5.12.2003 in accordance with NDS 75(1995)491.
	thnuc=452995.2
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.008) go to 384
	if(pbeta.le. 0.818) go to 161
	                    go to  81
c-----------------------------------------------------------------------
384	call beta(0.044,55.,0.,0.,t)
38400	thlev=21.e-12
	p=100.*rnd1(d)
	if(p.le.32.313) go to 38401
	if(p.le.98.259) go to 38402
	                go to 38403
38401	call nucltransK(0.384,0.036,2.0e-2,0.,tclev,thlev,tdlev)
	return
38402	call nucltransK(0.303,0.036,4.4e-2,0.,tclev,thlev,tdlev)
	go to 81000
38403	call nucltransK(0.223,0.036,9.8e-2,0.,tclev,thlev,tdlev)
	go to 16100
c-----------------------------------------------------------------------
161	call beta(0.267,55.,0.,0.,t)
16100	thlev=172.e-12
	p=100.*rnd1(d)
	if(p.le.10.287) go to 16101
	                go to 16102
16101	call nucltransK(0.161,0.036,3.0e-1,0.,tclev,thlev,tdlev)
	return
16102	call nucltransK(0.080,0.036,1.8e+0,0.,tclev,thlev,tdlev)
	go to 81000
c-----------------------------------------------------------------------
81	call beta(0.346,55.,0.,0.,t)
81000	thlev=6.28e-9
	call nucltransK(0.081,0.036,1.7e+0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Xe133.f
c
c Local Variables: --
c mode: fortran --
c End: --
