c I126.f 
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
	subroutine I126(tcnuc,tdnuc)
c Model for scheme of I126 decay (J.Katakura et al., Nucl. Data Sheets 
c 97(2002)765).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 25.11.2003.
	thnuc=1.1172e6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.47.300) go to 1 ! 47.300% beta- to 126Xe
	if(pdecay.le.98.992) go to 2 ! 51.692% EC    to 126Te
	                     go to 3 !  1.008% beta+ to 126Te
c-----------------------------------------------------------------------
c beta-
c-----------------------------------------------------------------------
1	pbeta=100.*rnd1(d)
	if(pbeta.le. 7.65) go to 880 !  7.65%
	if(pbeta.le.78.22) go to 389 ! 70.57%
	call beta(1.258,54.,0.,0.,t) ! 21.78%
        return
c-----------------------------------------------------------------------
880	call beta(0.378,54.,0.,0.,t)
88000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.20.51) go to 88001   
	               go to 88002   
88001	call nucltransK(0.880,0.035,2.2e-3,0.,tclev,thlev,tdlev)
	return
88002	call nucltransK(0.491,0.035,9.5e-3,0.,tclev,thlev,tdlev)
	go to 38900
c-----------------------------------------------------------------------
389	call beta(0.869,54.,0.,0.,t)
38900	thlev=41.3e-12
	p=100.*rnd1(d)
	call nucltransK(0.389,0.035,1.9e-2,0.,tclev,thlev,tdlev)
	return
c----------------------------------------------------------------------
c EC
c----------------------------------------------------------------------
2	call gamma(0.032,0.,0.,t)
	pec=100.*rnd1(d)
	if(pec.le. 0.014) go to 2045 !  0.014%
	if(pec.le. 0.015) go to 1873 !  0.001%
	if(pec.le. 8.630) go to 1420 !  8.615%
	if(pec.le.63.800) go to  666 ! 55.170%
        return                       ! 36.200%
c----------------------------------------------------------------------
2045	thlev=0.
	p=100.*rnd1(d)
	if(p.le.66.00) go to 20451   
	               go to 20452   
20451	call nucltransK(2.045,0.032,3.0e-4,2.3e-4,tclev,thlev,tdlev)
	return
20452	call nucltransK(1.379,0.032,1.2e-3,0.7e-4,tclev,thlev,tdlev)
	go to 66600
c----------------------------------------------------------------------
1873	thlev=0.
	call nucltransK(1.207,0.032,1.1e-3,0.1e-4,tclev,thlev,tdlev)
	go to 66600
c----------------------------------------------------------------------
1420	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 6.83) go to 14201  
	               go to 14202  
14201	call nucltransK(1.420,0.032,7.0e-4,0.2e-4,tclev,thlev,tdlev)
	return
14202	call nucltransK(0.754,0.032,2.8e-3,0.,tclev,thlev,tdlev)
	go to 66600
c----------------------------------------------------------------------
666	continue
66600	thlev=0.
	call nucltransK(0.666,0.032,3.8e-3,0.,tclev,thlev,tdlev)
	return
c----------------------------------------------------------------------
c beta+
c----------------------------------------------------------------------
3	pbeta=100.*rnd1(d)
	if(pbeta.le.19.64) go to 1666 ! 19.64%
	call beta(1.133,-52.,0.,0.,t) ! 80.36%
        return	
c-----------------------------------------------------------------------
1666	call beta(0.467,-52.,0.,0.,t)
	thlev=0.
	call nucltransK(0.666,0.032,3.8e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of I126.f
c
c Local Variables: --
c mode: fortran --
c End: --
