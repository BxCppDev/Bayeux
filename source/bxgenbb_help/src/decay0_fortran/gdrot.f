c gdrot.f 
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
	subroutine gdrot(p,costh,sinth,cosph,sinph)
c
c Rotates vector from one reference system into another.
c Theta and phi are anti-clockwise Eulerian angles between the
c two systems.
c
c M.Hansroul, G.Patrick (GEANT).
c
	dimension p(3)
	p1=p(1)
	p2=p(2)
	p3=p(3)
	p(1)= p1*costh*cosph - p2*sinph + p3*sinth*cosph
	p(2)= p1*costh*sinph + p2*cosph + p3*sinth*sinph
	p(3)=-p1*sinth                  + p3*costh
	return
	end
c
c end of gdrot.f
c
c Local Variables: --
c mode: fortran --
c End: --
