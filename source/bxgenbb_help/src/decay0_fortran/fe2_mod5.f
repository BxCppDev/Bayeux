c fe2_mod5.f 
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
	function fe2_mod5(e2)
c probability distribution for energy of second e-/e+ for modebb=5
c (0nu2b with Gelmini-Roncadelli Majoron (spectral index = 1),
c 0+ -> 0+, 2n mechanism)
	common/helpbb/Zdbb,Adbb,e0,e1
	common/const/pi,emass,datamass(50)
	fe2_mod5=0.
	if(e2.gt.e0-e1) return
	p2=sqrt(e2*(e2+2.*emass))
	fe2_mod5=(e2+emass)*p2*fermi(Zdbb,e2)*(e0-e1-e2)
	return
	end
c
c end of fe2_mod5.f
c
c Local Variables: --
c mode: fortran --
c End: --
