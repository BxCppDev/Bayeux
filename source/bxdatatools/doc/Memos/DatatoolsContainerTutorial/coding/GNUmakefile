# GNUmakefile

CPPFLAGS=$(shell datatools-config --cflags)
LDFLAGS=$(shell datatools-config --ldflags)

.PHONY : all
all : properties_2 properties_3 properties_4 properties_5 \
  properties_6 \
  properties_7 \
  properties_8 \
  multi_properties_0 \
  multi_properties_1 \
  multi_properties_2 \
  things_0 \
  things_1 \
  things_2



.PHONY : clean
clean :
	$(RM) *~
	$(RM) properties_2
	$(RM) properties_3
	$(RM) properties_4 properties_4.conf
	$(RM) properties_5
	$(RM) properties_6
	$(RM) properties_*.log
	$(RM) properties_7
	$(RM) properties_8
	$(RM) multi_properties_0
	$(RM) multi_properties_1
	$(RM) multi_properties_2
	$(RM) multi_properties_3
	$(RM) multi_properties_4
	$(RM) multi_properties_*.log
	$(RM) things_0
	$(RM) things_1
	$(RM) things_2
	$(RM) things_*.log

.PHONY : run
run : run_2 run_3 run_4 run_5 run_6 run_7 run_8 \
      mrun_0 mrun_1 mrun_2 mrun_3 mrun_4 \
      trun_0 trun_1  trun_É

.PHONY : run_2
run_2 : properties_2
	./properties_2 > properties_2.log 2>&1 

.PHONY : run_3
run_3 : properties_3
	./properties_3 > properties_3.log 2>&1 

.PHONY : run_4
run_4 : properties_4
	./properties_4 > properties_4.log 2>&1 

.PHONY : run_5
run_5 : properties_5
	./properties_5 > properties_5.log 2>&1 

.PHONY : run_6
run_6 : properties_6
	./properties_6 > properties_6.log 2>&1 

.PHONY : run_7
run_7 : properties_7
	./properties_7 > properties_7.log 2>&1 

.PHONY : run_8
run_8 : properties_8
	./properties_8 > properties_8.log 2>&1 

.PHONY : mrun_0
mrun_0 : multi_properties_0
	./multi_properties_0 > multi_properties_0.log 2>&1 

.PHONY : mrun_1
mrun_1 : multi_properties_1
	./multi_properties_1 > multi_properties_1.log 2>&1 

.PHONY : mrun_2
mrun_2 : multi_properties_2
	./multi_properties_2 > multi_properties_2.log 2>&1 

.PHONY : mrun_3
mrun_3 : multi_properties_3
	./multi_properties_3 > multi_properties_3.log 2>&1 

.PHONY : mrun_4
mrun_4 : multi_properties_4
	./multi_properties_4 > multi_properties_4.log 2>&1 

.PHONY : trun_0
trun_0 : things_0
	./things_0 > things_0.log 2>&1 

.PHONY : trun_1
trun_1 : things_1
	./things_1 > things_1.log 2>&1 

.PHONY : trun_2
trun_2 : things_2
	./things_2 > things_2.log 2>&1 

properties_2 : properties_2.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

properties_3 : properties_3.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

properties_4 : properties_4.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

properties_5 : properties_5.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

properties_6 : properties_6.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

properties_7 : properties_7.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

properties_8 : properties_8.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

multi_properties_% : multi_properties_%.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

things_% : things_%.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end of GNUmakefile
