c Zr96.f 
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
	subroutine Zr96(tcnuc,tdnuc)
c Scheme of Zr96 beta decay.
c It is supposed that decay goes to the first excited level (5+) of Nb96
c (E_exc=44 keV) with T1/2=2.4e20 y in accordance with: H.Heiskanen et al.,
C JPG 34(2007)837. Transition to the ground state (6+) is suppressed by 
c factor of ~1e-9, to the excited 4+ (E_exc=146 keV) - by 1e-2.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 07.05.1998.
c VIT, 18.08.2007: update to NDS 68(1993)165 and ENSDF at NNDC site.
	thnuc=7.574e27
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	call beta(0.117,41.,0.,0.,t)
	tclev=0.
	thlev=0.
	call nucltransK(0.044,0.019,2.4e0,0.,tclev,thlev,tdlev)
	return
	end
c
c end of Zr96.f
c
c Local Variables: --
c mode: fortran --
c End: --
