c funbeta2.f 
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
	function funbeta2(E)
c Function to search the maximum of beta spectrum curve by tgold
c subroutine.
c VIT, 30.07.1992; 15.10.1995; 31.03.2006.
	common/const/pi,emass,datamass(50)
	common/parbeta/Zdtr,Qbeta
	common/parbeta2/kf,c1,c2,c3,c4
	funbeta2=0.
	if(E.gt.0.) then
c allowed spectrum
	   all=sqrt(E*(E+2.*emass))*(E+emass)*(Qbeta-E)**2*fermi(Zdtr,E)
c correction factor
	   w=E/emass+1.
	   pel=sqrt(w**2-1.)
	   pnu=(Qbeta-E)/emass
	   cf=1.
	   if(kf.eq.1) cf=pel**2+c1*pnu**2
	   if(kf.eq.2) cf=pel**4+c1*pel**2*pnu**2+c2*pnu**4
	   if(kf.eq.3) cf=pel**6+c1*pel**4*pnu**2+c2*pel**2*pnu**4+
     +                  c3*pnu**6
	   if(kf.eq.4) cf=pel**8+c1*pel**6*pnu**2+c2*pel**4*pnu**4+
     +                  c3*pel**2*pnu**6+c4*pnu**8
c spectrum with correction
	   funbeta2=all*cf
	endif
	return
	end
c
c end of funbeta2.f
c
c Local Variables: --
c mode: fortran --
c End: --
