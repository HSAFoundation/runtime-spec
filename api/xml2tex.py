#!/usr/bin/python

import xml.etree.ElementTree as ET
import sys
import os

# deprecated elementsd
deprecated =[]

# Enumeration types that allow duplicated initialization constants. In general,
# duplicates seem to be indicative of an erroneous declaration, but we allow
# exceptions.
initializer_whitelist = ['hsa_packet_header_t', 'hsa_packet_header_width_t']

def get_ifdef(var):
  return "\#ifdef " + var + "\\\\"

def get_elif(var):
  return "\#elif " + var + "\\\\"

def get_else():
  return "\#else\\\\"

def get_endif():
  return "\#endif\\\\"

# A replacement directive (f, r) instructs the compiler to replace the text
# associated with a set of fields 'f' using the expression 'r'. The expression
# uses integer to denote the index of the fields in 'f'. So for instance 0 is the
# first field in 'f'
#
# Replacement directives are needed becomes Doxygen does not preserve any macro
# information. Therefore, we need to rebuild the macro information by hand here.
# For example, a couple of fields [f1,f2] might have been defined in the header as
#   #IFDEF BLABLA
#     f1;
#     f2;
#   #ELSE
#     f2;
#   #ENDIF
#
# The replacement directive would force replacement of [f1,f2] with ["#IFDEF
# BLABLA", f1, f2, '#ELSE', f2, "#ENDIF"]
replacement_directives=[
(["hsa_\-queue_\-t.base_\-address", "hsa_\-queue_\-t.reserved0", ],
 ["\\\\[-2mm]" + get_ifdef("HSA_LARGE_MODEL"), 0, get_elif(" defined HSA_LITTLE_ENDIAN"), 0, 1, get_else(), 1, 0, get_endif() + "[2mm]"])
,
(["hsa_\-kernel_\-dispatch_\-packet_\-t.kernarg_\-address", "hsa_\-kernel_\-dispatch_\-packet_\-t.reserved1", ],
 ["\\\\[-2mm]" + get_ifdef("HSA_LARGE_MODEL"), 0, get_elif(" defined HSA_LITTLE_ENDIAN"), 0, 1, get_else(), 1, 0, get_endif() + "[2mm]"])
,
(["hsa_\-agent_\-dispatch_\-packet_\-t.return_\-address","hsa_\-agent_\-dispatch_\-packet_\-t.reserved1",  ],
 ["\\\\[-2mm]" + get_ifdef("HSA_LARGE_MODEL"), 0, get_elif(" defined HSA_LITTLE_ENDIAN"), 0, 1, get_else(), 1, 0, get_endif() + "[2mm]"])
]


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
    if tag in ['emphasis','computeroutput'] :
      yield '\\textit{' + mytex + '}'
    elif tag == 'ref':
      # brutal hardcoding in order to find out if it is a function name
      if mytex.startswith("hsa_\-") and not (mytex.endswith("_\-t") or mytex.endswith("_\-s")):
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

  if tag == "infin":
    yield "$\\infty$"
    return

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

def is_deprecated(func):
  paras = func.findall('detaileddescription/para')
  if len(paras) == 0:
    return False
  deprecation =  paras[0].find('xrefsect/xreftitle')
  return deprecation is not None and node2tex(deprecation) == 'Deprecated'

# precondition: is_deprecated(node) is True
def deprecation_text_node(node):
  paras = node.findall('detaileddescription/para')
  return node2tex(paras[0].find('xrefsect/xrefdescription/para'))

def separate_deprecation_from_rest(node):
  paras = node.findall('detaileddescription/para')
  rest = "" if len(paras) == 1 else node2tex(paras[1])
  return deprecation_text_node(node), rest

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
  tex.write('\\vspace{-2.5mm}')
  tex.write("\\begin{mylongtable}{p{\\columnwidth}}" + "\n")
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
    firstline = definition[0 : definition.rfind("(") + 1]
    tex.write(firstline.replace("(*", " (*")) # for some reason Doxygen eats up whitespace after return type
    tex.write("\\rule{0pt}{3ex}" + "\\\\\n")
    args = definition[definition.rfind("(") + 1 : - 1].split(",")
    args = map(format_arg, args)
    tex.write(',\\\\\n'.join(args) + "\\rule[-2ex]{0pt}{0pt}" + ");\n")

  tex.write("\\end{mylongtable}\n")

  # brief
  tex.write('\\vspace{-2.5mm}')
  # if there is more than one paragraph, it goes into detaileddescription
  paras = typedef.findall('detaileddescription/para/parblock/para')
  paras = typedef.findall('briefdescription/para') if not paras else paras
  paras = map(lambda para: node2tex(para), paras)
  tex.write("\n\\\\[2mm]".join(paras))

  defs.append(('reftyp', name))
  typename_id[name] = typedef.get('id')


# Check that any field reference matches a local field name
def check_field_refs(fields, typename):
  # references correspond to @a Doxygen tag
  refs = set(map(lambda n : n.text, fields.findall(".//emphasis")))
  fields = set(map(lambda n : n.text, fields.findall(".//memberdef[@kind='variable']/name")))
  if (refs-fields):
    sys.exit("\nError: found reference(s) to non-existing field(s): " + repr(refs-fields) + "in " + typename)

# Find the index of the replacement directive for the specified fldname
# returns -1 if not found
def replacement_index(fldname):
  for index,value in enumerate(replacement_directives):
    if fldname in value[0]:
      return index
  return -1

# Perform a replacement using the pattern at the specified index and the list value
def apply_replacement_directive(replacement_index, txts):
  ret = []
  for x in replacement_directives[replacement_index][1]:
    # if the element is an integer, replace
    if isinstance(x, (int,long)):
      x = txts[x]
    ret.append(x)
  return ''.join(ret)

def process_struct_or_union(typedef, tex, defs):
  typename = node2tex(typedef.find('name'))
  tex.write('\\subsubsection{' + typename  + '}\n')

  # deprecated warning
  depre = is_deprecated(typedef)
  if depre:
    tex.write('\\vspace{-2.5mm}')
    tex.write("\\danger\\:\\textit{Deprecated");
    explanation = deprecation_text_node(typedef)
    tex.write("" if not explanation else ": ")
    tex.write(explanation)
    tex.write("}\n")

  # name
  tex.write('\\vspace{-2.5mm}')
  tex.write("\\begin{mylongtable}{p{\\columnwidth}}" + "\n")
  tex.write("\\rule{0pt}{3ex}")
  typ = typedef.find('type').text;
  actname = typedef.find('type/ref').text
  tex.write("typedef " + typ + " " + actname + " \{\\\\\n")

  # members. Doxygen stores their info in a separate file.
  membersfile = typedef.find('type/ref').get('refid') + ".xml"
  memberstree = ET.parse(os.path.join('xml',membersfile))
  membersroot = memberstree.getroot()
  members = membersroot.findall(".//memberdef[@kind='variable']")

  check_field_refs(membersroot, typedef.find('name').text)

  vals = []
  fields = []
  replace_txts = []
  for member in members:
    name = node2tex(member.find('name'))
    fldname = typename + "." + name
    # type + field name
    txt = "\\hspace{1.7em}"
    txt += node2tex(member.find('type'))
    txt += " \\reffld{" + name + "}"
    txt += node2tex(member.find('argsstring')) # array length, if any
    bitfield = member.find('bitfield')
    if bitfield is not None:
      txt += " : " + node2tex(bitfield)
    txt += ";\\\\\n"

    # if the field name is in the replacement list, perform replacement
    replace_idx = replacement_index(fldname)
    if replace_idx >=0:
      replace_txts.append(txt)
      if len(replace_txts) == len(replacement_directives[replace_idx][0]):
        # replace all field texts at once
        vals.append(apply_replacement_directive(replace_idx, replace_txts))
        replace_txts = []
    else:
      vals.append(txt)

    # field name + description
    defs.append(('reffld', fldname))
    typename_id[fldname] = fldname

    txt = "\\hypertarget{" + fldname + "}{\\reffld{" + name + "}}\\\\"
    paras = member.findall('detaileddescription/para')
    paraslst = map(lambda para: "\\hspace{2em}" + node2tex(para), paras)
    if paraslst:
      txt += "\\\\[1.25mm]\n".join(paraslst)
    fields.append(txt)

  tex.write(''.join(vals) + "\} ")
  tex.write(" \\hypertarget{" + typedef.get('id') + "}")
  tex.write("{\\textbf{" + typename + "}}")
  tex.write("\\rule[-2ex]{0pt}{0pt}")
  tex.write("\n\\end{mylongtable}" + "\n\n")
  defs.append(('reftyp', typename))
  typename_id[typename] = typedef.get('id')

  # brief
  tex.write('\\vspace{-2.5mm}')
  tex.write(node2tex(typedef.find('briefdescription/para')))
  tex.write("\n\n")

  # data fields
  tex.write("\\noindent\\textbf{Data Fields}\\\\[-7mm]" + "\n")
  tex.write("\\begin{longtable}{@{}>{\\hangindent=2em}p{\\textwidth}}" + "\n")
  tex.write("\\\\[2mm]\n".join(fields))
  tex.write("\n\\end{longtable}" + "\n\n")

  # detailed description
  allpara = typedef.findall('detaileddescription/para')
  paras = allpara[1:] if depre else allpara
  paraslst = map(lambda para: node2tex(para), paras)
  if paraslst:
    tex.write('\\vspace{-4mm}')
    tex.write("\\noindent\\textbf{Description}\\\\[1mm]"+ "\n")
    tex.write("\\\\[2mm]\n".join(paraslst))
    tex.write(" \n")

def check_name(enumname,valname):
  if not valname.startswith(enumname[: len(enumname) - 4].upper()):
    sys.exit("\nError: enumeration constant  " + valname + " is declared within " + enumname + ", but there is a naming mismatch.")

def process_enum(enum, tex, defs):
  depre = is_deprecated(enum)

  typename = node2tex(enum.find('name'))
  # anonymous enums start with @
  anonymous =  typename.startswith("@")
  typename = '' if anonymous else typename
  section_title = node2tex(enum.find('.//simplesect[@kind="remark"]/para')) if anonymous else typename
  tex.write('\\subsubsection{' + section_title + '}\n')

  vals = []
  valsdescs = []
  initializers = []
  valnodes = enum.findall("enumvalue")
  emptydescs = 0 # number of values with empty descriptions
  for val in enum.findall("enumvalue"):
    valname = node2tex(val.find('name'))
    if not anonymous:
      check_name(typename, valname)
    typename_id[valname] = val.get('id')
    defs.append(('refenu', valname))
    valtxt = "\\hspace{1.7em}\\hypertarget{" + val.get('id') + "}{"
    valtxt += "\\refenu{" + valname + "}}"

    inittxt = node2tex(val.find('initializer'))
    utypename = unscape(typename)
    if inittxt in initializers and utypename not in initializer_whitelist:
      sys.exit("\nError: initializer '" + inittxt + "' appears twice in " + utypename + ".")
    if not is_deprecated(val):
      initializers.append(inittxt)

    valtxt += ' ' + inittxt
    valtxt = valtxt.strip()
    valdesc = "\\hspace{-2em}\\refenu{" + valname + "}"

    if is_deprecated(val):
      depdesc = "\\danger\\:\\textit{Deprecated"
      (explanation, rest) = separate_deprecation_from_rest(val)
      depdesc += "" if not explanation else ": "
      depdesc += explanation
      depdesc += ("}\\\\[1mm]" if rest else "}")
      desc = depdesc + rest
      # deprecated.append(('enumvalue', val))
    else :
      desc = node2tex(val.find('detaileddescription'))

    if desc != '':
      valdesc +=  "\\\\" + desc
    else:
      emptydescs += 1
    vals.append(valtxt)
    valsdescs.append(valdesc)

  # deprecated warning
  if depre:
    tex.write('\\vspace{-2.5mm}')
    tex.write("\\danger\\:\\textit{Deprecated");
    explanation = deprecation_text_node(enum)
    tex.write("" if not explanation else ": ")
    tex.write(explanation)
    tex.write("}\n")

  # enum box: name and values
  tex.write('\\vspace{-2.5mm}')
  tex.write("\\begin{mylongtable}{p{\\columnwidth}}" + "\n")
  tex.write("\\rule{0pt}{3ex}")
  if typename is '':
    tex.write('enum \{\\\\')
  else:
    tex.write('typedef enum \{\\\\')
    defs.append(('reftyp', typename))
    typename_id[typename] = enum.get('id')
  tex.write(",\\\\\n".join(vals) + "\\\\\n")
  tex.write("\} \\hypertarget{" + enum.get('id') + "}")
  tex.write("{\\textbf{" + typename + "}};")
  tex.write("\\rule[-2ex]{0pt}{0pt}")
  tex.write("\\end{mylongtable}")

  # brief
  tex.write('\\vspace{-2.5mm}')
  tex.write(node2tex(enum.find('briefdescription/para')))
  tex.write("\n\n")

  # value descriptions
  if emptydescs == len(valnodes):
    print "\nWarning: Values section ommited in enum " + enum.find('name').text + " (all descriptions are empty).\n"
    return
  tex.write("\\noindent\\textbf{Values}\\\\[-7mm]" + "\n")
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

# returns None if function is not a memory order variant
# otherwise, it returns the name without the memory order
def function_name_minus_memory_order(name):
  memorders = ['_\-scacquire', '_\-scacq_\-screl', '_\-relaxed', '_\-screlease']
  return next((name[:-len(x)] for x in memorders if name.endswith(x)), None)

def function_name_memory_order_group(name):
  memorders = ['_\-scacquire', '_\-scacq_\-screl', '_\-relaxed', '_\-screlease']
  for x in memorders:
      if name.endswith(x):
          return 'sc' + name[:-len(x)]
  return None

# Check that any argument reference matches an argument name in the current function
def check_argument_refs(func):
  # references correspond to @p Doxygen tag
  refs = set(map(lambda n : n.text, func.findall(".//computeroutput")))
  args = set(map(lambda n : n.text, func.findall(".//param/declname")))
  if (refs-args):
    funname = func.find('name').text
    sys.exit("\nError: found reference(s) to non-existing argument(s): " + repr(refs-args) + "in function " + funname)

# print function signature (gray box)
def print_signature(func, tex):
  ret = ""
  funid = func.get('id')
  funname = node2tex(func.find('name'))
  ret += "\n\pbox{\\textwidth}{\\hspace{1mm}\\\\[1mm]"
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
      name = "\\refarg{" + node2tex(arg.find('declname')) + "}"
      # pointers to functions require the argument to be printed inside the type
      if "(*)" in type:
        type = type.replace("(*)", " (*" + name + ")")
        argtxt += type
      elif type.endswith("*"):
        argtxt += type + name
      else:
        argtxt += type + " " + name
      argtxt += node2tex(arg.find('array')) # array length, if any
      arglst.append(argtxt)
    ret += "\\\\" + ",\\\\".join(arglst)
  ret += ");\\\\}"
  return ret

def process_function(func, tex, listings, commands, variants):
  depre = is_deprecated(func)
  map(lambda f : register_function(f, tex, listings, commands), variants)
  funname = node2tex(func.find('name'))
  tex.write('\\subsubsection{')
  if len(variants) == 1:
     tex.write(funname)
  else:
     tex.write(function_name_minus_memory_order(funname))
  tex.write('}\n')

  check_argument_refs(func)

  # deprecated warning
  if depre:
    tex.write('\\vspace{-2.5mm}')
    tex.write("\\danger\\:\\textit{Deprecated");
    explanation = deprecation_text_node(func)
    tex.write("" if not explanation else ": ")
    tex.write(explanation)
    tex.write("}\n")

  # signature
  tex.write('\\vspace{-2.5mm}')
  tex.write("\\begin{mylongtable}{p{\\columnwidth}}" + "\n")
  tex.write("\\\\[4mm]\n".join(map(lambda f : print_signature(f, tex), variants)))
  tex.write("\\end{mylongtable}\n")

  # brief
  tex.write("\\vspace{-2.5mm}")
  tex.write(node2tex(func.find('briefdescription/para')))
  tex.write("\n\n")

  # parameters
  args = func.findall(".//parameterlist[@kind='param']/parameteritem")
  if args:
    tex.write("\\hspace*{-.3mm}\\textbf{Parameters}\\\\[-7mm]" + "\n")
    tex.write("\\noindent\\begin{longtable}{@{}>{\\hangindent=2em}p{\\textwidth}}" + "\n")
    arglst = []
    for arg in args:
      argnamenode = arg.find('./parameternamelist/parametername')
      argtxt = "\\refarg{" + node2tex(argnamenode) + "}" + "\\\\"
      direction = argnamenode.get('direction').replace("inout", "in, out")
      direction = '' if direction is None else "(" + direction + ") "

      # if the description contains several paragraphs, they show up within a single
      # parblock
      paras = arg.findall('parameterdescription/para/parblock/para')
      paras = arg.findall('parameterdescription/para') if not paras else paras
      paraslst = map(lambda para: "\\hspace{2em}" + node2tex(para), paras)
      argtxt += "\\\\[2mm]\n".join(paraslst)
      argtxt = argtxt.replace("\\hspace{2em}", "\\hspace{2em}" + direction, 1)
      arglst.append(argtxt)

    tex.write("\\\\[2mm]\n".join(arglst))
    tex.write("\n\\end{longtable}" + "\n\\vspace{-2mm}")

  # return values
  rets = func.findall(".//parameterlist[@kind='retval']/parameteritem")
  if rets:
    tex.write("\\textbf{Return Values}\\\\[-7mm]" + "\n")
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
    tex.write("\n\\end{longtable}" + "\n\\vspace{-2mm}")

  # returns/return description
  ret = func.find("detaileddescription/para/simplesect[@kind='return']")
  if ret is not None:
    tex.write("\\noindent\\textbf{Returns}\\\\[1mm]"+ "\n")
    tex.write(node2tex(ret) + "\n\n")

  # detailed description
  allpara = func.findall('detaileddescription/para')
  paras = allpara[1:] if depre else allpara
  paraslst = []
  for para in paras:
    # information about parameters is also in the detaileddescription, so skip that
    if para.find('parameterlist') is None and para.find("simplesect[@kind='return']") is None:
      paraslst.append(node2tex(para))
  if paraslst:
    tex.write("\\noindent\\textbf{Description}\\\\[1.5mm]"+ "\n")
    tex.write("\\\\[2mm]\n".join(paraslst))
  tex.write(" \n")

def function_variants(currfunc, funcs):
  ret = [currfunc]
  currname = node2tex(currfunc.find('name'))
  currpre = function_name_memory_order_group(currname)
  if currpre is not None:
    for func in funcs:
      name = node2tex(func.find('name'))
      pre = function_name_memory_order_group(name)
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

def print_deprecated_table_type(tex, type):
    tex.write('\\begin{itemize}')
    for _, node in filter(lambda (t, _) : t == type, deprecated):
      tex.write('\\item ')
      tex.write(node2tex(node.find('name')))
      tex.write('\\\\')
      tex.write(deprecation_text_node(node))
    tex.write('\\end{itemize}')


def print_deprecated_table():
  with open(os.path.join("altlatex", 'deprecated.tex'), "w+") as tex:
    tex.write('\\section{Enumerations}')
    print_deprecated_table_type(tex, 'enum')
    tex.write('\\section{Enumeration Constants}')
    print_deprecated_table_type(tex, 'enumvalue')
    tex.write('\\section{Functions}')
    print_deprecated_table_type(tex, 'func')
    tex.write('\\section{Structs and Unions}')
    print_deprecated_table_type(tex, 'structunion')

# Convert an identifier to a link its declaration in the document (where the
# displayed name
def id2link(id, type):
  refid = typename_id[id]
  if type is 'reffld':
    text = '\\' + type + '{' + id[id.index(".") + 1:] + '}'
  else:
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
  if sys.version_info[0] >= 3:
    sys.exit("Error: Your Python interpreter must be < 3.0\n")

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

  # print deprecation info
  # print_deprecated_table()
  return


if __name__ == "__main__":
    sys.exit(main())
