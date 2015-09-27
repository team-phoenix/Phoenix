# -*- coding: utf-8 -*-
from __future__ import print_function
from tarfile import TarFile
from cStringIO import StringIO
from collections import OrderedDict
from datetime import datetime

import json, sys, os, shlex, urllib2
from sqldatabase import SqlDatabase

def prettify(platform):
    system = platform.lower()
    if "colecovision" in system:
        return "ColecoVision"
    elif "gamecube" in system:
        return "GameCube (Wii)"
    elif "game boy" in system:
        return "GameBoy (Color)"
    elif "sega 8/16" in system:
        return "Sega 8 & 16 Bit"
    elif "atari st/ste" in system:
        return "Atari ST"
    elif "pc engine/pce" in system:
        return "PC Engine"
    elif "multi" in system:
        return "Arcade"
    elif "(various)" in system:
        return platform.replace(" (various)", "").title()
    elif "/ videopac+" in system:
        return " & ".join(platform.split(" / "))
    elif "saturn" == system:
        return "Sega Saturn"
    return platform

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

initializer_list = []
for k, v in output['cores'].iteritems():
    initializer_list.append("""    { %s, {
%s
    } }""" % (
        cpprepr(k), 
        ',\n'.join(['        { %s, %s }' % (cpprepr(k2), cpprepr(v2)) for k2, v2 in v.iteritems()])
    ))

metadataTable = "coreMetadata"
systemTable = "systems"
defaultSystemTable = "defaultSystems"
biosTable = "firmware"
headerOffsetsTable = "headerOffsets"
schemaVersion = 0

dbFile = os.path.join(os.getcwd().replace("python", "database"), 'systems.db')

if os.path.isfile( dbFile ):
    os.remove( dbFile )

with SqlDatabase(dbFile, autoCommit=True) as db:
    
    rowsDict = OrderedDict({"coreIndex": "INTEGER PRIMARY KEY AUTOINCREMENT"})

    rows = ["core", "supported_extensions", "display_version"
            , "license", "authors", "manufacturer"]

    for i in rows:
        rowsDict[i] = "TEXT"


    db.createTable( "schema_version", {"version": "INTEGER NOT NULL"} )
    db.execute( "schema_version", ["version"], [schemaVersion])

    db.createTable( metadataTable, rowsDict )
    db.createTable(defaultSystemTable, {"systemIndex": 
                                        "INTEGER PRIMARY KEY AUTOINCREMENT"
                                        , "systemname": "TEXT"}, )
    db.createTable(biosTable, {"biosFile": "TEXT", "sha1": "TEXT"})
    db.createTable(headerOffsetsTable, {"offset": "TEXT NOT NULL"
                                        , "lengthInBytes": "INTEGER NOT NULL"
                                        , "result": "TEXT NOT NULL"})
    
    coresRows = ["systemname"]
    coresRowsDict = OrderedDict({"coreIndex": "INTEGER PRIMARY KEY AUTOINCREMENT"})
    
    for row in coresRows:
        coresRowsDict[row] = "TEXT"

    db.createTable(systemTable, coresRowsDict )

    for k, v in output['cores'].iteritems():

        if "categories" not in v or v["categories"] != "Emulator":
            continue

        values = []
        coresTableRows = []

        for row in rows:
    
            if row is "core":
                values.append(k)
    
            elif row in v:
                values.append( v[row] )

        if "systemname" in v:
            val = v["systemname"]
            if val is "":
                if "display_name" in v:
                    val = v["display_name"]
                    
            val = prettify(val)
            coresTableRows.append(val)
        else: corseTableRows.append("")

        db.execute(metadataTable, rows, values)
        db.execute(systemTable, coresRows, coresTableRows)

