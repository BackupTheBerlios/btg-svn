RST2HTML=rst2html --date --strict --stylesheet-path=voidspace2.css --link-stylesheet
RST2LATEX=rst2latex --date --strict
PDFLATEX=pdflatex
HTML2TEXT=html2text -nobs -ascii

all: 

man-pages:
	@./convert-man.sh

RST_FILES=index.rst \
packages.rst \
howto.rst howto_bfs.rst howto_devel.rst \
howto-0.3.rst howto-0.4.rst howto-0.5.rst howto-0.6.rst howto-0.7.rst \
howto-0.9.6.rst howto-0.9.7.rst

commanddoc:
	cd ../commands && $(MAKE) commanddoc

commanddoc_copy:
	cp ../commands/commands.html .
 
html: commanddoc commanddoc_copy man-pages $(RST_FILES)
	$(RST2HTML) index.rst > index.html
	$(RST2HTML) packages.rst > packages.html
	$(RST2HTML) howto.rst > howto.html
	$(HTML2TEXT) howto.html > howto.txt
	$(RST2HTML) faq.rst > faq.html
	$(RST2HTML) manpages.rst > manpages.html
	$(RST2HTML) release-proc.rst > release-proc.html
	$(RST2HTML) howto_bfs.rst > howto_bfs.html
	$(RST2HTML) howto_devel.rst > howto_devel.html
	$(RST2HTML) howto-0.3.rst > howto-0.3.html
	$(RST2HTML) howto-0.4.rst > howto-0.4.html
	$(RST2HTML) howto-0.5.rst > howto-0.5.html
	$(RST2HTML) howto-0.6.rst > howto-0.6.html
	$(RST2HTML) howto-0.7.rst > howto-0.7.html
	$(RST2HTML) howto-0.9.6.rst > howto-0.9.6.html
	$(RST2HTML) howto-0.9.7.rst > howto-0.9.7.html
	$(RST2HTML) howto-0.9.8.rst > howto-0.9.8.html

# Target used to generate a pdf version of the howto.
# Assumes that rst2latex and latex is installed.
pdf: howto.rst
	@echo "Generating howto (PDF).."
	$(RST2LATEX) howto.rst > howto.latex && \
        $(PDFLATEX) howto.latex

clean:
	@rm -f *.html

