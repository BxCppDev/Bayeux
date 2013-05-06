c Sr90.f 
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
	subroutine Sr90(tcnuc,tdnuc)
c Scheme of Sr90 decay ("Table of Isotopes", 7th ed., 1978).
c Accuracy in description of: decay branches       - 0.001%,
c                           : deexcitation process - 0.001%.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c Slight update to NDS 82(1997)379.
c VIT, 9.08.1993, 22.10.1995, 26.10.2006.
	thnuc=0.9085184E+09
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
c	call beta(0.546,39.,0.,0.,t)
c Change from the allowed shape to the 1st forbidden unique with empirical
c correction from: H.H.Hansen, Appl. Rad. Isot. 34(1983)1241
	call beta_1fu(0.546,39.,0.,0.,t,0.,-0.032,0.,0.)
	return
	end
c
c end of Sr90.f
c
c Local Variables: --
c mode: fortran --
c End: --
