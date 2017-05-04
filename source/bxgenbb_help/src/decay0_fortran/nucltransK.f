c nucltransK.f 
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
	subroutine nucltransK(Egamma,Ebinde,conve,convp,tclev,
     +                        thlev,tdlev)
c Subroutine nucltransK choise one of the three concurent processes
c by which the transition from one nuclear state to another is
c occured: gamma-ray emission, internal conversion and internal
c pair creation. Conversion electrons are emitted only with one fixed energy
c (usually with Egamma-E(K)_binding_energy).
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call nucltransK(Egamma,Ebinde,conve,convp,tclev,thlev,tdlev)
c Input : Egamma - gamma-ray energy (MeV) [=difference in state energies];
c         Ebinde - binding energy of electron (MeV);
c         conve  - internal electron conversion coefficient [=Nelectron/Ngamma];
c         convp  - pair conversion coefficient [=Npair/Ngamma];
c         tclev  - time of creation of level from which particle will be
c                  emitted (sec);
c         thlev  - level halflife (sec).
c Output: tdlev               - time of decay of level (sec);
c         tevst               - time of event's start (sec);
c         npfull              - current number of last particle in event;
c         npgeant(npfull)     - GEANT number for particle identification
c                               (1 for gamma, 2 for e+ and 3 for e-);
c         pmoment(1-3,npfull) - x,y,z components of particle momentum (MeV);
c         ptime(npfull)       - time shift from previous time to calculate
c                               when particle was emitted (sec).
c VIT, 27.07.1992; 15.10.1995.
	common/const/pi,emass,datamass(50)
	p=(1.+conve+convp)*rnd1(d)
	if(p.le.1.) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	else if(p.le.1.+conve) then
	   call electron(Egamma-Ebinde,tclev,thlev,tdlev)
c	   call gamma(Ebinde,tdlev,0.,tdlev)                ! for real time
	   call gamma(Ebinde,0.,0.,tdlev)                   ! for time shift
	else
	   call pair(Egamma-2.*emass,tclev,thlev,tdlev)
	end if
	return
	end
c
c end of nucltransK.f
c
c Local Variables: --
c mode: fortran --
c End: --
