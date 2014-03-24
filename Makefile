.PHONY: all clean diff

DOC=main

ifeq ($(OS),Windows_NT)
   RM = del /Q
endif

all : $(DOC).pdf
diff : $(DOC)-diff.pdf

$(DOC).pdf: example/latex/temp $(DOC).tex
	latexmk -pdf -bibtex -use-make $(DOC)

# TODO(mmario) really lame temporary rule...
$(DOC)-diff.pdf: example/latex/temp $(DOC)-diff.tex
	latexmk -pdf -bibtex -use-make $(DOC)-diff

# Generate Latex corresponding to source code
example/latex/temp : api/
	cd example
        #&& mkdir -p latex && python generate.py

# Diff previous and current version of the document. The result is
# another Latex file.  The previous version is expected to be named
# $(DOC)-all-prev.tex
$(DOC)-diff.tex: $(DOC)-all.tex $(DOC)-all-prev.tex
	latexdiff -t UNDERLINE --append-safecmd="hsaarg,diffblock" --exclude-textcmd="emph,subsection,ttbf" --config="PICTUREENV=(?:picture|DIFnomarkup|lstlisting)[\w\d*@]*" $(DOC)-all-prev.tex $(DOC)-all.tex > $(DOC)-diff.tex

# Generate one Latex file out of resolving all \include tags in a
# given main file. This is is needed because `latexdiff --flatten`
# requires all files to be in the same directory
$(DOC)-all.tex: $(DOC).tex
	latexpand $(DOC).tex > $(DOC)-all.tex

clean:
	latexmk -bibtex -C $(DOC)
	cd example && cd latex && $(RM) temp
