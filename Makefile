# TODO (mmario): this Makefile is broken, needs to be simplified / use latexmk
DOC=main

all : $(DOC).pdf

$(DOC).pdf:
	# generate tex files from corebase.h
	cd example && python generate.py
	pdflatex $(DOC)
	bibtex $(DOC)
	bibtex $(DOC)
	makeindex $(DOC).idx
	count=3 ; while egrep -s 'Rerun (LaTeX|to get cross-references right)' $(DOC).log && [ $$count -gt 0 ] ; do \
		echo "Rerunning latex...." ;\
		pdflatex $(DOC) ;\
		count=`expr $$count - 1` ;\
	done

# diff:
# 	cp main.pdf hsa_core_runtime_0_74_0.pdf
# 	 #generate the main diff pdf from current version
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="emph,subsection,ttbf" coreapierror073.tex coreapierror074.tex > coreapierror.tex
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="emph,section,subsection,ttbf" coreapiopen073.tex coreapiopen074.tex > coreapiopen.tex
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="epmh,subsection,ttbf" coreapitopo073.tex coreapitopo074.tex > coreapitopo.tex
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="epmh,subsection,ttbf" coreapisignal073.tex coreapisignal074.tex > coreapisignal.tex
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="epmh,subsection,ttbf" coreapiqueue073.tex coreapiqueue074.tex > coreapiqueue.tex
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="epmh,subsection,ttbf" coreapiaql073.tex coreapiaql074.tex > coreapiaql.tex
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="epmh,subsection,ttbf" coreapimemory073.tex coreapimemory074.tex > coreapimemory.tex
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="epmh,subsection,ttbf" finalizer073.tex finalizer074.tex > finalizer.tex
# 	./latexdiff --append-safecmd="diffblock" --exclude-textcmd="epmh,subsection,ttbf" index073.tex index074.tex > index.tex
# 	pdflatex main
# 	bibtex main
# 	bibtex main
# 	makeindex main.idx
# 	pdflatex main
# 	latex_count=5 ; \
# 	while egrep -s 'Rerun (LaTeX|to get cross-references right)' main.log && [ $$latex_count -gt 0 ] ;\
# 	    do \
# 	      echo "Rerunning latex...." ;\
# 	      pdflatex main ;\
# 	      latex_count=`expr $$latex_count - 1` ;\
# 	    done
# 	cp main.pdf hsa_core_runtime_0_74_0_diff.pdf

clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl example/API*.tex example/STR*.tex example/ENU*.tex example/funclistfile example/structlistfile example/temp main.pdf
