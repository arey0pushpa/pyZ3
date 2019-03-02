filename=main

pdf: ps
	ps2pdf ${filename}.ps

pdf-print: ps
	ps2pdf -dColorConversionStrategy=/LeaveColorUnchanged -dPDFSETTINGS=/printer ${filename}.ps

#text: html
#	html2text -width 100 -style pretty ${filename}.html | sed -n '/./,$$p' | head -n-2 >${filename}.txt

#html:
#	@#latex2html -split +0 -info "" -no_navigation ${filename}
#	htlatex ${filename}

ps:	dvi
	dvips -t letter ${filename}.dvi

dvi:
	latex ${filename}
	bibtex ${filename}||true
	latex ${filename}
	latex ${filename}

read:
	evince ${filename}.pdf &

clean:
	rm -f *.ps
	rm -f *.pdf
	rm -f *.log
	rm -f *.aux
	rm -f *.out
	rm -f *.dvi
	rm -f *.bbl
	rm -f *.blg
	rm -f *.fls
	rm -f *.tmp
