c moller1.f 
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
	subroutine moller1(dcute,pe0,pe1,pe2)
c
c Simulates electron-electron Moller scattering.
c Call  : common/const/pi,emass
c         dimension pe0(3),pe1(3),pe2(3)
c         call moller1(dcute,pe0,pe1,pe2)
c Input : pe0   - momentum of initial electron (in MRS);
c         dcute - minimal energy of delta ray to be borned;
c                 if energy of initial electron < dcute/2
c                 delta rays are not produced;
c Output: pe1   - momentum of 1 scattered electron (in MRS);
c         pe2   - momentum of 2 scattered electron (in MRS).
c
c VIT, 07.12.1995 from GEANT subroutine GDRAY by D.Ward, L.Urban.
c
	dimension pe0(3),pe1(3),pe2(3)
	logical rotate
	common/const/pi,emass,datamass(50)
	p=sqrt(pe0(1)**2+pe0(2)**2+pe0(3)**2)
	xe=sqrt(p**2+emass**2)
	te=xe-emass
	gam=xe/emass
	gam2=gam*gam
	t=gam-1.
	x=dcute/(t*emass)
	if(x.ge.0.5) then
	   do i=1,3
	      pe2(i)=pe0(i)
	      pe1(i)=0.
	   enddo
	   return
	endif
	cc=1.-2.*x
1	e=x/(1.-cc*rnd1(d))
	b1=4./(9.*gam2-10.*gam+5.)
	b2=t*t*b1
	b3=(2.*gam2+2.*gam-1.)*b1
	e1=1.-e
	screj=b2*e*e-b3*e/e1+b1*gam2/(e1*e1)
	if(rnd1(d).gt.screj) go to 1
	eel=(t*e+1.)*emass
	tel=eel-emass
	pel=sqrt(abs((eel+emass)*tel))
	costh=(xe*eel+emass*(tel-xe))/(p*pel)
	if(costh.ge.1.) then
	   costh=1.
	   sinth=0.
	elseif(costh.le.-1.) then
	   costh=-1.
	   sinth=0.
	else
	   sinth=sqrt((1.+costh)*(1.-costh))
	endif
	phi=2.*pi*rnd1(d)
	cosphi=cos(phi)
	sinphi=sin(phi)
c Polar co-ordinates to momentum components
	pe2(1)=pel*sinth*cosphi
	pe2(2)=pel*sinth*sinphi
	pe2(3)=pel*costh
	pe1(1)=-pe2(1)
	pe1(2)=-pe2(2)
	pe1(3)=p-pe2(3)
c Rotate to MRS
	call gfang(pe0,costh,sinth,cosph,sinph,rotate)
	if(rotate) then
	   call gdrot(pe1,costh,sinth,cosph,sinph)
	   call gdrot(pe2,costh,sinth,cosph,sinph)
	endif
	return
	end
c
c end of moller1.f
c
c Local Variables: --
c mode: fortran --
c End: --
