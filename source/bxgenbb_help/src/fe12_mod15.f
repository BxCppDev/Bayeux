c fe12_mod15.f 
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
	function fe12_mod15(e2)
c two-dimensional probability distribution for energies of e-/e+ for
c modebb=15 (as tsimpr needs)
c (2nu2b, 0+ -> 0+, bosonic neutrinos, HSD: A.S.Barabash et al., NPB 783(2007)90,
c eq. (27a) integrated in Enu1)
	common/helpbb/Zdbb,Adbb,e0,e1
	common/const/pi,emass,datamass(50)
	fe12_mod15=0.
	if(e2.gt.e0-e1) return
	p1=sqrt(e1*(e1+2.*emass))
	p2=sqrt(e2*(e2+2.*emass))
	fe12_mod15=(e1+emass)*p1*fermi(Zdbb,e1)*
     +             (e2+emass)*p2*fermi(Zdbb,e2)*
     +             (e0-e1-e2)**5*(9.*(e0-e1-e2)**2+21.*(e2-e1)**2)
	return
	end
c
c end of fe12_mod15.f
c
c Local Variables: --
c mode: fortran --
c End: --
