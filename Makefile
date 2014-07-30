.PHONY: all clean diff checkversion checkprev dist

ifeq ($(OS),Windows_NT)
   RM = del /Q
   CP = xcopy /-Y /D
else
   RM = rm -Rf
   CP = cp -i -u
endif

ifneq ("$(wildcard main-diff.tex)","")
MAIN_DIFF = 1
endif

ifneq ("$(wildcard main-all-prev.tex)","")
MAIN_ALL_PREV = 1
endif

# LaTeX representation of Doxygen API documentation
DOXYLATEX:= $(wildcard api/altlatex/*.tex)

# Used to pass information to listings package (e.g., which function names
# should be emphasized)
LISTINGS:=api/altlatex/listings.tex

all : $(LISTINGS) main.pdf

checkprev:
	$(if $(MAIN_ALL_PREV),,$(error Error: file main-all-prev.tex is missing))

diff : checkprev all main-diff.pdf

checkversion :
	$(if $(HSA_VERSION),,$(error Error: variable HSA_VERSION is not set))

# Generate the public files
# Ex: make HSA_VERSION=1_01 dist
dist : checkversion diff
	$(CP) main.pdf public/hsa_runtime_$(HSA_VERSION).pdf
	$(CP) main-diff.pdf public/hsa_runtime_$(HSA_VERSION)_diff.pdf
	$(CP) main-all.tex public/hsa_runtime_$(HSA_VERSION).tex

%.pdf: $(DOXYLATEX) %.tex
	latexmk -g -xelatex -use-make $*.tex

# Use listings.tex as proxy for all the automatically generated LaTeX files
$(LISTINGS): api/hsa.h api/hsa_ext.h api/xml2tex.py
	cd api && doxygen Doxyfile
	cd api && python xml2tex.py

# Diff previous and current version of the document. The result is another Latex
# file. The previous version is expected to be named main-all-prev.tex
main-diff.tex: main-all.tex main-all-prev.tex
	./latexdiff-1.0.3.pl -t UNDERLINE --exclude-textcmd="section,subsection,subsubsection" --config="PICTUREENV=(?:picture|DIFnomarkup|tikzpicture|lstlisting)[\w\d*@]*" main-all-prev.tex main-all.tex > main-diff.tex

# Generate one Latex file out of resolving all \include tags in a given main
# file. This is is needed because `latexdiff --flatten` requires all included
# files to be in the same directory as the main doc.
main-all.tex: main.tex
	latexpand main.tex > main-all.tex

clean:
	@latexmk -silent -C main
	@$(RM) main-all.tex
        # latexdiff fails if the specified tex input does not exist, but the
        # diff file only exists if 'make diff' was previously invoked.
	$(if $(MAIN_DIFF),@latexmk -silent -C main-diff,)
	@$(RM) main-diff.tex
        # Manually clean Doxygen and xml2tex output
	@cd api && $(RM) xml && $(RM) altlatex
