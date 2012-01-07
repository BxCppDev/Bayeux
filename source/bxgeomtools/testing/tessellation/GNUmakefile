#

CXXOPT=
CPPFLAGS=-I.
LDFLAGS=-lm

.PHONY : all
all : tts


.PHONY :  run
run : tts
	./tts
	gnuplot tts.gp

.PHONY : clean
clean :
	$(RM) *~
	$(RM) tts

tts : tts.cxx
	$(CXX) $(CPPFLAGS) $(CXXOPT) $< -o $@ $(LDFLAGS)


# end
