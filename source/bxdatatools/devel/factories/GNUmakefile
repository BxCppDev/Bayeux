
CPPFLAGS=$(shell boost-config --cflags)
LDFLAGS=$(shell boost-config --ldflags)

.PHONY : all
all : p1 p2 p3

.PHONY : run
run : run1 run2 run3

.PHONY : run1
run1 : p1
	@echo "Running p1..."; ./p1

.PHONY : run2
run2 : p2
	@echo "Running p2..."; ./p2

.PHONY : run3
run3 : p3
	@echo "Running p3..."; ./p3


.PHONY : clean
clean : 
	$(RM) *~
	$(RM) p1
	$(RM) p2
	$(RM) p3

p% : p%.cxx
	$(CXX) $(CPPFLAGS) $< -o $@ -o $@

# end