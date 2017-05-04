# GNUmakefile
# -*- mode: make; -*-

SYSTEM=$(shell uname -s)
ifneq (${SYSTEM},Linux)
(error Please use only on Linux)
endif

THE_BASE_DIR=.
XFIG_DIR=$(THE_BASE_DIR)/fig
IMG_DIR=$(THE_BASE_DIR)/images
PNG_DIR=$(IMG_DIR)
TEX_DIR=$(THE_BASE_DIR)
PDFTEX_DIR=$(THE_BASE_DIR)/pdftex
PDFTEX_T_DIR=$(THE_BASE_DIR)/pdftex_t

vpath %.fig      $(XFIG_DIR)
vpath %.png      $(PNG_DIR)
vpath %.tex      $(TEX_DIR)
vpath %.pdf      $(PDFTEX_DIR)
vpath %.pdftex_t $(PDFTEX_T_DIR)

# Automaticaly build the list of Fig files to be processed
# by fig2dev : 
#fig_files           := $(notdir $(shell ls $(XFIG_DIR)/*.fig))  
#fig2pdfsrc_files    := $(notdir $(shell ls $(XFIG_DIR)/*.fig))  
#fig2pdftex_files    := $(fig2pdfsrc_files:%.fig=%.pdf)
#fig2pdftex_t_files  := $(fig2pdfsrc_files:%.fig=%.pdftex_t)

.PHONY : all
all : pdf

LATEX_MAIN_SRC_FILE=DatatoolsSerializationTutorial

.PHONY : uninstall
uninstall : 
	rm -f ../${LATEX_MAIN_SRC_FILE}.pdf 

.PHONY : install
install : pdf
	cp -f ${LATEX_MAIN_SRC_FILE}.pdf ../

.PHONY : clean
clean : #clean_pdftex clean_pdftex_t
	$(RM) *~
	$(RM) ${XFIG_DIR}/*.fig.bak
	$(RM) $(LATEX_MAIN_SRC_FILE).pdf
	$(RM) $(LATEX_MAIN_SRC_FILE).log
	$(RM) $(LATEX_MAIN_SRC_FILE).aux
	$(RM) $(LATEX_MAIN_SRC_FILE).out
	#rmdir $(PDFTEX_DIR)
	#rmdir $(PDFTEX_T_DIR)

.PHONY : clean_pdftex
clean_pdftex : 
	@test -d $(PDFTEX_DIR) && $(RM) $(PDFTEX_DIR)/*.pdf

.PHONY : clean_pdftex_t
clean_pdftex_t : 
	@test -d $(PDFTEX_T_DIR) && $(RM) $(PDFTEX_T_DIR)/*.pdftex_t

.PHONY : show
show : pdf
	@test  "xacroread" = "x" && echo "ERROR: No PDF browser! " || \
	 (which acroread >/dev/null 2>&1 ) && acroread $(LATEX_MAIN_SRC_FILE).pdf & 

.PHONY : pdf
pdf : $(LATEX_MAIN_SRC_FILE).pdf

$(LATEX_MAIN_SRC_FILE).pdf : $(LATEX_MAIN_SRC_FILE).tex special
	pdflatex $<
	pdflatex $<

.PHONY : special
special : images #fig2pdftex_t 
	@echo "special: Nothing to do here for now..."

.PHONY : images
images : 
	@echo "images: Nothing to do here for now..."

%.pdftex_t : %.fig
	@test -d $(PDFTEX_T_DIR) || mkdir $(PDFTEX_T_DIR)
	@echo "Building '%.pdftex_t' from '%.fig'..."
	@fig2dev -L pdftex_t  -l landscape \
	  -p "\pdftexpath/"$*.pdf	  $< \
	  $(PDFTEX_T_DIR)/$(notdir $(@))

%.pdf : %.fig
	@test -d $(PDFTEX_DIR) || mkdir $(PDFTEX_DIR)
	@echo "Building '%.pdf' from '%.fig'..."
	@fig2dev -L pdftex -l landscape $< $(PDFTEX_DIR)/$(notdir $(@))

.PHONY : fig2pdftex_t
fig2pdftex_t : $(fig2pdftex_t_files) fig2pdftex

.PHONY : fig2pdftex
fig2pdftex : $(fig2pdftex_files)

# end of GNUmakefile