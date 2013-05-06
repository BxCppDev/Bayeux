c Na22.f 
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
	subroutine Na22(tcnuc,tdnuc)
c Scheme of Na22 decay ("Table of Isotopes", 7th ed., 1978).
c Accuracy in description of: decay branches       - 0.001%,
c                           : deexcitation process - 0.001%.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 26.07.1993, 22.10.1995.
c VIT, 12.11.2006 (updated to NDS 106(2005)1 and change to beta spectra with
c experimental corrections).
c VIT, 26.08.2007 (corrected beta shapes).
	thnuc=8.2132717e+7
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.99.944) go to  1275
	                     go to 10000
c-----------------------------------------------------------------------
1275	thlev=3.63e-12
	if(pdecay.le.9.618) call gamma(0.001,0.,0.,t)
c	if(pdecay.gt.9.618) call beta(0.545,-10.,0.,0.,t)
c experimental correction to the shape of the beta spectrum from:
c H.Daniel, RMP 40(1968)659 and H.Wenninger et al., NPA 109(1968)561
	if(pdecay.gt.9.618) 
     +     call beta1(0.545,-10.,0.,0.,t,1.e-3,0.,0.,0.)
	call nucltransK(1.275,0.001,6.8e-6,2.1e-5,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
c10000	call beta(1.820,-10.,0.,0.,t)
c change to 2nd unique shape in accordance with H.Daniel, RMP 40(1968)659
10000	call beta2(1.820,-10.,0.,0.,t,2,3.333333,1.,0.,0.)
	return
c-----------------------------------------------------------------------
	end
c
c end of Na22.f
c
c Local Variables: --
c mode: fortran --
c End: --
