c fe1_mod7.f 
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
	function fe1_mod7(e1)
c probability distribution for energy of first e-/e+ for modebb=7
c (0nu2b with rhc, 0+ -> 2+, 2n mechanism)
	common/helpbb/Zdbb,Adbb,e0,xxx
	common/const/pi,emass,datamass(50)
	fe1_mod7=0.
	if(e1.gt.e0) return
	e2=e0-e1
	p1=sqrt(e1*(e1+2.*emass))
	p2=sqrt(e2*(e2+2.*emass))
	fe1_mod7=p1*fermi(Zdbb,e1)*p2*fermi(Zdbb,e2)*
     +           ((e1+emass)*(e2+emass)+emass**2)*(p1**2+p2**2)
	return
	end
c
c end of fe1_mod7.f
c
c Local Variables: --
c mode: fortran --
c End: --
