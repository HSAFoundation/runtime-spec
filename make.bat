del /s /f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl API*.tex STR*.tex ENU*.tex corebase.pdf funclistfile structlistfile temp

pdflatex corebase
echo ----
makeindex corebase.idx
echo ----
pdflatex corebase

setlocal enabledelayedexpansion
set count=5
:repeat
set content=X
for /F "tokens=*" %%T in ( 'findstr /C:"Rerun LaTeX" corebase.log' ) do set content="%%~T"
if !content! == X for /F "tokens=*" %%T in ( 'findstr /C:"Rerun to get cross-references right" corebase.log' ) do set content="%%~T"
if !content! == X goto :skip
set /a count-=1
if !count! EQU 0 goto :skip

echo ----
pdflatex corebase
goto :repeat
:skip
endlocal
