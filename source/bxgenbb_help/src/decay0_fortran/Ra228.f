c Ra228.f 
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
	subroutine Ra228(tcnuc,tdnuc)
c Scheme of Ra228 decay in accordance with NDS 80(1997)723 and
C ENSDF database at NNDC site on 8.08.2007.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 8.08.2007.
	thnuc=1.814512e8
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.30.) go to 331         ! 30%
	if(pbeta.le.50.) go to 202         ! 20%
	if(pbeta.le.90.) go to  67         ! 40%
	                 go to  63         ! 10%
c-----------------------------------------------------------------------
331	call beta(0.0128,89.,0.,0.,t)
33100	thlev=0.
	p=100.*rnd1(d)
	if(p.le.50.) go to 33101          !  50%
	             go to 33102          !  50%
33101	call nucltransK(0.0264,0.0198,2.1e2,0.,tclev,thlev,tdlev)
	go to 67000
33102	call nucltransK(0.0128,0.0050,8.7e0,0.,tclev,thlev,tdlev)
	go to 20200
c-----------------------------------------------------------------------
202	call beta(0.0257,89.,0.,0.,t)
20200	thlev=0.
	call nucltransK(0.0135,0.0050,6.1e0,0.,tclev,thlev,tdlev)
	go to 67000
c-----------------------------------------------------------------------
67	call beta(0.0392,89.,0.,0.,t)
67000	thlev=0.
	call nucltransK(0.0067,0.0050,1.6e6,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
63	call beta(0.0396,89.,0.,0.,t)
63000	thlev=0.
	call nucltransK(0.0063,0.0050,7.1e6,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Ra228.f
c
c Local Variables: --
c mode: fortran --
c End: --
