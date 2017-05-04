c Pa234m.f 
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
	subroutine Pa234m(tcnuc,tdnuc)
c Model (not the exact) scheme of Pa234m decay ("Table of Isotopes",
c 7th ed., 1978):
c decays of Pa234m to excited levels of U234 with energies
c greater than 1.045 MeV are not considered (p=0.20%).
c Three- and four-figured labels correspond to energies of
c U234 excited levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 18.08.1992, 22.10.1995;
c VIT, 3.11.2006 (update to NDS 71(1994)181 - however, levels with E_exc>1.045 
c MeV are still not considered; change of beta shape of the gs-to-gs beta decay 
c to the 1st forbidden with exp. corr.).
	thnuc=70.2
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
c-----------------------------------------------------------------------
c IT to Pa234
	if(pdecay.le.0.16) then
	   call nucltransK(0.074,0.021,1.1e+1,0.,tclev,thlev,tdlev)
	   return
	endif
c-----------------------------------------------------------------------
c beta decay to U234
	pbeta=100.*rnd1(d)
	if(pbeta.le.1.008) go to  1045   !  1.008%
	if(pbeta.le.1.012) go to   989   !  0.004%
	if(pbeta.le.1.022) go to   852   !  0.010%
	if(pbeta.le.1.712) go to   810   !  0.690%
	if(pbeta.le.1.744) go to   786   !  0.032%
	                   go to 10000   ! 98.256%
c-----------------------------------------------------------------------
1045	call beta(1.224,92.,0.,0.,t)
10450	thlev=0.
	p=100.*rnd1(d)
	if(p.le.83.7) go to 10451   ! 83.7%
	if(p.le.91.3) go to 10452   !  7.6%
	if(p.le.99.9) go to 10453   !  8.6%
	              go to 10454   !  0.1%
10451	call nucltransK(1.001,0.116,1.1e-2,0.,tclev,thlev,tdlev)
	go to 43000
10452	call nucltransK(0.258,0.116,5.5e-2,0.,tclev,thlev,tdlev)
	go to 78600
10453	call electron(0.120,tclev,thlev,tdlev)      ! only ec
	call gamma(0.116,0.,0.,tdlev)
	go to 81000
10454	call nucltransK(0.193,0.116,8.8e-1,0.,tclev,thlev,tdlev)
	go to 85200
c-----------------------------------------------------------------------
989	call beta(1.280,92.,0.,0.,t)
98900	thlev=0.
	p=100.*rnd1(d)
	if(p.le.44.8) go to 98901   ! 44.8%
	if(p.le.56.1) go to 98902   ! 11.3%
	if(p.le.92.3) go to 98903   ! 36.2%
	              go to 98904   !  7.7%
98901	call nucltransK(0.946,0.116,4.1e-3,0.,tclev,thlev,tdlev)
	go to 43000
98902	call nucltransK(0.203,0.116,1.5e0,0.,tclev,thlev,tdlev)
	go to 78600
98903	call nucltransK(0.140,0.116,5.5e0,0.,tclev,thlev,tdlev)
	go to 84900
98904	call nucltransK(0.063,0.022,4.3e-1,0.,tclev,thlev,tdlev)
	go to 92700
c-----------------------------------------------------------------------
92700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.40.4) go to 92701   ! 40.4%
	if(p.le.98.7) go to 92702   ! 58.3%
	              go to 92703   !  1.3%
92701	call nucltransK(0.927,0.116,1.3e-2,0.,tclev,thlev,tdlev)
	return
92702	call nucltransK(0.883,0.116,1.4e-2,0.,tclev,thlev,tdlev)
	go to 43000
92703	call nucltransK(0.783,0.116,1.8e-2,0.,tclev,thlev,tdlev)
	go to 14300
c-----------------------------------------------------------------------
852	call beta(1.417,92.,0.,0.,t)
85200	thlev=0.
	p=100.*rnd1(d)
	if(p.le.18.6) go to 85201   ! 18.6%
	if(p.le.64.6) go to 85202   ! 46.0%
	              go to 85203   ! 35.4%
85201	call nucltransK(0.852,0.116,1.5e-2,0.,tclev,thlev,tdlev)
	return
85202	call nucltransK(0.808,0.116,4.2e0,0.,tclev,thlev,tdlev)
	go to 43000
85203	call nucltransK(0.042,0.022,1.0e+1,0.,tclev,thlev,tdlev)
	go to 81000
c-----------------------------------------------------------------------
84900	thlev=0.
	p=100.*rnd1(d)
	if(p.le.51.8) go to 84901   ! 51.8%
	              go to 84902   ! 48.2%
84901	call nucltransK(0.806,0.116,5.5e-3,0.,tclev,thlev,tdlev)
	go to 43000
84902	call nucltransK(0.706,0.116,7.0e-3,0.,tclev,thlev,tdlev)
	go to 14300
c-----------------------------------------------------------------------
810	call beta(1.459,92.,0.,0.,t)
81000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.63.0) go to 81001   ! 63%
	              go to 81002   ! 37%
81001	call electron(0.694,tclev,thlev,tdlev)      ! only ec
	call gamma(0.116,0.,0.,tdlev)
	return
81002	call nucltransK(0.766,0.116,1.9e-2,0.,tclev,thlev,tdlev)
	go to 43000
c-----------------------------------------------------------------------
786	call beta(1.483,92.,0.,0.,t)
78600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.37.6) go to 78601   ! 37.6%
	              go to 78602   ! 62.4%
78601	call nucltransK(0.786,0.116,5.8e-3,0.,tclev,thlev,tdlev)
	return
78602	call nucltransK(0.743,0.116,6.4e-3,0.,tclev,thlev,tdlev)
	go to 43000
c-----------------------------------------------------------------------
14300	thlev=0.
	call nucltransK(0.100,0.022,1.4e+1,0.,tclev,thlev,tdlev)
	go to 43000
c-----------------------------------------------------------------------
43000	thlev=0.25e-9
	call nucltransK(0.043,0.022,7.2e+2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
c10000	call beta(2.269,92.,0.,0.,t)
c change to 1st forbidden shape with experimental shape factor from 
c A.G.Carles et al., NIMA 369(1996)431
c cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), w=e/emass+1.
10000	call beta1(2.269,92.,0.,0.,t,0.,-0.09,0.,0.)
	return
c-----------------------------------------------------------------------
	end
c
c end of Pa234m.f
c
c Local Variables: --
c mode: fortran --
c End: --
