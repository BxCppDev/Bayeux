
CXXFLAGS=-fPIC
CPPFLAGS=$(shell boost-config --cflags)
LDFLAGS=$(shell boost-config --ldflags serialization filesystem )

all : prg_bs0 prg_bs1 prg_bs2 test_multi_txt_ar

prg_bs0: prg_bs0.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

prg_bs1 : prg_bs1.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

prg_bs2 : prg_bs2.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

test_multi_txt_ar : test_multi_txt_ar.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

clean :
	find . -name "*.~[[:digit:]{1,}]~" -exec $(RM) \{\} \;
	$(RM) test_multi_txt_ar test.txt test.xml
	$(RM) prg_bs0
	$(RM) prg_bs0.out.txt
	$(RM) prg_bs0.out.xml
	$(RM) prg_bs1
	$(RM) prg_bs1.out.txt
	$(RM) prg_bs1.out.xml
	$(RM) prg_bs2
	$(RM) prg_bs2.out.txt
	$(RM) prg_bs2.out.xml
	$(RM) demo demofile.txt

demo : demo.cpp	
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)
