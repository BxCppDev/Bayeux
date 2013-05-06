c Bi208.f 
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
	subroutine Bi208(tcnuc,tdnuc)
c Scheme of Bi208 decay (NDS 47(1986)797 + ToI-1978).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 17.12.1995; 10.05.2005.
	thnuc=1.161288E+13
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.43.6)                    Egamma=0.088 ! EC-K 43.6%
	if(pdecay.gt.43.6.and.pdecay.le.83.8) Egamma=0.016 ! EC-L 40.2%
	if(pdecay.gt.83.8)                    Egamma=0.004 ! EC-M 16.2%
	call gamma(Egamma,0.,0.,t)
c-----------------------------------------------------------------------
2615	thlev=32.e-12
	call nucltransK(2.615,0.088,8.5e-3,4.3e-4,tclev,thlev,tdlev)
c-----------------------------------------------------------------------
	return
	end
c
c end of Bi208.f
c
c Local Variables: --
c mode: fortran --
c End: --
