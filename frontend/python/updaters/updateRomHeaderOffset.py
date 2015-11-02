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
        if "Sony - PlayStation" == system:
            return (37664, 11, "504c415953544154494f4e")
        if "Nintendo - GameCube" == system:
          return (24, 4, "5D1C9EA3")
        if "Nintendo - Wii" == system:
          return (24, 4, "C2339F3D")
        return ()

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            systems = self.phoenixSystems()

            # System, metadata
            for s, m in systems.iteritems():

                offset = self.getHeaderData(s)

                if len(offset) > 0:

                    seek, size, result = offset

                    db.insert( self.tableName
                              , self.columnsDict.keys()
                              , values=[s, size, seek, result]
                              , force=True )

if __name__ == "__main__":

    updater = RomHeaderOffsetUpdater(tableName="header")
    updater.updateTable()

