c beta1.f 
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
	subroutine beta1(Qbeta,Zdtr,tcnuc,thnuc,tdnuc,c1,c2,c3,c4)
c Subroutine beta1 simulates the angles and energy of beta particles
c emitted in beta decay of nucleus. The decay is considered as forbidden;
c correction factor to the allowed spectrum shape has a form
c typical for empirical corrections:
c cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), w=e/emass+1.
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call beta1(Qbeta,Zdtr,tcnuc,thnuc,tdnuc,c1,c2,c3,c4)
c Input : Qbeta       - beta energy endpoint (MeV; Qbeta>50 eV);
c         Zdtr        - atomic number of daughter nucleus (Zdtr>0 for e- and
c                       Zdtr<0 for e+ particles);
c         tcnuc       - time of creation of nucleus (sec);
c         thnuc       - nucleus halflife (sec);
c         c1,c2,c3,c4 - coefficients in correction factor to the allowed spectrum
c                       shape cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), 
c                       where w=e/emass+1, e - kinetic energy of electron.                        .
c Output: tdnuc               - time of decay of nucleus (sec);
c         tevst               - time of event's start (sec);
c         npfull              - current number of particle in event;
c         npgeant(npfull)     - GEANT number for particle identification
c                               (2 for beta+ and 3 for beta- decay);
c         pmoment(1-3,npfull) - x,y,z components of particle momentum (MeV);
c         ptime(npfull)       - time shift from previous time to calculate
c                               when particle was emitted (sec).
c VIT, 30.07.1992; 15.10.1995; 31.03.2006.
	external funbeta1
	common/const/pi,emass,datamass(50)
	common/parbeta/z,q
	common/parbeta1/c1h,c2h,c3h,c4h
	z=Zdtr
	q=Qbeta
	c1h=c1
	c2h=c2
	c3h=c3
	c4h=c4
	call tgold(50.e-6,Qbeta,funbeta1,0.001*Qbeta,2,em,fm)
1	E=50.e-6+(Qbeta-50.e-6)*rnd1(d)
	fe=funbeta1(E)
	f=fm*rnd1(d)
	if(f.gt.fe) go to 1
	if(Zdtr.ge.0.) np=3
	if(Zdtr.lt.0.) np=2
c isotropical emission of beta particle is supposed
	call particle(np,E,E,0.,pi,0.,2.*pi,tcnuc,thnuc,tdnuc)
	return
	end
c
c end of beta1.f
c
c Local Variables: --
c mode: fortran --
c End: --
