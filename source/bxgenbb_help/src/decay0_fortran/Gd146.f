c Gd146.f 
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
	subroutine Gd146(tcnuc,tdnuc)
c Scheme of 146Gd decay ("Table of Isotopes", 7th ed., 1978).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 10.03.1996.
	thnuc=4.173e6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le. 0.3) go to 806
	if(pdecay.le. 0.8) go to 743
	if(pdecay.le.77.2) go to 385
	                   go to 230
c-----------------------------------------------------------------------
806	call gamma(0.049,0.,0.,t)
80600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.36.) go to 80601
	             go to 80602
80601	call nucltransK(0.576,0.049,1.8e-2,0.,tclev,thlev,tdlev)
	go to 23000
80602	call nucltransK(0.421,0.049,4.5e-2,0.,tclev,thlev,tdlev)
	go to 38500
c-----------------------------------------------------------------------
743	call gamma(0.049,0.,0.,t)
74300	thlev=0.
	call nucltransK(0.743,0.049,1.0e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
385	p=100.*rnd1(d)
	if(p.le.99.91) call gamma(0.049,0.,0.,t)
	if(p.gt.99.91) call beta(0.343,-63.,0.,0.,t)
38500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.0.4) go to 38501
	             go to 38502
38501	call nucltransK(0.269,0.049,8.0e-2,0.,tclev,thlev,tdlev)
	go to 11500
38502	call nucltransK(0.155,0.049,6.5e-1,0.,tclev,thlev,tdlev)
	go to 23000
c-----------------------------------------------------------------------
230	call gamma(0.049,0.,0.,t)
23000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.0.2) go to 23001
	             go to 23002
23001	call nucltransK(0.230,0.049,1.4e-1,0.,tclev,thlev,tdlev)
	return
23002	call nucltransK(0.115,0.049,1.5e-0,0.,tclev,thlev,tdlev)
	go to 11500
c-----------------------------------------------------------------------
11500	thlev=0.
	call nucltransK(0.115,0.049,1.5e-0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Gd146.f
c
c Local Variables: --
c mode: fortran --
c End: --
