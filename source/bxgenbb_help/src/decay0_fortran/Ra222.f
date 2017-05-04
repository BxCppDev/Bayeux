c Ra222.f 
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
	subroutine Ra222(tcnuc,tdnuc)
c Model for scheme of Ra222 decay (NDS 107(2006)1027 and ENSDF at NNDC site 
c on 9.12.2007).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 9.12.2007.
	thnuc=36.17
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	palpha=100.*rnd1(d)
	if(palpha.le.0.0042) go to   840
	if(palpha.le.0.0083) go to   797
	if(palpha.le.0.0124) go to   653
	if(palpha.le.3.0635) go to   324
	                     go to 10000
c-----------------------------------------------------------------------
840	call alpha(5.734,0.,0.,t)
84000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.66.35) go to 84001         
	               go to 84002  
84001	call nucltransK(0.840,0.098,2.9e-2,0.,tclev,thlev,tdlev)
	return
84002	call nucltransK(0.516,0.098,2.5e-2,0.,tclev,thlev,tdlev)
	go to 32400
c-----------------------------------------------------------------------
797	call alpha(5.776,0.,0.,t)
79700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.96.75) go to 79701         
	               go to 79702  
79701	call nucltransK(0.473,0.098,1.0e-2,0.,tclev,thlev,tdlev)
	go to 32400
79702	call nucltransK(0.144,0.098,1.9e-1,0.,tclev,thlev,tdlev)
	go to 65300
c-----------------------------------------------------------------------
653	call alpha(5.917,0.,0.,t)
65300	thlev=0.
	call nucltransK(0.329,0.098,1.1e-1,0.,tclev,thlev,tdlev)
	go to 32400
c-----------------------------------------------------------------------
324	call alpha(6.240,0.,0.,t)
32400	thlev=0.
	call nucltransK(0.324,0.098,1.1e-1,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call alpha(6.559,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of Ra222.f
c
c Local Variables: --
c mode: fortran --
c End: --
