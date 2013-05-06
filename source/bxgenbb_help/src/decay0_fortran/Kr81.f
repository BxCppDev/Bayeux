c Kr81.f 
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
	subroutine Kr81(tcnuc,tdnuc)
c Scheme of Kr81 decay (NDS 79(1996)447 and ENSDF at NNDC site on 9.12.2007).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 9.12.2007.
	thnuc=7.226493e12
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.0.30) go to   276
	                   go to 10000
c-----------------------------------------------------------------------
276	call gamma(0.013,0.,0.,t) ! capture from only K shell is supposed
	thlev=9.7e-12
	call nucltransKLM(0.276,0.013,7.3e-3,0.002,7.8e-4,0.000,2.6e-4,
     +                    0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	pklm=100.*rnd1(d)
	if(                    pklm.le.84.73) call gamma(0.013,0.,0.,t)
	if(pklm.gt.84.73 .and. pklm.le.97.44) call gamma(0.002,0.,0.,t)
	if(pklm.gt.97.44                    ) call gamma(0.000,0.,0.,t)
c-----------------------------------------------------------------------
	return
	end
c
c end of Kr81.f
c
c Local Variables: --
c mode: fortran --
c End: --
