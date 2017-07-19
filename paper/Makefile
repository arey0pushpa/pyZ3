# Makefile for papers
MAIN = main
.PHONY: clean open spell vpdf

GIT_VERSION=$(shell git rev-parse HEAD)

all: $(MAIN).pdf

$(MAIN).pdf: *.tex *.bib *.cls
	latexmk -pdf $(MAIN).tex

vpdf: $(MAIN).pdf
	cp $(MAIN).pdf $(MAIN)-$(GIT_VERSION).pdf

spell:
	for f in `find . -name "*.tex"`; do aspell -t -l en_us -c $$f; done;
#	aspell -t -l en_us -c $(MAIN).tex

zip:
	zip -r $(MAIN).zip *.tex */*.tex *.bib *.cls

clean:
	rm -f $(MAIN).aux $(MAIN).bbl $(MAIN).blg $(MAIN).bst $(MAIN).dvi $(MAIN).fdb_latexmk $(MAIN).fls $(MAIN).idx $(MAIN).lof $(MAIN).log $(MAIN).nav $(MAIN).out $(MAIN).pdf $(MAIN).snm $(MAIN).synctex.gz $(MAIN).toc
	rm -f *.pdf
	rm -f $(MAIN).zip
	rm -f *.bak */*.bak *.auto *.log */*.log *~ */*~ */texput.log
