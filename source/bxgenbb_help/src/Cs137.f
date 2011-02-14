c Cs137.f 
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
	subroutine Cs137(tcnuc,tdnuc)
c Model for scheme of Cs137 decay (Nucl. Data Sheets 72(1994)355)
c (really it is model for (Cs137 + Ba137m)-decay but not the model of
c Cs137 decay alone).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 31.08.1992, 11.07.1995, 06.11.1995;
c VIT, 30.10.2006 (update to NDS 81(1997)579 and corrections to the beta shapes).
	thnuc=0.9489110E+09
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.94.4) go to 1
	                   go to 2
c-----------------------------------------------------------------------
c1	call beta(0.514,56.,0.,0.,t)
c change to the 1st forbidden unique shape without experimental corrections
1	call beta_1fu(0.514,56.,0.,0.,t,0.,0.,0.,0.)
	thlev=153.12
	call nucltransKL(0.662,0.037,9.0e-2,0.006,1.6e-2,0.,
c    +                   tdlev,thlev,tdlev)
c corrected; thanks to F.Cappella 7.08.2007
     +                   tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
c2	call beta(1.176,56.,0.,0.,t)
c change to 2nd forbidden non-unique shape with experimental correction factor
c from S.T.Hsue et al., NP 86(1966)47: p_nu^2+0.004*p_el^2
c cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), w=e/emass+1
c2	call beta1(1.176,56.,0.,0.,t,0.,-0.8690502,0.1888145,0.)
c wrong numerical coefficients; corrected on 30.01.2009, VIT
2	call beta1(1.176,56.,0.,0.,t,0.,-0.6060315,0.0921520,0.)
	return
c-----------------------------------------------------------------------
	end
c
c end of Cs137.f
c
c Local Variables: --
c mode: fortran --
c End: --
