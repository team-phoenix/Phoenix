from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
from collections import OrderedDict, defaultdict

class ExtensionUpdater(SqlTableUpdater):

    def __init__(self, tableName="", tableColumns=[], coreInfo={}):
        if len(tableColumns) == 0:
            tableColumns = ( ("system", "TEXT NOT NULL")
                        , ("extension", "TEXT NOT NULL") )

        SqlTableUpdater.__init__(self, tableName, tableColumns, coreInfo)

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            exts = defaultdict(lambda: set())

            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                # Get the list of systems (Libretro IDs) this core supports
                systems = []
                if "database" in v:
                    name = v["database"].split("|")

                    for n in name:
                        systems.append(n)

                # There are some cores that do not have "database" defined, use systemname instead
                elif "systemname" in v:
                        systems.append(v["systemname"])

                # Iterate through all systems defined in this .info file
                for s in systems:

                    # Now get all Phoenix IDs that map to the current system (usually it's just one, with a very big exception)
                    for p in self.libretroToPhoenix(s):

                        # Get the list of extensions defined in this .info file, add to the current system's set (no duplicates)
                        if "supported_extensions" in v:
                            rawExt = v["supported_extensions"]
                            if rawExt != "":
                                for e in rawExt.split("|"):
                                    exts[p].add(e)

            for p in self.phoenixSystems():
                for e in exts[p]:
                    values = (p, e)
                    db.insert(self.tableName, self.columnsDict.keys(), values)

if __name__ == "__main__":

    updater = ExtensionUpdater(tableName="extension")
    updater.updateTable()
