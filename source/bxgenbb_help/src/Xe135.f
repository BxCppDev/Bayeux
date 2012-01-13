c Xe135.f 
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
	subroutine Xe135(tcnuc,tdnuc)
c Model of Xe135 decay (Yu.V.Sergeenkov et al., Nucl. Data Sheets 
c 84(1998)115).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 9.10.2002.
	thnuc=32904.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.0.123) go to 1062 !  0.123%%
	if(pbeta.le.0.198) go to  981 !  0.075%
	if(pbeta.le.3.311) go to  608 !  3.113%
	if(pbeta.le.3.902) go to  408 !  0.591%
	                   go to  250 ! 96.098%
c-----------------------------------------------------------------------
1062	call beta(0.089,55.,0.,0.,t)
10620	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.34) go to 10621   !  3.34%
	if(p.le.60.40) go to 10622   ! 57.06%
	if(p.le.97.07) go to 10623   ! 36.67%
	               go to 10624   !  2.93%
10621	call nucltransK(1.062,0.036,2.0e-3,0.,tclev,thlev,tdlev)
	return
10622	call nucltransK(0.813,0.036,3.8e-3,0.,tclev,thlev,tdlev)
	go to 25000
10623	call nucltransK(0.654,0.036,6.5e-3,0.,tclev,thlev,tdlev)
	go to 40800
10624	call nucltransK(0.454,0.036,1.5e-2,0.,tclev,thlev,tdlev)
	go to 60800
c-----------------------------------------------------------------------
981	call beta(0.170,55.,0.,0.,t)
98100	thlev=0.
	p=100.*rnd1(d)
	if(p.le.73.53) go to 98101   ! 73.53%
	if(p.le.79.95) go to 98102   !  6.42%
	               go to 98103   ! 20.05%
98101	call nucltransK(0.732,0.036,5.0e-3,0.,tclev,thlev,tdlev)
	go to 25000
98102	call nucltransK(0.573,0.036,9.0e-3,0.,tclev,thlev,tdlev)
	go to 40800
98103	call nucltransK(0.373,0.036,2.5e-2,0.,tclev,thlev,tdlev)
	go to 60800
c-----------------------------------------------------------------------
608	call beta(0.543,55.,0.,0.,t)
60800	thlev=0.
	p=100.*rnd1(d)
	if(p.le.92.42) go to 60801   ! 92.42%
	if(p.le.99.62) go to 60802   !  7.20%
	               go to 60803   !  0.38%
60801	call nucltransK(0.608,0.036,7.5e-3,0.,tclev,thlev,tdlev)
	return
60802	call nucltransK(0.358,0.036,2.7e-2,0.,tclev,thlev,tdlev)
	go to 25000
60803	call nucltransK(0.200,0.036,1.4e-1,0.,tclev,thlev,tdlev)
	go to 40800
c-----------------------------------------------------------------------
408	call beta(0.743,55.,0.,0.,t)
40800	thlev=0.
	p=100.*rnd1(d)
	if(p.le.55.33) go to 40801   ! 55.33%
	               go to 40802   ! 44.67%
40801	call nucltransK(0.408,0.036,2.0e-2,0.,tclev,thlev,tdlev)
	return
40802	call nucltransK(0.158,0.036,2.5e-1,0.,tclev,thlev,tdlev)
	go to 25000
c-----------------------------------------------------------------------
250	call beta(0.901,55.,0.,0.,t)
25000	thlev=0.28e-9
	call nucltransK(0.250,0.036,7.6e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Xe135.f
c
c Local Variables: --
c mode: fortran --
c End: --
