from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
from collections import OrderedDict

class SystemsUpdater(SqlTableUpdater):

    def __init__(self, tableName="", tableColumns=[], coreInfo={}):
        if len(tableColumns) == 0:
            tableColumns = ( ("system", "TEXT"),
                          ("libretroSystemName", "TEXT NOT NULL"),
                          ("openvgdbSystemName", "TEXT"),
                          ("defaultCore", "TEXT"),
                          ("isSupported", "INTEGER NOT NULL"),
                        )

        SqlTableUpdater.__init__(self, tableName, tableColumns, coreInfo)

    #QStringLiteral( " FOREIGN KEY (collectionID) REFERENCES " ) + LibraryInternalDatabase::tableCollections +
     #       QStringLiteral( "(collectionID) ON DELETE CASCADE ON UPDATE CASCADE\n" )

    def updateTable(self):

        foreignKeyStatement = ", FOREIGN KEY (defaultCore) REFERENCES cores(core) ON DELETE CASCADE ON UPDATE CASCADE"
        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db, additionalStatement=foreignKeyStatement)

            systems = []
            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                name = ""
                if "systemname" in v:
                   name = v["systemname"]
                else:
                    if "display_name" in v:
                        name = v["display_name"]

                systems.append( name )

            systems = list(set(systems))
            for s in systems:

                metadata = self.systemMetadata[s]

                openvgdbSystem = metadata["openvgdbSystemName"]
                phoenixSystem = metadata["phoenixSystemName"]
                defaultCore = metadata["defaultCore"]
                isSupported = int(metadata["isSupported"])
                values = [phoenixSystem, s, openvgdbSystem, defaultCore, isSupported]

                db.insert(self.tableName, 
                self.rowsDict.keys()
                , values, force=False)

if __name__ == "__main__":

    updater = SystemsUpdater(tableName="systems")
    updater.updateTable()






