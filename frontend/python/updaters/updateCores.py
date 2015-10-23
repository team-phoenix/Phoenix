from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase

class CoresUpdater(SqlTableUpdater):

    def __init__(self, tableName="", tableColumns=[], coreInfo={}):
        if len(tableColumns) == 0:
            tableColumns = ( ("core", "TEXT")
                        , ("supported_extensions", "TEXT")
                        , ("display_version", "TEXT")
                        , ("license", "TEXT")
                        , ("authors", "TEXT")
                        , ("manufacturer", "TEXT")
                        , ("system", "TEXT NOT NULL" )
                        , ("libretroSystemName", "TEXT NOT NULL" )
                        )
        SqlTableUpdater.__init__(self, tableName, tableColumns, coreInfo)

    def updateTable(self):
    
        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            for k, v in self.coreInfo['cores'].iteritems():

                # Filter out anything that isn't an emulator
                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                # Iterate through tableColumns, defined up above
                values = []
                for row in self.rowsDict.keys():

                    # The key contains the name of the core
                    if row == "core":
                        values.append(k)

                    # The system name must be stored as the Phoenix system name
                    elif row == "system":
                        name = ""
                        if "systemname" in v:
                            name = self.libretroToPhoenix(v)
                        else:
                            if "display_name" in v:
                                name = v["display_name"]

                        values.append( name )

                    # Copy libretro system name
                    elif row == "libretroSystemName":
                        values.append( v["systemname"] )

                    # Directly copy data for the other columns, their keys match the libretro .info file keys exactly
                    elif row in v:
                        values.append( v[row] )

                db.insert(self.tableName, self.rowsDict.keys(), values)

if __name__ == "__main__":

    from collections import OrderedDict
    
    updater = CoresUpdater("cores", columns)
    updater.updateTable()


