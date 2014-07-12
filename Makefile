.PHONY: all clean diff

ifeq ($(OS),Windows_NT)
   RM = del /Q
else
   RM = rm -Rf
endif

# LaTeX representation of Doxygen API documentation
DOXYLATEX:= $(wildcard api/altlatex/*.tex)

# Used to pass information to listings package (e.g., which function names
# should be emphasized)
LISTINGS:=api/altlatex/listings.tex

all : $(LISTINGS) main.pdf
diff : all main-diff.pdf

%.pdf: $(DOXYLATEX) %.tex
	latexmk -g -xelatex -bibtex -use-make $*.tex

# Use listings.tex as proxy for all the automatically generated LaTeX files
$(LISTINGS): api/hsa.h api/hsa_ext.h api/xml2tex.py
	cd api && doxygen Doxyfile
	cd api && python xml2tex.py

# Diff previous and current version of the document. The result is another Latex
# file. The previous version is expected to be named main-all-prev.tex
main-diff.tex: main-all.tex main-all-prev.tex
	latexdiff -t UNDERLINE --append-safecmd="hypertarget,hsaarg,reffun,refarg,reffld,reftyp,refenu,refhsl,diffblock" --exclude-textcmd="emph,section,subsection,subsubsection" --config="PICTUREENV=(?:picture|DIFnomarkup|lstlisting|tikzpicture|figure)[\w\d*@]*" main-all-prev.tex main-all.tex > main-diff.tex

# Generate one Latex file out of resolving all \include tags in a given main
# file. This is is needed because `latexdiff --flatten` requires all included
# files to be in the same directory as the main doc.
main-all.tex: main.tex
	latexpand main.tex > main-all.tex

clean:
	latexmk -bibtex -C main
        # Manually clean Doxygen and xml2tex output
	cd api && $(RM) xml && $(RM) altlatex
