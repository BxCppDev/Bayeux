c Tl207.f 
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
	subroutine Tl207(tcnuc,tdnuc)
c Scheme of Tl207 decay ("Table of Isotopes", 7th ed., 1978).
c Three-figured labels correspond to energies of Pb207
c excited levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 14.08.1992, 22.10.1995; 
c VIT, 30.10.2006 (update to NDS 70(1993)315 and correction to the beta shape).
	thnuc=286.2
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.0.268) go to   898  !  0.268%
	                   go to 10000  ! 99.732%
c-----------------------------------------------------------------------
898	call beta(0.529,82.,0.,0.,t)
	thlev=0.115e-12
	p=100.*rnd1(d)
	if(p.le.99.29) go to 89801   ! 99.29%
	               go to 89802   !  0.71%
89801	call nucltransK(0.898,0.088,2.5e-2,0.,tclev,thlev,tdlev)
	return
89802	call nucltransK(0.328,0.088,3.5e-1,0.,tclev,thlev,tdlev)
	go to 57000
c-----------------------------------------------------------------------
57000	thlev=130.5e-12
	call nucltransK(0.570,0.088,2.2e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
c10000	call beta(1.427,82.,0.,0.,t)
c change to forbidden spectrum with experimental correction from
c J.M.Trischuk et al., NPA 90(1967)33 and H.Daniel, RMP 40(1968)659
c cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), w=e/emass+1.
10000	call beta1(1.427,82.,0.,0.,t,0.,0.024,0.,0.)
	return
c-----------------------------------------------------------------------
	end
c
c end of Tl207.f
c
c Local Variables: --
c mode: fortran --
c End: --
