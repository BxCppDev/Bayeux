c P32.f 
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
	subroutine P32(tcnuc,tdnuc)
c Scheme of P32 beta decay, ToI'1998 and ENSDF'2004.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 5.11.2006.
	thnuc=1.2323232e6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
c experimental corrections to the allowed beta shape from
c H.Daniel, RMP 40(1968)659 and M.J.Canty et al., NP 85(1966)317
c cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), w=e/emass+1
	call beta1(1.710,16.,0.,0.,t,0.,0.003,0.,0.)
	return
	end
c
c end of P32.f
c
c Local Variables: --
c mode: fortran --
c End: --
