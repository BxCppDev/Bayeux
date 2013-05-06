c compton.f 
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
	subroutine compton(E1,E2,teta1,teta2,phi1,phi2)
c
c To sample the momentum of initial gamma quantum for COMPTON1 subroutine
c and store the momenta of scattered gamma and electron in common/genevent/.
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call compton(E1,E2,teta1,teta2,phi1,phi2)
c Input : E1,E2       - range of kinetic energy of gamma (MeV);
c         teta1,teta2 - range of teta angle (radians);
c         phi1,phi2   - range of phi  angle (radians);
c Output: see description of common/genevent/. Times emission of scattered
c         gamma quantum and electron are supposed to be 0.
c VIT, 11.12.1995.
c
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	common/const/pi,emass,datamass(50)
	dimension pg0(3),pg1(3),pe1(3)
	phi=phi1+(phi2-phi1)*rnd1(d)
	ctet1=1.
	if(teta1.ne.0.) ctet1=cos(teta1)
	ctet2=-1.
	if(teta2.ne.pi) ctet2=cos(teta2)
	ctet=ctet1+(ctet2-ctet1)*rnd1(d)
	stet=sqrt(1.-ctet*ctet)
	E=E1
	if(E1.ne.E2) E=E1+(E2-E1)*rnd1(d)
	p=E
	pg0(1)=p*stet*cos(phi)
	pg0(2)=p*stet*sin(phi)
	pg0(3)=p*ctet
	call compton1(pg0,pg1,pe1)
	npfull=npfull+1
	if(npfull.gt.100) print *,
     +     'COMPTON: in event more than 100 particles',npfull
	npgeant(npfull)=1
	pmoment(1,npfull)=pg1(1)
	pmoment(2,npfull)=pg1(2)
	pmoment(3,npfull)=pg1(3)
	ptime(npfull)=0.
	npfull=npfull+1
	if(npfull.gt.100) print *,
     +     'COMPTON: in event more than 100 particles',npfull
	npgeant(npfull)=3
	pmoment(1,npfull)=pe1(1)
	pmoment(2,npfull)=pe1(2)
	pmoment(3,npfull)=pe1(3)
	ptime(npfull)=0.
	return
	end
c
c end of compton.f
c
c Local Variables: --
c mode: fortran --
c End: --
