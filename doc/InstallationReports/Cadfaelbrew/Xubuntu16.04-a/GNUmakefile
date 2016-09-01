# -*- mode: make; -*-

cb_report_label=xubuntu16.04
cb_report_version=1.0
cb_report_name=cadfaelbrew_$(cb_report_label)_report
cb_tagged_report_name=cadfaelbrew_$(cb_report_label)_report-$(cb_report_version)

.PHONY : all
all : html pdf

.PHONY : clean
clean :
	$(RM) $(cb_tagged_report_name).pdf
	$(RM) $(cb_report_name).html
	$(RM) $(cb_report_name).pdf
	$(RM) $(cb_report_name).aux
	$(RM) $(cb_report_name).log
	$(RM) $(cb_report_name).out
	$(RM) $(cb_report_name).tex
	$(RM) $(cb_report_name).toc
	find . -name "*~" -exec rm -f \{\} \;

.PHONY : tag
tag : final
	cp -f $(cb_tagged_report_name).pdf tagged/

.PHONY : final
final : $(cb_report_name).pdf
	cp -f $(cb_report_name).pdf $(cb_tagged_report_name).pdf

.PHONY : html
html : $(cb_report_name).html

.PHONY : pdf
pdf : $(cb_report_name).pdf

.PHONY : show
show : $(cb_report_name).html
	xdg-open $< &

.PHONY : show_pdf
show_pdf : $(cb_report_name).pdf
	xdg-open $< &

$(cb_report_name).html :  $(cb_report_name).rst
	rst2html $< > $@

$(cb_report_name).pdf : $(cb_report_name).tex
	pdflatex $<
	pdflatex $<

$(cb_report_name).tex :  $(cb_report_name).rst
	rst2latex --documentclass=article \
	  --documentoptions=a4paper,a4wide,11pt \
	  --use-latex-docinfo \
	  --use-verbatim-when-possible \
	  --stylesheet="style/geo" \
	  --use-latex-toc \
	  --syntax-highlight="long" \
	  --section-numbering \
	  $< > $@

# end
