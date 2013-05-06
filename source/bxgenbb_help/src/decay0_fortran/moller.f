c moller.f 
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
	subroutine moller(E1,E2,teta1,teta2,phi1,phi2,dcute)
c
c To sample the momentum of initial electron for MOLLER1 subroutine and
c store the momenta of scattered electron and delta ray in common/genevent/.
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call moller(E1,E2,teta1,teta2,phi1,phi2,dcute)
c Input : E1,E2       - range of kinetic energy of gamma (MeV);
c         teta1,teta2 - range of teta angle (radians);
c         phi1,phi2   - range of phi  angle (radians);
c         dcute       - lower energy threshold for emitted delta ray (MeV);
c Output: see description of common/genevent/. Times emission of scattered
c         electron and delta ray are supposed to be 0.
c VIT, 11.12.1995.
c
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	common/const/pi,emass,datamass(50)
	dimension pe0(3),pe1(3),pe2(3)
	phi=phi1+(phi2-phi1)*rnd1(d)
	ctet1=1.
	if(teta1.ne.0.) ctet1=cos(teta1)
	ctet2=-1.
	if(teta2.ne.pi) ctet2=cos(teta2)
	ctet=ctet1+(ctet2-ctet1)*rnd1(d)
	stet=sqrt(1.-ctet*ctet)
	E=E1
	if(E1.ne.E2) E=E1+(E2-E1)*rnd1(d)
	p=sqrt(e*(e+2.*emass))
	pe0(1)=p*stet*cos(phi)
	pe0(2)=p*stet*sin(phi)
	pe0(3)=p*ctet
	call moller1(dcute,pe0,pe1,pe2)
	npfull=npfull+1
	if(npfull.gt.100) print *,
     +     'MOLLER: in event more than 100 particles',npfull
	npgeant(npfull)=3
	pmoment(1,npfull)=pe1(1)
	pmoment(2,npfull)=pe1(2)
	pmoment(3,npfull)=pe1(3)
	ptime(npfull)=0.
	npfull=npfull+1
	if(npfull.gt.100) print *,
     +     'MOLLER: in event more than 100 particles',npfull
	npgeant(npfull)=3
	pmoment(1,npfull)=pe2(1)
	pmoment(2,npfull)=pe2(2)
	pmoment(3,npfull)=pe2(3)
	ptime(npfull)=0.
	return
	end
c
c end of moller.f
c
c Local Variables: --
c mode: fortran --
c End: --
