c Rb87.f 
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
	subroutine Rb87(tcnuc,tdnuc)
c Scheme of Rb87 decay in accordance with NDS 95(2002)543 and ENSDF
c at NNDC site on 6.08.2007.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 6.08.2007.
	thnuc=1.518e18
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
c Shape of the 3-rd forbidden non-unique beta decay in accordance with
c measurements of: A.G.Carles et al., NPA 767(2006)248.
	call beta2(0.283,38.,0.,0.,t,2,27.72,90.91,0.,0.)
	return
	end
c
c end of Rb87.f
c
c Local Variables: --
c mode: fortran --
c End: --
