#

.PHONY : all
all : images


.PHONY : clean
clean : 
	$(RM) *~
	$(RM) *.fig.bak
	$(RM) *.png

fig_files := $(notdir $(shell find . -name "*.fig" -exec basename \{\} \;))
png_files := $(fig_files:%.fig=%.png)

.PHONY : images
images : $(png_files)

%.png : %.fig
	@#ls -1 $<
	@fig2dev -L png $< $@

.PHONY : test
test :
	@echo "Fig files : $(fig_files)"
	@echo "PNG files : $(png_files)"


# end