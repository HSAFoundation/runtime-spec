#!/usr/bin/python

import xml.etree.ElementTree as ET
import sys
import os

def main():
  if sys.version_info[0] >= 3:
    sys.exit("Error: Your Python interpreter must be < 3.0\n")

  outdir = 'altlatex'
  if not os.path.exists(outdir):
    os.makedirs(outdir)

  output = open(os.path.join('altlatex', 'lstinputfunlisting.tex'), 'w+')
  files = filter(lambda file: file.endswith('.xml'), os.listdir('xml'))
  for file in files:
    sys.stdout.write('Processing ' + file + '...')
    tree = ET.parse(os.path.join('xml',file))
    root = tree.getroot()
    funs = filter(lambda x: x.get('kind') == 'function', root.findall('.//memberdef'))
    if len(funs) == 0:
      print('OK')
      continue
    output.write('\\makeatletter')
    output.write('\\newcommand{\\lstinputfunlisting}[2][0]{\\ifnum\\pdf@strcmp{#2}{blablablablabla}=0 blablablablabla')
    for fun in funs:
      name = fun.find('name')
      location = fun.find('location')
      start = int(location.get('bodystart'))
      end = int(location.get('bodyend'))

      output.write('\else\ifnum\pdf@strcmp{#2}{' + name.text + '}=0')

      output.write('\\ifnum #1=0')
      output.write('\\lstinputlisting[firstline=' + str(start))
      output.write(', lastline=' + str(end))
      output.write(']{example/examples.cc}\n')

      # exclude first and last line of function (normally, signature and closing brace)
      output.write('\\else\n')
      output.write('\\lstinputlisting[firstline=' + str(start + 1))
      output.write(', lastline=' + str(end - 1))
      output.write(']{example/examples.cc}\n')

      output.write('\\fi\n')

    output.write('\\else\\errmessage{Unknown function: #2. There is no function with that name in the examples}\n')
    output.write('\\fi' * (1 + len(funs)) + "}")
    output.write('\\makeatother')
    print('OK')

  output.close()
  return


if __name__ == '__main__':
    sys.exit(main())
