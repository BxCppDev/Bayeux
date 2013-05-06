c Ar39.f 
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
	subroutine Ar39(tcnuc,tdnuc)
c Model for Ar39 decay (Nucl. Phys. A 633(1998)1).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 12.09.2005; 29.10.2006.
	thnuc=8.488762e9
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
c	call beta(0.565,19.,0.,0.,t)
c change to the 1st forbidden unique shape
	call beta_1fu(0.565,19.,0.,0.,t,0.,0.,0.,0.)
	return
	end
c
c end of Ar39.f
c
c Local Variables: --
c mode: fortran --
c End: --
