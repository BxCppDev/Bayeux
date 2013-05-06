c Bi210.f 
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
	subroutine Bi210(tcnuc,tdnuc)
c Scheme of Bi210 decay ("Table of Isotopes", 7th ed., 1978).
c Three-figured labels correspond to energies of 206Tl excited
c levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 14.08.1992, 22.10.1995; 30.10.2006.
c Update to NDS 99(2003)949 and empirical correction to the beta shape, VIT, 28.10.2006. 
	thnuc=433036.8
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.1.32e-4) go to 10000   !  0.000132% alfa to 206Tl
	                      go to 20000   ! 99.999868% beta to 210Po
c-----------------------------------------------------------------------
10000	palfa=100.*rnd1(d)
	if(palfa.le.60.) go to 304   ! 60%
	                 go to 265   ! 40%
c-----------------------------------------------------------------------
304	call alpha(4.656,0.,0.,t)
	thlev=4.e-12
	call nucltransK(0.304,0.086,3.9e-1,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
265	call alpha(4.694,0.,0.,t)
	thlev=3.e-9
	call nucltransK(0.265,0.086,1.6e-1,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
c20000	call beta(1.161,84.,0.,0.,t)
c experimental correction factor to allowed spectrum from
c H.Daniel, NP 31(1962)293 and RMP 40(1968)659:
c cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), w=e/emass+1
20000	call beta1(1.162,84.,0.,0.,t,28.466,0.578,-0.658,0.)
	return
c-----------------------------------------------------------------------
	end
c
c end of Bi210.f
c
c Local Variables: --
c mode: fortran --
c End: --
