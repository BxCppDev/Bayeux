

.PHONY : all
all: tutorial.html

.PHONY : show_install
show_install : install
	firefox file://$(shell pwd)/html/tutorial.html &

.PHONY : install
install : tutorial.html
	@./install.sh 

.PHONY : show
show: tutorial.html
	firefox file://$(shell pwd)/$< &

.PHONY : clean
clean: up
	find . -name "*~" -exec $(RM) \{\} \;
	cd code; make clean
	$(RM) .prepare
	$(RM) html.tar.gz
	$(RM) -fr html/

.PHONY : up
up : 
	$(RM) tutorial.html

prepare :
	cd code; make; make run; 
	touch .prepare

tutorial.html : tutorial.m4.html
	test -f .prepare || make prepare
	m4 -I./code/ --debug=V func.m4 $< > $@



# end
