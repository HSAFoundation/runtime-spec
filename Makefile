.PHONY: all clean diff

DOC=main

all : $(DOC).pdf
diff : $(DOC)-diff.pdf

$(DOC).pdf: $(DOC).tex
	cd example && python generate.py
	latexmk -pdf -use-make $(DOC)

# TODO(mmario) really lame temporary rule...
$(DOC)-diff.pdf: $(DOC)-diff.tex
	cd example && python generate.py
	latexmk -pdf -use-make $(DOC)-diff

$(DOC)-diff.tex: $(DOC).tex $(DOC)-prev.tex
	latexdiff --append-safecmd="diffblock" --exclude-textcmd="emph,subsection,ttbf" $(DOC)-prev.tex $(DOC).tex > $(DOC)-diff.tex

clean:
	latexmk -C $(DOC)
	cd example && rm -f API*.tex STR*.tex ENU*.tex funclistfile structlistfile temp
