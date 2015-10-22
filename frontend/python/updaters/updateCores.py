from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase

class CoresUpdater(SqlTableUpdater):
    '''
        Base methods:
            SqlDatabase.updateTable( self, tableRows=O )
            SqlDatabase.prettifySystem( system="" )
            SqlDatabase.updateColumns( self, sqlDatabase = SqlDatabase() )
    '''

    def __init__(self, tableName="", tableRows=[], coreInfo={}):
        if len(tableRows) == 0:
            tableRows = ( ("core", "TEXT")
                        , ("supported_extensions", "TEXT")
                        , ("display_version", "TEXT")
                        , ("license", "TEXT")
                        , ("authors", "TEXT")
                        , ("manufacturer", "TEXT")
                        , ("infoSystemName", "INTEGER NOT NULL" )
                        )
        SqlTableUpdater.__init__(self, tableName, tableRows, coreInfo)

    def updateTable(self):
    
        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                values = []
                for row in self.rowsDict.keys():

                    if row == "core":
                        values.append(k)
                    elif row == "infoSystemName":
                        name = ""
                        if "systemname" in v:
                            name = v["systemname"]
                        else:
                            if "display_name" in v:
                                name = v["display_name"]

                        values.append( name )

                    elif row in v:
                        values.append( v[row] )

                db.insert(self.tableName, self.rowsDict.keys(), values)

if __name__ == "__main__":

    from collections import OrderedDict
    
    updater = CoresUpdater("cores", columns)
    updater.updateTable()


