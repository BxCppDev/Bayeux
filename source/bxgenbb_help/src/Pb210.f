c Pb210.f 
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
	subroutine Pb210(tcnuc,tdnuc)
c Scheme of Pb210 decay in accordance with NDS 99(2003)649 and ENSDF at
c the NNDC site on 6.08.2007.
c Labels correspond to energies of Bi210 excited levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c Beta decays are 1st forbidden (dJ^(dPi)=0^-,1^-); thus allowed shape
c is supposen.
c VIT, 6.08.2007.
	thnuc=7.0056e8
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.84.) go to    47
	                 go to 10000
c-----------------------------------------------------------------------
47	call beta(0.0170,83.,0.,0.,t)
47000	thlev=0.
	call nucltransKLM(0.0465,0.016,14.2,0.004,3.36,0.001,1.14,
     +                    0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call beta(0.0635,83.,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of Pb210.f
c
c Local Variables: --
c mode: fortran --
c End: --
