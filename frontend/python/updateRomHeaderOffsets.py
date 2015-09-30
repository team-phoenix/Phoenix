from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
import hashlib

class RomHeaderOffsetUpdater(SqlTableUpdater):
    '''
        Base methods:
            SqlDatabase.updateTable( self, tableRows=O )
            SqlDatabase.prettifySystem( system="" )
            SqlDatabase.updateColumns( self, sqlDatabase = SqlDatabase() )
    '''

    def __init__(self, tableName="", tableRows=[], coreInfo={}):
        if len(tableRows) == 0:
             tableRows = ( ("seekIndex", "INTEGER NOT NULL")
                         , ("byteLength", "INTEGER NOT NULL")
                         , ("systemIndex", "TEXT NOT NULL UNIQUE")
                         , ("result", "TEXT NOT NULL") )

        SqlTableUpdater.__init__(self, tableName, tableRows, coreInfo)

    def getHeaderData(self, system):
        if "Sony PlayStation" == system:
            return (37664, 11, "504c415953544154494f4e")
        elif "GameCube (Wii)" == system:
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
                   name = v["systemname"]
                else:
                    if "display_name" in v:
                        name = v["display_name"]

                name = self.prettifySystem(name)
                offset = self.getHeaderData(name)

                if len(offset) > 0:

                    seek, size, result = offset
                    h = hashlib.sha1(name).hexdigest()

                    db.insert( self.tableName
                              , self.rowsDict.keys()
                              , values=[seek, size, h, result]
                              , force=True )

if __name__ == "__main__":

    updater = RomHeaderOffsetUpdater(tableName="systemHeaderOffsets")
    updater.updateTable()

