c       -*- mode: fortran; -*-
c       FORTRAN include file
c       
c       
	character chfile*256
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +    ptime(100)
	common/genbbpar/nevents,ievstart,irndmst,iwrfile,chfile
	
c       end
