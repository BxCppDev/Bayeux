c     test_rnd.F
      program prog_rnd
      implicit none
      real*4 d, x
      integer i
      real*4 rnd1
      integer*4 seed, nshoots
      seed = 1
      !print * , '# Enter seed: '
      read * , seed
      !print * , '# Enter nshoots: '
      read * , nshoots
      call set_genbb_random_seed (seed)
      do i = 1, nshoots
         x = rnd1 (d)
1000     format (f8.4)
         print 1000 ,x
      enddo
      stop
      end
c     end of test_rnd.F
