c     test_rnd2.F
      program prog_rnd
      implicit none
      real*4 d, x
      integer i
      real*4 rnd1
      real*4 vec(10)
      integer*4 seed, nshoots
      seed = 1
      !print * , '# Enter seed: '
      read * , seed
      !print * , '# Enter nshoots: '
      read * , nshoots
      call set_random_seed (seed)
      do i = 1, nshoots
         call grndm (vec, 3)
1000     format (3f8.4)
         print 1000 , vec(1), vec(2), vec(3)  
      enddo
      stop
      end
c     end of test_rnd2.F
