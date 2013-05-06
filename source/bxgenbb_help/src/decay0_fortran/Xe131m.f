c Xe131m.f 
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
	subroutine Xe131m(tcnuc,tdnuc)
c Scheme of Xe129m decay (NDS 107(2006)2715 and ENSDF at NNDC site on 
c 13.11.2007).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 13.11.2007.
	thnuc=1.022976e+6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
c-----------------------------------------------------------------------
16400	thlev=0.
	call nucltransKLM(0.164,0.035,31.60,0.005,14.75,0.001,4.15,
     +                    0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Xe131m.f
c
c Local Variables: --
c mode: fortran --
c End: --
