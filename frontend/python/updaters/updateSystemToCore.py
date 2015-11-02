from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase

class SystemToCoreUpdater(SqlTableUpdater):

    def __init__(self, tableName="", tableColumns=[], coreInfo={}):
        if len(tableColumns) == 0:
            tableColumns = ( 
                             ("system", "TEXT" ),
                             ("core", "TEXT"),
                           )
        SqlTableUpdater.__init__(self, tableName, tableColumns, coreInfo)

    def updateTable(self):
    
        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            # Get Libretro system list
            libretroSystems = self.libretroSystemList()

            # Iterate through all cores available
            for k, v in self.coreInfo['cores'].iteritems():

                # Ignore anything that isn't an emulator
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

                # Iterate through all the Libretro IDs used by the core
                for s in systems:

                    # Now get all Phoenix IDs that map to the current system (usually it's just one, with a very big exception)
                    phxSystems = self.libretroToPhoenix(s)
                    for p in phxSystems:

                        # Iterate through the columns, defined up above
                        values = []
                        for row in self.columnsDict.keys():

                            # The system name must be stored as the Phoenix system name
                            if row == "system":
                                values.append(p)

                            # The key contains the name of the core
                            if row == "core":
                                values.append(k)


                        db.insert(self.tableName, self.columnsDict.keys(), values)

if __name__ == "__main__":

    from collections import OrderedDict
    
    updater = SystemToCoreUpdater("systemToCore", columns)
    updater.updateTable()


