.PHONY: all clean diff

ifeq ($(OS),Windows_NT)
   RM = del /Q
else
   RM = rm -Rf
endif

all : main.pdf
diff : main.pdf main-diff.pdf

main.pdf: api/hsa.h api/xml2tex.py main.tex
	cd api && doxygen Doxyfile
	cd api && python xml2tex.py
	latexmk -pdf -bibtex -use-make main

main-diff.pdf: main-diff.tex
	latexmk -pdf -bibtex -use-make main-diff

# Diff previous and current version of the document. The result is
# another Latex file.  The previous version is expected to be named
# main-all-prev.tex
main-diff.tex: main-all.tex main-all-prev.tex
	latexdiff -t UNDERLINE --append-safecmd="hsaarg,diffblock" --exclude-textcmd="emph,subsection,ttbf" --config="PICTUREENV=(?:picture|DIFnomarkup|lstlisting)[\w\d*@]*" main-all-prev.tex main-all.tex > main-diff.tex

# Generate one Latex file out of resolving all \include tags in a
# given main file. This is is needed because `latexdiff --flatten`
# requires all files to be in the same directory
main-all.tex: main.tex
	latexpand main.tex > main-all.tex

clean:
	latexmk -bibtex -C main
        # Manually clean Doxygen output
	cd api && $(RM) xml && $(RM) altlatex
