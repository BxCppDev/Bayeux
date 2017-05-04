c K40.f 
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
	subroutine K40(tcnuc,tdnuc)
c Scheme of K40 decay (J.A.Cameron et al., ENSDF 29.10.2002).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 3.08.1992, 22.10.1995, 28.11.2003;
c 3rd forbidden unique shape for beta decay, VIT, 27.10.2006.
	thnuc=3.94775e+16
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.89.140) then
c	 call beta(1.311,+20.,0.,0.,t)                       ! 89.140% b- 40Ca(gs)
c change to the 3rd forbidden unique shape in accordance with
c H.Daniel, RMP 40(1968)659 and W.H.Kelly et al., Nucl. Phys. 11(1959)492
c cf(e) = pel**6 + c1*pel**4*pnu**2 + c2*pel**2*pnu**4 + c3*pnu**6
	 call beta2(1.311,+20.,0.,0.,t,3,7.,7.,1.,0.)
	elseif(pdecay.le.99.800) then                        ! 10.660% ec 40Ar(1461)
	 call gamma(0.003,0.,0.,t)
	 thlev=1.12e-12
	 call nucltransK(1.461,0.003,3.0e-5,7.3e-5,tclev,thlev,tdlev)
	elseif(pdecay.le.99.999) then                        !  0.199% ec 40Ar(gs)
	 call gamma(0.003,0.,0.,t)
	else
         call beta(0.483,-18.,0.,0.,t)                       !  0.001% b+ 40Ar(gs)
	end if
	return
	end
c
c end of K40.f
c
c Local Variables: --
c mode: fortran --
c End: --
