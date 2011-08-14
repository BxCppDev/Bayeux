c     test_beta.F
      program prog_beta
      !implicit none
      integer i
      integer npfull
      integer npgeant
      real*4 Qbeta
      real*4 Zdtr
      real*4 tcnuc
      real*4 thnuc
      real*4 tdnuc
      real*4 pmoment
      real*4 ptime
      real*4 me, p2, w2, e
      integer*4 seed, nshoots
      common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     + ptime(100)
      seed = 1
      !print * , '# Enter seed: '
      read * , seed
      !print * , '# Enter nshoots: '
      read * , nshoots
      call set_genbb_random_seed (seed)
      Qbeta = 3.270
      Zdtr  = 84.0
      tcnuc = 0.0
      thnuc = 1.0
      me    = 0.511
      do i = 1, nshoots
         npfull=0
         call beta (Qbeta, Zdtr, tcnuc, thnuc, tdnuc)
c         print * , 'npfull=', npfull
c         print * , '  npgeant=', npgeant(1)
c         print * , '  pmoment[1]=', 
c     +    pmoment(1,1), pmoment(2,1), pmoment(3,1)
c         print * , '  pmoment[2]=', 
c     +    pmoment(1,2), pmoment(2,2), pmoment(3,2)
c         print * , '  pmoment[2]=', 
c     +    pmoment(1,3), pmoment(2,3), pmoment(3,3)
         p2 = pmoment(1,1) * pmoment(1,1)
     +      + pmoment(2,1) * pmoment(2,1)
     +      + pmoment(3,1) * pmoment(3,1)
         w2 = p2 + me * me
         e = sqrt (w2) - me
1000     format (f8.4)
         print 1000 ,e
      enddo
      stop
      end
c     end of test_beta.F
