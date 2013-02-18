# -*- mode: make; -*-
#
# do_cadfael_pro_setup
# do_bayeux_legacy_dev_setup
#
DPP_CFLAGS=-I../../__build-Linux-x86_64/include/ -I../../include
DPP_LDFLAGS=-L../../__build-Linux-x86_64/src/ -ldpp
CPPFLAGS=$(DPP_CFLAGS) $(shell datatools-config --cflags)
LDFLAGS=$(DPP_LDFLAGS) $(shell datatools-config --ldflags)

.PHONY : all
all : gen_sample_data

.PHONY : clean
clean :
	$(RM) *~
	$(RM) gen_sample_data

.PHONY : clean_data
clean_data :
	$(RM) data_0.xml*
	$(RM) data_0.brio
	$(RM) data_0.data*
	$(RM) data_0.txt*

gen_sample_data : gen_sample_data.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

# end
