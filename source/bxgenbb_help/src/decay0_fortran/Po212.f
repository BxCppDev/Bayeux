c Po212.f 
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
	subroutine Po212(tcnuc,tdnuc)
c Scheme of Po212 decay (Nucl. Data Sheets 66(1992)171).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 14.07.1995, 22.10.1995.
	thnuc=0.299e-6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	call alpha(8.785,0.,0.,t)
	return
	end
c
c end of Po212.f
c
c Local Variables: --
c mode: fortran --
c End: --
