c funbeta.f 
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
	function funbeta(E)
c Function to search the maximum of beta spectrum curve by tgold
c subroutine.
c VIT, 30.07.1992; 15.10.1995.
	common/const/pi,emass,datamass(50)
	common/parbeta/Zdtr,Qbeta
	funbeta=0.
	if(E.gt.0.) funbeta=sqrt(E*(E+2.*emass))*(E+emass)*(Qbeta-E)**2
     +                   *fermi(Zdtr,E)
	return
	end
c
c end of funbeta.f
c
c Local Variables: --
c mode: fortran --
c End: --
