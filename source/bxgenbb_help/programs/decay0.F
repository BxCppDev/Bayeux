c decay0.f 
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
c
	program DECAY0

c Last updated: 23.01.2011.

c Brief instruction on how to use DECAY0 as a standalone program:
c 1. Compile with Fortran compiler;
c 2. OBSOLETE: 
c    Link with CERNLIB library (for the use of random numbers and gauss
c    function);
c 3. Run DECAY0 and answer the questions; in particular, select an option to
c    write generated events in file. After, you can read these events
c    and simulate them with GEANT, EGS, MCNP or other simulation package.
c
c Please refer to: O.A.Ponkratenko, V.I.Tretyak, Yu.G.Zdesenko,
c                  "Event Generator DECAY4 for Simulating Double-Beta 
c                  Processes and Decays of Radioactive Nuclei",
c                  Phys. At. Nucl. 63 (2000) 1282 (nucl-ex/0104018).
c This paper describes mainly DECAY4 generator, and DECAY0 was mentioned
c as initial version of the code.
c Please send questions, comments and proposals to: 
c tretyak@kinr.kiev.ua or tretyak@lngs.infn.it.

c***********************************************************************
c   Generation of events of decay of natural radioactive isotopes and
c                various modes of double beta decay.
c***********************************************************************

c***********************************************************************
c DECAY units: energy and moment - MeV and MeV/c;
c              time              - sec.
c***********************************************************************

	character chfile*256,chnuclide*16,chart*4,chdspin*4
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	common/genbbpar/nevents,ievstart,irndmst,iwrfile,chfile
	common/enrange/ebb1,ebb2,toallevents,levelE,chdspin
	common/artificial/nartparts,    chart(10),
     +                    artemin(10),  artemax(10),
     +                    arttmin(10),  arttmax(10),
     +                    artfmin(10),  artfmax(10),
     +                    artQb(10),    artZd(10),
     +                    arttdelay(10),artthalf(10),
     +                    nartnpg(10)
	common/currentev/icurrent
	call GENBBdia(i2bbs,chnuclide,ilevel,modebb,istart)
	do i=1,nevents
	   icurrent=i
	   call GENBBsub(i2bbs,chnuclide,ilevel,modebb,istart,ier)
	enddo
	end
c
c end of decay0.f
c
c Local Variables: --
c mode: fortran --
c End: --
