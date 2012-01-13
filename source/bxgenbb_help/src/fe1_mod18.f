c fe1_mod18.f 
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
	function fe1_mod18(e1)
c probability distribution for energy of the first e-/e+ for modebb=18
c (0nu2b with right-handed currents, eta term, 0+ -> 0+, 2n mechanism)
	common/helpbb/Zdbb,Adbb,e0,xxx
	common/const/pi,emass,datamass(50)
	common/eta_nme/chi_GTw,chi_Fw,chip_GT,chip_F,chip_T,
     +               chip_P,chip_R
	fe1_mod18=0.
	if(e1.gt.e0) return
	e2=e0-e1
	p1=sqrt(e1*(e1+2.*emass))
	p2=sqrt(e2*(e2+2.*emass))
c total energies in the units of electron mass
	et0=e0/emass+1.
	et1=e1/emass+1.
	et2=e2/emass+1.
	a1=(et1*et2-1.)*(et1-et2)**2/(2.*et1*et2)
	a2=-2.*(et1-et2)**2/(9.*et1*et2)
	a3=2.*(et1*et2-1.)/(81.*et1*et2)
	r=3.107526e-3*Adbb**(1./3.)
	rksi=3./137.036*Zdbb+r*et0
	a4=8.*(et1*et2+1.)/(r**2*et1*et2)
	a5=-8.*(rksi*(et1*et2+1.)-2.*r*et0)/(3.*r**2*et1*et2)
	a6=2.*((rksi**2+4.*r**2)*(et1*et2+1.)-4.*rksi*r*et0)/
     +     (9.*r**2*et1*et2)
	chi_1plus =chip_GT+3.*chip_F-6.*chip_T
	chi_1minus=chip_GT-3.*chip_F-6.*chip_T
	chi_2plus =chi_GTw+chi_Fw-chi_1minus/9.
	a_eta=a1*chi_2plus**2+a2*chi_2plus*chi_1minus+a3*chi_1minus**2
     +     +a4*chip_R**2+a5*chip_R*chip_P+a6*chip_P**2
c	print *,'fe1_mod18:'
c	print *,'chi_GTw,chi_Fw,chip_GT,chip_F,chip_T,chip_P,chip_R'
c	print *,chi_GTw,chi_Fw,chip_GT,chip_F,chip_T,chip_P,chip_R
c	print *,'e1,r,rksi,a_eta=',e1,r,rksi,a_eta
c	if(e1.eq.e2) pause
	fe1_mod18=(e1+emass)*p1*fermi(Zdbb,e1)*
     +          (e2+emass)*p2*fermi(Zdbb,e2)*a_eta
	return
	end
c
c end of fe1_mod18.f
c
c Local Variables: --
c mode: fortran --
c End: --
