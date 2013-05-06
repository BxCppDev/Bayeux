c Cd113.f 
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
	subroutine Cd113(tcnuc,tdnuc)
c Scheme of Cd113 beta decay.
c Half-life and coefficients in the shape correction factor are taken from:
c F.A.Danevich et al., Phys. At. Nuclei 59(1996)1.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 31.03.2006.
	thnuc=2.42987e23
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	c1=1.01*7.
	c2=1.48*7.
	c3=0.684
c Q_beta=0.320 MeV, G.Audi et al., Nucl. Phys. A 729(2003)337.
	call beta2(0.320,49.,0.,0.,t,3,c1,c2,c3,0.)
	return
	end
c
c end of Cd113.f
c
c Local Variables: --
c mode: fortran --
c End: --
