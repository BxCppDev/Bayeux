

.PHONY : all
all: tutorial.html

.PHONY : show
show: tutorial.html
	firefox file://$(shell pwd)/$< &

.PHONY : clean
clean: 
	find . -name "*~" -exec $(RM) \{\} \;
	cd code; make clean
	$(RM) tutorial.html

tutorial.html : tutorial.m4.html
	cd code; make
	m4 -I./code/ $< > $@

# end
