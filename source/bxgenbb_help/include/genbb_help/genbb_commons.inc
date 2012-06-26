c -*- mode: fortran; -*-
c genbb_commons.inc
c
c Copyright 2007-2011 F. Mauger
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
c FORTRAN include file       
c       
	character chfile*256,chdspin*4
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +    ptime(100)
	common/genbbpar/nevents,ievstart,irndmst,iwrfile,chfile
	common/enrange/ebb1,ebb2,toallevents,levelE,chdspin
c	
c end of genbb_commons.inc
