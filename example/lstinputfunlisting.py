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
    output.write('\\newcommand{\\lstinputfunlisting}[1]{\\ifnum\\pdf@strcmp{#1}{blablablablabla}=0 blablablablabla')
    for fun in funs:
      name = fun.find('name')
      output.write('\else\ifnum\pdf@strcmp{#1}{' + name.text + '}=0')
      location = fun.find('location')
      output.write('{\\lstinputlisting[firstline=' + location.get('bodystart') + ', lastline=' + location.get('bodyend') + ']{example/examples.cc}}\n')
    output.write('\\fi' * (1 + len(funs)) + "}")
    output.write('\\makeatother')
    print('OK')

  output.close()
  return


if __name__ == '__main__':
    sys.exit(main())
