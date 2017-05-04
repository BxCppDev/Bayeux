c Y88.f 
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
	subroutine Y88(tcnuc,tdnuc)
c Scheme of Y88 decay ("Table of Isotopes", 7th ed., 1978).
c Accuracy in description of: decay branches       - 0.001%,
c                           : deexcitation process - 0.001%.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 20.07.1993, 22.10.1995.
c VIT, 12.11.2006 (update to NDS 105(2005)419 and change of beta+ spectrum 
c shape)
	thnuc=9.2124864e+6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.99.79) go to 1
	                    go to 2
c-----------------------------------------------------------------------
c EC to Sr88
1	call gamma(0.016,0.,0.,t)
	pec=pdecay
	if(pec.le. 0.065) go to 35850
	if(pec.le. 0.093) go to 32190
	if(pec.le.94.490) go to 27340
	                  go to 18360
c-----------------------------------------------------------------------
c b+ to Sr88
c2	call beta(0.765,-38.,0.,0.,t)
c change to the approximation to the 1st forbidden unique shape 
2	call beta2(0.765,-38.,0.,0.,t,1,1.,0.,0.,0.)
	go to 18360
c-----------------------------------------------------------------------
35850	thlev=0.14e-9
	call nucltransK(0.851,0.016,8.5e-4,0.,tclev,thlev,tdlev)
	go to 27340
c-----------------------------------------------------------------------
32190	thlev=0.13e-12
	p=100.*rnd1(d)
	if(p.le.25.) go to 32191
	             go to 32192
32191	call nucltransK(3.219,0.016,6.0e-5,8.7e-4,tclev,thlev,tdlev)
	return
32192	call nucltransK(1.382,0.016,2.6e-4,4.8e-5,tclev,thlev,tdlev)
	go to 18360
c-----------------------------------------------------------------------
27340	thlev=0.78e-12
	p=100.*rnd1(d)
	if(p.le.0.75) go to 27341
	              go to 27342
27341	call nucltransK(2.734,0.016,1.2e-4,3.3e-4,tclev,thlev,tdlev)
	return
27342	call nucltransK(0.898,0.016,3.1e-4,0.,tclev,thlev,tdlev)
	go to 18360
c-----------------------------------------------------------------------
18360	thlev=0.148e-12
	call nucltransK(1.836,0.016,1.4e-4,2.3e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Y88.f
c
c Local Variables: --
c mode: fortran --
c End: --
