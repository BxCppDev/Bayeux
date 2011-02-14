c Zn65.f 
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
	subroutine Zn65(tcnuc,tdnuc)
c Scheme of Zn65 decay (NDS 69(1993)209 and NNDC online corrections on 28.03.2007).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 28.03.2007.
	thnuc=2.1086784E+07
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbec=100.*rnd1(d)
	if(pbec.le.1.42) go to 1
	                 go to 2
c beta+ decay to g.s. of 65-Cu
1	call pair(0.329,0.,0.,tdlev)
	return
c X ray after EC to 65-Cu
2	call gamma(0.009,0.,0.,t)
	if(pbec.le.52.03) go to 11160
	                  go to 10000
c-----------------------------------------------------------------------
11160	thlev=0.285e-12
	p=100.*rnd1(d)
	if(p.le.99.994) go to 11161
	                go to 11162
11161	call nucltransK(1.116,0.009,1.9e-4,1.0e-6,tclev,thlev,tdlev)
	return
11162	call nucltransK(0.345,0.009,6.8e-3,0.,tclev,thlev,tdlev)
	go to 77100
c-----------------------------------------------------------------------
77100	thlev=99.e-15
	call nucltransK(0.771,0.009,3.8e-4,0.,tclev,thlev,tdlev)
c-----------------------------------------------------------------------
10000	return
	end
c
c end of Zn65.f
c
c Local Variables: --
c mode: fortran --
c End: --
