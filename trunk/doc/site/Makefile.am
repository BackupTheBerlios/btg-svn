RST2HTML=rst2html --date --strict --stylesheet-path=style.css --link-stylesheet
RST2LATEX=rst2latex --date --strict
PDFLATEX=pdflatex

all: 

man-pages:
	@./convert-man.sh

html: man-pages index.rst packages.rst howto.rst howto-0.3.rst howto-0.4.rst howto-0.5.rst
	$(RST2HTML) index.rst > index.html
	$(RST2HTML) packages.rst > packages.html
	$(RST2HTML) howto.rst > howto.html
	$(RST2HTML) faq.rst > faq.html
	$(RST2HTML) manpages.rst > manpages.html
	$(RST2HTML) release-proc.rst > release-proc.html
	$(RST2HTML) howto_bfs.rst > howto_bfs.html
	$(RST2HTML) howto-0.3.rst > howto-0.3.html
	$(RST2HTML) howto-0.4.rst > howto-0.4.html
	$(RST2HTML) howto-0.5.rst > howto-0.5.html
	$(RST2HTML) howto-0.6.rst > howto-0.6.html
	$(RST2HTML) howto-0.7.rst > howto-0.7.html

# Target used to generate a pdf version of the howto.
# Assumes that rst2latex and latex is installed.
pdf: howto.rst
	@echo "Generating howto (PDF).."
	$(RST2LATEX) howto.rst > howto.latex && \
        $(PDFLATEX) howto.latex

clean:
	@rm -f *.html

