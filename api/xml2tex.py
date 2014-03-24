#!/usr/bin/python

import xml.etree.ElementTree as ET
import sys
import os

# escape characters to make them Latex-friendly
def text2tex(text):
  return text.replace("_", "\\_")

# custom text (Latex) iterator that consumes XML
def myitertext(self):
  tag = self.tag
  if not isinstance(tag, str) and tag is not None:
    return
  if self.text:
    mytex = text2tex(self.text)
    if tag == 'emphasis':
      yield '\\hsaarg{' + mytex + '}'
    elif tag == 'ref':
      yield '\\hsatyp{' + self.get('refid') + '}{' + mytex + '}'
    else:
      yield mytex
  for e in self:
    for s in myitertext(e):
      yield s
    if e.tail:
      yield e.tail

# return Latex string corresponding to XML node
def node2tex(node):
  if node is None:
    return ''
  return ''.join(myitertext(node)).encode("utf-8").strip()

def process_file(file):
  tree = ET.parse(os.path.join('xml',file))
  root = tree.getroot()
  texfilename = os.path.splitext(file)[0] + ".tex"
  tex = open(os.path.join("altlatex", texfilename), "w+")

  # Process typedefs
  typedefs = root.findall(".//memberdef[@kind='typedef']")
  for typedef in typedefs:
    typ = typedef.find('type').text;
    if typ is not None:
      continue
    tex.write("\n\n")
    # begin box
    tex.write('\\noindent\\begin{tcolorbox}[nobeforeafter,arc=0mm,colframe=white,colback=lightgray,left=0mm]\n')
    # typedef definition. We use the 'definition' string because in the presence
    # of pointers to functions using the type itself is tricky.
    definition = node2tex(typedef.find('definition'))
    name = node2tex(typedef.find('name'))
    newname = " \\hsadef{" + typedef.get('id') + "}{" + name + "}"
    definition = definition.replace(name, newname, 1)
    tex.write(definition + "\n")
    # end box
    tex.write('\\end{tcolorbox}\n')

  # Process enums
  enums = root.findall(".//memberdef[@kind='enum']")
  for enum in enums:
    tex.write("\n\n")
    # begin box
    tex.write('\\noindent\\begin{tcolorbox}[nobeforeafter,arc=0mm,colframe=white,colback=lightgray,left=0mm]\n')
    # enum name
    tex.write('enum ' + "\\hsadef{" + enum.get('id') + "}")
    tex.write("{" + node2tex(enum.find('name')) + "}" + "\n")
    # end box
    tex.write('\\end{tcolorbox}\n')
    # brief
    tex.write(node2tex(enum.find('briefdescription/para')) + "\n\n")
    # values
    tex.write("\\noindent\\textbf{Values}\\\\[-5mm]" + "\n")
    tex.write('\\begin{longtable}{@{}>{\\hangindent=2em}p{\\linewidth}}' + "\n")
    vals = []
    for val in enum.findall("enumvalue"):
      valtxt = node2tex(val.find('name'))
      valtxt += ' ' + node2tex(val.find('initializer'))
      valdesc = node2tex(val.find('detaileddescription'))
      if valdesc != '':
        valtxt +=  "\\\\" + "\\hspace{2em}" + valdesc
      vals.append(valtxt)
    tex.write("\\\\[2mm]\n".join(vals))
    tex.write("\n\\end{longtable}")

  # Process structs and unions
  typedefs = root.findall(".//memberdef[@kind='typedef']")
  for typedef in typedefs:
    typ = typedef.find('type').text;
    if typ is None:
      continue
    typ = typ.strip()
    if typ not in ['struct', 'union']:
      continue
    tex.write("\n\n")
    # begin box
    tex.write('\\noindent\\begin{tcolorbox}[nobeforeafter,arc=0mm,colframe=white,colback=lightgray,left=0mm]\n')
    # typedef name
    tex.write(typ + " \\hsadef{" + typedef.get('id') + "}")
    tex.write("{" + node2tex(typedef.find('name')) + "} \\\\" + "\n")
    # members. Doxygen stores their info in a separate file.
    membersfile = typedef.find('type/ref').get('refid') + ".xml"
    memberstree = ET.parse(os.path.join('xml',membersfile))
    membersroot = memberstree.getroot()
    members = membersroot.findall(".//memberdef[@kind='variable']")
    tex.write("\\begin{tabular}{@{}l}" + "\n")
    vals = []
    fields = []
    for member in members:
      namenode = node2tex(member.find('name'))
      # value string
      txt = "\\hspace{1.7em}" + node2tex(member.find('type'))
      txt += " \\hsaarg{" + namenode + "}"
      bitfield = member.find('bitfield')
      if bitfield is not None:
        txt += " : " + node2tex(bitfield)
      vals.append(txt)
      # field string
      txt = "\hsaarg{" + namenode + "}" + "\\\\"
      txt += "\\hspace{2em}"
      txt += node2tex(member.find('detaileddescription/para'))
      fields.append(txt)
    tex.write("\\\\\n".join(vals))
    tex.write("\n\\end{tabular}" + "\n\n")
    # end box
    tex.write('\\end{tcolorbox}\n')
    # brief
    tex.write(node2tex(typedef.find('briefdescription/para')) + "\n\n")
    # data fields
    tex.write("\\noindent\\textbf{Data Fields}\\\\[-5mm]" + "\n")
    tex.write("\\begin{longtable}{@{}>{\\hangindent=2em}p{\\textwidth}}" + "\n")
    tex.write("\\\\[2mm]\n".join(fields))
    tex.write("\n\\end{longtable}" + "\n\n")

  # Process functions
  funcs = root.findall(".//memberdef[@kind='function']")
  for func in funcs:
    tex.write("\n\n")
    # begin box
    tex.write('\\noindent\\begin{tcolorbox}[nobeforeafter,colframe=white,colback=lightgray,left=0mm]\n')
    # signature - return value
    tex.write(node2tex(func.find('type')) + " ")
    # signature - func name
    tex.write("\\hsadef{" + func.get('id') + "}")
    tex.write("{" + node2tex(func.find('name')) + "}(")
    # signature - parameters
    sigargs = func.findall("param")
    if sigargs:
      tex.write("\\\\\n")
      tex.write("\\begin{tabular}{@{}l}" + "\n")
      arglst = []
      for arg in sigargs:
        argtxt = "\\hspace{1.7em}" + node2tex(arg.find('type'))
        argtxt += " \\hsaarg{" + node2tex(arg.find('declname')) + "}"
        arglst.append(argtxt)
      tex.write(",\\\\\n".join(arglst))
      tex.write(")\\end{tabular}")
    else:
      tex.write(")")
    tex.write("\n\n")
    # end box
    tex.write('\\end{tcolorbox}\n')
    # brief
    tex.write(node2tex(func.find('briefdescription/para')) + "\n\n")
    # parameters
    args = func.findall(".//parameterlist[@kind='param']/parameteritem")
    if args:
      tex.write("\\noindent\\textbf{Parameters}\\\\[-5mm]" + "\n")
      tex.write("\\noindent\\begin{longtable}{@{}>{\\hangindent=2em}p{\\textwidth}}" + "\n")
      arglst = []
      for arg in args:
        argnamenode = arg.find('./parameternamelist/parametername')
        argtxt = "\hsaarg{" + node2tex(argnamenode) + "}" + "\\\\"
        direction = argnamenode.get('direction')
        argtxt += "\\hspace{2em}"
        argtxt += '' if direction is None else "(" + direction + ") "
        argtxt += node2tex(arg.find('parameterdescription/para'))
        arglst.append(argtxt)
      tex.write("\\\\[2mm]\n".join(arglst))
      tex.write("\n\\end{longtable}" + "\n")
    else:
      # add the tabular anyway so margin are kept constant
      tex.write("\\noindent\\begin{longtable}{@{}>{\\hangindent=2em}p{\\textwidth}}" + "\n")
      tex.write("\n\\end{longtable}" + "\n")
    # return values
    rets = func.findall(".//parameterlist[@kind='retval']/parameteritem")
    if rets:
      if args:
        tex.write('\\vspace{-5mm}')
      tex.write("\\noindent\\textbf{Return Values}\\\\[-5mm]" + "\n")
      tex.write('\\noindent\\begin{longtable}{@{}>{\\hangindent=2em}p{\\linewidth}}' + "\n")
      arglst = []
      for ret in rets:
        argtxt = node2tex(ret.find('parameternamelist/parametername/ref')) + "\\\\"
        argtxt += "\\hspace{2em}" + node2tex(ret.find('parameterdescription'))
        arglst.append(argtxt)
      tex.write("\\\\[2mm]\n".join(arglst))
      tex.write("\n\\end{longtable}" + "\n")
    # returns/return description
    ret = func.find("detaileddescription/para/simplesect[@kind='return']")
    if ret is not None:
      tex.write('\\vspace{-5mm}')
      tex.write("\\noindent\\textbf{Returns}\\\\[1mm]"+ "\n")
      tex.write(node2tex(ret) + "\n\n")
      # add the tabular anyway so margin are kept constant
      tex.write('\\noindent\\begin{longtable}{@{}>{\\hangindent=2em}p{\\linewidth}}' + "\n")
      tex.write("\n\\end{longtable}" + "\n")
    # description
    para = func.find('detaileddescription/para')
    if para is not None and para.find('parameterlist') is None:
      tex.write('\\vspace{-5mm}')
      tex.write("\\noindent\\textbf{Description}\\\\"+ "\n")
      tex.write(node2tex(para))
    tex.write(" \n")
  tex.close()

def main():
  outdir = 'altlatex'
  if not os.path.exists(outdir):
    os.makedirs(outdir)
  for file in os.listdir('xml'):
    if file.find("group__API__") == 0 or file.find("group__ENU__") == 0 or file.find("group__STR__") == 0 or file.find("group__TDF__") == 0:
      sys.stdout.write('Processing ' + file + "...")
      process_file(file)
      print('OK')
  return

if __name__ == "__main__":
    sys.exit(main())
