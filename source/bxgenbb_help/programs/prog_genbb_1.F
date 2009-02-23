c     prog_genbb_1.F
c     
c     
c     
c     
      program prog_genbb_1
      include 'genbb_help/genbb_commons.inc'
      common/genbbdbg/idebug

      integer*4 seed
      integer   idebug
      integer   i,ievent
      
      integer*4 ierror
      integer   i2bbs,ilevel,modebb,istart
      character chnuclide*16
      real*4    test_alea(3)
      real*4    rate
      
      idebug = 1                ! idebug==1 --> activates debug print
                                ! idebug==0 --> desactivates debug print
      i2bbs  = 1
      ilevel = 0
      modebb = 0
      istart = -1
      chnuclide = 'se82'

      tevst = 0.
      nevents = 100
      ievstart = 1
      !irndmst=12345
      irndmst = 0
      iwrfile = 0
      chfile = 'genbb.output'
      seed = 0
      
c     
c     step 0: initialize random engine
c     
      print * , 'prog_genbb_1: input: enter random seed='
      read * , seed
      if (seed .eq. 0) then
         seed = time ()
      endif
      print * , 'prog_genbb_1: info: random seed=', seed
      call set_random_seed (seed)

      if (idebug .eq. 1) then
         print * , 'prog_genbb_1: info: starting...'
      endif
      print * ,'prog_genbb_1: info: initialize ranlux:'
      do i = 1, 5
         call grndm (test_alea, 3)
         print * ,'prog_genbb_1: info: alea=', 
     +        test_alea (1), 
     +        test_alea (2), 
     +        test_alea (3)
      enddo
c     
c     step 1: dialog routine
c     
      if (idebug .eq. 1) then
         print * , 'prog_genbb_1: info: invoking genbbdia...'
      endif
      ierror = 0
      call genbbdia (i2bbs, chnuclide, ilevel, modebb, istart, ierror)
      if (ierror .ne. 0) then
         print * , 'prog_genbb_1: error: error in genbbdia'
         print * , 'prog_genbb_1: error: abort!'
         stop
      else
         print * , 'prog_genbb_1: info: genbbdia ok!'
      endif
      call genbbparprint ()
c     
c     step 2:
c     
      if (idebug .eq. 1) 
     +     print * , 'prog_genbb_1: info: invoking genbbopen...'
      if (iwrfile .ne. 0) then
         call genbbopen ()
         chfile='no file' ! trick
      endif
      if (idebug .eq. 1) 
     +     print * , 'prog_genbb_1: info: genbbopen ok!'

c     
c     step 3: checks and initializations
c     
      if (idebug .eq. 1) then
         print * , 'prog_genbb_1: info: invoking genbbsub for ' ,
     +        'initializations...'
      endif 
      istart = -1 
      call genbbsub (i2bbs, chnuclide, ilevel, modebb, istart, ierror)
      if (ierror .ne. 0) then
         print * , 'prog_genbb_1: error: error in genbbsub'
         print * , 'prog_genbb_1: error: abort!'
         stop
      else
         if (idebug .eq. 1) then
            print * , 'prog_genbb_1: info: genbbsub ok!'
         endif
      endif
c     
c     step 4: generate events
c     
      print * , 'prog_genbb_1: info: nevents=',nevents
      
      do ievent = 1, nevents
         if (idebug .eq. 1) then
            if ((ievent .eq. 1) 
     +           .or. (mod (ievent, 1000) .eq. 0)
     +           .or. (ievent .eq. nevents)) then
               rate = (ievent * 100.0) / nevents
               print 1000, ievent,nevents,rate
 1000          format ('event: ', i8, '/', i8, ' (', f6.2, '%)')
            endif
         endif
         ierror = 0
         istart = 1
         call genbbsub (i2bbs, chnuclide, ilevel, 
     +        modebb, istart, ierror)
         if (ierror.ne.0) then
            print *,'prog_genbb_1: error: generation failed',
     +           ' at event ',
     +           ievent 
            print *,'prog_genbb_1: error: abort!'
            stop
         else 
            if (iwrfile .ne. 0) call genbbsave ()              
            if (idebug .ge. 1) then
                                !call genbbeventprint()
                                !call genbbparprint()
            endif
         endif
      enddo
c     
      if (idebug .eq. 1) 
     +     print * , 'prog_genbb_1: info: invoking genbbclose...'
      if (iwrfile .ne. 0) call genbbclose ()
      if (idebug .eq. 1) 
     +     print * , 'prog_genbb_1: info: genbbclose ok!'
      if (idebug .eq. 1) print * , 'prog_genbb_1: info: end of run.'
      stop
      end
c     
c     
c     end of prog_genbb_1.f

