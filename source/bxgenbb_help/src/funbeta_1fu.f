c funbeta_1fu.f 
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
	function funbeta_1fu(E)
c Function to search the maximum of beta spectrum curve by tgold
c subroutine.
c VIT, 26.10.2006.
	common/const/pi,emass,datamass(50)
	common/parbeta/Zdtr,Qbeta
	common/parbeta1/c1,c2,c3,c4
	common/bj69plog/plog69(48)
	common/bj69sl2/sl2(48)
	funbeta_1fu=0.
	if(E.gt.0.) then
c allowed spectrum
	   all=sqrt(E*(E+2.*emass))*(E+emass)*(Qbeta-E)**2*fermi(Zdtr,E)
c correction factor 1 (theoretical)
	   w=E/emass+1.
	   pel=sqrt(w**2-1.)
	   pnu=(Qbeta-E)/emass
c calculation of the screened lambda2 value by interpolation of the table 
c with the help of the divdif CERN function for logarithms of p
	   pellog=alog(pel) 
	   scrl2=divdif(sl2,plog69,48,pellog,2)
	   cf1=pnu**2+scrl2*pel**2
c correction factor 2 (empirical)
	   cf2=1.+c1/w+c2*w+c3*w**2+c4*w**3
c spectrum with correction
	   funbeta_1fu=all*cf1*cf2
	endif
	return
	end
c
c end of funbeta_1fu.f
c
c Local Variables: --
c mode: fortran --
c End: --
