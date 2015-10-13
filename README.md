In order to generate the pdf output, you need to run Doxygen and compile all the
Latex sources. This process has been automated in a Makefile but you still need
to have a few tools installed in your system (and included in the path):

  -Latex. In Linux you can simply install the 'textlive-full' package. In
   Windows, use Miktex.

  -Doxygen.

  -Python 2.X.

  -Perl (required by LatexMK)

  -GNU Make.

Most Linux distros already include these tools by default.

You can test your installation by invoking 'make' from the command line. If
sucessful, it will print a long log which ends with something like

  Output written on main.pdf (112 pages, 721076 bytes).
  Transcript written on main.log.
  Latexmk: All targets (main.pdf) are up-to-date

The generated document is named 'main.pdf'
