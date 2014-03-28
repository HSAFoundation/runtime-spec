#!/usr/bin/python

import xml.etree.ElementTree as ET
import sys
import os

# escape characters to make them Latex-friendly
def text2tex(text):
  text = text.replace("_", "\\_")
  return text

# custom text (Latex) iterator that consumes XML
def myitertext(self):
  tag = self.tag
  if not isinstance(tag, str) and tag is not None:
    return
  if self.text:
    mytex = text2tex(self.text)
    if tag == 'emphasis':
      yield '\\textit{' + mytex + '}'
    elif tag == 'ref':
      # brutal hardcoding in order to find out if it is a function name
      if mytex[:5] == "hsa\_" and mytex[-2:] != "_t":
        mytex = "\\reffun{" + mytex + "}"
      yield '\\hyperlink{' + self.get('refid') + '}{' + mytex + '}'
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
    typetext = node2tex(typedef.find('type'));
    if typetext[:5] in ['struc','union']:
      continue
    tex.write("\n\n")
    # begin box
    tex.write('\\noindent\\begin{tcolorbox}[nobeforeafter,arc=0mm,colframe=white,colback=lightgray,left=0mm]\n')
    # typedef definition. We use the 'definition' string because in the presence
    # of pointers to functions using the type itself is tricky.
    definition = node2tex(typedef.find('definition'))
    name = node2tex(typedef.find('name'))
    newname = " \\hypertarget{" + typedef.get('id') + "}{\\textbf{" + name + "}}"
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
    tex.write('enum ' + "\\hypertarget{" + enum.get('id') + "}")
    tex.write("{\\textbf{" + node2tex(enum.find('name')) + "}}" + "\n")
    # end box
    tex.write('\\end{tcolorbox}\n')
    # brief
    tex.write(node2tex(enum.find('briefdescription/para')) + "\n\n")
    # values
    tex.write("\\noindent\\textbf{Values}\\\\[-5mm]" + "\n")
    tex.write('\\begin{longtable}{@{}>{\\hangindent=2em}p{\\linewidth}}' + "\n")
    vals = []
    for val in enum.findall("enumvalue"):
      valtxt = "\\hypertarget{" + val.get('id') + "}{"
      valtxt += "\\refenu{" + node2tex(val.find('name')) + "}}"
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
    tex.write('\\noindent\\begin{tcolorbox}[breakable,nobeforeafter,arc=0mm,colframe=white,colback=lightgray,left=0mm]\n')
    # name
    tex.write(typ + " \\hypertarget{" + typedef.get('id') + "}")
    tex.write("{\\textbf{" + node2tex(typedef.find('name')) + "}}" + "\n")
    # members. Doxygen stores their info in a separate file.
    membersfile = typedef.find('type/ref').get('refid') + ".xml"
    memberstree = ET.parse(os.path.join('xml',membersfile))
    membersroot = memberstree.getroot()
    members = membersroot.findall(".//memberdef[@kind='variable']")
    tex.write("\\vspace{-3.5mm}\\begin{longtable}{@{}p{\\textwidth}}" + "\n")
    vals = []
    fields = []
    for member in members:
      name = node2tex(member.find('name'))
      # type + field name
      txt = "\\hspace{1.7em}"
      txt += node2tex(member.find('type'))
      txt += " \\reffld{" + name + "}"
      txt += node2tex(member.find('argsstring')) # array length, if any
      bitfield = member.find('bitfield')
      if bitfield is not None:
        txt += " : " + node2tex(bitfield)
      vals.append(txt)
      # field name + description
      txt = "\\reffld{" + name + "}" + "\\\\"
      txt += "\\hspace{2em}"
      txt += node2tex(member.find('detaileddescription/para'))
      fields.append(txt)
    tex.write("\\\\\n".join(vals))
    tex.write("\n\\end{longtable}" + "\n\n")
    # end box
    tex.write('\\end{tcolorbox}\n')
    # brief
    tex.write(node2tex(typedef.find('briefdescription/para')) + "\n\n")
    # data fields
    tex.write("\\noindent\\textbf{Data Fields}\\\\[-6mm]" + "\n")
    tex.write("\\begin{longtable}{@{}>{\\hangindent=2em}p{\\textwidth}}" + "\n")
    tex.write("\\\\[2mm]\n".join(fields))
    tex.write("\n\\end{longtable}" + "\n\n")

  # Process functions
  funcs = root.findall(".//memberdef[@kind='function']")
  for func in funcs:
    tex.write("\n\n")
    # begin box
    tex.write('\\noindent\\begin{tcolorbox}[breakable,nobeforeafter,colframe=white,colback=lightgray,left=0mm]\n')
    # signature - return value
    tex.write(node2tex(func.find('type')) + " ")
    # signature - func name
    tex.write("\\hypertarget{" + func.get('id') + "}")
    tex.write("{\\textbf{" + node2tex(func.find('name')) + "}}(")
    # signature - parameters
    sigargs = func.findall("param")
    if sigargs:
      tex.write("\n")
      tex.write("\\vspace{-3.5mm}\\begin{longtable}{@{}p{\\textwidth}}" + "\n")
      arglst = []
      for arg in sigargs:
        argtxt = "\\hspace{1.7em}" + node2tex(arg.find('type'))
        argtxt += " \\hsaarg{" + node2tex(arg.find('declname')) + "}"
        arglst.append(argtxt)
      tex.write(",\\\\\n".join(arglst))
      tex.write(")\\end{longtable}")
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
      tex.write("\\noindent\\textbf{Parameters}\\\\[-6mm]" + "\n")
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
      # add the longtable anyway so margin are kept constant
      tex.write("\\noindent\\begin{longtable}{@{}>{\\hangindent=2em}p{\\textwidth}}" + "\n")
      tex.write("\n\\end{longtable}" + "\n")
    # return values
    rets = func.findall(".//parameterlist[@kind='retval']/parameteritem")
    if rets:
      if args:
        tex.write('\\vspace{-5mm}')
      tex.write("\\noindent\\textbf{Return Values}\\\\[-6mm]" + "\n")
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
      # add the longtable anyway so margin are kept constant
      tex.write('\\noindent\\begin{longtable}{@{}>{\\hangindent=2em}p{\\linewidth}}' + "\n")
      tex.write("\n\\end{longtable}" + "\n")

    # description
    paras = func.findall('detaileddescription/para')
    paraslst = []
    for para in paras:
      # information about parameters is also in the detaileddescription, so skip that
      if para.find('parameterlist') is None:
        paraslst.append(node2tex(para))
    if paraslst:
      tex.write('\\vspace{-4mm}')
      tex.write("\\noindent\\textbf{Description}\\\\[1mm]"+ "\n")
      tex.write("\\\\[2mm]\n".join(paraslst))
    tex.write(" \n")
  tex.close()

def main():
  outdir = 'altlatex'
  if not os.path.exists(outdir):
    os.makedirs(outdir)
  for file in os.listdir('xml'):
    if file.find("group__") != 0:
      # other files(ex: structs) are recursively processed through their group
      continue
    sys.stdout.write('Processing ' + file + "...")
    process_file(file)
    print('OK')
  return

if __name__ == "__main__":
    sys.exit(main())
