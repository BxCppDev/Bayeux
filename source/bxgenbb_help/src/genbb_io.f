c genbb_io.f
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
      subroutine genbbopen () 
      include 'genbb_help/genbb_commons.inc'
      common/slate/isl(40)
      integer*4 id , it , isl 

      if (iwrfile .eq. 0) chfile='no file'
      if (iwrfile .ne. 0) then
         !call datime (id, it)
         print * , 'genbbopen: info: open unit 77'
         open (unit=77, file=chfile, status='replace')
c         write (77, 8) 
c 8       format('# genbb generated file: ')
c         write (77,9) isl(1), isl(2), isl(3), isl(4), isl(5), isl(6)
c 9       format ('# date and hour         : ',
c     +        2x, i4, '.', i1, '.', i2, 4x, i2, ':', i2, ':', i2)
      endif
      return
      end
c
c
c
      subroutine genbbclose ()
      include 'genbb_help/genbb_commons.inc'
      if (iwrfile .ne. 0) return
c      write (77, 8)
c 8    format ('# end of genbb generated file.')
      close (77)
      return
      end
c
c
c
      subroutine genbbsave ()
      include 'genbb_help/genbb_commons.inc'
      common/currentev/icurrent
      integer j, icurrent
      if (iwrfile .ne. 0) then
         write (77,6) icurrent, tevst, npfull
 6       format (i8, g13.6, i4)
         do j = 1, npfull
            write (77, 7) npgeant (j), pmoment(1,j), pmoment(2,j),
     +           pmoment(3,j), ptime (j)
 7          format (i3, 3g14.6, g13.6)
         enddo
      endif
      return
      end
c
c end of genbb_io.f
