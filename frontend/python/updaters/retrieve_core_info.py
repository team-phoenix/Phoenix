import sys
import shlex
import requests

from tarfile import TarFile
from io import BytesIO
from io import StringIO
from collections import OrderedDict


def retrieveCoreInfo():

    DIR_PREFIX = 'libretro-super-master/dist/info/'

    response = requests.get('https://github.com/libretro/libretro-super/archive/master.tar.gz')
    # res = ().read()

    # In memory string buffer
    archive = BytesIO(response.content)

    tf = TarFile.open(mode='r:gz', fileobj=archive)
    # filter files and sort them
    files = [f for f in tf.getmembers() if f.name.startswith(DIR_PREFIX)]
    files = sorted(files, key=lambda m: m.name.split(DIR_PREFIX)[1])

    def infoToDict(files: list = None):
        if not files:
            files = []

        output = {'cores': OrderedDict()}

        for f in files:
            info = OrderedDict()
            for l in tf.extractfile(f).readlines():
                s = shlex.shlex(l.decode())
                s.quotes = '"'
                op = []
                try:
                    while True:
                        t = s.get_token()
                        if t == s.eof:
                            break
                        op.append(t)

                        if len(op) == 3:
                            check = op[1] in ('=', ':')

                            if not check:
                                raise Exception("op[1] not in ('=', ':')")

                            if op[1] == ':':
                                op = [''.join(op)]
                                continue

                            # Strip quotes
                            if op[2][0] == '"' and op[2][-1] == '"':
                                op[2] = op[2][1:-1]

                            # Convert string to boolean
                            if op[2].lower() == 'true':
                                op[2] = True
                            elif op[2].lower() == "false":
                                op[2] = False

                            # Decode utf-8 into unicode
                            # if isinstance(op[2], str):
                            #     op[2] = op[2].decode('utf-8')

                            info[op[0]] = op[2]

                            break

                except (ValueError, AssertionError) as e:
                    print("Could not process file %s (%s) " % (f.name, e), file=sys.stderr)
                    continue
            output['cores'][f.name.split(DIR_PREFIX)[1].split('.info')[0]] = info
        return output
    return infoToDict(files)


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


def infoString(output: dict = None):
    if not output:
        output = {}  # Avoid mutable defaults

    initializer_list = []

    for k, v in output['cores']:
        initializer_list.append("""    { %s, {
    %s
        } }""" % (
            cpprepr(k),
            ',\n'.join(['        { %s, %s }' % (cpprepr(k2), cpprepr(v2)) for k2, v2 in v.items()])
        ))
