c Ca48.f 
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
	subroutine Ca48(tcnuc,tdnuc)
c Scheme of Ca48 beta decay. 
c It is supposed that decay goes to excited 5+ level of Sc48 (E_exc=131 keV)
c with T1/2=1.1e21 y calculated in M.Aunola et al., Europhys. Lett. 46(1999)577 
c (transition to g.s. is suppressed by factor of ~1e-10; to 4+ excited level 
c with E_exc=252 keV - by factor of ~1e-5).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 07.05.1998; update of 13.08.2007.
	thnuc=3.47e28
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	call beta(0.151,21.,0.,0.,t)
	tclev=0.
	thlev=0.
	call nucltransK(0.131,0.004,8.1e-3,0.,tclev,thlev,tdlev)
	return
	end
c
c end of Ca48.f
c
c Local Variables: --
c mode: fortran --
c End: --
