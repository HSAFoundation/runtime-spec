#!/usr/bin/python

import xml.etree.ElementTree as ET
import sys
import os

# map from a type name to its unique ID. This is necessary because Doxygen does
# not generate enough information for 'typedefs' such that we can build internal
# links. For instance, if there is a pointer to a function defined as
# void (*some_name)(hsa_status_t status), then there is no metainformation
# stating that the argument 'status' is indeed of type hsa_status: it is up to
# us to infer it. Since we are only interested in detecting HSA types, and all
# of them follow the hsa_XXX_t naming convention, we store a pair (name, ID)
# when they are declared, and use it when processing 'typedefs'.
typename_id = {}


def text2tex(text):
  # add hyphenation break hints at underscores, avoids overfull boxes
  return text.replace('_','_\-')

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
      if mytex.startswith("hsa_\-") and not mytex.endswith("_\-t"):
        mytex = "\\reffun{" + mytex + "}"
      yield '\\hyperlink{' + self.get('refid') + '}{' + mytex + '}'
    else:
      yield mytex

  # consume paragraphs
  paras = self.findall('./para')
  paraslst = []
  for para in paras:
    # consume iterator
    txt =''
    for s in myitertext(para):
      txt += s
    paraslst.append(txt)
  if paraslst:
    yield "\\\\[2mm]\n".join(paraslst)
    return

  # consume lists
  if tag == 'itemizedlist':
    txt = ''
    for e in self:
      txt += '\\item '
      for s in myitertext(e):
        txt += s
      txt += '\n'
    yield '\\begin{itemize}' + txt + '\\end{itemize}'
    return
  paraslst = []
  for para in paras:
    # consume iterator
    txt =''
    for s in myitertext(para):
      txt += s
    paraslst.append(txt)
  if paraslst:
    yield "\n".join(paraslst)

  for e in self:
    if e in paras:
      continue
    for s in myitertext(e):
      yield s
    # TODO(mmario): when does the tail exist?
    if e.tail:
      yield e.tail


# return Latex string corresponding to XML node
def node2tex(node):
  if node is None:
    return ''
  return ''.join(myitertext(node)).encode("utf-8").strip()

def unscape(text):
  return text.replace("_\-","_")

def linkify(definition):
  ret = definition
  done = [] # types already processed
  start = definition.find('hsa_', 0)
  while start != -1:
    end = definition.find('_\-t', start) + len('_\-t')
    candidate = definition[start:end]
    refid = typename_id.get(candidate)
    if (candidate not in done) and (refid is not None):
      ret = ret.replace(candidate, '\\hyperlink{' + refid + '}{' + candidate + '}')
      done += candidate
    start = definition.find('hsa_', end)
  return ret

def format_arg(line):
  argname_start = line.rfind(' ') + 1
  argname = " \\reffld{" + line[argname_start : ] + "}"
  ret = line[: argname_start] + argname
  return "\\hspace{1.7em}" + ret.strip()

def process_typedef(typedef, tex, defs):
  name = node2tex(typedef.find('name'))
  if name in ['hsa_\-signal_\-value_\-t']:
    #blacklist
    return
  tex.write('\\subsubsection{' + name + '}\n')
  # typedef definition. We use the 'definition' string because in the presence
  # of pointers to functions using the type itself is tricky.
  tex.write("\\vspace{-5.5mm}\\begin{mylongtable}{@{}p{\\textwidth}}" + "\n")
  definition = node2tex(typedef.find('definition'))
  definition = linkify(definition)
  newname = " \\hypertarget{" + typedef.get('id') + "}{\\textbf{" + name + "}}"
  definition = definition.replace(name, newname, 1)

  if node2tex(typedef.find('argsstring')) == '':
    tex.write("\\rule{0pt}{3ex}")
    tex.write("\\rule[-2.5ex]{0pt}{0pt}")
    tex.write(definition + ";\n")
  else:
    # pointer to function
    tex.write(definition[0 : definition.rfind("(") + 1])
    tex.write("\\rule{0pt}{3ex}" + "\\\\\n")
    args = definition[definition.rfind("(") + 1 : - 1].split(",")
    args = map(format_arg, args)
    tex.write(',\\\\\n'.join(args) + "\\rule[-2ex]{0pt}{0pt}" + ");\n")

  tex.write("\\end{mylongtable}\\vspace{-3mm}\n")
  defs.append(('reftyp', name))
  typename_id[name] = typedef.get('id')
  # brief
  tex.write('\\vspace{-2mm}')
  paras = typedef.findall('briefdescription/para')
  map(lambda para: tex.write(node2tex(para) + "\n\\\\"), paras)

def process_struct_or_union(typedef, tex, defs):
  typename = node2tex(typedef.find('name'))
  tex.write('\\subsubsection{' + typename + '}\n')
  # name
  tex.write("\\vspace{-5.5mm}\\begin{mylongtable}{@{}p{\\textwidth}}" + "\n")
  typ = typedef.find('type').text;
  actname = typedef.find('type/ref').text
  tex.write("typedef " + typ + " " + actname + " \{\\\\\n")
  # members. Doxygen stores their info in a separate file.
  membersfile = typedef.find('type/ref').get('refid') + ".xml"
  memberstree = ET.parse(os.path.join('xml',membersfile))
  membersroot = memberstree.getroot()
  members = membersroot.findall(".//memberdef[@kind='variable']")
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
    vals.append(txt + ";\\\\\n")
    # field name + description
    txt = "\\reffld{" + name + "}" + "\\\\"
    paras = member.findall('detaileddescription/para')
    paraslst = map(lambda para: "\\hspace{2em}" + node2tex(para), paras)
    if paraslst:
      txt += "\\\\\n".join(paraslst)
    fields.append(txt)
  tex.write(''.join(vals) + "\} ")
  tex.write(" \\hypertarget{" + typedef.get('id') + "}")
  tex.write("{\\textbf{" + typename + "}}")
  tex.write("\n\\end{mylongtable}" + "\n\n")
  defs.append(('reftyp', typename))
  typename_id[typename] = typedef.get('id')

  # brief
  tex.write('\\vspace{-2mm}')
  tex.write(node2tex(typedef.find('briefdescription/para')) + "\n\n")
  # data fields
  tex.write("\\noindent\\textbf{Data Fields}\\\\[-6mm]" + "\n")
  tex.write("\\begin{longtable}{@{}>{\\hangindent=2em}p{\\textwidth}}" + "\n")
  tex.write("\\\\[2mm]\n".join(fields))
  tex.write("\n\\end{longtable}" + "\n\n")

  # detailed description
  paras = typedef.findall('detaileddescription/para')
  paraslst = map(lambda para: node2tex(para), paras)
  if paraslst:
    tex.write('\\vspace{-4mm}')
    tex.write("\\noindent\\textbf{Description}\\\\[1mm]"+ "\n")
    tex.write("\\\\[2mm]\n".join(paraslst))
    tex.write(" \n")

def process_enum(enum, tex, defs):
  typename = node2tex(enum.find('name'))
  tex.write('\\subsubsection{' + typename + '}\n')

  vals = []
  valsdescs = []
  for val in enum.findall("enumvalue"):
    valname = node2tex(val.find('name'))
    typename_id[valname] = val.get('id')
    defs.append(('refenu', valname))
    valtxt = "\\hspace{1.7em}\\hypertarget{" + val.get('id') + "}{"
    valtxt += "\\refenu{" + valname + "}}"
    valtxt += ' ' + node2tex(val.find('initializer'))
    valtxt = valtxt.strip()
    valdesc = "\\hspace{-2em}\\refenu{" + valname + "}"
    desc = node2tex(val.find('detaileddescription'))
    if desc != '':
      valdesc +=  "\\\\" + desc
    vals.append(valtxt)
    valsdescs.append(valdesc)

  # enum box: name and values
  tex.write("\\vspace{-5.5mm}\\begin{mylongtable}{@{}p{\\textwidth}}" + "\n")
  tex.write("\\rule{0pt}{3ex}")
  tex.write('typedef enum \{\\\\')
  tex.write(",\\\\\n".join(vals) + "\\\\\n")
  tex.write("\} \\hypertarget{" + enum.get('id') + "}")
  tex.write("{\\textbf{" + typename + "}};")
  tex.write("\\rule[-2ex]{0pt}{0pt}")
  tex.write("\\end{mylongtable}\n")
  defs.append(('reftyp', typename))
  typename_id[typename] = enum.get('id')

  # brief
  tex.write('\\vspace{-2mm}')
  tex.write(node2tex(enum.find('briefdescription/para')) + "\n\n")

  # value descriptions
  tex.write("\\noindent\\textbf{Values}\\\\[-5mm]" + "\n")
  tex.write('\\begin{longtable}{@{\\hspace{2em}}p{\\linewidth-2em}}' + "\n")
  tex.write("\\\\[2mm]\n".join(valsdescs))
  tex.write("\n\\end{longtable}")

def register_function(func, tex, listings, commands):
  funid = func.get('id')
  funname = node2tex(func.find('name'))
  unescaped_funname = unscape(funname)
  index = 'ext' if funname.startswith('hsa_\-ext_\-') else 'api';
  tex.write("\\index[" + index + "]{" + unescaped_funname + "}") # add function name to function API index
  listings.write(unescaped_funname + ",") # add function name to listings keyword list
  commands.append(('reffun', funname))
  typename_id[funname] = funid

# print function signature (gray box)
def print_signature(func, tex):
  ret = ""
  funid = func.get('id')
  funname = node2tex(func.find('name'))
  ret += "\n\pbox{\\textwidth}{"
  # return type
  rettype = node2tex(func.find('type'))
  rettype = rettype.replace(' HSA_\-API','') # macros are passed as part of the type, so we have to delete them manually
  rettype = rettype.replace(' HSA_API','')
  ret += rettype + " "
  # func name
  ret += "\\hypertarget{" + funid + "}"
  ret += "{\\textbf{" + funname + "}}("
  # parameters
  sigargs = func.findall("param")
  if sigargs:
    arglst = []
    for arg in sigargs:
      argtxt = "\\hspace*{1.7em}"
      type = node2tex(arg.find('type'))
      name = "\\hsaarg{" + node2tex(arg.find('declname')) + "}"
      # pointers to functions require the argument to be printed inside the type
      if "(*)" in type:
        type = type.replace("(*)", " (" + name + "*)")
        argtxt += type
      else:
        argtxt += type + " " + name
      argtxt += node2tex(arg.find('array')) # array length, if any
      arglst.append(argtxt)
    ret += "\\\\" + ",\\\\".join(arglst)
  ret += ");}"
  return ret

# returns None if function is not a memory order variant
# otherwise, it returns the name without the memory order
def function_name_minus_memory_order(name):
  memorders = ['_\-acquire', '_\-acq_\-rel', '_\-relaxed', '_\-release']
  return next((name[:-len(x)] for x in memorders if name.endswith(x)), None)

def process_function(func, tex, listings, commands, variants):
  map(lambda f : register_function(f, tex, listings, commands), variants)
  funname = node2tex(func.find('name'))
  tex.write('\\subsubsection{')
  if len(variants) == 1:
     tex.write(funname)
  else:
     tex.write(function_name_minus_memory_order(funname))
  tex.write('}\n')

  # signature
  tex.write("\\vspace{-5.5mm}\\begin{mylongtable}{@{}p{\\textwidth}}" + "\n")
  tex.write("\\rule{0pt}{5ex}")
  tex.write("\\\\[8mm]\n".join(map(lambda f : print_signature(f, tex), variants)))
  tex.write("\\rule[-4ex]{0pt}{0pt}")
  tex.write("\\end{mylongtable}\n")
  # brief
  tex.write('\\vspace{-2mm}')
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
    tex.write("\n\\end{longtable}" + "\n\\vspace{-5mm}")

  # return values
  rets = func.findall(".//parameterlist[@kind='retval']/parameteritem")
  if rets:
    tex.write("\\noindent\\textbf{Return Values}\\\\[-6mm]" + "\n")
    tex.write('\\noindent\\begin{longtable}{@{}>{\\hangindent=2em}p{\\linewidth}}' + "\n")
    arglst = []
    for ret in rets:
      argtxt = node2tex(ret.find('parameternamelist/parametername/ref'))
      retdesc = node2tex(ret.find('parameterdescription'))
      # allow empty return value descriptions
      if retdesc != "":
        argtxt +=  "\\\\" + "\\hspace{2em}" + retdesc
      arglst.append(argtxt)
    tex.write("\\\\[2mm]\n".join(arglst))
    tex.write("\n\\end{longtable}" + "\n\\vspace{-5mm}")

  # returns/return description
  ret = func.find("detaileddescription/para/simplesect[@kind='return']")
  if ret is not None:
    tex.write("\\noindent\\textbf{Returns}\\\\[1mm]"+ "\n")
    tex.write(node2tex(ret) + "\n\n")

  # detailed description
  paras = func.findall('detaileddescription/para')
  paraslst = []
  for para in paras:
    # information about parameters is also in the detaileddescription, so skip that
    if para.find('parameterlist') is None and para.find("simplesect[@kind='return']") is None:
      paraslst.append(node2tex(para))
  if paraslst:
    tex.write("\\noindent\\textbf{Description}\\\\[1mm]"+ "\n")
    tex.write("\\\\[2mm]\n".join(paraslst))
  tex.write(" \n")

def function_variants(currfunc, funcs):
  ret = [currfunc]
  currname = node2tex(currfunc.find('name'))
  currpre = function_name_minus_memory_order(currname)
  if currpre is not None:
    for func in funcs:
      name = node2tex(func.find('name'))
      pre = function_name_minus_memory_order(name)
      if pre == currpre and func != currfunc:
        ret += [func]
  return ret

def process_file(file, listings, defs):
  tree = ET.parse(os.path.join('xml',file))
  root = tree.getroot()
  texfilename = (os.path.splitext(file)[0] + ".tex").replace("__", "-")
  tex = open(os.path.join("altlatex", texfilename), "w+")
  memberdefs = root.findall(".//memberdef")

  # Doxygen sorts members by type (e.g. enums appear first and all together)
  # Instead, we sort them according to location (source line number)
  memberdefs.sort(key=lambda memberdef: int(memberdef.find('location').get('line')))
  funcdefs = filter(lambda x : x.get('kind') == 'function', memberdefs)
  processed_funcs = []

  # main processing loop
  for memberdef in memberdefs:
    tex.write("\n\n")
    k = memberdef.get('kind')
    if k == 'typedef':
      typetext = node2tex(memberdef.find('type'));
      if typetext[:5] not in ['struc','union']:
        process_typedef(memberdef, tex, defs)
      else:
        process_struct_or_union(memberdef, tex, defs)
    elif k == 'enum':
      process_enum(memberdef, tex, defs)
    elif k == 'function' and memberdef not in processed_funcs:
      variants = function_variants(memberdef, funcdefs)
      process_function(memberdef, tex, listings, defs, variants)
      processed_funcs += variants

  tex.close()

# Convert an identifier to a link its declaration in the document (where the
# displayed name
def id2link(id, type):
  refid = typename_id[id]
  text = '\\' + type + '{' + id + '}'
  return '\\hyperlink{' + refid + '}{' + text + '}'

def generate_hsaref(defs):
  hsaref = '\\makeatletter\n\\newcommand{\\hsaref}[1]{'
  # create random 'if' so we can treat all the actual definitions in the same way
  hsaref += '\\ifnum\\pdf@strcmp{#1}{blablablablabla}=0 blablablablabla\n'
  # definitions
  defs = map(lambda x: '\\else\ifnum\\pdf@strcmp{#1}{' + x[1].replace('_\-', '_') + '}=0' + id2link(x[1], x[0]), defs)
  hsaref += ''.join(defs)
  # reference not found
  hsaref += '\\else\\errmessage{Unknown reference: #1. Declaration not found in hsa.h}\n'
  hsaref += '\\fi' * (1 + len(defs))
  hsaref += '}\n\makeatother\n'
  return hsaref

# Files (groups) are processed according to the header line number where they
# start. This simplifies the processing since in the common case every
# referenced entity has already been defined.
#
# If groups definitions are scattered across the header (i.e. \addgroup is used)
# then this logic is broken
#
def group_location(file):
  tree = ET.parse(os.path.join('xml',file))
  root = tree.getroot()
  line = int(root.find(".//location").get('line'))
  # groups in hsa_ext.h are processed after groups in hsa.h
  header = os.path.basename(root.find(".//location").get("file"))
  if header == "hsa_ext.h":
    return line + 100000
  return line

def main():
  outdir = 'altlatex'
  if not os.path.exists(outdir):
    os.makedirs(outdir)
  commands = open(os.path.join("altlatex", "commands.tex"), "w+")
  defs = []
  listings = open(os.path.join("altlatex", "listings.tex"), "w+")
  listings.write("\\lstset{emph={")
  # other files(ex: structs) are recursively processed through their group
  files = filter(lambda file: file.find("group__") == 0, os.listdir('xml'))
  # process groups according to their line number location
  files.sort(key=lambda file: group_location(file))
  for file in files:
    sys.stdout.write('Processing ' + file + "...")
    process_file(file, listings, defs)
    print('OK')
  listings.write("}}")
  listings.close()
  commands.write(generate_hsaref(defs))
  commands.close()
  return


if __name__ == "__main__":
    sys.exit(main())
