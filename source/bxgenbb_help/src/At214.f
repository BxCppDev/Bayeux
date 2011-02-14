c At214.f 
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
	subroutine At214(tcnuc,tdnuc)
c Model for scheme of At214 decay (NDS 99(2003)649 and ENSDF at NNDC site 
c on 9.12.2007).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 9.12.2007.
	thnuc=558.e-9
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	palpha=100.*rnd1(d)
	if(palpha.le.0.32) go to   563
	if(palpha.le.0.90) go to   348
	if(palpha.le.1.05) go to   320
	                   go to 10000
c-----------------------------------------------------------------------
563	call alpha(8.267,0.,0.,t)
	thlev=0.
	call nucltransK(0.563,0.091,8.6e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
348	call alpha(8.478,0.,0.,t)
	thlev=0.
	call nucltransK(0.348,0.091,7.9e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
320	call alpha(8.505,0.,0.,t)
	thlev=0.
	call nucltransK(0.320,0.091,3.9e-1,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call alpha(8.819,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of At214.f
c
c Local Variables: --
c mode: fortran --
c End: --
