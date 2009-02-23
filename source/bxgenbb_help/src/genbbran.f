      subroutine set_random_seed (seed_)
      implicit none
      integer*4 seed_, code
      integer*4 rng_shoot_reset
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
         rvec_ (i) = rng_shoot_flat ()
      end do         
      !!!call ranlux (rvec, len)
      !print *, 'genbbran.f: debug: grndm'
      !do i=1,len
      !   print *, 'rvec(',i,') = ', rvec(i)
      !end do         
      !print *, ''
      return
      end

c.c.
c.    ******************************************************************
c.    *                                                                *
c.    *       To set/retrieve the seed of the random number generator  *
C.    *                                                                *
C.    *  CHOPT  - A character specifying the action which              *
C.    *           GRNDMQ should take.                                  *
C.    *    'S'  - To initialize random number generator                *
C.    *    'Q'  - To initialize seed                                   *
C.    *    'G'  - To get status                                        *
C.    *                                                                *
C.    *    ==>Called by : <USER>, many GEANT routines                  *
C.    *       Author    V.I.Tretyak          *********                 *
C.    *                                                                *
C.    ******************************************************************
C.
      subroutine grndmq (is1, is2, iseq, chopt)
      common /gcrlux/ lux,intrlux,k1rlux,k2rlux
      integer         lux,intrlux,k1rlux,k2rlux
*--   User data cards 
*     ----------------------------------------------------------------------
*     Name  Type  N of var.    Description
*     ----------------------------------------------------------------------
*     RNDM   I       2       LUX INT parameters to initialize RANLUX generator
*                                    by call to RLUXGO(LUX,INT,K1,K2)
*                            LUX - luxority level, default value LUX=3
*                            INT - seed
*     RLUX   I       2       K1 K2   parameters to initialize RANLUX generator
*                                    by call to RLUXGO(LUX,INT,K1,K2)            
*                            
*.------------------------------------------------------------------------------
      integer is1,is2,iseq
      character*(*) chopt
      character*12  cchopt
      integer seed_from_time

      print *,'genbbran.f: debug: grndmq'
      cchopt = chopt

      if (index (chopt, 's') .ne. 0) then
c        print *,' iseq=',iseq
        if (iseq .eq. 1) then
	  lux = is1
	  intrlux = is2
        end if
c	print *,'lux=',lux,' intrlux =',intrlux,
c    &	' k1rlux=',k1rlux,' k2rlux=',k2rlux
        call rluxgo (lux, intrlux, k1rlux, k2rlux)
      end if
	
      if (index (chopt, 'q') .ne. 0) then
        if (iseq .le. 4 .and. iseq .gt. 0) then
	  lux = iseq
	else  
	  lux = 3
	endif
	
c	intrlux = seed_from_time()
c f.m. 29/1/2004
        !intrlux = time ()
c	print *,' q  lux=',lux,' intrlux =',intrlux,
c     &	' k1rlux=',k1rlux,' k2rlux=',k2rlux
             
      end if

      if(index (chopt, 'g') .ne. 0) then
         call rluxat (lux, intrlux, k1rlux, k2rlux)
         is1 = k1rlux
         is2 = k2rlux
      end if

      end

c end of genbbran.f
