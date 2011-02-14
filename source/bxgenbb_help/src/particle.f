c particle.f 
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
	subroutine particle(np,E1,E2,teta1,teta2,phi1,phi2,tclev,
     +                      thlev,tdlev)
c Generation of isotropical emission of particle in the range of
c energies and angles.
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call particle(np,E1,E2,teta1,teta2,phi1,phi2,tclev,thlev,tdlev)
c Input : np          - GEANT number for particle identification:
c                        1 - gamma         2 - positron     3 - electron
c                        4 - neutrino      5 - muon+        6 - muon-
c                        7 - pion0         8 - pion+        9 - pion-
c                       10 - kaon0 long   11 - kaon+       12 - kaon-
c                       13 - neutron      14 - proton      15 - antiproton
c                       16 - kaon0 short  17 - eta         18 - lambda
c                       19 - sigma+       20 - sigma0      21 - sigma-
c                       22 - xi0          23 - xi-         24 - omega
c                       25 - antineutron  26 - antilambda  27 - antisigma-
c                       28 - antisigma0   29 - antisigma+  30 - antixi0
c                       31 - antixi+      32 - antiomega+  45 - deuteron
c                       46 - tritium      47 - alpha       48 - geantino
c                       49 - He3          50 - Cerenkov;
c         E1,E2       - range of kinetic energy of particle (MeV);
c         teta1,teta2 - range of teta angle (radians);
c         phi1,phi2   - range of phi  angle (radians);
c         tclev       - time of creation of level from which particle will be
c                       emitted (sec);
c         thlev       - level halflife (sec).
c Output: tdlev               - time of decay of level (sec);
c         tevst               - time of event's start (sec);
c         npfull              - current number of particle in event;
c         npgeant(npfull)     - GEANT number for particle identification
c                               (=np of input);
c         pmoment(1-3,npfull) - x,y,z components of particle momentum (MeV);
c         ptime(npfull)       - time shift from previous time to calculate
c                               when particle was emitted (sec).
c VIT, 15.10.1995.
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	common/const/pi,emass,datamass(50)
	npfull=npfull+1
	if(npfull.gt.100) print *,
     +    'PARTICLE: in event more than 100 particles ',npfull
c	if(np.lt.1.or.np.gt.50.or.(np.gt.32.and.np.lt.45)) print *,
c    1     'PARTICLE: unknown particle number ',np
	npgeant(npfull)=np
	pmass=datamass(np)
	phi=phi1+(phi2-phi1)*rnd1(d)
	ctet1=1.
	if(teta1.ne.0.) ctet1=cos(teta1)
	ctet2=-1.
	if(teta2.ne.pi) ctet2=cos(teta2)
	ctet=ctet1+(ctet2-ctet1)*rnd1(d)
	stet=sqrt(1.-ctet*ctet)
	E=E1
	if(E1.ne.E2) E=E1+(E2-E1)*rnd1(d)
	p=sqrt(E*(E+2.*pmass))
	pmoment(1,npfull)=p*stet*cos(phi)
	pmoment(2,npfull)=p*stet*sin(phi)
	pmoment(3,npfull)=p*ctet
	tdlev=tclev
	if(thlev.gt.0.) tdlev=tclev-thlev/alog(2.)*alog(rnd1(d))
	ptime(npfull)=tdlev
	return
	end
c
c end of particle.f
c
c Local Variables: --
c mode: fortran --
c End: --
