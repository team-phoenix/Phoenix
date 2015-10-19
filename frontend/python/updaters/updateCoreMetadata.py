from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase

class CoreMetadataUpdater(SqlTableUpdater):
    '''
        Base methods:
            SqlDatabase.updateTable( self, tableRows=O )
            SqlDatabase.prettifySystem( system="" )
            SqlDatabase.updateColumns( self, sqlDatabase = SqlDatabase() )
    '''

    def __init__(self, tableName="", tableRows=[], coreInfo={}):
        if len(tableRows) == 0:
            tableRows = ( ("coreIndex", "INTEGER PRIMARY KEY")
                        , ("core", "TEXT")
                        , ("supported_extensions", "TEXT")
                        , ("display_version", "TEXT")
                        , ("license", "TEXT")
                        , ("authors", "TEXT")
                        , ("manufacturer", "TEXT") )
        SqlTableUpdater.__init__(self, tableName, tableRows, coreInfo)

    def updateTable(self):
    
        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            i = 1
            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                values = [i]
                for row in self.rowsDict.keys():

                    if row == "core":
                        values.append(k)
                    elif row in v:
                        values.append( v[row] )


                db.insert(self.tableName, self.rowsDict.keys(), values)
                i = i + 1
        
if __name__ == "__main__":

    from collections import OrderedDict
    
    

    updater = CoreMetadataUpdater("coreMetadata", columns)
    updater.updateTable()


