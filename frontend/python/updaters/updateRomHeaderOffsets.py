from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
import hashlib

class RomHeaderOffsetUpdater(SqlTableUpdater):

    def __init__(self, tableName="", tableColumns=[], coreInfo={}):
        if len(tableColumns) == 0:
             tableColumns = ( ("system", "TEXT NOT NULL UNIQUE")
                         , ("byteLength", "INTEGER NOT NULL")
                         , ("seekIndex", "INTEGER NOT NULL")
                         , ("result", "TEXT NOT NULL") )

        SqlTableUpdater.__init__(self, tableName, tableColumns, coreInfo)

    def getHeaderData(self, system):
        if "Sony PlayStation" == system:
            return (37664, 11, "504c415953544154494f4e")
        elif "GameCube / Wii" == system:
            return (24, 4, "C2339F3D|"      # Wii ID
                            + "5d1c9ea3")  # GameCube ID
        return ()

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                name = ""
                if "systemname" in v:
                   name = self.libretroToPhoenix(v)
                else:
                    if "display_name" in v:
                        name = v["display_name"]

                offset = self.getHeaderData(name)

                if len(offset) > 0:

                    seek, size, result = offset

                    db.insert( self.tableName
                              , self.rowsDict.keys()
                              , values=[name, size, seek, result]
                              , force=True )

if __name__ == "__main__":

    updater = RomHeaderOffsetUpdater(tableName="headers")
    updater.updateTable()

