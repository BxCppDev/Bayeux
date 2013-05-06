c Pb211.f 
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
	subroutine Pb211(tcnuc,tdnuc)
c Scheme of Pb211 decay ("Table of Isotopes", 7th ed., 1978).
c Three- and four-figured labels correspond to energies of Bi211
c excited levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 14.08.1992, 22.10.1995;
c VIT, 31.10.2006 (updated to NDS 103(2004)183). 
c Main beta decays are 1st forbidden (dJ^(dPi)=0^-,1^-); thus allowed shape
c is supposen.
	thnuc=2166.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.0.0186) go to  1271  !  0.0186%
	if(pbeta.le.0.0199) go to  1234  !  0.0013%
	if(pbeta.le.0.0369) go to  1196  !  0.0170%
	if(pbeta.le.0.8660) go to  1109  !  0.8291%
	if(pbeta.le.0.8706) go to  1104  !  0.0046%
	if(pbeta.le.0.9265) go to  1081  !  0.0559%
	if(pbeta.le.0.9485) go to   951  !  0.0220%
	if(pbeta.le.7.2616) go to   832  !  6.3131%
	if(pbeta.le.8.7999) go to   405  !  1.5383%
	                    go to 10000  ! 91.2001%
c-----------------------------------------------------------------------
1271	call beta(0.096,83.,0.,0.,t)
12710	thlev=0.
	p=100.*rnd1(d)
	if(p.le.36.76) go to 12711   ! 36.76%
	if(p.le.68.65) go to 12712   ! 31.89%
	               go to 12713   ! 31.35%
12711	call nucltransK(1.271,0.091,1.0e-2,0.2e-4,tclev,thlev,tdlev)
	return
12712	call nucltransK(0.866,0.091,2.8e-2,0.,tclev,thlev,tdlev)
	go to 40500
12713	call nucltransK(0.504,0.091,1.1e-1,0.,tclev,thlev,tdlev)
	go to 76700
c-----------------------------------------------------------------------
1234	call beta(0.133,83.,0.,0.,t)
	call nucltransK(1.234,0.091,1.1e-2,0.1e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1196	call beta(0.171,83.,0.,0.,t)
11960	thlev=0.
	p=100.*rnd1(d)
	if(p.le.62.96) go to 11961   ! 62.96%
	               go to 11962   ! 37.04%
11961	call nucltransK(1.196,0.091,1.2e-2,0.1e-4,tclev,thlev,tdlev)
	return
11962	call nucltransK(0.430,0.091,1.8e-1,0.,tclev,thlev,tdlev)
	go to 76700
c-----------------------------------------------------------------------
1109	call beta(0.258,83.,0.,0.,t)
11090	thlev=0.
	p=100.*rnd1(d)
	if(p.le.13.9) go to 11091   ! 13.9%
	if(p.le.72.8) go to 11092   ! 58.9%
	if(p.le.77.0) go to 11093   !  4.2%
	              go to 11094   ! 23.0%
11091	call nucltransK(1.109,0.091,1.5e-2,0.4e-6,tclev,thlev,tdlev)
	return
11092	call nucltransK(0.705,0.091,5.0e-2,0.,tclev,thlev,tdlev)
	go to 40500
11093	call nucltransK(0.343,0.091,3.2e-1,0.,tclev,thlev,tdlev)
	go to 76700
11094	call nucltransK(0.095,0.016,9.6e+0,0.,tclev,thlev,tdlev)
	go to 10140
c-----------------------------------------------------------------------
1104	call beta(0.263,83.,0.,0.,t)
11040	call nucltransK(1.104,0.091,1.5e-2,0.3e-6,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1081	call beta(0.286,83.,0.,0.,t)
10810	thlev=0.
	p=100.*rnd1(d)
	if(p.le.21.8) go to 10811   ! 21.8%
	if(p.le.44.9) go to 10812   ! 23.1%
	              go to 10813   ! 55.1%
10811	call nucltransK(1.081,0.091,1.6e-2,0.1e-4,tclev,thlev,tdlev)
	return
10812	call nucltransK(0.677,0.091,5.3e-2,0.,tclev,thlev,tdlev)
	go to 40500
10813	call nucltransK(0.314,0.091,4.1e-1,0.,tclev,thlev,tdlev)
	go to 76700
c-----------------------------------------------------------------------
10140	thlev=0.
	p=100.*rnd1(d)
	if(p.le.28.7) go to 10141   ! 28.7%
	              go to 10142   ! 71.3%
10141	call nucltransK(1.014,0.091,1.9e-2,0.,tclev,thlev,tdlev)
	return
10142	call nucltransK(0.609,0.091,7.0e-2,0.,tclev,thlev,tdlev)
	go to 40500
c-----------------------------------------------------------------------
951	call beta(0.416,83.,0.,0.,t)
95100	thlev=0.
	call nucltransK(0.951,0.091,2.2e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
832	call beta(0.535,83.,0.,0.,t)
83200	thlev=0.
	p=100.*rnd1(d)
	if(p.le.57.4) go to 83201   ! 57.4%
	if(p.le.90.3) go to 83202   ! 32.9%
	              go to 83203   !  9.7%
83201	call nucltransK(0.832,0.091,2.9e-2,0.,tclev,thlev,tdlev)
	return
83202	call nucltransK(0.427,0.091,1.9e-1,0.,tclev,thlev,tdlev)
	go to 40500
83203	call nucltransK(0.065,0.016,6.9e+0,0.,tclev,thlev,tdlev)
	go to 76700
c-----------------------------------------------------------------------
76700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.57.4) go to 76701   ! 93.7%
	              go to 76702   !  6.3%
76701	call nucltransK(0.767,0.091,4.0e-2,0.,tclev,thlev,tdlev)
	return
76702	call nucltransK(0.362,0.091,2.8e-1,0.,tclev,thlev,tdlev)
	go to 40500
c-----------------------------------------------------------------------
405	call beta(0.962,83.,0.,0.,t)
40500	thlev=0.317e-9
	call nucltransK(0.405,0.091,1.3e-1,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call beta(1.367,83.,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of Pb211.f
c
c Local Variables: --
c mode: fortran --
c End: --
