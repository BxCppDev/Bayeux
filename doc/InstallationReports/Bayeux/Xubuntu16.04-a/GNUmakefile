# -*- mode: make; -*-

bx_report_label=xubuntu16.04
bx_report_version=2.0
bx_report_name=bayeux-trunk_$(bx_report_label)_report
bx_tagged_report_name=bayeux-trunk_$(bx_report_label)_report-$(bx_report_version)

.PHONY : all
all : html pdf

.PHONY : clean
clean :
	$(RM) $(bx_tagged_report_name).pdf
	$(RM) $(bx_report_name).html
	$(RM) $(bx_report_name).pdf
	$(RM) $(bx_report_name).aux
	$(RM) $(bx_report_name).log
	$(RM) $(bx_report_name).out
	$(RM) $(bx_report_name).tex
	$(RM) $(bx_report_name).toc
	find . -name "*~" -exec rm -f \{\} \;

.PHONY : tag
tag : final
	cp -f $(bx_tagged_report_name).pdf tagged/

.PHONY : final
final : $(bx_report_name).pdf
	cp -f $(bx_report_name).pdf $(bx_tagged_report_name).pdf

.PHONY : html
html : $(bx_report_name).html

.PHONY : pdf
pdf : $(bx_report_name).pdf

.PHONY : show
show : $(bx_report_name).html
	xdg-open $< &

.PHONY : show_pdf
show_pdf : $(bx_report_name).pdf
	xdg-open $< &

$(bx_report_name).html :  $(bx_report_name).rst
	rst2html $< > $@

$(bx_report_name).pdf : $(bx_report_name).tex
	pdflatex $<
	pdflatex $<

$(bx_report_name).tex :  $(bx_report_name).rst
	rst2latex --documentclass=article \
	  --documentoptions=a4paper,a4wide,10pt \
	  --use-latex-docinfo \
	  --use-verbatim-when-possible \
	  --stylesheet="style/geo" \
	  --use-latex-toc \
	  --syntax-highlight="long" \
	  --section-numbering \
	  $< > $@

# end
