from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
from collections import OrderedDict

class ExtensionUpdater(SqlTableUpdater):

    def __init__(self, tableName="", tableColumns=[], coreInfo={}):
        if len(tableColumns) == 0:
            tableColumns = ( ("system", "TEXT NOT NULL")
                        , ("extension", "TEXT NOT NULL") )

        SqlTableUpdater.__init__(self, tableName, tableColumns, coreInfo)

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            i = 1
            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                name = ""
                if "systemname" in v:
                   name = self.libretroToPhoenix(v)
                else:
                    if "display_name" in v:
                        name = v["display_name"]

                if "supported_extensions" in v:
                    rawExt = v["supported_extensions"]
                    if rawExt != "":
                        exts = rawExt.split("|")

                        for e in exts:
                            values = (name, e)
                            db.insert(self.tableName, self.rowsDict.keys(), values)

                i = i + 1

if __name__ == "__main__":

    updater = ExtensionUpdater(tableName="extensions")
    updater.updateTable()
