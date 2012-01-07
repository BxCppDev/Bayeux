#

CXXOPT=
CPPFLAGS=-I. $(shell geomtools-config --cflags)
LDFLAGS=$(shell geomtools-config --ldflags)

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
