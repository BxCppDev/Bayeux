c tgold.f 
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
	subroutine tgold(a,b,f,eps,minmax,xextr,fextr)
c Subroutine tgold determines maximum or minimum of the function f(x) in
c the interval [a,b] by the gold section method.
c Call : 	external f
c		call tgold(a,b,f,eps,minmax,xextr,fextr)
c Input:	a,b    - beginning and the end of the interval [a,b];
c 		f      - name of the external function; should be as
c                        function f(x), and additional parameters have
c                        to be transmitted with help of common blocks;
c		eps    - desired uncertainty of xextr determination;
c		minmax - if minmax = 1 minimum of f(x) is searched,
c		                   = 2 maximum of f(x) is searched.
c Output:	xextr  - x-point of extremum;
c		fextr  - f(xextr).
c V.Tretyak, 25.09.1985.
	qc=0.61803395
	al=a
	bl=b
	xp=al+(bl-al)*qc
	xl=bl-(bl-al)*qc
	yp=f(xp)
	yl=f(xl)
3	goto (4,5) minmax
4	if(yp.lt.yl) goto 1
	goto 6
5	if(yp.gt.yl) goto 1
6	bl=xp
	xp=xl
	yp=yl
	xl=bl-(bl-al)*qc
	yl=f(xl)
	goto 2
1	al=xl
	xl=xp
	yl=yp
	xp=al+(bl-al)*qc
	yp=f(xp)
2	if((bl-al).gt.eps) goto 3
	xextr=0.5*(al+bl)
	fextr=f(xextr)
	return
	end
c
c end of tgold.f
c
c Local Variables: --
c mode: fortran --
c End: --
