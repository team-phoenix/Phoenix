
'''
This file is used to combine multiple no-intro xml files into one
sqlite file. This is used for checksum lookups to set up the artwork scraper.
'''

import xml.etree.ElementTree as ET
import sqlite3
import os
import sys
import getopt

UNKNOWN_VALUE = ""
TABLE_VALUE = "NOINTRO"
VERSION_FILE = "console_database_version.txt"

def xml2sqlite(file, connection):
    '''
    Converts opens up an XML file and inserts found elements into an SQL database.
    '''
    with open(file, 'r') as open_file:
        tree = ET.parse(open_file)
        root = tree.getroot()
        c = connection.cursor()
        
        try:
            for child in root:   #(child.tag == game), (child.attrib == [name])
                if child.tag != "game":
                    continue
                    
                gamename = UNKNOWN_VALUE
                description = UNKNOWN_VALUE
                romname = UNKNOWN_VALUE
                size = UNKNOWN_VALUE
                crc = UNKNOWN_VALUE
                sha1 = UNKNOWN_VALUE
                
                if "name" in child.attrib.keys():
                    gamename = child.attrib['name']
                
                for subchild in child: #(subchild.tag == ["description", "rom"]), (subchild.attrib == [name, size, crc, sha1])

                    keys = subchild.attrib.keys()
                    if subchild.tag == "description":
                        print("[ADD] ", subchild.text)
                        description = subchild.text
                        
                    if "name" in keys:
                        romname = subchild.attrib['name']
                    if "size" in keys:
                        size = subchild.attrib['size']
                    if "crc" in keys:
                        crc = subchild.attrib['crc']
                    if "sha1" in keys:
                        sha1 = subchild.attrib['sha1']

                    params = (gamename, description, romname, size, crc, sha1)
                    if "" in params:
                        continue;

                    c.execute("INSERT INTO " + TABLE_VALUE + " VALUES (?, ?, ?, ?, ?, ?)", params)
                    connection.commit()
                    
            c.close()
            return True;
            
        except sqlite3.Error as err:
            for i in err.args:
                print(i)
            c.close()
            return False

def create_version_file(files_list):
    with open(VERSION_FILE, "w") as out_file:
        for i in files_list:
            out_file.write("{:}\n".format(i))

def main(argv):
    files_list = list()
    out_file = ""
    try:
      opts, args = getopt.getopt(argv,"hi:o:",["input=","output="])
    except getopt.GetoptError:
        print("xml2sqlite.py -i '[<input_file>, <input_file>]' -o <output_file>")
        sys.exit(2)
      
    for opt, arg in opts:
        if opt == "-h":
            print("\nxml2sqlite.py -i <input_directory> -o <output_file>")
            print("\n-i, --input = Takes in directory to where the xml files are located")
            print("-o, --output = Is a single file")
            sys.exit()
        elif opt in ("-i", "--input"):
            if not os.path.isdir(arg):
                print("Input directory does not exist.")
                sys.exit(2)
            files_list = [os.path.join(arg, i) for i in os.listdir(arg) if os.path.isfile(os.path.join(arg, i)) ]
        elif opt in ("-o", "--output"):
            out_file = arg

    if len(files_list) == 0 or out_file == "":
        print("args aren't correct")
        sys.exit(2)
    
    if os.path.isfile(out_file):
        os.remove(out_file)

    connection = sqlite3.connect(out_file)
    c = connection.cursor()
    c.execute("CREATE TABLE "
              + TABLE_VALUE
              + " (gamename TEXT, description TEXT, romname TEXT, size TEXT, crc TEXT, sha1 TEXT)")
    
    print("Generating database...")

    results = list()
    for i in files_list:
        if ".xml" not in i:
            print("\nSkipping ", i, "\n")
            continue
        status = xml2sqlite(i, connection)
        if status:
            results.append("[OK] {:}".format(i))
        else:
            results.append("[Error] {:}".format(i))
    c.close()
    create_version_file(results)
    
    print()
    for i in results:
        print(i)

            
def test_read():
    '''
    This used to test the created database with single lookups.
    '''
    out_file = os.path.dirname(os.path.realpath(__file__)) + "/gamedatabase.db"
    connection = sqlite3.connect(out_file)
    c = connection.cursor()
    c.execute("SELECT * FROM " + TABLE_VALUE)
    for row in c:
        print(row)
    c.close()        

if __name__ == "__main__":
    main(sys.argv[1:])
