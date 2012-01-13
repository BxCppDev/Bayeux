c gamma.f 
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
	subroutine gamma(E,tclev,thlev,tdlev)
c Generation of emission of gamma quantum with fixed energy
c isotropically in the whole space.
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call gamma(E,tclev,thlev,tdlev)
c Input : E     - kinetic energy of particle (MeV);
c         tclev - time of creation of level from which particle will be
c                 emitted (sec);
c         thlev - level halflife (sec).
c Output: tdlev               - time of decay of level (sec);
c         tevst               - time of event's start (sec);
c         npfull              - current number of particle in event;
c         npgeant(npfull)     - GEANT number for particle identification (=1);
c         pmoment(1-3,npfull) - x,y,z components of particle momentum (MeV);
c         ptime(npfull)       - time shift from previous time to calculate
c                               when particle was emitted (sec).
c VIT, 15.10.1995.
	common/const/pi,emass,datamass(50)
	call particle(1,E,E,0.,pi,0.,2.*pi,tclev,thlev,tdlev)
	return
	end
c
c end of gamma.f
c
c Local Variables: --
c mode: fortran --
c End: --
