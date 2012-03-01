c genbb_help trick:
	print 33                                           !**********
33	format('$to write generated events in file ? ')    ! comment
	read 3,chn                                         ! if in
	if(chn(1:1).eq.'Y'.or.chn(1:1).eq.'y') then        ! nemo*.car
	   iwrfile=1                                       !
	   print 34                                        !
34	   format('$name of file                      : ') !
	   read 3,chfile                                   !
	endif                                              !**********
c end of genbb_help trick.
