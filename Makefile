.PHONY: all clean diff checkversion main-all-prev main-all dist

ifeq ($(OS),Windows_NT)
   RM = del /Q
   CP = xcopy /-Y /D
   MKDIR = mkdir
   PYTHON = c:\python27\python
else
   RM = rm -Rf
   CP = cp -i -u
   MKDIR = mkdir -p
   PYTHON = python2
endif

ifneq ("$(wildcard main-diff.tex)","")
MAIN_DIFF = 1
endif

# LaTeX representation of Doxygen API documentation
DOXYLATEX:= $(wildcard api/altlatex/*.tex)

# Used to pass information to listings package (e.g., which function names
# should be emphasized)
LISTINGS:=api/altlatex/listings.tex

# Commands that allow LaTeX to include a C function by providing its name
EXAMPLES:=examples/altlatex/lstinputfunlisting.tex

# Temporary directory where to copy intermediate files before distribution
DISTNAME:=hsa_runtime_$(HSA_VERSION)
TMPDIST:=public/$(DISTNAME)

all : $(EXAMPLES) $(LISTINGS) main.pdf

diff : main-diff.pdf

checkversion :
	$(if $(HSA_VERSION),,$(error Error: variable HSA_VERSION is not set))

# Generate the public files
# syntax: make HSA_VERSION=<current version, any string will do> COMMIT=<hash corresponding to previous version>
# Ex: make HSA_VERSION=1_1_20151231 COMMIT=26862995d24908be0e7a870e71807b8a7d6c1996 dist
dist : checkversion diff main.pdf
	$(MKDIR) $(TMPDIST)/include/hsa
	$(CP) main.pdf $(TMPDIST)/$(DISTNAME).pdf
	$(CP) main-diff.pdf $(TMPDIST)/$(DISTNAME)_diff.pdf
	$(CP) main-all.tex $(TMPDIST)/$(DISTNAME).tex
	$(CP) ChangeLog $(TMPDIST)
	$(CP) api/hsa.h $(TMPDIST)/include/hsa
	$(CP) api/hsa_ext.h $(TMPDIST)/include/hsa
	cd public && zip -r -9 $(DISTNAME) $(DISTNAME)
	$(RM) $(TMPDIST)

%.pdf: %.tex
	latexmk -g -xelatex -use-make $*.tex

$(EXAMPLES): example/examples.cc example/lstinputfunlisting.py
	cd example && doxygen Doxyfile
	cd example && $(PYTHON) lstinputfunlisting.py

# Use listings.tex as proxy for all the automatically generated LaTeX files
$(LISTINGS): api/hsa.h api/hsa_ext.h api/xml2tex.py
	cd api && doxygen Doxyfile
	cd api && $(PYTHON) xml2tex.py

# Diff previous and current version of the document. The result is another Latex
# file. The current version is expected to be named main-all.tex
main-diff.tex: main-all-prev main-all.tex
	./latexdiff-1.1.0-so.pl -t UNDERLINE --append-safecmd="hypertarget,hyperlink,reffun,refarg,reffld,reftyp,refenu,refhsl" --exclude-textcmd="chapter,section,subsection,subsubsection" --config="PICTUREENV=(?:picture|DIFnomarkup|tikzpicture|lstlisting|figure)[\w\d*@]*" main-all-prev.tex main-all.tex  > main-diff.tex

main-all-prev:
	git stash
	git checkout $(COMMIT)
	make clean main-all.tex
	$(CP) main-all.tex main-all-prev.tex
	git checkout master
	git stash pop

# Generate one Latex file out of resolving all \include tags in a given main
# file. This is is needed because `latexdiff --flatten` requires all included
# files to be in the same directory as the main doc.
main-all.tex: $(EXAMPLES) $(LISTINGS) main.tex listingexpand.py
	latexpand main.tex > main-all.tex
	$(PYTHON) listingexpand.py main-all.tex

clean:
	@latexmk -silent -C main
	@$(RM) main-all.tex
	@$(RM) main-all-prev.tex
	@$(RM) main-all-new.tex
        # latexdiff fails if the specified tex input does not exist, but the
        # diff file only exists if 'make diff' was previously invoked.
	$(if $(MAIN_DIFF),@latexmk -silent -C main-diff,)
	@$(RM) main-diff.tex
        # Manually clean Doxygen and xml2tex output
	@cd api && $(RM) xml && $(RM) altlatex
