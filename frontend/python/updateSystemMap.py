from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
from collections import OrderedDict
import hashlib

class SystemMapUpdater(SqlTableUpdater):
    '''
        Base methods:
            SqlDatabase.updateTable( self, tableRows=O )
            SqlDatabase.prettifySystem( system="" )
            SqlDatabase.updateColumns( self, sqlDatabase = SqlDatabase() )
    '''

    def __init__(self, tableName="", tableRows=[], coreInfo={}):
        if len(tableRows) == 0:
            tableRows = ( ("systemIndex", "TEXT NOT NULL UNIQUE")
                        , ("systemname", "TEXT NOT NULL UNIQUE") )

        SqlTableUpdater.__init__(self, tableName, tableRows, coreInfo)

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            systems = []
            hashes = []
            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue
                
                name = ""
                if "systemname" in v:
                   name = v["systemname"]
                else:
                    if "display_name" in v:
                        name = v["display_name"]

                name = self.prettifySystem(name)
                systems.append( name )
                h = hashlib.sha1(name).hexdigest()
                hashes.append(h)

            sysDict = OrderedDict({})
            hashDict = OrderedDict({})
            for i in systems:
                sysDict[i] = None
            for a in hashes:
                hashDict[a] = None

            for s, h in zip(sysDict.keys(), hashDict.keys()):
                db.insert(self.tableName, self.rowsDict.keys(), values=[h, s])

if __name__ == "__main__":

    updater = SystemMapUpdater(tableName="systemMap")
    updater.updateTable()


