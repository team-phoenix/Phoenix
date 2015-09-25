# -*- coding: utf-8 -*-
from __future__ import print_function
from tarfile import TarFile
from cStringIO import StringIO
import urllib2
import shlex
from collections import OrderedDict
import json
import sys
import os
from datetime import datetime

DIR_PREFIX = 'libretro-super-master/dist/info/'

res = urllib2.urlopen('https://github.com/libretro/libretro-super/archive/master.tar.gz')

archive = StringIO(res.read())

tf = TarFile.open(mode='r:gz', fileobj=archive)
# filter files and sort them
files = [f for f in tf.getmembers() if f.name.startswith(DIR_PREFIX)]
files = sorted(files, key=lambda m: m.name.split(DIR_PREFIX)[1])

output = {'cores': OrderedDict()}

for f in files:
    info = OrderedDict()
    for l in tf.extractfile(f).readlines():
        s = shlex.shlex(l)
        s.quotes = '"'
        op = []
        try:
            while True:
                t = s.get_token()
                if t == s.eof:
                    break
                op.append(t)

                if len(op) == 3: # 'var' '=' 'value'
                    assert op[1] in ('=', ':')
                    if op[1] == ':': # for: 'some:var = value' syntax
                        op = [''.join(op)]
                        continue

                    if op[2][0] == '"' and op[2][-1] == '"':
                        op[2] = op[2][1:-1]

                    if op[2] in ('true', 'True'):
                        op[2] = True
                    elif op[2] in ('false', 'False'):
                        op[2] = False

                    if isinstance(op[2], str):
                        op[2] = op[2].decode('utf-8')

                    info[op[0]] = op[2]
                    break

        except (ValueError, AssertionError) as e:
            print("Could not process file %s (%s) " % (f.name, e), file=sys.stderr)
            continue

    output['cores'][f.name.split(DIR_PREFIX)[1].split('.info')[0]] = info

#print(json.dumps(output, indent=4))

def cpprepr(s):
    if isinstance(s, bool):
        return repr(s).lower()

    ret = []
    position = 0

    ret.append('"')
    position += 1
    for c in s:
        newline = False
        if c == "\n":
            to_add = "\\\n"
            newline = True
        elif ord(c) >= 0xa0:
            to_add = "\\u%04x" % ord(c)
        elif ord(c) < 32 or 0x80 <= ord(c) <= 0xff:
            to_add = "\\x%02x" % ord(c)
        elif "\\\"".find(c) != -1:
            to_add = "\\%c" % c
        else:
            to_add = c

        ret.append(to_add)
        position += len(to_add)
        if newline:
            position = 0


    ret.append('"')

    return "".join(ret)

# [1] Create coresInfoMap
initializer_list = []
for k, v in output['cores'].iteritems():
    if k == 'example':
        continue
    initializer_list.append("""    { %s, {
%s
    } }""" % (
        cpprepr(k), 
        ',\n'.join(['        { %s, %s }' % (cpprepr(k2), cpprepr(v2)) for k2, v2 in v.iteritems()])
    ))

fileString = """
#include <QVariant>
#include <QMap>

// this file is machine generated, DO NOT EDIT
// last generated at %s UTC

namespace Library {

    const QMap<QString, QVariantMap> coresInfoMap {
    %s
    };

}""" % (datetime.utcnow().replace(microsecond=0), ',\n'.join(initializer_list))

os.chdir("../cpp/library")
outputFile = "coresinfomap.h"
with open( outputFile, "w") as file:
    file.write( fileString )
    print("Created " + file.name )

# ~[1]

# [2] Create cores.h and cores.cpp files
caseList = []
enumList = []
count = 0
whitespace = "            "
for k, v in output['cores'].iteritems():
    if k == 'example':
        continue
    enumVal = "CORE_" + k.upper()
    tempStr = whitespace + enumVal + " = " + str(count) + ","
    count = count + 1
    enumList.append( tempStr )

    splitName = k.replace('_libretro', '').lower().split("_")

    caseList.append( '''
    case %s:
        return QStringLiteral( "%s" );''' % (enumVal
                                             , ' '.join( splitName ).title()))

enumList.append( whitespace + "CORE_MAX = " + str(count) + "," )

enumFile = "cores.h"

enumStr_cpp = '''
#include "%s"

// this file is machine generated, DO NOT EDIT
// last generated at %s UTC

using namespace Library;

const QString Cores::toString( const LibretroCore &core ) {
    switch ( core ) {
    %s
    default:
        // This default case should never be called
        break;
    }
    Q_ASSERT( false );
    return QStringLiteral( "" );
}''' % (enumFile, datetime.utcnow().replace(microsecond=0), '\n'.join(caseList))

enumStr_header = '''
#include <QObject>
#include <QString>

// this file is machine generated, DO NOT EDIT
// last generated at %s UTC

namespace Library {

    class Cores : public QObject {
        Q_OBJECT
    public:
        enum LibretroCore {
            CORE_INVALID = -1,
%s
        };
        Q_ENUMS( LibretroCore )

        static const QString toString( const LibretroCore &core );
    };

}''' % ( datetime.utcnow().replace(microsecond=0), '\n'.join(enumList) )

# write to cores.h
with open( enumFile, "w" ) as file:
    file.write( enumStr_header )
    print("Created " + file.name )

# write to cores.cpp
with open( enumFile.replace('.h', '.cpp'), 'w') as file:
    file.write( enumStr_cpp )
    print("Created " + file.name )
# ~[2]

print( "done." )
