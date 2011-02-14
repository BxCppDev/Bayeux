c Y90.f 
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
	subroutine Y90(tcnuc,tdnuc)
c Scheme of Y90 decay ("Table of Isotopes", 7th ed., 1978).
c Accuracy in description of: decay branches       - 0.001%,
c                           : deexcitation process - 0.001%.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c Slight update to NDS 82(1997)379.
c VIT, 9.08.1993, 22.10.1995, 26.10.2006; 27.02.2007.
	thnuc=230400.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.0.0115) go to  1761
	                    go to 10000
c-----------------------------------------------------------------------
c1761	call beta(0.523,40.,0.,0.,t)
1761	call beta_1fu(0.519,40.,0.,0.,t,0.,0.,0.,0.)
17610	thlev=61.3e-9
	p=100.*rnd1(d)
c to reproduce 31.86e-6 branching ratio for e+e- pair, 
c R.G.Selwyn et al., Appl. Rad. Isot. 65(2007)318
	if(p.le.27.7) go to 17611
	              go to 17612
17611	call pair(0.739,tclev,thlev,tdlev)
	return
17612	call electron(1.743,tclev,thlev,tdlev)
	call gamma(0.018,0.,0.,tdlev)
	return
c-----------------------------------------------------------------------
c10000	call beta(2.284,40.,0.,0.,t)
c Change from the allowed shape to the 1st forbidden unique with empirical
c correction from: H.H.Hansen, Appl. Rad. Isot. 34(1983)1241
10000	call beta_1fu(2.280,40.,0.,0.,t,0.,-0.0078,0.,0.)
	return
c-----------------------------------------------------------------------
	end
c
c end of Y90.f
c
c Local Variables: --
c mode: fortran --
c End: --
