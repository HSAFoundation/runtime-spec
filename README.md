LaTeX and C99 header files corresponding to the HSA Foundation Runtime
specification.

Editors of the specification must use this repository in order to introduce modifications in the document. This repository does not contain any specification binaries (.pdf), which can be found at:

* https://members.hsafoundation.com/wg/HSA_SRW (all revisions, requires registration)
* http://www.hsafoundation.com/standards (official revisions)


# Installation
In order to generate the pdf output, you need to run Doxygen and compile
all the Latex sources. This process has been automated in a Makefile but you still need to have a few tools installed in your system (and included in the path):

- Git
- Latex. In Linux you can  install the (massive) `texlive-full` package. In Windows, use Miktex
- Doxygen
- Python 2.X
- Perl (required by LatexMK)
- GNU Make

Most of these packages are already included with the popular Linux distributions. In Linux Mint (17.1) you can use the following one-liner to install the missing dependencies:
```
sudo apt-get install git texlive-full doxygen
```
, and then clone the repository
```
git clone https://github.com/HSAFoundation/runtime-spec.git
```

You can test your installation by invoking 'make' from the command line
```
cd runtime-spec
make
```
If sucessful, it will print a long log which ends with something like:
```
Output written on main.pdf (148 pages).
Transcript written on main.log.
Latexmk: All targets (main.pdf) are up-to-date
```
The output document is `main.pdf`


# Document Generation
There are two sets of files contaning the specification sources. The first set contains the text itself, in LaTeX format. Currently, there is only one such file (`main.tex`). The second set contains the C99 headers listing the Runtime API. The headers can be found inside the `api` directory (`hsa.h`, `hsa_ext.h`, etc).

The final document is generated out of compiling the C headers to LaTeX sources that are included from the main LaTeX file. The compilation process relies on Doxygen (.h => .xml) and a custom Python compiler (.xml => .tex). The custom compiler corresponds to `xml2tex.py`. All these steps are automatically invoked from the Makefile when you run `make`.


# Diff between Document Revisions
A pdf document highlighting the differences between the current specification and the previous one can be generated by invoking
