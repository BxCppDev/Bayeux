c Mn54.f 
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
	subroutine Mn54(tcnuc,tdnuc)
c Scheme of Mn54 decay ("Table of Isotopes", 8th ed., 1996 + NDS 50(1987)255).
c Accuracy in description of: decay branches       - 0.001%,
c                           : deexcitation process - 0.001%.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 16.04.1998.
c VIT, 1.04.2007, updated to NDS 107(2006)1393.
	thnuc=2.696717E+07
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
c 100% EC to Cr54
	call gamma(0.006,0.,0.,t)
	thlev=7.9e-12
	call nucltransK(0.835,0.006,2.4e-4,0.,tclev,thlev,tdlev)
	return
	end
c
c end of Mn54.f
c
c Local Variables: --
c mode: fortran --
c End: --
