
        subroutine pair(Epair,tclev,thlev,tdlev)
	common/const/pi,emass,datamass(50)
	phi=2.*pi*rnd1(d)
	ctet=-1.+2.*rnd1(d)
	teta=acos(ctet)
	E=0.5*Epair
	call particle(2,E,E,teta,teta,phi,phi,tclev,thlev,tdlev)
	call particle(3,E,E,teta,teta,phi,phi,0.,0.,tdlev)
	return
	end
