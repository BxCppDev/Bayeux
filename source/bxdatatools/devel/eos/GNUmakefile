#
CPPFLAGS=-I. $(shell boost-config --cflags)
LDFLAGS=$(shell boost-config --ldflags serialization)

.PHONY :
check : p1.data
	@cp -f references/p1.data.md5 p1.data.md5
	@md5sum -c p1.data.md5 || echo "Unmatching MD5 sum !" && echo "MD5 sum is OK !"
	@rm -f p1.data.md5
	@diff p1.data references/p1.data || echo "There are differences !"  && echo "No difference !"

p1 : p1.cxx
	$(CXX) $< -o $@ $(CPPFLAGS) $(LDFLAGS)

p1.data : p1
	@./p1

p1.data.md5 : p1.data
	@md5sum p1.data > p1.data.md5

.PHONY :
reference : p1.data.md5 p1.data
	test -d references || mkdir references
	mv -f p1.data references/
	mv -f p1.data.md5 references/
	tree

.PHONY :
clean :
	$(RM) *~
	$(RM) p1
	$(RM) p1.data
	$(RM) p1.data.md5

# end