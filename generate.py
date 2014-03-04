#!/usr/bin/python
#
# I have never written a python script or used python before -- VT
#
import re

print "begin processing"

corebase = open("corebase.h", "r")
grp = re.compile(r'addtogroup') 

funclistfile = open("funclistfile", "w")
structlistfile = open("structlistfile", "w")

while True:
  line = corebase.readline()
# print '1:'+line
  if re.match('#endif /\*HSA',line):
    break
  matchgrp = re.search(grp,line)
  if matchgrp:
    texfilename = line.split(' ')[2]
    texfile = open(texfilename+'.tex', "w")
    tempfile = open("temp", "w+")
    tempfile.write("\\begin{framed}\n")
    tempfile.write("\\begin{description}[style=nextline,font=\\tt ] \n")
    files = [texfilename, 'temp']

    if texfilename[0] == 'S':
#     texfile.write("\\begin{framed}\n")
      texfile.write("\\begin{lstlisting}\n")
      while True:
        line = corebase.readline()
#       print '2:'+line
        if re.search("typedef struct|union", line):
          texfile.write(line)
          break
      structname = line.split()[2]
      structname = structname.split('{')[0]
      structname_t = structname[:-1]+'t'
      structlistfile.write(structname_t+', ')
      print '\nprocessing struct '+structname_t
      while True:
        comment = ' '
        line = corebase.readline()
#       print '3:'+line
        if re.search('/\*\*', line):
          while True:
            line = corebase.readline()
#           print '4:'+line
            if re.search('\*/', line):
              line = corebase.readline()
              print '5:'+line
              keyw = line.split()[1]
              keyw = keyw.split(';')[0]
              keyw = keyw.strip('*')
              keyw = keyw.replace(']','\\rbrack')
              print '  strip processing element '+keyw
              texfile.write(line)
              descr = '    \\item['+keyw.replace('_','\_')+']  '
              tempfile.write(descr+comment)
              break
            else:
              line = re.sub(r'\*', ' ', line)
              comment += line
        elif re.search(structname_t,line):
            texfile.write(line)
            texfile.write("\\end{lstlisting}\n")
#           texfile.write("\\end{framed}\n")
            tempfile.write('\\end{description}\n')
            tempfile.write("\\end{framed}\n")
            tempfile.seek(0,0)
            texfile.write(tempfile.read())
            break
        else:
          continue
    elif texfilename[0] == 'E':
#     texfile.write("\\begin{framed}\n")
      texfile.write("\\begin{lstlisting}\n")
      while True:
        line = corebase.readline()
#       print '2:'+line
        if re.search('typedef enum', line):
          texfile.write(line)
          break
      structname = line.split()[2]
      structname = structname.split('{')[0]
      structname_t = structname[:-1]+'t'
      structlistfile.write(structname_t+',')
      print '\nprocessing enum '+structname_t
      while True:
        comment = ' '
        line = corebase.readline()
#       print '3:'+line
        if re.search('/\*\*', line):
          while True:
            line = corebase.readline()
#           print '4:'+line
            if re.search('\*/', line):
              line = corebase.readline()
              print '5:'+line
              keyw = line
              keyw = keyw.split(',')[0]
              keyw = keyw.strip('\n')
              keyw = keyw.strip()
              print '  processing element '+keyw
              texfile.write(line)
              descr = '    \\item['+keyw.replace('_','\_')+']  '
              tempfile.write(descr+comment)
              break
            else:
              line = re.sub(r'\*', ' ', line)
              comment += line
        elif re.search(structname_t,line):
            texfile.write(line)
            texfile.write("\\end{lstlisting}\n")
#           texfile.write("\\end{framed}\n")
            tempfile.write('\\end{description}\n')
            tempfile.write("\\end{framed}\n")
            tempfile.seek(0,0)
            texfile.write(tempfile.read())
            break
        else:
          continue
    elif texfilename[0] == 'A':
      while True:
        line = corebase.readline()
#       print '6:'+line
        if re.search('@}', line):
          break
        paramkeyw = ' '
        paramcomment = ' '
        retvalcomment = ' '
        retvalkeyw = ' '
        signature = ' '

        def endprocess( ):
          global line
          global re
          global signature
          global corebase
          global texfile 
          global tempfile
          line = corebase.readline()
#         print '10:'+line
          if re.search('\(', line):
            funcname = line.split('(')[0]
            funcname = funcname.split()[1]
            funclistfile.write(funcname+', ')
            print 'processing function '+funcname
            print 'line is '+line
            leftpar = re.search('\(', line).start()
            rightpar = 0
            if re.search('\)', line):
              rightpar = re.search('\)', line).start()
            if (rightpar != 0) and ((rightpar-leftpar) <=2):
              signature += line
              texfile.write("\\begin{lstlisting}\n")
              texfile.write(signature)
              texfile.write("\\end{lstlisting}\n")
            else:
              while True:
                signature += line
                if re.search('\);', line):
                  break
                else:
                  line = corebase.readline()
#                 print '11:'+line
              print 'processed function '+signature+'\n'
#             texfile.write("\\begin{framed}\n")
              texfile.write("\\begin{lstlisting}\n")
              texfile.write(signature)
              texfile.write("\\end{lstlisting}\n")
#             texfile.write("\\end{framed}\n")
              tempfile.write('\\end{description}\n')
              tempfile.write('\\end{framed}\n')
              tempfile.seek(0, 0)
              texfile.write(tempfile.read())
              tempfile.seek(0, 0)
              tempfile.truncate()
              tempfile.write("\\begin{framed}\n")
              tempfile.write('\\begin{description}[style=nextline,font=\\tt ] \n')

        def retprocess( ):
          global line
          global re
          global signature
          global corebase
          global texfile 
          global tempfile
          global retvalkeyw
          global retvalcomment
          retvalkeyw = (line.split('@retval ')[1]).rstrip()
#         print '8:'+line
          while True:
            last_pos = corebase.tell()
            line = corebase.readline()
#           print '9:'+line
            if re.search('\*/', line):
              descr = '    \\item['+retvalkeyw.replace('_','\_')+']  '
              tempfile.write(descr+retvalcomment)
              corebase.seek(last_pos)
              break
            else:
              repline = line.replace('*',' ')
              retvalcomment += repline

        def paramprocess( ):
          global line
          global re
          global signature
          global corebase
          global texfile 
          global tempfile
          global paramkeyw
          global paramcomment
          paramkeyw = (line.split('@param ')[1]).rstrip()
          paramkeyw = paramkeyw.strip('*')
          print '  processing parameter '+paramkeyw
          while True:
            last_pos = corebase.tell()
            line = corebase.readline()
#           print '7.1:'+line
            if re.search('@param', line):
              descr = '    \\item['+paramkeyw.replace('_','\_')+']  '
              tempfile.write(descr+paramcomment)
              corebase.seek(last_pos)
              break
            if re.search('@retval', line):
              descr = '    \\item['+paramkeyw.replace('_','\_')+']  '
              tempfile.write(descr+paramcomment)
              corebase.seek(last_pos)
              break
            elif re.search('\*/', line):
              descr = '    \\item['+paramkeyw.replace('_','\_')+']  '
              tempfile.write(descr+paramcomment)
              corebase.seek(last_pos)
              break
            else:
              repline = line.replace('*',' ')
              paramcomment += repline
          paramcomment = ' '

        if re.search('/\*\*', line):
          while True:
            line = corebase.readline()
#           print '7:'+line
            if re.search('@param',line):
              paramprocess()
            elif re.search('@retval', line):
              retprocess()
            elif re.search('\*/', line):
              endprocess()
              break 
            else:
              continue

    else:             
      print line+" unknown, neither API nor STRUCT"

    tempfile.close()
    texfile.close()

corebase.close()
funclistfile.close()
structlistfile.close()

funclistfile = open("funclistfile", "r")
structlistfile = open("structlistfile", "r")
of = open("corebase.tex","w")
funclist = funclistfile.read()
structlist = structlistfile.read()
funclistfile.close()
structlistfile.close()
funclist = funclist[:-2]
structlist = structlist[:-2]

import fileinput
for line in fileinput.FileInput("corebasetemplate.tex"):
      newline = line.replace("funcfilelist",funclist)
      if newline == line:
        newline = line.replace("structfilelist",structlist)
      of.write(newline)

fileinput.close()
of.close()
