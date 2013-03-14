c genbb_random.f 
c
c This file provides some routines for RANDOM NUMBER GENERATION.
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
      subroutine set_genbb_random_seed (seed_)
      implicit none
      integer*4 seed_, code
      integer*4 rng_shoot_reset
      !print *, 'genbbran.f: set_genbb_random_seed: invoke "rng_shoot_reset"'
      code = rng_shoot_reset (seed_)
      return
      end
C.
C.    ******************************************************************
C.    *                                                                *
C.    *       To generate a vector RVEC of LEN random numbers          *
C.    *         using the CERN Library routine RANLUX                  *
C.    *                                                                *
C.    *    ==>Called by : <USER>, many GEANT routines                  *
C.    *                                                                *
C.    ******************************************************************
C.
      subroutine grndm (rvec_, len_)
      implicit none
      real*4    rvec_(10)
      integer*4 len_, i
      real*4    rng_shoot_flat
      !print *, 'grndm: invoking rng_shoot_flat...'
      do i = 1, len_
         rvec_(i) = rng_shoot_flat ()
      end do         
      return
      end
c
c
c
	real*4 function rnd1(x)
c Function rnd1 gives random number from 0 to 1 by using the most 
c convenient or available to user random number generator (CERN, IMSL,
c internal FORTRAN, etc.). Please, choose what you want.
c VIT, 15.11.2006.
	real*4 r(10) ! fix length to 10 (was 1)
	call grndm(r,1)
	! print *, 'DEVEL: rnd1: r(1)=', r(1)
	rnd1=r(1)
	return
	end
c
c
c
      subroutine rdmin (irndmst_)
      implicit none
      integer*4 irndmst_
      ! nothing special
      return
      end
c
c
c
      subroutine rdmout (irndmst_)
      implicit none
      integer*4 irndmst_
      ! nothing special
      return
      end
c
c$$$c.c.
c$$$c.    ******************************************************************
c$$$c.    *                                                                *
c$$$c.    *       To set/retrieve the seed of the random number generator  *
c$$$C.    *                                                                *
c$$$C.    *  CHOPT  - A character specifying the action which              *
c$$$C.    *           GRNDMQ should take.                                  *
c$$$C.    *    'S'  - To initialize random number generator                *
c$$$C.    *    'Q'  - To initialize seed                                   *
c$$$C.    *    'G'  - To get status                                        *
c$$$C.    *                                                                *
c$$$C.    *    ==>Called by : <USER>, many GEANT routines                  *
c$$$C.    *       Author    V.I.Tretyak          *********                 *
c$$$C.    *                                                                *
c$$$C.    ******************************************************************
c$$$C.
c$$$      subroutine grndmq (is1, is2, iseq, chopt)
c$$$      common /gcrlux/ lux,intrlux,k1rlux,k2rlux
c$$$      integer         lux,intrlux,k1rlux,k2rlux
c$$$*--   User data cards 
c$$$*     ----------------------------------------------------------------------
c$$$*     Name  Type  N of var.    Description
c$$$*     ----------------------------------------------------------------------
c$$$*     RNDM   I       2       LUX INT parameters to initialize RANLUX generator
c$$$*                                    by call to RLUXGO(LUX,INT,K1,K2)
c$$$*                            LUX - luxority level, default value LUX=3
c$$$*                            INT - seed
c$$$*     RLUX   I       2       K1 K2   parameters to initialize RANLUX generator
c$$$*                                    by call to RLUXGO(LUX,INT,K1,K2)            
c$$$*                            
c$$$*.------------------------------------------------------------------------------
c$$$      integer is1,is2,iseq
c$$$      character*(*) chopt
c$$$      character*12  cchopt
c$$$      integer seed_from_time
c$$$
c$$$      print *,'genbbran.f: debug: grndmq'
c$$$      cchopt = chopt
c$$$
c$$$      if (index (chopt, 's') .ne. 0) then
c$$$c        print *,' iseq=',iseq
c$$$        if (iseq .eq. 1) then
c$$$	  lux = is1
c$$$	  intrlux = is2
c$$$        end if
c$$$c	print *,'lux=',lux,' intrlux =',intrlux,
c$$$c    &	' k1rlux=',k1rlux,' k2rlux=',k2rlux
c$$$        call rluxgo (lux, intrlux, k1rlux, k2rlux)
c$$$      end if
c$$$	
c$$$      if (index (chopt, 'q') .ne. 0) then
c$$$        if (iseq .le. 4 .and. iseq .gt. 0) then
c$$$	  lux = iseq
c$$$	else  
c$$$	  lux = 3
c$$$	endif
c$$$	
c$$$c	intrlux = seed_from_time()
c$$$c f.m. 29/1/2004
c$$$        !intrlux = time ()
c$$$c	print *,' q  lux=',lux,' intrlux =',intrlux,
c$$$c     &	' k1rlux=',k1rlux,' k2rlux=',k2rlux
c$$$             
c$$$      end if
c$$$
c$$$      if(index (chopt, 'g') .ne. 0) then
c$$$         call rluxat (lux, intrlux, k1rlux, k2rlux)
c$$$         is1 = k1rlux
c$$$         is2 = k2rlux
c$$$      end if
c$$$
c$$$      end
c
c end of genbb_random.f
