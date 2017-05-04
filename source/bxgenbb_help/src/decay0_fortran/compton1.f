c compton1.f 
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
	subroutine compton1(pg0,pg1,pe1)
c
c Simulates photon-electron Compton scattering.
c Call  : common/const/pi,emass
c         dimension pg0(3),pg1(3),pe1(3)
c         call compton1(pg0,pg1,pe1)
c Input : pg0 - momentum of initial gamma quantum (in MRS);
c Output: pg1 - momentum of scattered gamma quantum (in MRS);
c         pe1 - momentum of recoil electron (in MRS).
c
c VIT, 07.12.1995 from GEANT subroutine GCOMP by G.Patrick, L.Urban.
c
	dimension pg0(3),pg1(3),pe1(3)
	logical rotate
	common/const/pi,emass,datamass(50)
	egam0=sqrt(pg0(1)**2+pg0(2)**2+pg0(3)**2)
	ezero=egam0/emass
	emini=1.+2.*ezero
	emin=1./emini
	dsig1=log(emini)
	dsig2=0.5*(1.-emin*emin)
	dsigt=dsig1+dsig2
c Decide which part of f(e)=1/e+e to sample from
1	if(dsig1.lt.dsigt*rnd1(d)) then
c Sample from f2(e) distribution
	   brd=rnd1(d)
	   if(ezero.ge.(ezero+1.)*rnd1(d)) then
	      brd=max(brd,rnd1(d))
	   endif
	   br=emin+(1.-emin)*brd
	else
	   br=emin*exp(dsig1*rnd1(d))
	endif
c Scattered photon energy
	egam1=br*egam0
c Calculate rejection function g(e)
	t=emass*(1.-br)/egam1
	sinth=max(0.,t*(2.-t))
	rej=1.0-(br*sinth)/(1.+br*br)
	if(rnd1(d).gt.rej) go to 1
c Generate photon angles with respect to a Z-axis defined
c along the parent photon; phi is generated isotropically
	sinth=sqrt(sinth)
	costh=1.-t
	phi=2.*pi*rnd1(d)
	cosphi=cos(phi)
	sinphi=sin(phi)
c Polar co-ordinates to momentum components
	pg1(1)=egam1*sinth*cosphi
	pg1(2)=egam1*sinth*sinphi
	pg1(3)=egam1*costh
c Momentum vector of recoil electron
	pe1(1)=-pg1(1)
	pe1(2)=-pg1(2)
	pe1(3)=egam0-pg1(3)
c Rotate electron and scattered photon into MRS system
	call gfang(pg0,costh,sinth,cosph,sinph,rotate)
	if(rotate) then
	   call gdrot(pg1,costh,sinth,cosph,sinph)
	   call gdrot(pe1,costh,sinth,cosph,sinph)
	endif
	return
	end
c
c end of compton1.f
c
c Local Variables: --
c mode: fortran --
c End: --
